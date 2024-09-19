#include "mx_menu.hpp"

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

        Menu::Menu(mxApp &app) {

        }



        void Menu::draw(mxApp &app) {
            int x = 10; 
            int y = 0;  
            int padding = 10;

            for (auto &header : menu) {
                if (header.enabled) {
                    
                    SDL_Surface* headerSurface = TTF_RenderText_Blended(app.font, header.text.c_str(), {255, 255, 255, 255});
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
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                
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