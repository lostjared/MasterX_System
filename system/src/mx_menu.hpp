#ifndef __MX_MENU_H___
#define __MX_MENU_H___


#include "mx_controls.hpp"

namespace mx {

    using menuCallback = std::function<bool(mxApp &, Window *, SDL_Event &)>;

    
    template<typename F>
    struct Menu_Item {
        std::string text;
        SDL_Rect item_rect;
        F callback;
        bool visible;
        bool enabled;
        bool underline = false;
        SDL_Texture *icon = nullptr;
        Menu_Item() : text{}, callback{nullptr}, visible{false}, enabled{true}, icon{nullptr} {}
        ~Menu_Item() {
            if(icon != nullptr) {
                    SDL_DestroyTexture(icon);
                    icon = nullptr;
            }
        }
    };



    struct Menu_Header {
        std::string text;
        SDL_Rect header_rect;
        SDL_Rect text_rect;
        bool enabled = true;
        bool visible = false;
        bool window_menu = false;
        Menu_Header();
        ~Menu_Header();
        std::vector<Menu_Item<menuCallback>> items;
    };

    Menu_Header create_header(const std::string &text);
    Menu_Item<menuCallback>   create_menu_item(const std::string &text, menuCallback callback);
    
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
        Menu_ID addItem(Menu_ID header, const Menu_Item<menuCallback>  &i);
        Window *win;
        bool underline = false, menu_active = false;
        void hide();
    };

}


#endif