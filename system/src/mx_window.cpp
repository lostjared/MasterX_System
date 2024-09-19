#include"mx_window.hpp"
#include"mx_abstract_control.hpp"
#include"mx_system_bar.hpp"
#include"dimension.hpp"
#include"mx_menu.hpp"

namespace mx {

    extern bool cursor_shown;

    Window::Window(mxApp &app) : x{0}, y{0}, w{320}, h{240}, shown{false}, minimizeHovered(SDL_FALSE), closeHovered(SDL_FALSE), maximizeHovered(SDL_FALSE), title{"windwow"}, menu{app, this} {
        dim_w = app.width;
        dim_h = app.height; 
        setCanResize(false);
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
            }
                x = restoreTargetX;
                y = restoreTargetY;
                w = restoreTargetW;
                h = restoreTargetH;
                isRestoring = false; 
                dragging = false;
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
            if (dragging) {
                    if (e.motion.y > 0) {
                        x = e.motion.x - dragOffsetX;
                        y = e.motion.y - dragOffsetY;
                        isMinimizing = false;
                        isRestoring = false;
                        return true;
                    }
            }          
        } 
        
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            SDL_Point mousePoint = {e.button.x, e.button.y};
            SDL_Rect titleBarRect = {x, y, w, 30};

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
        if (m && !maximized) {
            oldX = x;
            oldY = y;
            oldW = w;
            oldH = h;
            x = 0;
            y = 26;
            w = dim_w;
            h = dim_h-76;
        } else if (!m && maximized) {
            x = oldX;
            y = oldY;
            w = oldW;
            h = oldH;
        }
        maximized = m;
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