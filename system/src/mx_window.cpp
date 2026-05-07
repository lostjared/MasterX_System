#include"mx_window.hpp"
#include"mx_abstract_control.hpp"
#include"mx_system_bar.hpp"
#include"dimension.hpp"
#include"mx_menu.hpp"
#include"loadpng.hpp"

namespace mx {

    extern bool cursor_shown;

    namespace {
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
      restoreAnimationStep(5),
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
        rc.x += 4;
        rc.w -= 7;
        rc.y += 31;
        rc.h -= 35;
    }

    void Window::setIcon(SDL_Texture *icon) {
        this->icon = icon;
    }

    void Window::destroyWindow() {
        dim->destroyWindow(this);
    }

    bool Window::isPointInside(int xx, int yy) {
        return (xx >= x && xx <= x + w &&
                yy >= y && yy <= y + h);
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
            
            x += (restoreTargetX - x) / restoreAnimationStep;
            y += (restoreTargetY - y) / restoreAnimationStep;
            w += (restoreTargetW - w) / restoreAnimationStep;
            h += (restoreTargetH - h) / restoreAnimationStep;

            if (abs(x - restoreTargetX) < 1 && abs(y - restoreTargetY) < 1 && 
                abs(w - restoreTargetW) < 1 && abs(h - restoreTargetH) < 1) {
                x = restoreTargetX;
                y = restoreTargetY;
                w = restoreTargetW;
                h = restoreTargetH;
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

        SDL_SetRenderDrawColor(app.ren, 205, 205, 205, 255);
        SDL_RenderFillRect(app.ren, &rc);

        int startGray = 165;
        int endGray = 205;
        for (int i = 0; i < h; ++i) {
            int grayValue = startGray + (endGray - startGray) * i / h;
            SDL_SetRenderDrawColor(app.ren, grayValue, grayValue, grayValue, 255);
            SDL_RenderDrawLine(app.ren, x, y + i, x + w, y + i);
        }
        drawMenubar(app);

        for (auto &c : children) {
            c->setWindowPos(x, y);
            c->draw(app);
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
        int titleBarHeight = 30;

        for (int i = 0; i < titleBarHeight; ++i) {
            int red = lightBlue.r + (darkBlue.r - lightBlue.r) * i / titleBarHeight;
            int green = lightBlue.g + (darkBlue.g - lightBlue.g) * i / titleBarHeight;
            int blue = lightBlue.b + (darkBlue.b - lightBlue.b) * i / titleBarHeight;
            SDL_SetRenderDrawColor(app.ren, red, green, blue, 255);
            SDL_RenderDrawLine(app.ren, x, y + i, x + w, y + i);
        }

        int buttonSize = 19;
        int buttonPadding = 5;

        minimizeButton = {x + w - 3 * (buttonSize + buttonPadding), y + 5, buttonSize, buttonSize};
        maximizeButton = {x + w - 2 * (buttonSize + buttonPadding), y + 5, buttonSize, buttonSize};
        closeButton = {x + w - (buttonSize + buttonPadding), y + 5, buttonSize, buttonSize};

        
        auto drawButtonWithBevel = [&](SDL_Rect button, bool hovered) {
            for (int i = 0; i < button.h; ++i) {
                int gray = 200 - i * 4; 
                SDL_SetRenderDrawColor(app.ren, gray, gray, gray, 255);
                SDL_RenderDrawLine(app.ren, button.x, button.y + i, button.x + button.w, button.y + i);
            }
            SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255); 
            SDL_RenderDrawLine(app.ren, button.x, button.y, button.x + button.w - 1, button.y);       
            SDL_RenderDrawLine(app.ren, button.x, button.y, button.x, button.y + button.h - 1);       
            SDL_SetRenderDrawColor(app.ren, 100, 100, 100, 255); 
            SDL_RenderDrawLine(app.ren, button.x, button.y + button.h - 1, button.x + button.w - 1, button.y + button.h - 1); 
            SDL_RenderDrawLine(app.ren, button.x + button.w - 1, button.y, button.x + button.w - 1, button.y + button.h - 1); 
            if (hovered) {
                SDL_SetRenderDrawColor(app.ren, 255, 0, 0, 255); 
                SDL_RenderDrawRect(app.ren, &button);
            }
        };

        drawButtonWithBevel(minimizeButton, minimizeHovered);
        drawButtonWithBevel(maximizeButton, maximizeHovered);
        drawButtonWithBevel(closeButton, closeHovered);
        SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);  
        int padding = 4; 
        SDL_RenderDrawLine(app.ren, maximizeButton.x + padding - 1, maximizeButton.y + padding, maximizeButton.x + buttonSize - padding, maximizeButton.y + padding);
        SDL_RenderDrawLine(app.ren, maximizeButton.x + padding - 1, maximizeButton.y + padding, maximizeButton.x + padding - 1, maximizeButton.y + buttonSize - padding);
        SDL_RenderDrawLine(app.ren, maximizeButton.x + padding - 1, maximizeButton.y + buttonSize - padding, maximizeButton.x + buttonSize - padding, maximizeButton.y + buttonSize - padding);
        SDL_RenderDrawLine(app.ren, maximizeButton.x + buttonSize - padding, maximizeButton.y + padding, maximizeButton.x + buttonSize - padding, maximizeButton.y + buttonSize - padding);

        SDL_Surface* minimizeSurface = TTF_RenderText_Blended(app.font, "_", {255, 255, 255});
        SDL_Texture* minimizeTexture = SDL_CreateTextureFromSurface(app.ren, minimizeSurface);
        int minimizeTextW = 0, minimizeTextH = 0;
        SDL_QueryTexture(minimizeTexture, nullptr, nullptr, &minimizeTextW, &minimizeTextH);
        SDL_Rect minimizeTextRect = {minimizeButton.x + (buttonSize - minimizeTextW) / 2, minimizeButton.y + (buttonSize - minimizeTextH) / 2, minimizeTextW, minimizeTextH};
        SDL_RenderCopy(app.ren, minimizeTexture, nullptr, &minimizeTextRect);
        SDL_DestroyTexture(minimizeTexture);
        SDL_FreeSurface(minimizeSurface);

        SDL_Surface* closeSurface = TTF_RenderText_Blended(app.font, "X", {255, 255, 255});
        SDL_Texture* closeTexture = SDL_CreateTextureFromSurface(app.ren, closeSurface);
        int closeTextW = 0, closeTextH = 0;
        SDL_QueryTexture(closeTexture, nullptr, nullptr, &closeTextW, &closeTextH);
        SDL_Rect closeTextRect = {closeButton.x + (buttonSize - closeTextW) / 2, closeButton.y + (buttonSize - closeTextH) / 2, closeTextW, closeTextH};
        SDL_RenderCopy(app.ren, closeTexture, nullptr, &closeTextRect);
        SDL_DestroyTexture(closeTexture);
        SDL_FreeSurface(closeSurface);
        
        SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);
        SDL_RenderDrawLine(app.ren, x, y + titleBarHeight, x + w - 1, y + titleBarHeight);
        SDL_RenderDrawLine(app.ren, x, y, x + w - 1, y);
        SDL_RenderDrawLine(app.ren, x, y, x, y + h - 1);


        SDL_SetRenderDrawColor(app.ren, 192, 192, 192, 255);
        SDL_RenderDrawLine(app.ren, x + w , y, x + w , y + h - 1);
        SDL_RenderDrawLine(app.ren, x, y + h - 1, x + w , y + h - 1);

        SDL_SetRenderDrawColor(app.ren, 192, 192, 192, 255);


        SDL_RenderDrawLine(app.ren, x + 1, y + titleBarHeight + 1, x + w - 2, y + titleBarHeight + 1);
        SDL_RenderDrawLine(app.ren, x + w - 1, y + titleBarHeight + 1, x + w - 1, y + h - 2); 

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
        int iconY = y + (titleBarHeight - iconHeight) / 2;  
        SDL_Rect iconRect = { iconX, iconY, iconWidth, iconHeight };
        SDL_RenderCopy(app.ren, icon == nullptr ? app.icon : icon, nullptr, &iconRect);
    }

    void Window::show(bool b) {
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

   bool Window::event(mxApp &app, SDL_Event &e) {
        
        static Uint32 lastClickTime = 0;   
        if (e.type == SDL_MOUSEMOTION) {
            SDL_Point mousePoint = {e.motion.x, e.motion.y};
            minimizeHovered = SDL_PointInRect(&mousePoint, &minimizeButton);
            closeHovered = SDL_PointInRect(&mousePoint, &closeButton);
            maximizeHovered = SDL_PointInRect(&mousePoint, &maximizeButton);
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
                // Top edge stays below the menu bar (y >= 26); bottom stays above
                // the system bar (handled via dim_h - 50, mirroring maximize()).
                const int screenW = dim_w > 0 ? dim_w : nw;
                const int screenH = dim_h > 0 ? dim_h : nh;
                const int topLimit = 26;
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
                        const int topLimit = 26;
                        if (y < topLimit) y = topLimit;
                        isMinimizing = false;
                        isRestoring = false;
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
            SDL_Rect titleBarRect = {x, y, w, 30};

            // Edge-drag resize: takes priority over title-bar drag so corners that
            // overlap the title bar still resize instead of moving.
            int edges = hitTestEdges(e.button.x, e.button.y);
            if (edges) {
                resizing_ = true;
                resizeEdges_ = edges;
                resizeStartX_ = x; resizeStartY_ = y;
                resizeStartW_ = w; resizeStartH_ = h;
                resizeMouseX_ = e.button.x; resizeMouseY_ = e.button.y;
                return true;
            }

            if (SDL_PointInRect(&mousePoint, &titleBarRect) &&
                !SDL_PointInRect(&mousePoint, &minimizeButton) &&
                !SDL_PointInRect(&mousePoint, &closeButton)) {
                dragging = true;
                dragOffsetX = e.button.x - x;
                dragOffsetY = e.button.y - y;
            }

            if (SDL_PointInRect(&mousePoint, &closeButton)) {
                show(false);
                if(remove_on) { 
                    destroyWindow();
                    return true;
                }
            }

            if (SDL_PointInRect(&mousePoint, &minimizeButton)) {
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
                
            if (canResize() == true && minimized == false && SDL_PointInRect(&mousePoint, &maximizeButton)) {

                maximize(!maximized);
                shown = true;
                if (can_resize) {
                    for (auto &c : children) {
                        if(c->show) {
                            c->setWindowPos(x,y);
                            c->resizeWindow(w, h);
                        }
                    }
                }
            }
        } 
        
        
        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            dragging = false;
            isMinimizing = false;
            isRestoring = false;
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
                SDL_Rect titleBarRect = {x, y, w, 30};  
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

        for (auto &c : children) {
            if (c->event(app, e)) return true;
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
            originalX = x;
            originalY = y;  
            originalWidth = w;
            originalHeight = h;
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
        bool changed = false;
        if (m && !maximized) {
            oldX = x;
            oldY = y;
            oldW = w;
            oldH = h;
            x = 0;
            y = 26;
            w = dim_w;
            h = dim_h-76;
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
   }
    
    bool Window::canResize() const {
        return can_resize;
    }
}