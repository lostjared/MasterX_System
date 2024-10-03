#include"mx_system_bar.hpp"
#include"window.hpp"
#include"dimension.hpp"
#include"mx_window.hpp"

namespace mx {

    SystemBar::SystemBar(mxApp &app) {
        dimensions = nullptr;
        font = TTF_OpenFont(getPath(app.system_font).c_str(), 14);
        if (!font) {
            mx::system_err << "Error opening font: " << getPath(app.system_font) << "\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        objects.push_back(std::make_unique<MenuBar>(app));
        menu = dynamic_cast<MenuBar *>(objects[0].get());
        if(!menu) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        menu->menuOpen = false;
        animationComplete = false;
        yPos = 0;
        prev_dim = 0;
        cur_dim = 0;
    }

    SystemBar::~SystemBar() {
        if(font != nullptr)
            TTF_CloseFont(font);
    }

    void SystemBar::addMinimizedWindow(Window *window) {
          DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](cur_dim).get());
          if(con != nullptr) {
            con->mini_win.push_back(window);
            mx::system_out << "MasterX System: Minimized Window.\n";
          } else {
            mx::system_err << "MasterX System: Error: Wrong Type\n";
          }
    }

    void SystemBar::restoreWindow(Window *window) {
        if (window->minimized) {
            window->isMinimizing = false;  
            window->restoreTargetX = window->originalX; 
            window->restoreTargetY = window->originalY; 
            window->restoreTargetW = window->originalWidth; 
            window->restoreTargetH = window->originalHeight; 
            window->restoreAnimationStep = 10; 
            window->isRestoring = true;
            window->minimize(false);
            DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](cur_dim).get());
            if(con != nullptr) {
                auto w = std::find(con->mini_win.begin(), con->mini_win.end(), window);
                if(w != con->mini_win.end()) {
                    con->mini_win.erase(w);
                }
            }
        }
    }
    
    void SystemBar::drawMinimizedMenu(mxApp &app, int button_x, int button_y, int button_width) {
        if (!showMinimizedMenu) {
            return;
        }

        DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](cur_dim).get());
        if (!con) return;

        SDL_Color white = {0xBD, 0, 0, 255};
        SDL_Color darkGray = {160, 160, 160, 255};
        SDL_Color lightGray = {240, 240, 240, 255};
        SDL_Color gradStart = {255, 255, 255, 255};
        SDL_Color gradEnd = {180, 180, 180, 255};   

        int menu_width = 150;
        int menu_height = 20 + static_cast<int>(con->mini_win.size()) * 30;

        int menu_x = button_x + (button_width - menu_width) / 2;
        int menu_y = button_y - menu_height - 5;
    
        for (int y = 0; y < menu_height; ++y) {
            float factor = static_cast<float>(y) / menu_height;
            SDL_Color color;
            color.r = gradStart.r + static_cast<Uint8>(factor * (gradEnd.r - gradStart.r));
            color.g = gradStart.g + static_cast<Uint8>(factor * (gradEnd.g - gradStart.g));
            color.b = gradStart.b + static_cast<Uint8>(factor * (gradEnd.b - gradStart.b));
            color.a = 255;

            SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b, color.a);
            SDL_RenderDrawLine(app.ren, menu_x, menu_y + y, menu_x + menu_width, menu_y + y);
        }

        SDL_SetRenderDrawColor(app.ren, lightGray.r, lightGray.g, lightGray.b, 255);
        SDL_RenderDrawLine(app.ren, menu_x, menu_y, menu_x + menu_width, menu_y); // Top border
        SDL_RenderDrawLine(app.ren, menu_x, menu_y, menu_x, menu_y + menu_height); // Left border

        SDL_SetRenderDrawColor(app.ren, darkGray.r, darkGray.g, darkGray.b, 255);
        SDL_RenderDrawLine(app.ren, menu_x, menu_y + menu_height, menu_x + menu_width, menu_y + menu_height); // Bottom border
        SDL_RenderDrawLine(app.ren, menu_x + menu_width, menu_y, menu_x + menu_width, menu_y + menu_height); // Right border

      
        int yOffset = 20;
        for (size_t i = 0; i < con->mini_win.size(); ++i) {
            Window *win = con->mini_win[i];
            if(menuHover == true && hoverIndex == static_cast<int>(i))
                TTF_SetFontStyle(font, TTF_STYLE_UNDERLINE);
            SDL_Surface *textSurface = TTF_RenderText_Blended(font, win->title.c_str(), white);
            TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
            SDL_Texture *textTexture = SDL_CreateTextureFromSurface(app.ren, textSurface);
            int text_width = textSurface->w;
            int text_height = textSurface->h;
            SDL_FreeSurface(textSurface);
            SDL_Rect textRect = {menu_x + 30, menu_y + yOffset, text_width, text_height};
            SDL_Rect icoRect = {menu_x + 5, menu_y +yOffset, 16 ,16 }; 
            SDL_RenderCopy(app.ren, textTexture, nullptr, &textRect);
            SDL_RenderCopy(app.ren, win->icon == nullptr ? app.icon : win->icon, nullptr, &icoRect);
            SDL_DestroyTexture(textTexture);
            yOffset += 30;
        }
    }



    void SystemBar::setDimensions(std::vector<std::unique_ptr<Screen>> *dim) {
        dimensions = dim;
    }

    void SystemBar::activateDimension(int dim) {
        auto it = std::find(activeDimensionsStack.begin(), activeDimensionsStack.end(), dim);;
        if (it != activeDimensionsStack.end()) {
            activeDimensionsStack.erase(it);
        }
        activeDimensionsStack.push_back(dim);  
    }

    void SystemBar::deactivateDimension(int dim) {
        auto it = std::find(activeDimensionsStack.begin(), activeDimensionsStack.end(), dim);
        if (it != activeDimensionsStack.end()) {
            activeDimensionsStack.erase(it);
        }
    }
 
    void SystemBar::drawDimensions(mxApp &app) {

         int button_height = 0;
         int bar_height = 0;

                if (dimensions != nullptr && animationComplete == true) {
                    int activeIndex = 0;  
                    bool tcursor = false;
                    int button_width = 150;  
                    int button_spacing = 10; 
                    int arrow_width = 15;    

            
                    int available_width = app.width - (arrow_width * 2) - (button_spacing * 2);
                    int max_buttons = available_width / (button_width + button_spacing);

                    for (size_t j = bar_offset; j < bar_offset+max_buttons && j < activeDimensionsStack.size(); ++j) {
                        
                    int i = activeDimensionsStack[j];
                    DimensionContainer *dim = dynamic_cast<DimensionContainer *>(dimensions->operator[](i).get());
                
                    if (dim != nullptr && dim->isActive()) { 

                        const std::string &name = dim->name;
                        SDL_Color white = {255, 255, 255, 255};
                        SDL_Surface* textSurface = TTF_RenderText_Blended(font, name.c_str(), white);
                        if (textSurface == nullptr) {
                            continue;
                        }

                        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(app.ren, textSurface);
                        int text_width = textSurface->w;
                        int text_height = textSurface->h;
                        SDL_FreeSurface(textSurface);
                        if (textTexture == nullptr) {
                            continue;
                        }

                        button_width = 150; 
                        button_height = text_height + 20;
                        bar_height = 50;
                        int button_y = (bar_height - button_height) / 2 + app.height - bar_height; 
                        
                        int button_x = 10 + activeIndex * (button_width + 10);  
                        activeIndex++;  

                        SDL_Color gradStart, gradEnd;

                        if (static_cast<int>(i) == cur_dim) {
                            gradStart = {0, 0, 255, 255};  
                            gradEnd = {0, 0, 139, 255};    
                        } else {
                            gradStart = {192, 192, 192, 255}; 
                            gradEnd = {128, 128, 128, 255};   
                        }

                        SDL_Rect buttonRect = {button_x, button_y, button_width, button_height};

                        for (int y = buttonRect.y; y < buttonRect.y + buttonRect.h; ++y) {
                            float factor = static_cast<float>(y - buttonRect.y) / buttonRect.h;
                            SDL_Color color;
                            color.r = gradStart.r + static_cast<Uint8>(factor * (gradEnd.r - gradStart.r));
                            color.g = gradStart.g + static_cast<Uint8>(factor * (gradEnd.g - gradStart.g));
                            color.b = gradStart.b + static_cast<Uint8>(factor * (gradEnd.b - gradStart.b));
                            color.a = 255;

                            SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b, color.a);
                            SDL_RenderDrawLine(app.ren, buttonRect.x, y, buttonRect.x + buttonRect.w, y);
                        }

                        SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);
                        SDL_RenderDrawRect(app.ren, &buttonRect);

                        SDL_Rect textRect = {button_x + 30, button_y + (button_height - text_height) / 2, text_width, text_height};
                        SDL_RenderCopy(app.ren, textTexture, nullptr, &textRect);
                        SDL_DestroyTexture(textTexture);
                        textRect.x -= 24;
                        textRect.y -= 2;
                        textRect.w = 20;
                        textRect.h = 20;
                        SDL_RenderCopy(app.ren, dim->icon == nullptr ? app.icon : dim->icon, nullptr, &textRect);
                        if (i == cur_dim && dim->mini_win.size() > 0 && showMinimizedMenu) {
                            drawMinimizedMenu(app, button_x, button_y, button_width);
                        }

                        if(dim->name != "Dashboard") {
                            int square_size = 20;
                            int square_x = button_x + button_width - square_size - 5;
                            int square_y = button_y + (button_height - square_size) / 2;

                            SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 255);
                            SDL_Rect closeButtonRect = {square_x, square_y, square_size, square_size};
                            if (dim->hoveringX) {
                                SDL_SetRenderDrawColor(app.ren, 0xBD, 0, 0, 255);  
                                tcursor = true;
                            } 
                            SDL_RenderFillRect(app.ren, &closeButtonRect);

                            SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);
                            SDL_RenderDrawRect(app.ren, &closeButtonRect);

                            SDL_Color black = {0, 0, 0, 255};
                            SDL_Surface* xSurface = TTF_RenderText_Blended(font, "X", black);
                            if (xSurface != nullptr) {
                                SDL_Texture* xTexture = SDL_CreateTextureFromSurface(app.ren, xSurface);
                                int x_width = xSurface->w;
                                int x_height = xSurface->h;
                                SDL_FreeSurface(xSurface);

                                SDL_Rect xRect = {square_x + (square_size - x_width) / 2, square_y + (square_size - x_height) / 2, x_width, x_height};
                                SDL_RenderCopy(app.ren, xTexture, nullptr, &xRect);
                                SDL_DestroyTexture(xTexture);
                            }
                        }
                    }
                }
            
            if (activeIndex > 0) {
                int arrow_width = 15; 
                int arrow_height = 30; 
                int arrow_y = app.height - bar_height + (bar_height - arrow_height) / 2;
                int left_arrow_x = 10 + activeIndex * (button_width + 10);
                drawArrow(app.ren, left_arrow_x, arrow_y, arrow_width, arrow_height, "left");
                int right_arrow_x = left_arrow_x + arrow_width + 10;
                drawArrow(app.ren, right_arrow_x, arrow_y, arrow_width, arrow_height, "right");
            }
            if(tcursor == true) {
                cursor_shown = true;
            } 
        }
    }

    void SystemBar::drawArrow(SDL_Renderer* ren, int x, int y, int width, int height, const std::string& direction) {
        SDL_Color gradStart = {192, 192, 192, 255};  
        SDL_Color gradEnd = {128, 128, 128, 255};   
        for (int py = y; py < y + height; ++py) {
            float factor = static_cast<float>(py - y) / height;
            SDL_Color color = {
                static_cast<Uint8>(gradStart.r+(factor * (gradEnd.r - gradStart.r))),
                static_cast<Uint8>(gradStart.g+(factor * (gradEnd.g - gradStart.g))),
                static_cast<Uint8>(gradStart.b+(factor * (gradEnd.b - gradStart.b))),
                255
            };
            SDL_SetRenderDrawColor(ren, color.r, color.g, color.b, color.a);

            if (direction == "left") {
                SDL_RenderDrawLine(ren, x + width, py, x + (py - y) / 2, py);
            } else if (direction == "right") {
                SDL_RenderDrawLine(ren, x, py, x + width - (py - y) / 2, py);
            }
        }
        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_Rect arrowRect = {x, y, width, height};
        SDL_RenderDrawRect(ren, &arrowRect);
        if(direction == "left")
            arrowRectLeft = arrowRect;
        else
            arrowRectRight = arrowRect;
    }

    void SystemBar::setCurrentDimension(int dim) {
        if(dim != cur_dim) {
            DimensionContainer *old, *setv;
            old = dynamic_cast<DimensionContainer *>(dimensions->operator[](cur_dim).get());
            if(!old) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            setv = dynamic_cast<DimensionContainer *>(dimensions->operator[](dim).get());
            if(!setv) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            prev_dim = cur_dim;
            cur_dim = dim;
            if (setv) {
                setv->setActive(true);  
                setv->setVisible(true);
                mx::system_out << "MasterX System: Switching from: " << old->name << " to " << setv->name << "\n";
                setv->startTransition(old ? old->wallpaper : nullptr);  
            }
        }    
    }

    int  SystemBar::getCurrentDimension() const {
            return cur_dim;
    }

    void SystemBar::draw(mxApp &app) {
        int barHeight = 50;
        int windowWidth = app.width;
        int windowHeight = app.height;
        int targetYPos = windowHeight - barHeight;
        int startButtonSize = 100;
       static Uint32 lastTime = SDL_GetTicks();



    if (!animationComplete) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 deltaTime = currentTime - lastTime;   
        float speed = 0.8f;
        yPos += speed * deltaTime;
        if (yPos >= targetYPos) {
            yPos = targetYPos;

            animationComplete = true;
            DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](cur_dim).get());
            if(!con) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            con->setVisible(true);
        }
        lastTime = currentTime;
    }

        SDL_Color gradBarStart = {240, 240, 240, 255}; 
        SDL_Color gradBarEnd = {200, 200, 200, 255};   
        SDL_Rect barRect = {0, animationComplete == true ? app.height- barHeight : yPos, windowWidth, barHeight};
        for (int y = 0; y < barHeight; ++y) {
            float factor = static_cast<float>(y) / barHeight;
            SDL_Color color;
            color.r = gradBarStart.r + static_cast<Uint8>(factor * (gradBarEnd.r - gradBarStart.r));
            color.g = gradBarStart.g + static_cast<Uint8>(factor * (gradBarEnd.g - gradBarStart.g));
            color.b = gradBarStart.b + static_cast<Uint8>(factor * (gradBarEnd.b - gradBarStart.b));
            color.a = 255;

            SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b, color.a);
            SDL_RenderDrawLine(app.ren, barRect.x, barRect.y + y, barRect.x + barRect.w, barRect.y + y);
        }

        SDL_Color lightBevel = {255, 255, 255, 255}; 
        SDL_Color darkBevel = {128, 128, 128, 255};  

        
        SDL_SetRenderDrawColor(app.ren, lightBevel.r, lightBevel.g, lightBevel.b, lightBevel.a);
        SDL_RenderDrawLine(app.ren, barRect.x, barRect.y, barRect.x + barRect.w, barRect.y); 
        SDL_RenderDrawLine(app.ren, barRect.x, barRect.y, barRect.x, barRect.y + barRect.h); 

        
        SDL_SetRenderDrawColor(app.ren, darkBevel.r, darkBevel.g, darkBevel.b, darkBevel.a);
        SDL_RenderDrawLine(app.ren, barRect.x, barRect.y + barRect.h - 1, barRect.x + barRect.w - 1, barRect.y + barRect.h - 1); 
        SDL_RenderDrawLine(app.ren, barRect.x + barRect.w - 1, barRect.y, barRect.x + barRect.w - 1, barRect.y + barRect.h - 1); 
                        
        SDL_Color buttonColor = {169, 169, 169, 255};
        SDL_Color textColor = {0, 0, 0, 255};       

        if (isHovering) {
            buttonColor = {0, 0, 139, 255}; 
            textColor = {255, 255, 255, 255}; 
            cursor_shown = true;
        }
        SDL_Rect startButton = {windowWidth - startButtonSize, animationComplete == true ? app.height - barHeight : yPos, startButtonSize, barHeight};
        
        if (isHovering) {
            SDL_Color gradStart = {0, 0, 255, 255}; 
            SDL_Color gradEnd = {0, 0, 139, 255};   

            for (int y = 0; y < startButton.h; ++y) {

                float factor = static_cast<float>(y) / startButton.h;
                SDL_Color color;
                color.r = gradStart.r + static_cast<Uint8>(factor * (gradEnd.r - gradStart.r));
                color.g = gradStart.g + static_cast<Uint8>(factor * (gradEnd.g - gradStart.g));
                color.b = gradStart.b + static_cast<Uint8>(factor * (gradEnd.b - gradStart.b));
                color.a = 255;

                SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b, color.a);
                SDL_RenderDrawLine(app.ren, startButton.x, startButton.y + y, startButton.x + startButton.w, startButton.y + y);
            }
        } else {
            SDL_Color gradStart = {192, 192, 192, 255}; 
            SDL_Color gradEnd = {128, 128, 128, 255};   

            for (int y = 0; y < startButton.h; ++y) {
                float factor = static_cast<float>(y) / startButton.h;
                SDL_Color color;
                color.r = gradStart.r + static_cast<Uint8>(factor * (gradEnd.r - gradStart.r));
                color.g = gradStart.g + static_cast<Uint8>(factor * (gradEnd.g - gradStart.g));
                color.b = gradStart.b + static_cast<Uint8>(factor * (gradEnd.b - gradStart.b));
                color.a = 255;

                SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b, color.a);
                SDL_RenderDrawLine(app.ren, startButton.x, startButton.y + y, startButton.x + startButton.w, startButton.y + y);
            }
        }

        
        SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);  
        SDL_RenderDrawLine(app.ren, startButton.x, startButton.y, startButton.x + startButton.w - 1, startButton.y);  
        SDL_RenderDrawLine(app.ren, startButton.x, startButton.y, startButton.x, startButton.y + startButton.h - 1);  

        SDL_SetRenderDrawColor(app.ren, 128, 128, 128, 255);  
        SDL_RenderDrawLine(app.ren, startButton.x, startButton.y + startButton.h - 1, startButton.x + startButton.w - 1, startButton.y + startButton.h - 1);  
        SDL_RenderDrawLine(app.ren, startButton.x + startButton.w - 1, startButton.y, startButton.x + startButton.w - 1, startButton.y + startButton.h - 1);  

        
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, "Launch", textColor);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(app.ren, textSurface);

        int textWidth = textSurface->w;
        int textHeight = textSurface->h;
        SDL_FreeSurface(textSurface);

        SDL_Rect textRect = {
            startButton.x + (startButton.w - textWidth) / 2,
            startButton.y + (startButton.h - textHeight) / 2,
            textWidth,
            textHeight
        };

        SDL_RenderCopy(app.ren, textTexture, nullptr, &textRect);
        SDL_DestroyTexture(textTexture);

        
        drawDimensions(app);
        SDL_SetRenderTarget(app.ren, nullptr);


        for (auto &i : objects) {
            i->draw(app);
        }

        if(menuHover) {
            cursor_shown = true;
        }
        
    }

    bool SystemBar::empty() const {
        return activeDimensionsStack.empty();
    }

    bool SystemBar::event(mxApp &app, SDL_Event &e) {


        if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            SDL_Point p = {e.button.x, e.button.y};
            if(SDL_PointInRect(&p, &arrowRectLeft)) {
                if(bar_offset > 0) {
                    bar_offset--;
                }
                return true;
            }
            if(SDL_PointInRect(&p, &arrowRectRight)) {
                int button_width = 150;  
                int button_spacing = 10; 
                int arrow_width = 15;            
                int available_width = app.width - (arrow_width * 2) - (button_spacing * 2);
                int max_buttons = available_width / (button_width + button_spacing);

                if ((bar_offset + max_buttons) < activeDimensionsStack.size()) {
                    bar_offset++;  
                }
                return true;
            }
        }
        
        DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](cur_dim).get());
        if (!con) {
            return false;  
        }

        int dpos = 0;

        for(auto &i : activeDimensionsStack) {
            if(i == cur_dim) break;
            dpos ++;
        }

        
       


        int button_width = 150;  
        int button_y = app.height - 50;  
        int button_x = 10 + dpos * (button_width + 10);  
        int menu_width = 150;
        int menu_height = 20 + static_cast<int>(con->mini_win.size()) * 30;  
        int menu_x = button_x + (button_width - menu_width) / 2;
        int menu_y = button_y - menu_height - 5;  
        bool chover = false;
        hoverIndex = -1;

        if (showMinimizedMenu && e.type == SDL_MOUSEMOTION) {
            int mouseX = e.motion.x;  
            int mouseY = e.motion.y;  
            SDL_Rect menuRect = {menu_x, menu_y, menu_width, menu_height};
            SDL_Point rc = {mouseX, mouseY};
            if (SDL_PointInRect(&rc, &menuRect)) {            
                int yOffset = 20;
                for (size_t i = 0; i < con->mini_win.size(); ++i) {
                    SDL_Rect textRect = {menu_x + 10, menu_y + yOffset, 180, 20};  
                    if (SDL_PointInRect(&rc, &textRect)) {
                       chover = true;
                       hoverIndex = static_cast<int>(i);
                       break;
                    }
                    yOffset += 30;  
                }
            }
        }
        
        menuHover = chover;

        if (showMinimizedMenu && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;  
            int mouseY = e.button.y;  
            SDL_Rect menuRect = {menu_x, menu_y, menu_width, menu_height};
            SDL_Point rc = {mouseX, mouseY};
            if (SDL_PointInRect(&rc, &menuRect)) {            
                int yOffset = 20;
                for (size_t i = 0; i < con->mini_win.size(); ++i) {
                    SDL_Rect textRect = {menu_x + 10, menu_y + yOffset, 180, 20};  
                    if (SDL_PointInRect(&rc, &textRect)) {
                        restoreWindow(con->mini_win[i]);  
                        mx::system_out << "MasterX System: Restored Window\n";
                        showMinimizedMenu = false;
                        return true;
                    }
                    yOffset += 30;  
                }
            } else {
                showMinimizedMenu = false;
            }
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_RIGHT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;

            int button_width = 150;
            int button_height = 30;
            int button_y = app.height - 50;

            for (int j = 0; j < static_cast<int>(activeDimensionsStack.size()); ++j) {
               int button_x = 10 + j * (button_width + 10);
                SDL_Rect buttonRect = {button_x, button_y, button_width, button_height};
                SDL_Point mousePoint = {mouseX, mouseY};

                if (SDL_PointInRect(&mousePoint, &buttonRect)) {
                    if (holdingDimension) {
                        Uint32 holdTime = SDL_GetTicks() - holdStartTime;
                        if (holdTime > 500) {
                            showMinimizedMenu = true;
                            holdingDimension = false;
                        }
                    } else {
                        showMinimizedMenu = !showMinimizedMenu;
                    }
                    break;
                }
            }
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            bool clickedOutside = true;

            int button_width = 150;
            int button_height = 30;
            int button_y = app.height - 50;

            for (int j = 0; j < static_cast<int>(activeDimensionsStack.size()); ++j) {
                int button_x = 10 + j * (button_width + 10);
                SDL_Rect buttonRect = {button_x, button_y, button_width, button_height};
                SDL_Point mousePoint = {mouseX, mouseY};

                if (SDL_PointInRect(&mousePoint, &buttonRect)) {
                    clickedOutside = false;
                    break;
                }
            }

            SDL_Point mousePoint = {mouseX, mouseY};
            SDL_Rect menuRect = {menu_x, menu_y, menu_width, menu_height};
            if (SDL_PointInRect(&mousePoint, &menuRect)) {
                clickedOutside = false;
            }

            if (clickedOutside) {
                showMinimizedMenu = false;
            }
        }
        
        if (e.type == SDL_MOUSEMOTION) {  
            
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            int barHeight = 50;
            int button_height = 30;  
            int button_y = app.height - barHeight;
            int button_width = 150;
            int square_size = 20;  
            int activeIndex = 0;  

            if (dimensions != nullptr) {
                    for (size_t j = 0; j < activeDimensionsStack.size(); ++j) {
                        int i = activeDimensionsStack[j];
                        DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](i).get());
                        if(!con) {
                            mx::system_err << "MasterX System: Bad cast..\n";
                            mx::system_err.flush();
                            exit(EXIT_FAILURE);
                        }
                        if (con->isActive()) {
                            int button_x = 10 + activeIndex * (button_width + 10);
                            activeIndex++;
                            SDL_Rect closeButtonRect = {button_x + button_width - square_size - 5, button_y + (button_height - square_size) / 2, square_size, square_size};
                            SDL_Point cur_point { mouseX, mouseY };
                        if (con->name != "Dashboard" && SDL_PointInRect(&cur_point, &closeButtonRect)) {
                            con->hoveringX = true; 
                        } else {
                            con->hoveringX = false;
                        }
                    }
                }
            }
        }

        if (e.type == SDL_MOUSEMOTION) {    
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            int barHeight = 50;
            int startButtonSize = 100;
            int windowWidth = app.width;
            int targetYPos = app.height - barHeight;

            SDL_Rect startButtonRect = {windowWidth - startButtonSize, targetYPos, startButtonSize, barHeight};

            if (mouseX >= startButtonRect.x && mouseX <= (startButtonRect.x + startButtonRect.w) &&
                mouseY >= startButtonRect.y && mouseY <= (startButtonRect.y + startButtonRect.h)) {
                isHovering = true;
            } else {
                isHovering = false;
            }
        } else if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_LEAVE) {
            isHovering = false;
            menuHover = false;
            return true;
        }


        if(e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            if (holdingDimension) {
                Uint32 holdTime = SDL_GetTicks() - holdStartTime;
                
                if (holdTime > 500) { 
                    showMinimizedMenu = true;  
                }

                holdingDimension = false;  
                return true;
            }
        }

            
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            int barHeight = 50;
            int startButtonSize = 100;
            int windowWidth = app.width;
            int targetYPos = app.height - barHeight;

            SDL_Rect startButtonRect = {windowWidth - startButtonSize, targetYPos, startButtonSize, barHeight};
            SDL_Point mousePos = {mouseX, mouseY};
            if (SDL_PointInRect(&mousePos, &startButtonRect)) {
                menu->menuOpen = !menu->menuOpen;
                menu->animating = true;
                if (menu->menuOpen) {
                    menu->currentY = targetYPos - barHeight;
                    menu->targetY = targetYPos - (app.height / 2);
                } else {
                    menu->currentY = targetYPos - (app.height / 2);
                    menu->targetY = targetYPos - barHeight;
                }
                return true;
            }

            if (dimensions != nullptr) {
                int button_width = 150;
                int button_height = 30; 
                int button_y = app.height - barHeight;
                int activeIndex = 0;
                for (size_t j = 0; j < activeDimensionsStack.size(); ++j) {
                    int i = activeDimensionsStack[j];
                    DimensionContainer *con = dynamic_cast<DimensionContainer *>(dimensions->operator[](i).get());
                    if(!con) {
                       mx::system_err << "MasterX System: Bad cast..\n";
                       mx::system_err.flush();
                       exit(EXIT_FAILURE);
                    }
                    if(con->isActive()) {
                        int button_x = 10 + activeIndex * (button_width + 10);
                        activeIndex++;
                        int square_size = 20;
                        SDL_Rect closeButtonRect = {button_x + button_width - square_size - 5, button_y + (button_height - square_size) / 2, square_size, square_size};
                        SDL_Point cur_point = { mouseX, mouseY };
                        if (con->name != "Dashboard" && SDL_PointInRect(&cur_point, &closeButtonRect)) {
                            con->setActive(false);  
                            if(i == cur_dim) {
                                deactivateDimension(i);
                                prev_dim = 0;
                                setCurrentDimension(0); 
                            } else {
                                deactivateDimension(i);
                            }
                            return true;
                        } else {
                            SDL_Rect buttonRect = {button_x, button_y, button_width, button_height};
                            if (mouseX >= buttonRect.x && mouseX <= (buttonRect.x + buttonRect.w) &&
                                mouseY >= buttonRect.y && mouseY <= (buttonRect.y + buttonRect.h)) {
                                if(getCurrentDimension() != i) {
                                    setCurrentDimension(i);  
                                    return true;
                                }
                            } 
                        }
                    }
                }
            }
            int action = menu->itemClicked(app, mouseX, mouseY);
            if(action != -1) {
                performAction(app,action);
                menu->menuOpen = false;
                menu->animating = true;
                menu->targetY = app.height;
                return true;
            } else if (menu->menuOpen) {
                int menuX = app.width - (app.width / 6) - 20;
                int menuY = menu->currentY;
                int menuWidth = app.width / 6;
                int menuHeight = app.height / 2;

                SDL_Rect menuRect = {menuX, menuY, menuWidth, menuHeight};

                if (mouseX >= menuRect.x && mouseX <= (menuRect.x + menuRect.w) &&
                    mouseY >= menuRect.y && mouseY <= (menuRect.y + menuRect.h)) {
                    return false;
                }
                menu->menuOpen = false;
                menu->animating = true;
                menu->targetY = app.height; 
                return true;
            }
        }

        for (auto &i : objects) {
            if (i->event(app, e))
                return true;
        }

        return false;
    }

    void SystemBar::loadDimension(int id) {
        DimensionContainer *dim = dynamic_cast<DimensionContainer *>(dimensions->operator[](id).get());
        if(!dim) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        if(std::find(activeDimensionsStack.begin(), activeDimensionsStack.end(), id) == activeDimensionsStack.end()) {
            dim->setActive(true);
            setCurrentDimension(id);
            activateDimension(id);
        } else {
            dim->setActive(true);
            setCurrentDimension(id);
        }
    }

    void SystemBar::performAction(mxApp &app, int action) {
        switch (action) {
            case 1: {
                     loadDimension(1);
                }
                break;
            case 2: {
                loadDimension(3);
            }
                break;
            case 3: {
                loadDimension(0);
                    
            }
                break;
            case 4:
                loadDimension(4);
                break;
            case 5: {
                loadDimension(5);
            }
                break;
            case 7:
                loadDimension(2);
                break;
            case 6:
                loadDimension(6);
                break;
            case 8:
                mx::system_out << "MasterX System: Shutdown signal sent...\n";
                app.shutdown();
                break;    
            default:
                mx::system_out << "No valid action\n";
                break;
        }
    }

    MenuBar::MenuBar(mxApp &app) {
        menuOpen = false;
        currentY = app.height;  
        targetY = app.height / 2;  
        font = TTF_OpenFont(getPath(app.system_font).c_str(), 14);
        if (!font) {
            mx::system_err << "Error opening font: " << getPath(app.system_font) << "\n";
            exit(EXIT_FAILURE);
        }
        white = { 255,255,255,255 };
        itemTexture = nullptr;
        itemSurface = nullptr;
        animating = false;
        
        
    }

    MenuBar::~MenuBar() {
        TTF_CloseFont(font);
    }

    void MenuBar::draw(mxApp &app) {
        if (!menuOpen && !animating) {
            return;
        }

        if (animating) {
            if (menuOpen) {
                currentY -= 30;
                if (currentY <= targetY) {
                    currentY = targetY;
                    animating = false;
                }
            } else {
                currentY += 30;
                if (currentY >= targetY) {
                    currentY = targetY;
                    animating = false;
                }
            }
        }
        int menuWidth = 1280 / 6;
        int menuHeight = app.height / 2;
        int menuX = app.width - menuWidth - 20;

        SDL_SetRenderTarget(app.ren, app.tex);

        SDL_Color hoverColor = {0, 0, 128, 255};
        SDL_SetRenderDrawColor(app.ren, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
        SDL_Rect rightSideRect = {menuX + menuWidth, currentY, 20, menuHeight};
        SDL_RenderFillRect(app.ren, &rightSideRect);

        int gradientHeight = menuHeight;
        for (int i = 0; i < gradientHeight; i++) {
            int grayValue = 192 - (64 * i / gradientHeight);  
            SDL_SetRenderDrawColor(app.ren, grayValue, grayValue, grayValue, 255);
            SDL_RenderDrawLine(app.ren, menuX, currentY + i, menuX + menuWidth, currentY + i);
        }

        SDL_SetRenderDrawColor(app.ren, 128, 128, 128, 255);
        SDL_Rect menuRect = {menuX, currentY, menuWidth, menuHeight};
        SDL_RenderDrawRect(app.ren, &menuRect);

        SDL_Color black = {0, 0, 0, 255};
        SDL_Color white = {255, 255, 255, 255};

        const char* items[] = {"Welcome", "Terminal", "Settings", "MasterPiece", "Asteroids", "Tetris", "About", "Shutdown"};
        int numItems = sizeof(items) / sizeof(items[0]);
        int itemHeight = 30;
        SDL_Rect itemRect;

        bool tcursor = false;

        for (int i = 0; i < numItems; i++) {
            itemRect = {menuX + 10, currentY + 10 + i * (itemHeight + 5), menuWidth - 20, itemHeight};

            int mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            SDL_Point cursor_pos = {mouseX, mouseY};
            bool isHovering = SDL_PointInRect(&cursor_pos, &itemRect);

            if (isHovering) {
                SDL_SetRenderDrawColor(app.ren, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
                SDL_RenderFillRect(app.ren, &itemRect);
                tcursor = true;
            } else {
                SDL_SetRenderDrawColor(app.ren, 192, 192, 192, 255);
                SDL_RenderFillRect(app.ren, &itemRect);
            }

            SDL_SetRenderDrawColor(app.ren, 128, 128, 128, 255);
            SDL_RenderDrawRect(app.ren, &itemRect);

            SDL_Color textColor = isHovering ? white : black;
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, items[i], textColor);
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(app.ren, textSurface);
            int textWidth = textSurface->w;
            int textHeight = textSurface->h;
            SDL_FreeSurface(textSurface);

            SDL_Rect textRect = {itemRect.x + (itemRect.w - textWidth) / 2, itemRect.y + (itemRect.h - textHeight) / 2, textWidth, textHeight};
            SDL_RenderCopy(app.ren, textTexture, nullptr, &textRect);
            SDL_DestroyTexture(textTexture);
        }

        if(tcursor == true) {
            cursor_shown = true;
        }

        SDL_SetRenderTarget(app.ren, nullptr);
    }
    int MenuBar::itemClicked(mxApp &app, int x, int y) {
        int menuX = app.width - (app.width / 6) - 20; 
        int menuY = currentY;
        int itemHeight = 30;  
        int menuWidth = app.width / 6;  

        SDL_Rect menuItem1 = {menuX, menuY + 10, menuWidth, itemHeight};  
        SDL_Rect menuItem2 = {menuX, menuY + 10 + (itemHeight + 5), menuWidth, itemHeight};  
        SDL_Rect menuItem3 = {menuX, menuY + 10 + 2 * (itemHeight + 5), menuWidth, itemHeight};  
        SDL_Rect menuItem4 = {menuX, menuY + 10 + 3 * (itemHeight + 5), menuWidth, itemHeight};  
        SDL_Rect menuItem5 = {menuX, menuY + 10 + 4 * (itemHeight + 5), menuWidth, itemHeight};  
        SDL_Rect menuItem6 = {menuX, menuY + 10 + 5 * (itemHeight + 5), menuWidth, itemHeight};  
        SDL_Rect menuItem7 = {menuX, menuY + 10 + 6 * (itemHeight + 5), menuWidth, itemHeight};  
        SDL_Rect menuItem8 = {menuX, menuY + 10 + 7 * (itemHeight + 5), menuWidth, itemHeight};
        SDL_Point cursor_pos = {x, y};

        if (SDL_PointInRect(&cursor_pos, &menuItem1)) {
            return 1;  
        } else if (SDL_PointInRect(&cursor_pos, &menuItem2)) {
            return 2;  
        } else if (SDL_PointInRect(&cursor_pos, &menuItem3)) {
            return 3;  
        } else if (SDL_PointInRect(&cursor_pos, &menuItem4)) {
            return 4;  
        } else if (SDL_PointInRect(&cursor_pos, &menuItem5)) {
            return 5;  
        } else if (SDL_PointInRect(&cursor_pos, &menuItem6)) {
            return 6;  
        } else if(SDL_PointInRect(&cursor_pos, &menuItem7)) {
            return 7;
        } else if(SDL_PointInRect(&cursor_pos,&menuItem8)) {
            return 8;
        }
        return -1;  
    }
    
    bool MenuBar::event(mxApp &app, SDL_Event &e) {

        if (e.type == SDL_WINDOWEVENT) {
            if (e.window.event == SDL_WINDOWEVENT_LEAVE) {
                this->animating = false;
                this->menuOpen = true;
                return false;
            }
        }

        return false;
    }

}