#ifndef _DIMENSION_H
#define _DIMENSION_H

#include "window.hpp"
#include<memory>
#include<vector>
#include<string>
#include<iostream>
#include<functional>
#include"mx_event.hpp"
#include<random>
#include"cfg.hpp"


namespace mx {

    extern bool cursor_shown;

     class Window;
     class SystemBar;
     class MenuBar;
     class MasterPiece;
     class Dimension;

    class DimensionContainer : public Screen {
    public:
        friend class Window;
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
        void setIcon(SDL_Texture *icon);
        bool hoveringX = false;
        SDL_Texture *wallpaper, *nextWallpaper, *icon = nullptr;
        EventHandler events;
        void setMatrix(SDL_Texture *t, bool m);
        bool getMatrix() const;
        void setDimension(Dimension *d);
        SDL_Texture *matrix_tex = nullptr;
        Dimension *dim = nullptr;
        SDL_Rect icon_rect = {0};
        bool underline = false;
        SystemBar *system_bar;
    private:
        bool active = false;
        bool visible = false;
        bool transitioning = false;
        int transitionAlpha = 255;
        int transitionSpeed = 3;
        bool matrix_on = false;
             
    };
    
    class Terminal;
    class Label;
    class Button;
    class Image;
    class AsteroidsWindow;
    class TetrisWindow;
    
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
        std::string selectRandomImage(std::vector<std::string>& logos, std::mt19937& gen);
        void drawIconWithText(bool underline, SDL_Renderer* renderer, TTF_Font* font, const std::string &name, SDL_Texture *ico, int x, int y);
        void drawIcons(SDL_Renderer* renderer, TTF_Font* font, SDL_Texture *dicon, int windowW, int windowH);
    private:
        SDL_Texture *wallpaper, *matrix_texture = nullptr;
        std::vector<std::unique_ptr<Screen>> objects;
        std::vector<std::unique_ptr<Screen>> dimensions;
        DimensionContainer *welcome, *dash;
        DimensionContainer *about;
        DimensionContainer *term, *asteroid, *tetris;
        SystemBar *system_bar;
        Window *welcome_window, *welcome_help;
        Window *about_window, *settings_window;
        Button *about_window_ok, *welcome_ok, *toggle_fullscreen, *toggle_matrix;
        Image *welcome_image;
        Label *about_window_info, *welcome_help_info;
        SDL_Texture *hand_cursor, *reg_cursor;
        int cursor_x = 0, cursor_y = 0;
        Terminal *termx;
        DimensionContainer *piece_cont;
        MasterPiece *piece;
        AsteroidsWindow *asteroid_window;
        TetrisWindow *tetris_window;
    };

}

#endif
