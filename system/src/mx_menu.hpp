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
        bool underline = false;
        SDL_Texture *icon;
        Menu_Item();
        ~Menu_Item();
    };

    struct Menu_Header {
        std::string text;
        SDL_Rect header_rect;
        bool enabled = true;
        bool visible = false;
        Menu_Header();
        ~Menu_Header();
        std::vector<Menu_Item> items;
    };
    
    using Menu_ID = int;

    class Menu : public Control {
    public:
        std::vector<Menu_Header> menu;
        Menu(mxApp &app, Window *w);
        virtual ~Menu() {}
        virtual void draw(mxApp &) override;
        virtual bool event(mxApp &, SDL_Event &) override;
        virtual void setWindowPos(int w, int h) override;
        Menu_ID addHeader(const Menu_Header &h);
        Menu_ID addItem(Menu_ID header, const Menu_Item  &i);
        Window *win;
        bool win_visible = false, underline = false;
        void hide();
    };

}


#endif