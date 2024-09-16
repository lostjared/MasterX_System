/* 

    MasterX System written by Jared Bruni
    (C) 2024 LostSideDead Software
    https://lostsidedead.biz

*/

#include"SDL.h"
#include<iostream>
#include"window.hpp"
#include"splash.hpp"
#include"dimension.hpp"
#include<vector>
#include<memory>
#include"argz.hpp"
#include<limits.h>

#ifdef _WIN32
#include<windows.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif


std::vector<std::unique_ptr<mx::Screen>> *screens = nullptr;
int cur_screen = 0;
mx::mxApp *p_app = nullptr;

void setScreen(int scr) {
    if(screens != nullptr && scr >= 0 && scr < static_cast<int>(screens->size()))
        cur_screen = scr;
    else {
        std::cerr << "Error screen out of bounds\n";
    }
}

void draw(mx::mxApp &app) {
    SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
    SDL_RenderClear(app.ren);

    SDL_SetRenderTarget(app.ren, app.tex);
    SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
    SDL_RenderClear(app.ren);

    (*screens)[cur_screen]->draw(app);

    SDL_RenderCopy(app.ren, app.tex, nullptr, nullptr);
    SDL_RenderPresent(app.ren);
}

 void eventProc() {
    static SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)) {
            p_app->active = false;
        }
        if(screens != nullptr && p_app != nullptr)
            (*screens)[cur_screen]->event(*p_app, e);
    }
    draw(*p_app);
}

void init(mx::mxApp &app) {
    if(screens != nullptr) {
        screens->push_back(std::make_unique<mx::Splash>(app));
        screens->push_back(std::make_unique<mx::Dimension>(app));
    }
}

#ifdef FOR_WASM
std::string cur_path = "/assets";
#else
std::string cur_path = "assets";
#endif

std::string getPath(const std::string &name) {
    return cur_path + "/" + name;
}

std::optional<std::string> get_current_directory() {
 #ifdef _WIN32
    char cwd[MAX_PATH];
    if (GetCurrentDirectoryA(sizeof(cwd), cwd)) {
        return std::string(cwd);
    } else {
        return std::nullopt;
    }
#elif !defined(FOR_WASM) && !defined(__APPLE__)
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        return std::string(cwd);
    } else {
        return std::nullopt;
    }
 #endif
    return std::nullopt;
}

SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name) {
    int w, h;
    return loadTexture(app, name, w, h);
}

extern SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name, int &w, int &h) {
    SDL_Surface *surf = SDL_LoadBMP(getPath(name).c_str());
    if(!surf) {
        std::cerr << "Error loading surface; " << getPath(name) << "\n";
        std::cerr.flush();
        exit(EXIT_FAILURE);
    }
    w = surf->w;
    h = surf->h;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(app.ren, surf);
    if(!tex) {
        std::cerr << "Error creating texture from surface: " << name << "\n";
        std::cerr.flush();
        exit(EXIT_FAILURE);
    }
    return tex;
}

void quit() {
    std::cout << "MasterX System: Exiting...\n";
    SDL_ShowCursor(SDL_TRUE);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char **argv) {
    Argz<std::string> argz(argc, argv);
    argz.addOptionSingleValue('p', "path to assets").addOptionDoubleValue('P', "path", "path to assets").addOptionSingleValue('v', "info").addOptionSingle('h', "info").addOptionSingle('f', "set fullscreen").addOptionDouble('F',"fullscreen", "set fullscreen");
    std::string path;
    bool full = false;
    int value = 0;
    Argument<std::string> arg;
    try {
        while((value = argz.proc(arg)) != -1) {
            switch(value) {
                case 'h':
                case 'v':
                    argz.help(std::cout);
                    exit(EXIT_SUCCESS);
                    break;
                case 'p':
                case 'P':
                    path = arg.arg_value;
                    break;
                case 'f':
                case 'F':
                    full = true;
                    break;
            }
        }
    } catch(const ArgException<std::string> &e) {
        std::cerr << "Syntax Error: " << e.text() << "\n";
    }

    if(path.length()>0) {
        std::cout << "MasterX System: path set to: " << path << "\n";
        cur_path = path;
    } else {
    #ifndef FOR_WASM
        argz.help(std::cout);
        std::cout.flush();
        exit(EXIT_FAILURE);
    #endif
    }

    mx::mxApp app;  
    p_app = &app;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if(TTF_Init() < 0) {
        std::cerr << "Error initializing SDL_ttf: " << TTF_GetError() << "\n";
        return 1;
    }

    atexit(quit);

    SDL_ShowCursor(SDL_FALSE);

    std::vector<std::unique_ptr<mx::Screen>> screen_obj;
    screens = &screen_obj;
    if(!app.init("MasterX", 1280, 720)) {
        std::cerr.flush();
        std::cout.flush();
        exit(EXIT_FAILURE);
        return 1;
    }
    init(app);

    if(full) {
        app.set_fullscreen(app.win, true);
    }

    app.active = true;

#ifndef FOR_WASM
    while(app.active == true) {
        eventProc();
    }
#else
    emscripten_set_main_loop(eventProc, 0, 1);
#endif
    return 0;

 
}
