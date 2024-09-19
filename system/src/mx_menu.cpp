#include "mx_menu.hpp"
#include "mx_window.hpp"

namespace mx {


        Menu_Item::Menu_Item() : text{}, callback{nullptr}, visible{false}, enabled{true}, icon{nullptr} {}
        Menu_Item::~Menu_Item() {
            if(icon != nullptr) {
                SDL_DestroyTexture(icon);
            }
        }

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
            int padding = 10;
            int x = 105; 
            int y = 5;  
            if(win_visible) {
                SDL_Rect rcw = {0, 25, 100, 25};
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
                if (header.enabled) {
                    
                    SDL_Surface* headerSurface = TTF_RenderText_Blended(app.font, header.text.c_str(), {0, 0, 0, 255});
                    SDL_Texture* headerTexture = SDL_CreateTextureFromSurface(app.ren, headerSurface);
                    SDL_QueryTexture(headerTexture, NULL, NULL, &header.header_rect.w, &header.header_rect.h);
                    header.header_rect.x = x;
                    header.header_rect.y = y;

                    SDL_RenderCopy(app.ren, headerTexture, NULL, &header.header_rect);
                    SDL_FreeSurface(headerSurface);
                    SDL_DestroyTexture(headerTexture);

                    x += header.header_rect.w + padding;

                    
                    if (header.visible) {
                        int itemY = y + header.header_rect.h;  
                        for (auto &item : header.items) {
                            if (item.visible) {
                                SDL_Surface* itemSurface = TTF_RenderText_Blended(app.font, item.text.c_str(), {255, 255, 255, 255});
                                SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(app.ren, itemSurface);
                                SDL_QueryTexture(itemTexture, NULL, NULL, &item.item_rect.w, &item.item_rect.h);
                                item.item_rect.x = header.header_rect.x;
                                item.item_rect.y = itemY;

                                
                                SDL_RenderCopy(app.ren, itemTexture, NULL, &item.item_rect);
                                SDL_FreeSurface(itemSurface);
                                SDL_DestroyTexture(itemTexture);

                                itemY += item.item_rect.h; 
                            }
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
                        for (auto &item : header.items) {
                            item.visible = !item.visible;  
                        }
                        return true;
                    }
                }
   
                for (auto &header : menu) {
                    for (auto &item : header.items) {
                        SDL_Point p = {e.button.x, e.button.y};
                        if (item.visible && SDL_PointInRect(&p, &item.item_rect)) {
                            if (item.callback) {
                                return item.callback(app, nullptr, e);
                            }
                        }
                    }
                }
            }
            else if(e.type == SDL_MOUSEBUTTONUP  && e.button.button == SDL_BUTTON_LEFT) {
                SDL_Rect rc = {0, 25, 100, 25};
                if(SDL_PointInRect(&p, &rc)) {
                    win->show(false);
                    hide();
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
            }

            return false;
        }

    int Menu::addHeader(const Menu_Header &h) {
        menu.push_back(h);
        return menu.size()-1;
    }
    
    int Menu::addItem(int header, const Menu_Item  &i) {
        menu[header].items.push_back(i);
        return menu[header].items.size()-1;
    }
    
    
    
}