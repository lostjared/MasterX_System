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
#include"loadpng.hpp"
#ifdef _WIN32
#include<windows.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#if defined(_MSC_VER)
    #if _MSC_VER >= 1930
    #define SAFE_FUNC
    #endif
#endif

std::string curTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    #ifdef SAFE_FUNC
    struct tm localTime;
    time(&currentTime);
    localtime_s(&localTime, &currentTime);
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    #else
    std::tm* localTime = std::localtime(&currentTime);
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    #endif
    
    return oss.str();
}

std::vector<std::unique_ptr<mx::Screen>> *screens = nullptr;
int cur_screen = 0;
mx::mxApp *p_app = nullptr;

void setScreen(int scr) {
    if(screens != nullptr && scr >= 0 && scr < static_cast<int>(screens->size()))
        cur_screen = scr;
    else {
        mx::system_err << "Error screen out of bounds\n";
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

void draw_loading(mx::mxApp &app) {
    SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
    SDL_RenderClear(app.ren);
    SDL_SetRenderTarget(app.ren, app.tex);
    SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
    SDL_RenderClear(app.ren);
    SDL_Color col = {255,255,255,255};
    app.printText(25, 25, "Loading...", col);
    SDL_SetRenderTarget(app.ren, nullptr);
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
        screens->push_back(std::make_unique<mx::Dimension>(*p_app));
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


SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name, int &w, int &h) {
    return loadTexture(app, name, w, h, false, {});
}

SDL_Texture *loadTexture(mx::mxApp &app, const std::string &name, int &w, int &h, bool key, SDL_Color color) {
    SDL_Surface *surf = nullptr;
    if(name.rfind(".bmp") != std::string::npos) {
        surf = SDL_LoadBMP(getPath(name).c_str());
    } else if(name.rfind(".png") != std::string::npos) {
        surf = mx::LoadPNG(getPath(name).c_str());
    }
    if(!surf) {
        mx::system_err << "Error loading surface; " << getPath(name) << "\n";
        mx::system_err.flush();
        exit(EXIT_FAILURE);
    }
    if(key)
        SDL_SetColorKey(surf, SDL_TRUE, SDL_MapRGB(surf->format, color.r,color.g,color.b));
    w = surf->w;
    h = surf->h;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(app.ren, surf);
    if(!tex) {
        mx::system_err << "Error creating texture from surface: " << name << "\n";
        mx::system_err.flush();
        exit(EXIT_FAILURE);
    }
    return tex;
}

void quit() {
    mx::system_out << "MasterX System: Exiting...\n";
    SDL_ShowCursor(SDL_TRUE);
    TTF_Quit();
    SDL_Quit();
}

int main(int argc, char **argv) {
    Argz<std::string> argz(argc, argv);
    argz.addOptionSingleValue('p', "path to assets")
    .addOptionDoubleValue('P', "path", "path to assets")
    .addOptionSingleValue('v', "info")
    .addOptionSingle('h', "info")
    .addOptionSingle('f', "set fullscreen")
    .addOptionDouble('F',"fullscreen", "set fullscreen")
    .addOptionSingleValue('r', "resolution ex 1280x720")
    .addOptionDoubleValue('R', "resolution", "resolution ex: 1280x720");
    
    std::string path;
    bool full = false;
    int value = 0;
    int window_width = 1280, window_height = 720;
    Argument<std::string> arg;
    try {
        while((value = argz.proc(arg)) != -1) {
            switch(value) {
                case 'h':
                case 'v':
                    argz.help(mx::system_out);
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
                case 'R':
                case 'r': {
                    auto pos = arg.arg_value.find("x");
                    if(pos == std::string::npos)  {
                        mx::system_err << "MasterX argument error: use format WitthxHeight\n";
                        mx::system_err.flush();
                        exit(EXIT_FAILURE);
                    }
                    std::string left = arg.arg_value.substr(0, pos);
                    std::string right = arg.arg_value.substr(pos + 1);
                    window_width = atoi(left.c_str());
                    window_height = atoi(right.c_str());
                    mx::system_out << "MasterX System: setting resolution: " << arg.arg_value << "\n";
                }
                    break;
            }
        }
    } catch(const ArgException<std::string> &e) {
        mx::system_err << "Syntax Error: " << e.text() << "\n";
    }

    if(path.length()>0) {
        mx::system_out << "MasterX System: path set to: " << path << "\n";
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
        mx::system_err << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if(TTF_Init() < 0) {
        mx::system_err << "Error initializing SDL_ttf: " << TTF_GetError() << "\n";
        return 1;
    }

    atexit(quit);

    SDL_ShowCursor(SDL_FALSE);

    std::vector<std::unique_ptr<mx::Screen>> screen_obj;
    screens = &screen_obj;
    if(!app.init("MasterX System", window_width, window_height)) {
        mx::system_err.flush();
        mx::system_out.flush();
        exit(EXIT_FAILURE);
        return 1;
    }
    if(full) {
        app.set_fullscreen(app.win, true);
    }
    draw_loading(app);
    init(app);
    app.active = true;
    mx::system_out << "MasterX System: Up and running @ " << curTime() << "\n";

#ifndef FOR_WASM
    while(app.active == true) {
        eventProc();
    }
#else
    emscripten_set_main_loop(eventProc, 0, 1);
#endif
    return 0;

 
}
