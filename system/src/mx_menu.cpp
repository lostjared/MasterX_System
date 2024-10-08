#include "mx_menu.hpp"
#include "mx_window.hpp"

namespace mx {

        Menu_Header create_header(const std::string &text) {
            Menu_Header header;
            header.enabled = true;
            header.text = text;
            return header;
        }
        Menu_Item<menuCallback>   create_menu_item(const std::string &text, menuCallback callback) {
            Menu_Item<menuCallback> item;
            item.callback = callback;
            item.text = text;
            item.enabled = true;
            item.icon = -1;
            return item;
        }


        Menu_Header::Menu_Header() {
            visible = false;
         }
        
        Menu_Header::~Menu_Header() {

        }

        void Menu::setWindowPos(int w, int h) {

        }

        Menu::Menu(mxApp &app, Window *w) : win{w} {
            Menu_Header header;
            header.enabled = true;
            header.text = "Window";
            header.window_menu =true;
            Menu_ID id = addHeader(header);
            Menu_Item<menuCallback> item_;
            item_.item_rect = {0};
            item_.callback = [](mxApp &app, Window *win, SDL_Event &e) -> bool {
                win->show(false);
                return true;
            };
            item_.text = "Quit";
            addItem(id, addIcon(loadTexture(app, "images/xicon.png")), item_);
            hide();
        }
        int Menu::addIcon(SDL_Texture *tex) {
            icons.push_back(tex);
            return icons.size()-1;
        }

        void Menu::hide() {
            menu_active = false;
            for(auto &i : menu) {
                i.visible = false;
                for (auto &mi: i.items) {
                    mi.underline = false;
                }
            }
        }

        void Menu::draw(mxApp &app) {
            TTF_SetFontStyle(app.font, TTF_STYLE_BOLD);
            SDL_Rect rc = {0, 0, app.width, 30};
            SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 200);
            SDL_RenderFillRect(app.ren, &rc);
            SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
            SDL_Rect rci = {10,7,20,20};
            SDL_RenderCopy(app.ren, win->icon != nullptr ? win->icon : app.icon,nullptr,&rci);
            int padding = 25;
            int x = 15 + padding; 
            int y = 7;  
            for (auto &header : menu) {
                SDL_Surface* headerSurface = TTF_RenderText_Blended(app.font, header.window_menu ? win->title.c_str() : header.text.c_str(), {0, 0, 0, 255});
                SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(app.ren, headerSurface);
                SDL_QueryTexture(headerTexture, NULL, NULL, &header.header_rect.w, &header.header_rect.h);
                header.header_rect.x = x;
                header.header_rect.y = y;
                if(header.is_messagebox == false) {
                    SDL_RenderCopy(app.ren, headerTexture, NULL, &header.header_rect);
                    header.text_rect = header.header_rect;
                }
                SDL_FreeSurface(headerSurface);
                SDL_DestroyTexture(headerTexture);
                header.header_rect.x -= 5;
                x += header.header_rect.w + padding;
                
                
                if (header.visible) {
                    int itemY = 30;  
                    for (auto &item : header.items) {
                        SDL_Color col;
                        if(item.underline) {
                            TTF_SetFontStyle(app.font,TTF_STYLE_UNDERLINE | TTF_STYLE_BOLD);
                            cursor_shown = true;
                            col = {0,0,255,255};
                        } else {
                            col = {0,0,0,255};
                            TTF_SetFontStyle(app.font, TTF_STYLE_BOLD);
                        }
                        
                        SDL_Surface* itemSurface = TTF_RenderText_Blended(app.font, item.text.c_str(), col);
                        SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(app.ren, itemSurface);
                        SDL_QueryTexture(itemTexture, NULL, NULL, &item.item_rect.w, &item.item_rect.h);
                        item.item_rect.x = header.header_rect.x+30;
                        item.item_rect.y = itemY+5;
                        SDL_Rect rcx = {header.header_rect.x, itemY, 175, 30};
                        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
                        SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 200);
                        SDL_RenderFillRect(app.ren, &rcx); 
                        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
                        SDL_RenderCopy(app.ren, itemTexture, NULL, &item.item_rect);
                        TTF_SetFontStyle(app.font, TTF_STYLE_BOLD);
                        SDL_FreeSurface(itemSurface);
                        SDL_DestroyTexture(itemTexture);
                        if(item.icon != -1) {
                            SDL_Rect ico_rect = {header.header_rect.x + 5, itemY + 5, 20, 20};  
                            SDL_RenderCopy(app.ren, icons[item.icon], nullptr, &ico_rect);
                        }

                        itemY += 30; 
                    }
                }
            }
            TTF_SetFontStyle(app.font, TTF_STYLE_NORMAL);
        }

        bool Menu::event(mxApp &app, SDL_Event &e) {
            if (menu_active) {
                if (e.type == SDL_MOUSEMOTION) {
                    SDL_Point p = {e.motion.x, e.motion.y};
                    for (auto &header : menu) {
                        if (SDL_PointInRect(&p, &header.text_rect)) {
                            for (auto &h : menu) { h.visible = false; }
                            header.visible = true;
                            return true;
                        }
                    }
                }

                if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                    SDL_Point p = {e.button.x, e.button.y};
                    bool clicked_outside = true;
                    for (auto &header : menu) {
                        if (header.visible && header.is_messagebox == false) {
                            for (auto &item : header.items) {
                                if (SDL_PointInRect(&p, &item.item_rect)) {
                                    if (item.callback) {
                                        if (item.callback(app, win, e)) {
                                            menu_active = false;
                                            hide();
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                        if (SDL_PointInRect(&p, &header.text_rect)) {
                            clicked_outside = false;
                        }
                    }
                    if (clicked_outside) {
                        menu_active = false;
                        hide();
                    }
                }
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                SDL_Point p = {e.button.x, e.button.y};
                for (auto &header : menu) {
                    if (SDL_PointInRect(&p, &header.text_rect)) {
                        menu_active = true;
                        for (auto &h : menu) { h.visible = false; }
                        header.visible = true;
                        return true;
                    }
                }
            }

            if (e.type == SDL_MOUSEMOTION) {
                SDL_Point p = {e.motion.x, e.motion.y};
                for (auto &header : menu) {
                    if (header.visible) {
                        for (auto &item : header.items) {
                            if (SDL_PointInRect(&p, &item.item_rect)) {
                                item.underline = true;
                            } else {
                                item.underline = false;
                            }
                        }
                    }
                }
            }
            return false;
    }


    Menu_ID Menu::addHeader(const Menu_Header &h) {
        menu.push_back(h);
        return menu.size()-1;
    }
    
    Menu_ID Menu::addItem(int header, int id, const Menu_Item<menuCallback> &i) {
        menu[header].items.push_back(i);
        menu[header].items.back().icon = id;
        return menu[header].items.size()-1;
    }
    
    
    
}