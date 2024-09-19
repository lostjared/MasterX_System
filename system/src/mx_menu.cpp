#include "mx_menu.hpp"
#include "mx_window.hpp"

namespace mx {


       

        Menu_Header::Menu_Header() {

         }
        
        Menu_Header::~Menu_Header() {

        }

        void Menu::setWindowPos(int w, int h) {

        }

        Menu::Menu(mxApp &app, Window *w) : win{w} {

        }

        void Menu::hide() {
            win_visible = false;
            for(auto &i : menu) {
                i.visible = false;
                for(auto &z : i.items) {
                    z.visible = false;
                }
            }
        }

        void Menu::draw(mxApp &app) {

            SDL_Rect rc = {0, 0, app.width, 25};
            SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 185);
            SDL_RenderFillRect(app.ren, &rc);
            SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
            SDL_Color col = {0,0,0,255};
            TTF_SetFontStyle(app.font, TTF_STYLE_BOLD);
            app.printText(5, 5, win->title, col);
            TTF_SetFontStyle(app.font, TTF_STYLE_NORMAL);
            int win_w, win_h;
            TTF_SizeText(app.font, win->title.c_str(), &win_w, &win_h);
            int padding = 25;
            int x = win_w + 5 + padding; 
            int y = 5;  
            if(win_visible) {
                SDL_Rect rcw = {0, 25, 175, 25};
                SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
                SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 185);
                SDL_RenderFillRect(app.ren, &rcw);   
                if(underline) {
                    TTF_SetFontStyle(app.font, TTF_STYLE_UNDERLINE);
                    SDL_Color col = {0,0,255,255};
                    cursor_shown = true;
                    app.printText(rcw.x + 5, 25+5, "Quit", col);
                } else {
                    app.printText(rcw.x + 5, 25+5, "Quit", col);
                }
                TTF_SetFontStyle(app.font, TTF_STYLE_NORMAL);
                SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
            }

            for (auto &header : menu) {
                SDL_Surface* headerSurface = TTF_RenderText_Blended(app.font, header.text.c_str(), {0, 0, 0, 255});
                SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(app.ren, headerSurface);
                SDL_QueryTexture(headerTexture, NULL, NULL, &header.header_rect.w, &header.header_rect.h);
                header.header_rect.x = x;
                header.header_rect.y = y;
                
                SDL_RenderCopy(app.ren, headerTexture, NULL, &header.header_rect);
                SDL_FreeSurface(headerSurface);
                SDL_DestroyTexture(headerTexture);
                header.header_rect.x -= 5;
                x += header.header_rect.w + padding;
                
                
                if (header.visible) {
                    int itemY = 25;  
                    for (auto &item : header.items) {
                        if (item.visible) {
                            SDL_Color col;
                            if(item.underline) {
                                TTF_SetFontStyle(app.font, TTF_STYLE_UNDERLINE);
                                cursor_shown = true;
                                col = {0,0,255,255};
                            } else {
                                col = {0,0,0,255};
                            }
                            SDL_Surface* itemSurface = TTF_RenderText_Blended(app.font, item.text.c_str(), col);
                            TTF_SetFontStyle(app.font, TTF_STYLE_NORMAL);
                            SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(app.ren, itemSurface);
                            SDL_QueryTexture(itemTexture, NULL, NULL, &item.item_rect.w, &item.item_rect.h);
                            item.item_rect.x = header.header_rect.x+5;
                            item.item_rect.y = itemY+5;
                            SDL_Rect rcx = {header.header_rect.x, itemY, 175, 25};
                            SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
                            SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 185);
                            SDL_RenderFillRect(app.ren, &rcx); 
                            SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
                            SDL_RenderCopy(app.ren, itemTexture, NULL, &item.item_rect);
                            SDL_FreeSurface(itemSurface);
                            SDL_DestroyTexture(itemTexture);
                            itemY += 25; 
                        }
                    }
                
                }
            }
           
        }

        bool Menu::event(mxApp &app, SDL_Event &e) {
            SDL_Point p = {e.button.x, e.button.y};
            int fw = 0, fh = 0;
            TTF_SizeText(app.font, win->title.c_str(), &fw, &fh);
            SDL_Rect rc = { 0, 0, 5+fw, 5+fh };
                
            if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                if(SDL_PointInRect(&p,  &rc)) {
                    win_visible = !win_visible;
                    return true;
                }

                for (auto &header : menu) {
                    SDL_Point p = {e.button.x, e.button.y};
                    if (SDL_PointInRect(&p, &header.header_rect)) {
                        header.visible = true;
                        for (auto &item : header.items) {
                            item.visible = true;
                        }
                        return true;
                    }
                }
   
            }
            else if(e.type == SDL_MOUSEBUTTONUP  && e.button.button == SDL_BUTTON_LEFT) {
                SDL_Point p = {e.button.x, e.button.y};
                for (auto &header : menu) {
                    if(header.visible) {
                        for (auto &item : header.items) {
                            if (item.visible && SDL_PointInRect(&p, &item.item_rect)) {
                                if (item.callback) {
                                    if(item.callback(app, win, e)) {
                                        header.visible = false;
                                        return true;
                                    }
                                }
                            }
                        }
                    }
                }
                SDL_Rect rc = {0, 25, 100, 25};
                if(SDL_PointInRect(&p, &rc)) {
                    win->show(false);
                    hide();
                    return true;
                }
            }
            else if(e.type == SDL_MOUSEMOTION) {
                SDL_Rect rc = {0, 25, 100, 25};
                SDL_Point p = {e.motion.x, e.motion.y};
                if(SDL_PointInRect(&p, &rc)) {
                    underline = true;
                } else {
                    underline = false;
                }
                for(auto &h : menu) {
                    for(auto &i : h.items) {
                        if(SDL_PointInRect(&p, &i.item_rect)) {
                            i.underline = true;
                        } else {
                            i.underline = false;
                        }
                    }
                }
            }

            return false;
        }

    int Menu::addHeader(const Menu_Header &h) {
        menu.push_back(h);
        return menu.size()-1;
    }
    
    int Menu::addItem(int header, const Menu_Item<menuCallback> &i) {
        menu[header].items.push_back(i);
        return menu[header].items.size()-1;
    }
    
    
    
}