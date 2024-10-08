#include"window.hpp"
#include"matrix.hpp"

namespace mx {

    void mxApp::resize(int w, int h) {
      SDL_SetWindowSize(win, w, h);
      SDL_RenderSetLogicalSize(ren, w, h);
      width = w;
      height = h;
      SDL_DestroyTexture(tex);
      tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        if (tex == nullptr) {
            mx::system_err << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            SDL_Quit();
            exit(EXIT_FAILURE);
        }
    }
    
    bool mxApp::init(const std::string &name, int w, int h) {

        config.loadFile(getPath("mx.cfg"));
        version = config.itemAtKey("app", "version").value;
        system_font = config.itemAtKey("fonts", "system").value;
        term_font = config.itemAtKey("fonts", "term").value;
        matrix_font = config.itemAtKey("fonts", "matrix").value;
        
        matrix_font_ = loadFont(matrix_font, 14);

        mx::system_out << "MasterX System v" << version << " Loading .... \n";
        mx::system_out << "(C) 2024 LostSideDead Software\n";
        
        win = SDL_CreateWindow(name.c_str(), 
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        w, h, SDL_WINDOW_SHOWN);
        if (win == nullptr) {
            mx::system_err << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
            SDL_Quit();
            return false;
        }

        ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (ren == nullptr) {
            mx::system_err << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }

        tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        if (tex == nullptr) {
            mx::system_err << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            SDL_Quit();
            return false;
        }
        init_ = true;
        width = w;
        height = h;
        font = TTF_OpenFont(getPath(system_font).c_str(), 14);
        if(!font) {
            mx::system_err << "MasterX System: font: " << getPath(system_font) << " Could not be loaded.\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        icon = loadTexture(*this, "images/xicon.bmp");
        SDL_Surface *ico = SDL_LoadBMP(getPath("images/xicon.bmp").c_str());
        SDL_SetWindowIcon(win, ico);
        SDL_FreeSurface(ico);
        mx::system_out << "MasterX: Initalized System Objects\n";
        return true;
    }

    SDL_Texture* mxApp::convertToStreamingTexture(SDL_Texture* originalTexture) {
        int width, height;
        Uint32 format;
        SDL_QueryTexture(originalTexture, &format, nullptr, &width, &height);
        SDL_Texture* streamingTexture = SDL_CreateTexture(ren, format, SDL_TEXTUREACCESS_STREAMING, width, height);
        if (!streamingTexture) {
            mx::system_err << "Error creating streaming texture: " << SDL_GetError() << "\n";
            return nullptr;
        }
        SDL_SetRenderTarget(ren, streamingTexture);
        SDL_RenderCopy(ren, originalTexture, nullptr, nullptr);
        SDL_SetRenderTarget(ren, nullptr);
        return streamingTexture;
    }

    void mxApp::release() {
        if(init_ == true) {
            releaseMatrix();
            mx::system_out << "MasterX: Releasing System Objects\n";
            SDL_DestroyTexture(icon);
            SDL_DestroyTexture(tex);
            SDL_DestroyRenderer(ren);
            SDL_DestroyWindow(win);
            init_ = false;
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

    void mxApp::font_printText(TTF_Font *font, int x, int y, const std::string &text,  const SDL_Color col) {
        SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), col);
        if(!surf) {
            mx::system_err << "MasterX: System Font Render failed...\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        SDL_Texture *tex_ = SDL_CreateTextureFromSurface(ren, surf);
        if(!tex_) {
            mx::system_err << "MastesrX System: Could not create texture..\n";
            mx::system_err.flush();
            SDL_FreeSurface(surf);
            exit(EXIT_FAILURE);
        }
        SDL_Rect rc = {x, y, surf->w, surf->h};
        SDL_RenderCopy(ren, tex_, nullptr,  &rc);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex_);
    }
    void mxApp::font_printText_Solid(TTF_Font *font, int x, int y, const std::string &text, const SDL_Color col) {
        SDL_Surface *surf = TTF_RenderText_Solid(font, text.c_str(), col);
        if(!surf) {
            mx::system_err << "MasterX: System Font Render failed...\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        SDL_Texture *tex_ = SDL_CreateTextureFromSurface(ren, surf);
        if(!tex_) {
            mx::system_err << "MastesrX System: Could not create texture..\n";
            mx::system_err.flush();
            SDL_FreeSurface(surf);
            exit(EXIT_FAILURE);
        }
        SDL_Rect rc = {x, y, surf->w, surf->h};
        SDL_RenderCopy(ren, tex_, nullptr,  &rc);
        SDL_FreeSurface(surf);
        SDL_DestroyTexture(tex_);
    }
  

    void mxApp::printText(int x, int y, const std::string &text, const SDL_Color col) {
        font_printText(font, x, y, text, col);
    }

    void mxApp::printText_Solid(int x, int y, const std::string &text, const SDL_Color col) {
        font_printText_Solid(font, x, y, text, col);
    }

    TTF_Font *mxApp::loadFont(const std::string &font, const int size) {
        TTF_Font *fnt = TTF_OpenFont(getPath(font).c_str(), size);
        if(!fnt) {
            mx::system_err << "MasterX System: Error loading font..: " << getPath(font) << "\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        return fnt;
    }

    mxApp::~mxApp() {
        release();
        TTF_CloseFont(font);
        TTF_CloseFont(matrix_font_);
    }
}