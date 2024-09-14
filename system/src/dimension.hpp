#ifndef _DIMENSION_H
#define _DIMENSION_H

#include "window.hpp"
#include<memory>
#include<vector>
#include<string>
#include<iostream>
#include<functional>
#include"mx_event.hpp"

namespace mx {

    extern bool cursor_shown;

     class Window;
     class SystemBar;
     class MenuBar;
    
    class DimensionContainer : public Screen {
    public:
        std::string name = "Default";
        DimensionContainer(mxApp &app);
        virtual ~DimensionContainer();
        void init(SystemBar *sysbar, const std::string &name, SDL_Texture *wallpaperx);
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        std::vector<std::unique_ptr<Screen>> objects;
        std::vector<Window *> mini_win;
        void setActive(bool a);
        bool isActive() const;
        void setVisible(bool v);
        bool isVisible() const;
        void startTransition(SDL_Texture *);
        void updateTransition();
        void destroyWindow(Window *win);
        Window *createWindow(mxApp &app);
        bool hoveringX = false;
        SDL_Texture *wallpaper, *nextWallpaper;
        EventHandler events;
    private:
        SystemBar *system_bar;
        bool active = false;
        bool visible = false;
        bool transitioning = false;
        int currentDimension = 0;
        int nextDimension = -1;
        int transitionAlpha = 255;
        int transitionSpeed = 3;
        
    };
    
    class Terminal;
    class Label;
    class Button;
    class Image;

    class Dimension : public Screen {
    public:
        Dimension(mxApp &app);
        virtual ~Dimension();
        void setCurrentDimension(int dim);
        int  getCurrentDimension() const;
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        void drawDash(mxApp &app);
        Screen *getDimension();
        Screen *getDimension(int index);
    private:
        SDL_Texture *wallpaper;
        std::vector<std::unique_ptr<Screen>> objects;
        std::vector<std::unique_ptr<Screen>> dimensions;
        DimensionContainer *welcome, *dash;
        DimensionContainer *about;
        DimensionContainer *term;
        SystemBar *system_bar;
        Window *welcome_window, *welcome_help;
        Window *about_window, *settings_window;
        Button *about_window_ok, *welcome_ok, *toggle_fullscreen;
        Image *welcome_image;
        Label *about_window_info, *welcome_help_info;
        Image *about_window_logo;
        int current_dim;
        SDL_Texture *hand_cursor, *reg_cursor;
        int cursor_x = 0, cursor_y = 0;
        Terminal *termx;
    };

}

#endif