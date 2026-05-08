#include"mx_window.hpp"
#include"mx_abstract_control.hpp"
#include"mx_system_bar.hpp"
#include"dimension.hpp"
#include"mx_menu.hpp"
#include"loadpng.hpp"

namespace mx {

    extern bool cursor_shown;

    namespace {
        // kTitleBarHeight is defined in mx_window.hpp and shared across TUs.
        static constexpr int kClientInsetLeft = 4;
        static constexpr int kClientInsetRight = 4;
        static constexpr int kClientInsetTop = kTitleBarHeight + 1;
        static constexpr int kClientInsetBottom = 4;

        // Custom resize cursors loaded lazily on first hover. These are
        // bundled PNGs (Adwaita, LGPL) under assets/images/cursors/. If
        // loading fails we fall back to SDL's built-in system cursors.
        struct ResizeCursorEntry {
            const char *file;
            int hotX;
            int hotY;
            SDL_SystemCursor fallback;
        };

        SDL_Cursor *g_resizeCursors[4] = {nullptr, nullptr, nullptr, nullptr};
        bool g_resizeCursorsInitialized = false;

        enum ResizeCursorKind {
            kCursorEW   = 0,  // left/right
            kCursorNS   = 1,  // top/bottom
            kCursorNWSE = 2,  // TL/BR corner
            kCursorNESW = 3,  // TR/BL corner
        };

        SDL_Cursor *createCursorFromPNG(const char *relPath, int hotX, int hotY) {
            SDL_Surface *surf = mx::LoadPNG(getPath(relPath).c_str());
            if (!surf) return nullptr;
            SDL_Cursor *c = SDL_CreateColorCursor(surf, hotX, hotY);
            SDL_FreeSurface(surf);
            return c;
        }

        SDL_Cursor *getResizeCursor(ResizeCursorKind k) {
            if (!g_resizeCursorsInitialized) {
                g_resizeCursorsInitialized = true;
                // Hotspots match the source cursors in /usr/share/icons/Adwaita
                // (36x36 frames). Center for straight edges; visual midpoint
                // for the diagonal arrows.
                static const ResizeCursorEntry table[4] = {
                    { "images/cursors/ew-resize.png",   18, 18, SDL_SYSTEM_CURSOR_SIZEWE   },
                    { "images/cursors/ns-resize.png",   18, 19, SDL_SYSTEM_CURSOR_SIZENS   },
                    { "images/cursors/nwse-resize.png", 16, 16, SDL_SYSTEM_CURSOR_SIZENWSE },
                    { "images/cursors/nesw-resize.png", 16, 16, SDL_SYSTEM_CURSOR_SIZENESW },
                };
                for (int i = 0; i < 4; ++i) {
                    g_resizeCursors[i] = createCursorFromPNG(table[i].file, table[i].hotX, table[i].hotY);
                    if (!g_resizeCursors[i]) {
                        g_resizeCursors[i] = SDL_CreateSystemCursor(table[i].fallback);
                    }
                }
            }
            return g_resizeCursors[k];
        }
    } // namespace

    Window::Window(mxApp &app) 
    : dim(nullptr),
      minimized(false),
      dragging(false),
      x(0), y(0), w(0), h(0),
      dim_w(0), dim_h(0),
      shown(false),
      remove_on(false),
      maximized(false),
      dragOffsetX(0), dragOffsetY(0),
      oldX(0), oldY(0), oldW(0), oldH(0),
      is_visible(true),
      reload_window(false),
      can_resize(false),
      isMinimizing(false),
      minTargetX(0), minTargetY(0),
      minTargetW(0), minTargetH(0),
      minAnimationStep(1),
      restoreTotalFrames(12), restoreFrame(0),
      restoreStartX(0), restoreStartY(0),
      restoreStartW(0), restoreStartH(0),
    lastMaxToggleTicks_(0),
      originalX(0), originalY(0),
      originalWidth(0), originalHeight(0),
      isRestoring(false),
      restoreTargetX(0), restoreTargetY(0),
      restoreTargetW(0), restoreTargetH(0),
      orig_x(0), orig_y(0),
      icon(nullptr),
      minimizeHovered(SDL_FALSE),
      closeHovered(SDL_FALSE),
      maximizeHovered(SDL_FALSE),
      menu(app, this) {
        minimizeButton = {0, 0, 0, 0}; 
        closeButton = {0, 0, 0, 0};    
        maximizeButton = {0, 0, 0, 0}; 
        dim_w = app.width;
        dim_h = app.height; 
        setCanResize(true);
    }

    Window::~Window() {
        if(icon != nullptr)
            SDL_DestroyTexture(icon);
        mx::system_out << "MasterX: Releasing Window: " << title << "\n";
    }


    void Window::create(DimensionContainer *dim, const std::string &n, const int xx, const int yy, const int ww, const int hh) {
        title = n;
        x = xx;
        y = yy+25;
        w = ww;
        h = hh;
        orig_x = x;
        orig_y = y;
        this->dim = dim;
    }

    void Window::getDrawRect(SDL_Rect &rc) {
        getRect(rc);
        rc.x += kClientInsetLeft;
        rc.w -= (kClientInsetLeft + kClientInsetRight);
        rc.y += kClientInsetTop;
        rc.h -= (kClientInsetTop + kClientInsetBottom);
    }

    void Window::setIcon(SDL_Texture *icon) {
        this->icon = icon;
    }

    void Window::destroyWindow() {
        dim->destroyWindow(this);
    }

    bool Window::isPointInside(int xx, int yy) {
        return (xx >= x && xx < x + w &&
            yy >= y && yy < y + h);
    }

    void Window::draw(mxApp &app) {

        if(shown == false || (minimized == true && isMinimizing == false)) return;
        
        if (isMinimizing) {
            x += (minTargetX - x) / minAnimationStep;
            y += (minTargetY - y) / minAnimationStep;
            w += (minTargetW - w) / minAnimationStep;
            h += (minTargetH - h) / minAnimationStep;

            if (abs(x - minTargetX) < 1 && abs(y - minTargetY) < 1 && 
                abs(w - minTargetW) < 1 && abs(h - minTargetH) < 1) {
                isMinimizing = false;
                minimized = true;
                minimizeHovered = SDL_FALSE;
                maximizeHovered = SDL_FALSE;
                closeHovered = SDL_FALSE; 
            }
        }

        if (isRestoring) {
            // Fixed-frame lerp: all axes advance by the same normalised t
            // each frame so they start and finish together.
            restoreFrame++;
            if (restoreFrame >= restoreTotalFrames) {
                x = restoreTargetX;
                y = restoreTargetY;
                w = restoreTargetW;
                h = restoreTargetH;
            } else {
                auto lerp = [](int start, int end, int frame, int total) -> int {
                    return start + (end - start) * frame / total;
                };
                x = lerp(restoreStartX, restoreTargetX, restoreFrame, restoreTotalFrames);
                y = lerp(restoreStartY, restoreTargetY, restoreFrame, restoreTotalFrames);
                w = lerp(restoreStartW, restoreTargetW, restoreFrame, restoreTotalFrames);
                h = lerp(restoreStartH, restoreTargetH, restoreFrame, restoreTotalFrames);
            }

            if (restoreFrame >= restoreTotalFrames) {
                isRestoring = false;
                minimized = false;
                minimizeHovered = SDL_FALSE;
                maximizeHovered = SDL_FALSE;
                closeHovered = SDL_FALSE;

                for(auto &c : children) {
                    c->setWindowPos(x, y);
                    c->resizeWindow(w, h);
                }
                dragging = false;
            }
        }
        SDL_Rect rc = {x, y, w, h};

        if (isRestoring || isMinimizing) {
            SDL_SetRenderDrawColor(app.ren, 205, 205, 205, 255);
            SDL_RenderFillRect(app.ren, &rc);
            drawMenubar(app);
            SDL_Rect animClip = {
                x + 1,
                y + kTitleBarHeight + 1,
                w - 2,
                h - kTitleBarHeight - 2
            };
            if (animClip.w > 0 && animClip.h > 0) {
                SDL_RenderSetClipRect(app.ren, &animClip);
                for (auto &c : children) {
                    c->setWindowPos(x, y);
                    c->draw(app);
                }
                SDL_RenderSetClipRect(app.ren, nullptr);
            }
        } else {
            SDL_SetRenderDrawColor(app.ren, 205, 205, 205, 255);
            SDL_RenderFillRect(app.ren, &rc);

            int startGray = 165;
            int endGray = 205;
            const int frameRight = x + w - 1;
            for (int i = 0; i < h; ++i) {
                int grayValue = startGray + (endGray - startGray) * i / h;
                SDL_SetRenderDrawColor(app.ren, grayValue, grayValue, grayValue, 255);
                SDL_RenderDrawLine(app.ren, x, y + i, frameRight, y + i);
            }
            drawMenubar(app);
            SDL_Rect clientClip = {
                x + 1,
                y + kTitleBarHeight + 1,
                w - 2,
                h - kTitleBarHeight - 2
            };
            SDL_RenderSetClipRect(app.ren, &clientClip);
            for (auto &c : children) {
                c->setWindowPos(x, y);
                c->draw(app);
            }
            SDL_RenderSetClipRect(app.ren, nullptr);
        }
    }

    bool Window::isVisible() const {
       return shown;
    }

    bool Window::isDraw() const {
        if(shown == false || (minimized == true && isMinimizing == false)) return false;
        return true;
    }

   void Window::drawMenubar(mxApp &app) {
        SDL_Color lightBlue = {173, 216, 230}; 
        SDL_Color darkBlue = {0, 0, 139};     
        const int frameRight = x + w - 1;
        const int frameBottom = y + h - 1;

        for (int i = 0; i < kTitleBarHeight; ++i) {
            int red = lightBlue.r + (darkBlue.r - lightBlue.r) * i / kTitleBarHeight;
            int green = lightBlue.g + (darkBlue.g - lightBlue.g) * i / kTitleBarHeight;
            int blue = lightBlue.b + (darkBlue.b - lightBlue.b) * i / kTitleBarHeight;
            SDL_SetRenderDrawColor(app.ren, red, green, blue, 255);
            SDL_RenderDrawLine(app.ren, x, y + i, frameRight, y + i);
        }

        // Windows-style title bar buttons: wide flat rectangles that span
        // the full titlebar height, with a crisp icon always visible.
        // Close=rightmost, Maximize=middle, Minimize=left of those.
        const int btnW  = 46;
        const int btnH  = kTitleBarHeight;   // full bar height
        const bool allowMaximizeButton = canResize();

        const int closeBtnX    = x + w - btnW;
        const int maximizeBtnX = closeBtnX - btnW;
        const int minimizeBtnX = allowMaximizeButton
            ? maximizeBtnX - btnW
            : closeBtnX   - btnW;

        // Store hit rects (full height so click targets match visual).
        minimizeButton = { minimizeBtnX, y, btnW, btnH };
        maximizeButton = allowMaximizeButton
            ? SDL_Rect{ maximizeBtnX, y, btnW, btnH }
            : SDL_Rect{ 0, 0, 0, 0 };
        closeButton    = { closeBtnX, y, btnW, btnH };

        // Draw one button: optional hover fill, then icon.
        // 'closeStyle': if true, hover fill is red; otherwise light gray.
        auto drawWinBtn = [&](SDL_Rect btn, bool hovered, bool closeStyle) {
            if (hovered) {
                if (closeStyle)
                    SDL_SetRenderDrawColor(app.ren, 196, 43, 28, 255);  // Win11 red
                else
                    SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 40); // subtle tint
                SDL_RenderFillRect(app.ren, &btn);
            }
        };

        drawWinBtn(minimizeButton, minimizeHovered, false);
        if (allowMaximizeButton)
            drawWinBtn(maximizeButton, maximizeHovered, false);
        drawWinBtn(closeButton, closeHovered, true);

        // Icon colour: white normally, slightly dimmed when not hovered.
        // Icons are always visible (Windows convention).
        const SDL_Color iconCol      = { 255, 255, 255, 255 };
        const SDL_Color iconColDim   = { 200, 200, 200, 255 };

        // ── Minimize: single horizontal bar near the bottom of the icon area ──
        {
            const int cx   = minimizeButton.x + minimizeButton.w / 2;
            const int cy   = minimizeButton.y + minimizeButton.h / 2;
            const SDL_Color &c = minimizeHovered ? iconCol : iconColDim;
            SDL_SetRenderDrawColor(app.ren, c.r, c.g, c.b, 255);
            // Two-pixel-thick bar centred horizontally, sitting just below centre.
            SDL_RenderDrawLine(app.ren, cx - 5, cy + 3, cx + 5, cy + 3);
            SDL_RenderDrawLine(app.ren, cx - 5, cy + 4, cx + 5, cy + 4);
        }

        // ── Maximize / Restore: square outline with bold top edge ──
        if (allowMaximizeButton) {
            const int cx   = maximizeButton.x + maximizeButton.w / 2;
            const int cy   = maximizeButton.y + maximizeButton.h / 2;
            const SDL_Color &c = maximizeHovered ? iconCol : iconColDim;
            SDL_SetRenderDrawColor(app.ren, c.r, c.g, c.b, 255);
            if (!maximized) {
                // Single square.
                SDL_Rect sq{ cx - 5, cy - 5, 11, 11 };
                SDL_RenderDrawRect(app.ren, &sq);
                // Bold top edge (title-bar line convention).
                SDL_RenderDrawLine(app.ren, sq.x, sq.y + 1, sq.x + sq.w - 1, sq.y + 1);
            } else {
                // Restore: two overlapping squares (back square offset +2,+2).
                SDL_Rect back { cx - 3, cy - 7,  9, 9 };
                SDL_Rect front{ cx - 7, cy - 3,  9, 9 };
                SDL_RenderDrawRect(app.ren, &back);
                SDL_RenderDrawLine(app.ren, back.x, back.y + 1, back.x + back.w - 1, back.y + 1);
                // Redraw the front square on top.
                SDL_RenderDrawRect(app.ren, &front);
                SDL_RenderDrawLine(app.ren, front.x, front.y + 1, front.x + front.w - 1, front.y + 1);
            }
        }

        // ── Close: × made of two diagonal line pairs (2px thick each) ──
        {
            const int cx   = closeButton.x + closeButton.w / 2;
            const int cy   = closeButton.y + closeButton.h / 2;
            const SDL_Color &c = closeHovered ? iconCol : iconColDim;
            SDL_SetRenderDrawColor(app.ren, c.r, c.g, c.b, 255);
            // Main diagonals.
            SDL_RenderDrawLine(app.ren, cx - 5, cy - 5, cx + 5, cy + 5);
            SDL_RenderDrawLine(app.ren, cx + 5, cy - 5, cx - 5, cy + 5);
            // Thicken by 1px offset.
            SDL_RenderDrawLine(app.ren, cx - 4, cy - 5, cx + 5, cy + 4);
            SDL_RenderDrawLine(app.ren, cx - 5, cy - 4, cx + 4, cy + 5);
            SDL_RenderDrawLine(app.ren, cx + 4, cy - 5, cx - 5, cy + 4);
            SDL_RenderDrawLine(app.ren, cx + 5, cy - 4, cx - 4, cy + 5);
        }
        
        SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);
        SDL_RenderDrawLine(app.ren, x, y + kTitleBarHeight, frameRight, y + kTitleBarHeight);
        SDL_RenderDrawLine(app.ren, x, y, frameRight, y);
        SDL_RenderDrawLine(app.ren, x, y, x, frameBottom);


        SDL_SetRenderDrawColor(app.ren, 192, 192, 192, 255);
        SDL_RenderDrawLine(app.ren, frameRight, y, frameRight, frameBottom);
        SDL_RenderDrawLine(app.ren, x, frameBottom, frameRight, frameBottom);

        SDL_SetRenderDrawColor(app.ren, 192, 192, 192, 255);


        SDL_RenderDrawLine(app.ren, x + 1, y + kTitleBarHeight + 1, frameRight - 1, y + kTitleBarHeight + 1);
        SDL_RenderDrawLine(app.ren, frameRight, y + kTitleBarHeight + 1, frameRight, frameBottom - 1); 

        SDL_SetRenderDrawColor(app.ren, 64, 64, 64, 255);
        SDL_Surface* surface = TTF_RenderText_Blended(app.font, title.c_str(), {255, 255, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(app.ren, surface);
        int textW = 0, textH = 0;
        SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = {x + 25, y + 5, textW, textH};
        SDL_RenderCopy(app.ren, texture, nullptr, &textRect);
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
        int iconWidth = 16;
        int iconHeight = 16;
        int iconX = x + 5;  
        int iconY = y + (kTitleBarHeight - iconHeight) / 2;  
        SDL_Rect iconRect = { iconX, iconY, iconWidth, iconHeight };
        SDL_RenderCopy(app.ren, icon == nullptr ? app.icon : icon, nullptr, &iconRect);
    }

    void Window::show(bool b) {
        if (!b && (minimized || isMinimizing || isRestoring) &&
            originalWidth > 0 && originalHeight > 0) {
            x = originalX;
            y = originalY;
            w = originalWidth;
            h = originalHeight;
            minimized = false;
            isMinimizing = false;
            isRestoring = false;
            minimizeHovered = SDL_FALSE;
            maximizeHovered = SDL_FALSE;
            closeHovered = SDL_FALSE;
            for (auto &c : children) {
                c->setShow(true);
                c->setWindowPos(x, y);
                c->resizeWindow(w, h);
            }
            if (systemBar) {
                if (auto *con = dim) {
                    auto it = std::find(con->mini_win.begin(),
                                        con->mini_win.end(), this);
                    if (it != con->mini_win.end()) con->mini_win.erase(it);
                }
            }
        }
        shown = b;
        stateChanged(false, false, shown);
        if(shown == false) {
            dim->events.setNextVisible();
        }
    }

    void Window::getRect(SDL_Rect &rc) {
        rc.x = x;
        rc.y = y;
        rc.w = w;
        rc.h = h;
    }

    void Window::setRect(const SDL_Rect &rc) {
        x = rc.x;
        y = rc.y;
        w = rc.w;
        h = rc.h;
    }

    int Window::hitTestEdges(int mx, int my) const {
        if (!can_resize || minimized || maximized || !shown) return 0;
        // Only treat points inside the window frame as resize candidates.
        // This prevents nearby UI (desktop icons, menu items) from being
        // interpreted as edge-resize zones of overlapping windows.
        if (mx < x || mx > x + w || my < y || my > y + h) return 0;
        const int b = kResizeBorder;
        SDL_Point p{ mx, my };
        // Corners take priority so diagonal resize works at the four extremes.
        SDL_Rect tl{ x - b,         y - b,         2 * b, 2 * b };
        SDL_Rect tr{ x + w - b,     y - b,         2 * b, 2 * b };
        SDL_Rect bl{ x - b,         y + h - b,     2 * b, 2 * b };
        SDL_Rect br{ x + w - b,     y + h - b,     2 * b, 2 * b };
        if (SDL_PointInRect(&p, &tl)) return 1 | 4;
        if (SDL_PointInRect(&p, &tr)) return 2 | 4;
        if (SDL_PointInRect(&p, &bl)) return 1 | 8;
        if (SDL_PointInRect(&p, &br)) return 2 | 8;
        // Straight edges. The top edge is intentionally a thin strip along the
        // very top of the frame so the rest of the title bar still works as a
        // move-drag handle.
        SDL_Rect topEdge   { x + b,     y - b,     w - 2 * b, 2 * b };
        SDL_Rect bottomEdge{ x + b,     y + h - b, w - 2 * b, 2 * b };
        SDL_Rect leftEdge  { x - b,     y + b,     2 * b,     h - 2 * b };
        SDL_Rect rightEdge { x + w - b, y + b,     2 * b,     h - 2 * b };
        if (SDL_PointInRect(&p, &topEdge))    return 4;
        if (SDL_PointInRect(&p, &bottomEdge)) return 8;
        if (SDL_PointInRect(&p, &leftEdge))   return 1;
        if (SDL_PointInRect(&p, &rightEdge))  return 2;
        return 0;
    }

    void Window::applyEdgeCursor(int edges) const {
        ResizeCursorKind kind;
        switch (edges) {
            case 1: case 2:           kind = kCursorEW;   break;
            case 4: case 8:           kind = kCursorNS;   break;
            case 1|4: case 2|8:       kind = kCursorNWSE; break;
            case 2|4: case 1|8:       kind = kCursorNESW; break;
            default:                  return;
        }
        SDL_Cursor *cur = getResizeCursor(kind);
        if (cur) SDL_SetCursor(cur);
    }

    void Window::removeAtClose(bool value) {
        remove_on = value;
    }

    void Window::toggleMaximize(mxApp &app) {
        if (!canResize() || minimized || isMinimizing || isRestoring) {
            return;
        }
        constexpr Uint32 kMaxToggleDebounceMs = 180;
        const Uint32 now = SDL_GetTicks();
        if (lastMaxToggleTicks_ != 0 && (now - lastMaxToggleTicks_) < kMaxToggleDebounceMs) {
            return;
        }
        lastMaxToggleTicks_ = now;
        // Refresh available workspace size so maximize always uses current bounds.
        dim_w = app.width;
        dim_h = app.height;
        maximize(!maximized);
        shown = true;
    }

   bool Window::event(mxApp &app, SDL_Event &e) {
        if (!can_resize && resizing_) {
            resizing_ = false;
            resizeEdges_ = 0;
            SDL_SetCursor(SDL_GetDefaultCursor());
        }
        
        static Uint32 lastClickTime = 0;   
        if (e.type == SDL_MOUSEMOTION) {
            SDL_Point mousePoint = {e.motion.x, e.motion.y};
            minimizeHovered = SDL_PointInRect(&mousePoint, &minimizeButton);
            closeHovered = SDL_PointInRect(&mousePoint, &closeButton);
            maximizeHovered = (canResize() && SDL_PointInRect(&mousePoint, &maximizeButton)) ? SDL_TRUE : SDL_FALSE;
            if (resizing_) {
                int dx = e.motion.x - resizeMouseX_;
                int dy = e.motion.y - resizeMouseY_;
                int nx = resizeStartX_, ny = resizeStartY_;
                int nw = resizeStartW_, nh = resizeStartH_;
                if (resizeEdges_ & 1) { nx = resizeStartX_ + dx; nw = resizeStartW_ - dx; }
                if (resizeEdges_ & 2) { nw = resizeStartW_ + dx; }
                if (resizeEdges_ & 4) { ny = resizeStartY_ + dy; nh = resizeStartH_ - dy; }
                if (resizeEdges_ & 8) { nh = resizeStartH_ + dy; }
                if (nw < kMinW) {
                    if (resizeEdges_ & 1) nx -= (kMinW - nw);
                    nw = kMinW;
                }
                if (nh < kMinH) {
                    if (resizeEdges_ & 4) ny -= (kMinH - nh);
                    nh = kMinH;
                }
                // Clamp to screen so the user can't drag the window offscreen.
                // Top edge stays below the menu bar (y >= kTitleBarHeight+1); bottom stays above
                // the system bar (handled via dim_h - 50, mirroring maximize()).
                const int screenW = dim_w > 0 ? dim_w : nw;
                const int screenH = dim_h > 0 ? dim_h : nh;
                const int topLimit = kTitleBarHeight + 1;
                const int bottomLimit = screenH - 50;
                if (resizeEdges_ & 1) {
                    if (nx < 0) { nw += nx; nx = 0; if (nw < kMinW) nw = kMinW; }
                }
                if (resizeEdges_ & 2) {
                    if (nx + nw > screenW) nw = screenW - nx;
                    if (nw < kMinW) nw = kMinW;
                }
                if (resizeEdges_ & 4) {
                    if (ny < topLimit) { nh += (ny - topLimit); ny = topLimit; if (nh < kMinH) nh = kMinH; }
                }
                if (resizeEdges_ & 8) {
                    if (ny + nh > bottomLimit) nh = bottomLimit - ny;
                    if (nh < kMinH) nh = kMinH;
                }
                x = nx; y = ny; w = nw; h = nh;
                for (auto &c : children) {
                    if (c->show) {
                        c->setWindowPos(x, y);
                        c->resizeWindow(w, h);
                    }
                }
                return true;
            }
            if (dragging) {
                    if (e.motion.y > 0) {
                        x = e.motion.x - dragOffsetX;
                        y = e.motion.y - dragOffsetY;
                        // Clamp so the title bar never slides under the menu bar.
                        const int topLimit = kTitleBarHeight + 1;
                        if (y < topLimit) y = topLimit;
                        return true;
                    }
            }
            if (can_resize && !dragging && shown && !minimized && !maximized) {
                int edges = hitTestEdges(e.motion.x, e.motion.y);
                // Use cursor_handled (reset each motion event by Dimension) so
                // we react to *this* frame's prior dispatch (e.g. menu hover)
                // instead of last frame's stale draw-time cursor_shown.
                if (edges && !cursor_handled) {
                    applyEdgeCursor(edges);
                    cursor_handled = true;
                } else if (isPointInside(e.motion.x, e.motion.y) && !cursor_handled) {
                    // Cursor is over the window but not on an edge; restore
                    // the default arrow so it doesn't stay stuck as a resize
                    // cursor after leaving an edge.
                    SDL_SetCursor(SDL_GetDefaultCursor());
                    cursor_handled = true;
                }
            }
        } 
        
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            SDL_Point mousePoint = {e.button.x, e.button.y};
            SDL_Rect titleBarRect = {x, y, w, kTitleBarHeight};
            const bool inTitleBar = SDL_PointInRect(&mousePoint, &titleBarRect);
            const bool inMinButton = SDL_PointInRect(&mousePoint, &minimizeButton);
            const bool inMaxButton = canResize() && SDL_PointInRect(&mousePoint, &maximizeButton);
            const bool inCloseButton = SDL_PointInRect(&mousePoint, &closeButton);
            const bool inWindowButtons = inMinButton || inMaxButton || inCloseButton;

            // Edge-drag resize: takes priority over title-bar drag so corners that
            // overlap the title bar still resize instead of moving.
            // Do not steal clicks from the window control buttons.
            if (!inWindowButtons) {
                int edges = hitTestEdges(e.button.x, e.button.y);
                if (edges) {
                    resizing_ = true;
                    resizeEdges_ = edges;
                    resizeStartX_ = x; resizeStartY_ = y;
                    resizeStartW_ = w; resizeStartH_ = h;
                    resizeMouseX_ = e.button.x; resizeMouseY_ = e.button.y;
                    return true;
                }
            }

            if (inTitleBar && !inWindowButtons && e.button.clicks >= 2) {
                toggleMaximize(app);
                dragging = false;
                return true;
            }

            if (inTitleBar && !inWindowButtons) {
                dragging = true;
                dragOffsetX = e.button.x - x;
                dragOffsetY = e.button.y - y;
            }

            if (inCloseButton) {
                show(false);
                if(remove_on) { 
                    destroyWindow();
                    return true;
                }
            }

            if (inMinButton) {
                if (!minimized) {
                    minimize(true);
                    if (systemBar) systemBar->addMinimizedWindow(this);
                    minimizeHovered = SDL_FALSE;
                } else {
                    systemBar->restoreWindow(this);
                    if (systemBar) systemBar->restoreWindow(this);
                    minimizeHovered = SDL_FALSE;
                }
            }
                
            if (inMaxButton) {
                toggleMaximize(app);
                return true;
            }
        } 
        
        
        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            dragging = false;
            if (resizing_) {
                resizing_ = false;
                resizeEdges_ = 0;
                return true;
            }
        }

        if (!shown || minimized) {
            
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                Uint32 currentTime = SDL_GetTicks();  
                SDL_Point mousePoint = {e.button.x, e.button.y};
                SDL_Rect titleBarRect = {x, y, w, kTitleBarHeight};  
                if (SDL_PointInRect(&mousePoint, &titleBarRect)) {
                    if (currentTime - lastClickTime < 500) {  
                        if (minimized) {
                            minimize(false);  
                            if (systemBar) systemBar->restoreWindow(this);
                            return true;
                        }
                    }       
                    lastClickTime = currentTime;
                }
            }
            return false;
        }

        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_LEAVE) {
                minimizeHovered = SDL_FALSE;
                closeHovered = SDL_FALSE;
                maximizeHovered = SDL_FALSE;
            }
        }

        for (auto it = children.rbegin(); it != children.rend(); ++it) {
            if ((*it)->event(app, e)) return true;
        }
     
      
        return false;
    }

    void Window::setSystemBar(SystemBar *s) {
        systemBar = s;
    }

    Control *Window::getControl() {
        if(children.size()>0)
            return children[children.size()-1].get();
        mx::system_err << "MasterX System: Trying to acess out of bounds control.\n";
        return nullptr;
    }
    Control *Window::getControl(int index) {
        if(index >= 0 && index < static_cast<int>(children.size()))
            return children[index].get();
        mx::system_err << "MasterX System: Trying to acess out of bounds control.\n";
        return nullptr;
    }

    void Window::minimize(bool m) {

        if(!systemBar) {
            mx::system_err << "MasterX: Window " << this->title << " missing system bar link\n";
            return;
        }

        if (m) {
           if (!minimized) {  
                originalX = x;
                originalY = y;
                originalWidth = w;
                originalHeight = h;
            }
            isMinimizing = true;
            minimized = true;
            minTargetX = 5;  
            minTargetY = systemBar->yPos - 50;
            minTargetW = 200;  
            minTargetH = 5; 
            if(isVisible()) {  
                for (auto &c : children) {
                    c->setShow(false);  
                }
            }
            w = 200;
            h = 25;
            
        } else if (!m && minimized) {
            minimized = false;
            isRestoring = true;
            restoreFrame = 0;
            restoreStartX = x;
            restoreStartY = y;
            restoreStartW = w;
            restoreStartH = h;
            restoreTargetX = originalX;
            restoreTargetY = originalY;
            restoreTargetW = originalWidth;
            restoreTargetH = originalHeight;
            shown = true;
            dragging = false;
            if(isVisible()) {
                for (auto &c : children) {
                    c->setShow(true);  
                }
            }
        }
        stateChanged(m, false, false);
    }

    void Window::maximize(bool m) {
        if (m && !canResize()) {
            return;
        }
        bool changed = false;
        if (m && !maximized) {
            oldX = x;
            oldY = y;
            oldW = w;
            oldH = h;
            x = 0;
            y = kTitleBarHeight + 1;
            w = (dim_w > 0) ? dim_w : w;
            h = (dim_h > 0) ? (dim_h - (kTitleBarHeight + 1 + 50)) : h;
            dragging = false;
            changed = true;
        } else if (!m && maximized) {
            x = oldX;
            y = oldY;
            w = oldW;
            h = oldH;
            dragging = false;
            changed = true;
        }
        maximized = m;
        if (changed) {
            for (auto &c : children) {
                if (c->show) {
                    c->setWindowPos(x, y);
                    c->resizeWindow(w, h);
                }
            }
        }
        stateChanged(false, maximized, false);
    }

    bool Window::reload() const {
        return reload_window;
    }
    
    void Window::setReload(bool r) {
        reload_window = r;
    }

   void Window::setCanResize(bool r) {
        can_resize = r;
       if (!can_resize) {
          resizing_ = false;
          resizeEdges_ = 0;
          maximizeHovered = SDL_FALSE;
       }
   }
    
    bool Window::canResize() const {
        return can_resize;
    }
}