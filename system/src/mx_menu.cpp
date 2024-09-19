#include "mx_menu.hpp"

namespace mx {


        Menu_Item::Menu_Item() : text{}, callback{nullptr}, visible{false}, enabled{true}, icon{nullptr} {}
        Menu_Item::~Menu_Item() {
            if(icon != nullptr) {
                SDL_DestroyTexture(icon);
            }
        }

        void Menu::draw(mxApp &app) {

        }

        bool Menu::event(mxApp &app, SDL_Event &e) {


            return false;
        }
}