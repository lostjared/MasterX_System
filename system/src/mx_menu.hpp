#ifndef __MX_MENU_H___
#define __MX_MENU_H___


#include "mx_controls.hpp"

namespace mx {

    using menuCallback = std::function<bool(mxApp &, Window *, SDL_Event &)>;

    
    template<typename F>
    struct Menu_Item {
        std::string text;
        SDL_Rect item_rect = {0};
        F callback;
        bool visible;
        bool enabled;
        bool underline = false;
        int icon = -1;
        Menu_Item() : text{}, callback{nullptr}, visible{false}, enabled{true}, icon{-1} {}
        ~Menu_Item() {}
    };



    struct Menu_Header {
        std::string text;
        SDL_Rect header_rect = {0};
        SDL_Rect text_rect = {0};
        bool enabled = true;
        bool visible = false;
        bool window_menu = false;
        bool is_messagebox = false;
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
        virtual ~Menu() {
            for(auto &i : icons) {
                SDL_DestroyTexture(i);
            }
        }
        virtual void draw(mxApp &) override;
        virtual bool event(mxApp &, SDL_Event &) override;
        virtual void setWindowPos(int w, int h) override;
        Menu_ID addHeader(const Menu_Header &h);
        Menu_ID addItem(Menu_ID header, int ico, const Menu_Item<menuCallback>  &i);
        Window *win;
        bool underline = false, menu_active = false;
        void hide();
        std::vector<SDL_Texture *> icons;
        int addIcon(SDL_Texture *tex);
    };

}


#endif