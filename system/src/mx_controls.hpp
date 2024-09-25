#ifndef __MX_CONTROLS__H_
#define __MX_CONTROLS__H_

#include "window.hpp"
#include<optional>
#include"mx_abstract_control.hpp"

namespace mx {

    extern bool cursor_shown;

    class Label : public Control {
    public:
        Label(mxApp &app);
        virtual ~Label();
        virtual void draw(mxApp &app) override;
        virtual void setWindowPos(int xx, int yy) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        void create(Window *parent, const std::string &text, SDL_Color col, int x, int y);
        void create_multi(Window *parent, std::vector<std::string>  &t, SDL_Color col, int xx,  int yy);
        void loadFont(const std::string &name, int size);
        void setText(const std::string  &text, SDL_Color color);
        void setMultiLine(std::vector<std::string> &v);
        void setGeometry(int xx, int yy);
        void linkMode(bool m);
    private:
        TTF_Font *font_ = 0;
        std::string name_;
        std::string text_;
        int x = 0,y = 0,size_ = 0;
        SDL_Color color_ = {0};
        int wx = 0, wy = 0;
        int w = 0, h = 0;
        bool mode = false;
        bool under_ = false;
        bool multi_lined = false;
        std::vector<std::string> multi_text;
    };

    

    class Button : public Control {
    public:
        Button(mxApp &app);
        virtual ~Button();
        virtual void draw(mxApp &app) override;
        virtual void setWindowPos(int xx, int yy) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void resizeWindow(int w, int h) override;
        void create(Window *parent, const std::string &text, int x, int y, int w, int h);
        void setText(const std::string &t);
        void setGeometry(int xx, int yy, int ww, int hh);
     private:
        std::string text;
        int x = 0, y = 0, w = 0, h = 0;
        int wx = 0, wy = 0;
        bool hover = false;
        bool pressed = false;
        SDL_Color fgColor = {0};
        SDL_Color bgColor = {0};
        SDL_Color hover_fg = {0};
        SDL_Color hover_bg = {0};
        SDL_Color pressed_fg = {0};
        SDL_Color pressed_bg = {0};
        SDL_Color textColor = {0};
    };

    class Image : public Control {
    public:
        Image(mxApp &app);
        virtual ~Image();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event  &e) override;
        virtual void setWindowPos(int x, int y) override;
        virtual void resizeWindow(int w, int h) override;
        void create(mxApp &app, Window *parent, const std::string &path, int x, int y);
        void setGeometry(int x, int y, int w, int h);
        void setSourceRect(int x, int y, int w, int h);
        void getRect(SDL_Rect &rc);
    private:
        int x = 0, y = 0, w = 0, h = 0;
        SDL_Texture *image;
        int sw = 0, sh = 0;
        int wx = 0, wy = 0;
        bool use_rect = false;
        SDL_Rect src = {0};
    };
}


#endif