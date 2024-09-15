#include"window.hpp"

namespace mx {
    
    bool mxApp::init(const std::string &name, int w, int h) {
        win = SDL_CreateWindow(name.c_str(), 
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        w, h, SDL_WINDOW_SHOWN);
        if (win == nullptr) {
            std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (ren == nullptr) {
            std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }

        tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        if (tex == nullptr) {
            std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }
        init_ = true;
        width = w;
        height = h;
        font = TTF_OpenFont(getPath("fonts/arial.ttf").c_str(), 14);
        if(!font) {
            std::cerr << "MasterX System: font: " << getPath("fonts/arial.ttf") << " Could not be loaded.\n";
            std::cerr.flush();
            exit(EXIT_FAILURE);
        }
        icon = loadTexture(*this, "images/icon.bmp");
        SDL_Surface *ico = SDL_LoadBMP(getPath("images/icon.bmp").c_str());
        SDL_SetWindowIcon(win, ico);
        SDL_FreeSurface(ico);
        std::cout << "MasterX: Initalized System Objects\n";
        return true;
    }

    SDL_Texture* mxApp::convertToStreamingTexture(SDL_Texture* originalTexture) {
        int width, height;
        Uint32 format;
        SDL_QueryTexture(originalTexture, &format, nullptr, &width, &height);
        SDL_Texture* streamingTexture = SDL_CreateTexture(ren, format, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (!streamingTexture) {
            std::cerr << "Error creating streaming texture: " << SDL_GetError() << "\n";
            return nullptr;
        }
        SDL_SetRenderTarget(ren, streamingTexture);
        SDL_RenderCopy(ren, originalTexture, nullptr, nullptr);
        SDL_SetRenderTarget(ren, nullptr);
        return streamingTexture;
    }

    void mxApp::release() {
        if(init_ == true) {
            std::cout << "MasterX: Releasing System Objects\n";
            SDL_DestroyTexture(icon);
            SDL_DestroyTexture(tex);
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
        }
    }

    void mxApp::shutdown() {
        active = false;
    }

    void mxApp::set_fullscreen(SDL_Window* window, bool fullscreen) {
        if (fullscreen) {
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        } else {
            SDL_SetWindowFullscreen(window, 0);
        }
        full = fullscreen;
    }

    mxApp::~mxApp() {
        release();
    }
}