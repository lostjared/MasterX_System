#ifndef __WINDOW_H__
#define __WINDOW_H__

#include"SDL.h"
#include"SDL_ttf.h"

#include<string>
#include<iostream>
#include<memory>
#include<vector>
#include<optional>
#include"tee_stream.hpp"
#include"cfg.hpp"


extern std::string getPath(const std::string &name);
extern std::optional<std::string> get_current_directory();
enum { ID_LOAD=0, ID_DIM };

namespace mx {

    struct mxApp {
        SDL_Window *win;
        SDL_Renderer *ren;
        SDL_Texture *tex;
        bool init_ = false;
        bool active = false;
        mxApp() = default;
        ~mxApp();
        bool init(const std::string &name, int w, int h);
        void release();
        int width = 0, height = 0;
        TTF_Font *font = 0;
        SDL_Texture *icon = 0;
        SDL_Texture* convertToStreamingTexture(SDL_Texture* originalTexture);
        void set_fullscreen(SDL_Window* window, bool fullscreen);
        void shutdown();
        bool full = false;
        void font_printText(TTF_Font *fnt, int x, int y, const std::string &text,  const SDL_Color col);
        void font_printText_Solid(TTF_Font *fnt, int x, int y, const std::string &text, const SDL_Color col);
        void printText(int x, int y, const std::string &text,  const SDL_Color col);
        void printText_Solid(int x, int y, const std::string &text, const SDL_Color col);
        void resize(int w, int h);
        TTF_Font *loadFont(const std::string &font, const int size);
        ConfigFile config;
        std::string version;
        std::string system_font;
        std::string term_font;
        std::string matrix_font;
        TTF_Font *matrix_font_;
    };

    class Screen {
    public:
        Screen() = default;
        virtual ~Screen() = default;
        virtual void draw(mx::mxApp &app) = 0;
        virtual bool event(mxApp &app, SDL_Event &e) = 0;
    };
}

extern void setScreen(int screen);
extern SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name);
extern SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name, int &w, int &h, bool key, SDL_Color color);
extern SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name, int &w, int &h);
#endif