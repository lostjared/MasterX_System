#ifndef __MX_MENU_H___
#define __MX_MENU_H___


#include "mx_controls.hpp"

namespace mx {


    using menuCallback = std::function<bool(mxApp &, Window *, SDL_Event &)>;

    struct Menu_Item {
        std::string text;
        SDL_Rect item_rect;
        menuCallback callback;
        bool visible;
        bool enabled;
        SDL_Texture *icon;
        Menu_Item();
        ~Menu_Item();
    };

    struct Menu_Header {
        std::string text;
        SDL_Rect header_rect;
        bool enabled = true;
        Menu_Header();
        ~Menu_Header();
        std::vector<Menu_Item> items;
    };

    class Menu : public Control {
    public:
        std::vector<Menu_Header> menu;
        virtual ~Menu() = default;
        virtual void draw(mxApp &) override;
        virtual bool event(mxApp &, SDL_Event &) override;
    };

}


#endif