#include "terminal_tabs.hpp"
#include "mx_window.hpp"
#include "dimension.hpp"
#include "mx_system_bar.hpp"
#include <algorithm>

namespace mx {

    TerminalTabs::TerminalTabs(mxApp &app) : Window(app) {
        appPtr_ = &app;
        Window::setCanResize(true);
        // Spawn the initial bash session.
        newTab(app);
    }

    TerminalTabs::~TerminalTabs() = default;

    Terminal *TerminalTabs::activeTab() {
        if (tabs_.empty()) return nullptr;
        if (active_ < 0) active_ = 0;
        if (active_ >= static_cast<int>(tabs_.size()))
            active_ = static_cast<int>(tabs_.size()) - 1;
        return tabs_[active_].get();
    }

    const Terminal *TerminalTabs::activeTab() const {
        if (tabs_.empty()) return nullptr;
        int idx = active_;
        if (idx < 0) idx = 0;
        if (idx >= static_cast<int>(tabs_.size()))
            idx = static_cast<int>(tabs_.size()) - 1;
        return tabs_[idx].get();
    }

    void TerminalTabs::newTab(mxApp &app) {
        auto t = std::make_unique<Terminal>(app);
        t->setEmbedded(true);
        t->dim = this->dim;
        t->systemBar = this->systemBar;
        if (wallpaper_) t->setWallpaper(wallpaper_);
        if (effect_ != Terminal::TermEffect::None)
            t->setEffect(effect_, app);
        // Embedded children must be "visible" since the parent decides
        // when to draw them; isVisible() guards Terminal::draw / event.
        t->show(true);
        // Inherit the parent window's rect so the embedded terminal has
        // a valid rect from frame zero (parent will refine in draw()).
        SDL_Rect outer;
        Window::getRect(outer);
        if (outer.w > 0 && outer.h > 0) {
            // Allocate inner rect minus title+menu+tab strips.
            SDL_Rect inner = terminalRect();
            t->setRect(inner);
        }
        tabs_.push_back(std::move(t));
        active_ = static_cast<int>(tabs_.size()) - 1;
    }

    void TerminalTabs::closeTab(int idx) {
        // -1 means "currently active tab".
        if (idx < 0) idx = active_;
        if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return;
        // Refuse to close the last remaining tab — the user can close the
        // window instead via the title-bar X.
        if (tabs_.size() == 1) return;
        tabs_.erase(tabs_.begin() + idx);
        if (active_ >= static_cast<int>(tabs_.size()))
            active_ = static_cast<int>(tabs_.size()) - 1;
        if (active_ < 0) active_ = 0;
    }

    void TerminalTabs::switchTab(int idx) {
        if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return;
        active_ = idx;
    }

    void TerminalTabs::setWallpaper(SDL_Texture *tex) {
        wallpaper_ = tex;
        for (auto &t : tabs_) t->setWallpaper(tex);
    }

    void TerminalTabs::setEffect(Terminal::TermEffect e, mxApp &app) {
        effect_ = e;
        for (auto &t : tabs_) t->setEffect(e, app);
    }

    bool TerminalTabs::hasSelectedText() const {
        const Terminal *t = activeTab();
        return t && t->hasSelectedText();
    }
    void TerminalTabs::copyToClipboard() {
        if (Terminal *t = activeTab()) t->copyToClipboard();
    }
    void TerminalTabs::copySelectionToClipboard() {
        if (Terminal *t = activeTab()) t->copySelectionToClipboard();
    }
    void TerminalTabs::pasteFromClipboard() {
        if (Terminal *t = activeTab()) t->pasteFromClipboard();
    }

    void TerminalTabs::screenResize(int w, int h) {
        Window::screenResize(w, h);
        SDL_Rect rc;
        Window::getRect(rc);
        const int topLimit    = 26;
        const int bottomLimit = h - 50;
        const int maxH        = std::max(1, bottomLimit - topLimit);
        if (isMaximized()) {
            // Fill the host window (under the menu bar, above the system bar).
            rc = { 0, topLimit, w, h - 76 };
        } else if (rc.w <= 0 || rc.h <= 0) {
            // Initial layout: centered default size scaled to host.
            const int baseWidth  = 1280;
            const int baseHeight = 720;
            float scaleX = static_cast<float>(w) / baseWidth;
            float scaleY = static_cast<float>(h) / baseHeight;
            rc.w = static_cast<int>(800 * scaleX);
            rc.h = static_cast<int>(505 * scaleY);
            rc.x = (w - rc.w) / 2;
            rc.y = (h - rc.h) / 2;
            if (rc.y < topLimit) rc.y = topLimit;
        } else {
            // Preserve the user's chosen size; clamp to fit the new host.
            if (rc.w > w)    rc.w = w;
            if (rc.h > maxH) rc.h = maxH;
            if (rc.x < 0)             rc.x = 0;
            if (rc.y < topLimit)      rc.y = topLimit;
            if (rc.x + rc.w > w)      rc.x = std::max(0, w - rc.w);
            if (rc.y + rc.h > bottomLimit)
                rc.y = std::max(topLimit, bottomLimit - rc.h);
        }
        Window::setRect(rc);
        layoutActiveTab();
        // Forward to all tabs so each PTY learns the new size.
        for (auto &t : tabs_) {
            if (t.get() != activeTab()) {
                // Inactive tabs share the same content rect.
                t->setRect(terminalRect());
            }
            t->screenResize(w, h);
        }
    }

    void TerminalTabs::stateChanged(bool min, bool max, bool closed) {
        // Forward to active tab so its scrollbar / wrap recompute.
        if (Terminal *t = activeTab())
            t->stateChanged(min, max, closed);
    }

    SDL_Rect TerminalTabs::tabBarRect() const {
        SDL_Rect rc;
        const_cast<TerminalTabs*>(this)->Window::getRect(rc);
        // Window's title bar (drawMenubar) is 30px high; tab bar sits
        // flush against its bottom edge.
        SDL_Rect bar{ rc.x, rc.y + 30, rc.w, kTabBarH };
        if (bar.h > rc.h - 30) bar.h = std::max(0, rc.h - 30);
        return bar;
    }

    SDL_Rect TerminalTabs::terminalRect() const {
        SDL_Rect rc;
        const_cast<TerminalTabs*>(this)->Window::getRect(rc);
        // Terminal subtracts 28 from rc.y / rc.h internally to skip its
        // own (would-be) title bar. Shift the rect up by 28 so its
        // content lands flush against the bottom of our tab bar.
        const int desiredContentY = rc.y + 30 + kTabBarH;
        SDL_Rect inner{
            rc.x,
            desiredContentY - 28,           // Terminal will add 28 back
            rc.w,
            rc.h - 30 - kTabBarH + 28
        };
        if (inner.h < 28 + 1) inner.h = 28 + 1;
        return inner;
    }

    void TerminalTabs::layoutActiveTab() {
        SDL_Rect inner = terminalRect();
        for (auto &t : tabs_)
            t->setRect(inner);
    }

    std::string TerminalTabs::tabTitle(int idx) const {
        if (idx < 0 || idx >= static_cast<int>(tabs_.size())) return "";
        // Number tabs 1-based in the title for clarity.
        return std::string("Shell ") + std::to_string(idx + 1);
    }

    void TerminalTabs::drawTabBar(mxApp &app) {
        SDL_Rect bar = tabBarRect();
        // Background strip.
        SDL_SetRenderDrawColor(app.ren, 60, 60, 60, 255);
        SDL_RenderFillRect(app.ren, &bar);
        SDL_SetRenderDrawColor(app.ren, 30, 30, 30, 255);
        SDL_RenderDrawLine(app.ren, bar.x, bar.y + bar.h - 1,
                           bar.x + bar.w, bar.y + bar.h - 1);

        tabHits_.clear();
        const int n = static_cast<int>(tabs_.size());
        if (n == 0) return;

        // Fixed tab width — we scroll horizontally instead of shrinking
        // tabs once they no longer fit.
        const int tabW = kTabMinW;

        // Reserve space on the right for "+" new-tab button.
        int reservedRight = kNewTabBtnW + kTabPad * 2;

        // Determine if scrolling is needed. The available width for tabs
        // (no arrows) is the bar minus left padding and reserved right.
        const int rawAvail = bar.w - reservedRight - kTabPad;
        const int needWidth = n * (tabW + 2);
        arrowsVisible_ = (needWidth > rawAvail);

        int reservedLeft  = kTabPad;
        if (arrowsVisible_) {
            reservedLeft  = kArrowBtnW + kTabPad * 2;            // < button
            reservedRight = kArrowBtnW + kTabPad + kNewTabBtnW + kTabPad * 2;
        }

        // Clamp scroll so we never leave blank space on the right when
        // there are still tabs further left we could show.
        const int avail = bar.w - reservedLeft - reservedRight;
        const int visCount = std::max(1, avail / (tabW + 2));
        const int maxScroll = std::max(0, n - visCount);
        if (tabScroll_ > maxScroll) tabScroll_ = maxScroll;
        if (tabScroll_ < 0) tabScroll_ = 0;
        // Auto-scroll to keep active tab in view.
        if (active_ < tabScroll_) tabScroll_ = active_;
        else if (active_ >= tabScroll_ + visCount)
            tabScroll_ = active_ - visCount + 1;

        TTF_Font *fnt = app.font;
        const int leftEdge  = bar.x + reservedLeft;
        const int rightEdge = bar.x + bar.w - reservedRight;
        int x = leftEdge;
        for (int i = tabScroll_; i < n; ++i) {
            if (x + tabW > rightEdge) break;
            SDL_Rect tab{ x, bar.y + 2, tabW, bar.h - 4 };

            const bool isActive = (i == active_);
            if (isActive)
                SDL_SetRenderDrawColor(app.ren, 30, 30, 30, 255);
            else
                SDL_SetRenderDrawColor(app.ren, 90, 90, 90, 255);
            SDL_RenderFillRect(app.ren, &tab);
            SDL_SetRenderDrawColor(app.ren, 20, 20, 20, 255);
            SDL_RenderDrawRect(app.ren, &tab);

            // Label.
            std::string label = tabTitle(i);
            if (fnt) {
                SDL_Color col = isActive ? SDL_Color{255, 255, 255, 255}
                                         : SDL_Color{210, 210, 210, 255};
                int tw = 0, th = 0;
                TTF_SizeText(fnt, label.c_str(), &tw, &th);
                int textMax = tab.w - kTabCloseSize - kTabPad * 3;
                if (textMax < 10) textMax = 10;
                std::string shown = label;
                while (tw > textMax && shown.size() > 1) {
                    shown.pop_back();
                    TTF_SizeText(fnt, (shown + "…").c_str(), &tw, &th);
                }
                if (shown != label) shown += "…";
                app.font_printText(fnt, tab.x + kTabPad,
                                   tab.y + (tab.h - th) / 2, shown, col);
            }

            // Close button.
            SDL_Rect close{
                tab.x + tab.w - kTabCloseSize - kTabPad,
                tab.y + (tab.h - kTabCloseSize) / 2,
                kTabCloseSize, kTabCloseSize
            };
            SDL_SetRenderDrawColor(app.ren, isActive ? 70 : 110,
                                          isActive ? 70 : 110,
                                          isActive ? 70 : 110, 255);
            SDL_RenderFillRect(app.ren, &close);
            SDL_SetRenderDrawColor(app.ren, 230, 230, 230, 255);
            SDL_RenderDrawLine(app.ren, close.x + 3, close.y + 3,
                               close.x + close.w - 4, close.y + close.h - 4);
            SDL_RenderDrawLine(app.ren, close.x + close.w - 4, close.y + 3,
                               close.x + 3, close.y + close.h - 4);

            tabHits_.push_back({ i, tab, close });
            x += tabW + 2;
        }

        // Scroll arrows (only when scrolling needed).
        auto drawArrowBtn = [&](SDL_Rect r, bool leftArrow, bool enabled) {
            SDL_SetRenderDrawColor(app.ren, enabled ? 90 : 60,
                                            enabled ? 90 : 60,
                                            enabled ? 90 : 60, 255);
            SDL_RenderFillRect(app.ren, &r);
            SDL_SetRenderDrawColor(app.ren, 20, 20, 20, 255);
            SDL_RenderDrawRect(app.ren, &r);
            SDL_SetRenderDrawColor(app.ren,
                enabled ? 230 : 130,
                enabled ? 230 : 130,
                enabled ? 230 : 130, 255);
            int cx = r.x + r.w / 2;
            int cy = r.y + r.h / 2;
            // Triangle (4 lines for visibility).
            if (leftArrow) {
                SDL_RenderDrawLine(app.ren, cx + 3, cy - 5, cx - 3, cy);
                SDL_RenderDrawLine(app.ren, cx - 3, cy, cx + 3, cy + 5);
                SDL_RenderDrawLine(app.ren, cx + 3, cy - 5, cx + 3, cy + 5);
            } else {
                SDL_RenderDrawLine(app.ren, cx - 3, cy - 5, cx + 3, cy);
                SDL_RenderDrawLine(app.ren, cx + 3, cy, cx - 3, cy + 5);
                SDL_RenderDrawLine(app.ren, cx - 3, cy - 5, cx - 3, cy + 5);
            }
        };

        if (arrowsVisible_) {
            leftArrow_ = SDL_Rect{ bar.x + kTabPad,
                                   bar.y + 2, kArrowBtnW, bar.h - 4 };
            rightArrow_ = SDL_Rect{
                bar.x + bar.w - kNewTabBtnW - kTabPad - kArrowBtnW - kTabPad,
                bar.y + 2, kArrowBtnW, bar.h - 4 };
            drawArrowBtn(leftArrow_,  true,  tabScroll_ > 0);
            drawArrowBtn(rightArrow_, false, tabScroll_ < maxScroll);
        } else {
            leftArrow_  = SDL_Rect{0, 0, 0, 0};
            rightArrow_ = SDL_Rect{0, 0, 0, 0};
        }

        // "+" new-tab button on the right.
        newTabBtn_ = SDL_Rect{
            bar.x + bar.w - kNewTabBtnW - kTabPad,
            bar.y + 3,
            kNewTabBtnW,
            bar.h - 6
        };
        SDL_SetRenderDrawColor(app.ren, 90, 90, 90, 255);
        SDL_RenderFillRect(app.ren, &newTabBtn_);
        SDL_SetRenderDrawColor(app.ren, 20, 20, 20, 255);
        SDL_RenderDrawRect(app.ren, &newTabBtn_);
        // Plus glyph.
        int cx = newTabBtn_.x + newTabBtn_.w / 2;
        int cy = newTabBtn_.y + newTabBtn_.h / 2;
        SDL_SetRenderDrawColor(app.ren, 230, 230, 230, 255);
        SDL_RenderDrawLine(app.ren, cx - 5, cy, cx + 5, cy);
        SDL_RenderDrawLine(app.ren, cx, cy - 5, cx, cy + 5);
    }

    void TerminalTabs::draw(mxApp &app) {
        if (!isVisible()) return;

        // Auto-close tabs whose bash session has exited (e.g. the user
        // typed `exit`). When the LAST remaining tab's shell dies, the
        // window has nothing useful left to do, so we quit the program.
        for (size_t i = 0; i < tabs_.size(); /* manual */) {
            if (!tabs_[i]->sessionAlive()) {
                if (tabs_.size() == 1) {
                    // Last shell exited — quit the program.
                    SDL_Event quit_event;
                    quit_event.type = SDL_QUIT;
                    SDL_PushEvent(&quit_event);
                    ++i;
                    continue;
                }
                tabs_.erase(tabs_.begin() + static_cast<long>(i));
                if (active_ >= static_cast<int>(tabs_.size()))
                    active_ = static_cast<int>(tabs_.size()) - 1;
            } else {
                ++i;
            }
        }

        // Window chrome (title bar + frame). Window::draw also runs
        // minimize/restore animations.
        Window::draw(app);
        if (!isDraw()) return;

        // Always re-sync the active tab's rect; the parent window may
        // have moved or been resized this frame.
        layoutActiveTab();

        // Propagate parent state (dim/systemBar/wallpaper) to every tab.
        // The first tab is created during our own constructor when the
        // parent has not yet been added to a DimensionContainer, so dim
        // is still nullptr at that point — fix that up lazily here.
        for (auto &t : tabs_) {
            if (!t->dim) t->dim = this->dim;
            if (!t->systemBar) t->systemBar = this->systemBar;
        }

        // Draw the active terminal first — its shader/wallpaper covers
        // the entire screen behind our window. We then re-draw chrome
        // (title bar + tab strip) ON TOP so they remain visible over
        // the fullscreen effect.
        if (Terminal *t = activeTab())
            t->draw(app);

        Window::drawMenubar(app);
        drawTabBar(app);
    }

    bool TerminalTabs::event(mxApp &app, SDL_Event &e) {
        if (!isVisible()) return false;

        // Window-level chrome (drag, resize, close, minimize, maximize)
        // must run first so the user can move/resize the tabbed window.
        if (Window::event(app, e)) {
            // Even if Window consumed it, keep our active-tab rect in sync.
            layoutActiveTab();
            return true;
        }

        // Tab bar interactions.
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mxp = e.button.x, myp = e.button.y;
            auto inRect = [&](const SDL_Rect &r) {
                return r.w > 0 && r.h > 0 &&
                       mxp >= r.x && mxp < r.x + r.w &&
                       myp >= r.y && myp < r.y + r.h;
            };
            // Scroll arrows.
            if (arrowsVisible_ && inRect(leftArrow_)) {
                if (tabScroll_ > 0) tabScroll_--;
                return true;
            }
            if (arrowsVisible_ && inRect(rightArrow_)) {
                tabScroll_++;            // clamped next draw()
                return true;
            }
            // New-tab button.
            if (inRect(newTabBtn_)) {
                newTab(app);
                layoutActiveTab();
                return true;
            }
            // Per-tab close / switch (hit list contains real tab indices).
            for (const auto &h : tabHits_) {
                if (inRect(h.close)) {
                    closeTab(h.index);
                    layoutActiveTab();
                    return true;
                }
                if (inRect(h.tab)) {
                    switchTab(h.index);
                    layoutActiveTab();
                    return true;
                }
            }
        }
        // Mouse wheel scrolls the tab strip when arrows are visible and
        // the cursor is over the tab bar.
        if (e.type == SDL_MOUSEWHEEL && arrowsVisible_) {
            int mxp = 0, myp = 0;
            SDL_GetMouseState(&mxp, &myp);
            SDL_Rect bar = tabBarRect();
            if (mxp >= bar.x && mxp < bar.x + bar.w &&
                myp >= bar.y && myp < bar.y + bar.h) {
                if (e.wheel.y > 0 && tabScroll_ > 0) tabScroll_--;
                else if (e.wheel.y < 0)              tabScroll_++;
                return true;
            }
        }

        // Forward everything else to the active terminal.
        if (Terminal *t = activeTab())
            return t->event(app, e);
        return false;
    }

}
