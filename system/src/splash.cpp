#include"splash.hpp"
#include"loadpng.hpp"
#include<random>

namespace mx {

    Splash::Splash(mxApp &app) {
        std::vector<std::string> logos = app.config.splitByComma(app.config.itemAtKey("desktop", "backgrounds").value);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, logos.size() - 1);

        int random_index = dis(gen);


        SDL_Surface *temp = mx::LoadPNG(getPath(logos[random_index]).c_str());
        if(!temp) {
            mx::system_err << "Error loading bitmap: " << getPath("images/startup.png") << "\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        bg = SDL_CreateTextureFromSurface(app.ren, temp);
        if(!bg) {
            mx::system_err << "Error creating texture...\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        font = TTF_OpenFont(getPath("fonts/arial.ttf").c_str(), 120);
        if(!font) {
            mx::system_err << "Error could not load font: " << getPath("fonts/arial.ttf") << "\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
    }

    Splash::~Splash() {
        mx::system_out << "MasterX: Releasing Splash..\n";
        SDL_DestroyTexture(bg);
        TTF_CloseFont(font);
    }

   
   void Splash::draw(mxApp &app) {
        SDL_SetRenderTarget(app.ren, app.tex);
        Uint32 elapsed = SDL_GetTicks();
        float progress = (elapsed % 5000) / 5000.0f;

        SDL_Color textColor = {0xBD, 0, 0, 255};
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, "MasterX", textColor);
        if (!textSurface) {
            mx::system_err << "MasterX: Error rendering text: " << TTF_GetError() << "\n";
            return;
        }
        int tw = textSurface->w;
        int th = textSurface->h;
        SDL_Texture *tex1 = SDL_CreateTextureFromSurface(app.ren, textSurface);
        if(!tex1) {
            mx::system_err << "MasterX: Error creating texture..\n";
            return;
        }
        SDL_SetRenderTarget(app.ren, app.tex);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 0);
        SDL_RenderClear(app.ren);
        SDL_RenderCopy(app.ren, bg, nullptr, nullptr);
        int padding = 50;
        int window_width = app.width;
        int available_width = window_width - 2 * padding;
        int bar_width = static_cast<int>(available_width * progress);
        int bar_height = 50;
        int bar_x = padding;
        int bar_y = (app.height - bar_height) / 2;
        SDL_SetRenderDrawColor(app.ren, 200, 200, 200, 255);
        SDL_Rect background = { bar_x, bar_y, available_width, bar_height };
        SDL_RenderFillRect(app.ren, &background);
        SDL_Color startColor = {0, 0, 255, 255}; 
        SDL_Color endColor = {0, 0, 100, 255};   
        for (int i = 0; i < bar_width; ++i) {
            int r = startColor.r + (endColor.r - startColor.r) * i / bar_width;
            int g = startColor.g + (endColor.g - startColor.g) * i / bar_width;
            int b = startColor.b + (endColor.b - startColor.b) * i / bar_width;
            SDL_SetRenderDrawColor(app.ren, r, g, b, 255);
            SDL_RenderDrawLine(app.ren, bar_x + i, bar_y, bar_x + i, bar_y + bar_height);
        }
        int text_x = (app.width - tw) / 2;
        int text_y = bar_y - th - 20;
        SDL_Rect textRect = {text_x, text_y, tw, th};
        SDL_RenderCopy(app.ren, tex1, nullptr, &textRect);
        SDL_SetRenderTarget(app.ren, nullptr);
        SDL_DestroyTexture(tex1);
        SDL_FreeSurface(textSurface);
        if (progress >= 0.95f) {
            setScreen(ID_DIM);
            mx::system_out << "MasterX: Screen Dimension\n";
        }
    }

    bool Splash::event(mxApp &app, SDL_Event &e) { 
        if(e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {
            setScreen(ID_DIM);
            return true;
        } 
        return false;
    }
}