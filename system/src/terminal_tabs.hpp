#ifndef __TERMINAL_TABS_H__
#define __TERMINAL_TABS_H__

#include "terminal.hpp"
#include <memory>
#include <vector>

namespace mx {

    // TerminalTabs is a Window that presents a tabbed UI containing one or
    // more independent Terminal instances. Each tab owns its own bash
    // (PTY-backed) session; closing a tab terminates that session, and a
    // new tab spawns a fresh shell process.
    //
    // Layout: title bar (28px) | menu bar (30px) | tab bar (kTabBarH) |
    //         active terminal content.
    //
    // The contained Terminals run in "embedded" mode: they do not draw
    // their own window chrome and do not consume window-level events
    // (drag/resize/close); TerminalTabs owns those.
    class TerminalTabs : public Window {
    public:
        explicit TerminalTabs(mxApp &app);
        ~TerminalTabs() override;

        void draw(mxApp &app) override;
        bool event(mxApp &app, SDL_Event &e) override;
        void screenResize(int w, int h) override;
        void stateChanged(bool min, bool max, bool closed) override;

        // Pass-throughs to the active tab.
        void setWallpaper(SDL_Texture *tex);
        void setEffect(Terminal::TermEffect e, mxApp &app);

        Terminal *activeTab();
        const Terminal *activeTab() const;

        // Tab management.
        void newTab(mxApp &app);
        void closeTab(int idx);
        void switchTab(int idx);
        int tabCount() const { return static_cast<int>(tabs_.size()); }

        // Convenience helpers used by menu callbacks.
        bool hasSelectedText() const;
        void copyToClipboard();
        void copySelectionToClipboard();
        void pasteFromClipboard();

    private:
        static constexpr int kTabBarH      = 26;
        static constexpr int kTabMinW      = 110;
        static constexpr int kTabMaxW      = 200;
        static constexpr int kTabPad       = 4;
        static constexpr int kTabCloseSize = 14;
        static constexpr int kNewTabBtnW   = 22;
        static constexpr int kArrowBtnW    = 18;

        std::vector<std::unique_ptr<Terminal>> tabs_;
        int active_ = 0;
        int tabScroll_ = 0;            // index of leftmost visible tab
        SDL_Texture *wallpaper_ = nullptr;
        Terminal::TermEffect effect_ = Terminal::TermEffect::None;
        mxApp *appPtr_ = nullptr;

        // Cached hit rectangles for the tab bar (recomputed each draw()).
        struct TabHit {
            int index;
            SDL_Rect tab;
            SDL_Rect close;
        };
        std::vector<TabHit> tabHits_;
        SDL_Rect newTabBtn_{0, 0, 0, 0};
        SDL_Rect leftArrow_{0, 0, 0, 0};
        SDL_Rect rightArrow_{0, 0, 0, 0};
        bool arrowsVisible_ = false;

        SDL_Rect tabBarRect() const;     // strip allocated for the tab bar
        SDL_Rect terminalRect() const;   // outer rect to assign to the active terminal
        void layoutActiveTab();
        void drawTabBar(mxApp &app);
        std::string tabTitle(int idx) const;
    };

}

#endif
