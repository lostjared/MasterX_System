#include"masterpiece.hpp"

namespace mx {

    MasterPiece::MasterPiece(mxApp &app) : Window(app) {
        load_gfx(app);
   
    }

    MasterPiece::~MasterPiece() {
        release_gfx();
    }

    void MasterPiece::activate() {
        resetGame();
    }

    void MasterPiece::resetGame() {
        startTime = 0;
        waitTime = 3000;
        cur_screen = 0;
        cursor_pos = 0;
    }
        
    bool MasterPiece::event(mxApp &app, SDL_Event &e) {

        switch(e.type) {
            case SDL_KEYDOWN:
            switch(e.key.keysym.sym) {
                case SDLK_UP:
                    switch(cur_screen) {
                        case 1:
                        if(cursor_pos > 0) cursor_pos --;
                        break;
                    }
                break;
                case SDLK_DOWN:
                    switch(cur_screen) {
                        case 1:
                        if(cursor_pos < 3) cursor_pos++;
                        break;
                    }
                break;
                case SDLK_RETURN:
                    switch(cur_screen) {
                        case 1:
                        switch(cursor_pos) {
                            case 0:
                            break;
                            case 1:
                            break;
                            case 2:
                            break;
                            case 3:
                            show(false);
                            break;
                        }
                        break;
                    }
                break;
            }
            break;
        }

        return Window::event(app, e);
    }

    void MasterPiece::draw(mxApp &app) {

        if(!Window::isVisible()) 
            return;

        if(!Window::isDraw())
            return;

        Window::draw(app);  
        Window::drawMenubar(app);             
        switch(cur_screen) {
            case 0:
            draw_intro(app);
            break;
            case 1:
            draw_start(app);
            break;
        }
    }

    void MasterPiece::newGame() {
         
    }

    void MasterPiece::load_gfx(mxApp &app) {
        logo = loadTexture(app, "images/mp_dat/intro.png");
        start = loadTexture(app, "images/mp_dat/start.png");
        SDL_Color color = {255,0,255};
        int w, h;
        cursor = loadTexture(app, "images/mp_dat/cursor.png", w, h, true, color);
    }

    void MasterPiece::release_gfx() {
        if(logo != nullptr)
            SDL_DestroyTexture(logo);
        if(start != nullptr)
            SDL_DestroyTexture(start);
        if(cursor != nullptr) 
            SDL_DestroyTexture(start);
    }

    void MasterPiece::draw_intro(mxApp &app) {
        SDL_Rect rc;
        Window::getDrawRect(rc);
 
         if (startTime == 0) {    
            startTime = SDL_GetTicks();
        }

        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if (elapsedTime > waitTime) {
            elapsedTime = waitTime;
        }

        Uint8 alphaLogo = 255 - (255 * elapsedTime / waitTime);
        Uint8 alphaStart = 255 * elapsedTime / waitTime;

        SDL_SetTextureAlphaMod(logo, alphaLogo);
        SDL_RenderCopy(app.ren, logo, nullptr, &rc);

        SDL_SetTextureAlphaMod(start, alphaStart);
        SDL_RenderCopy(app.ren, start, nullptr, &rc);

        if (elapsedTime >= waitTime) {
            cur_screen = 1;
            startTime = 0;
        }
    }

    void MasterPiece::draw_start(mxApp &app) {
        SDL_Rect drc;
        Window::getDrawRect(drc);
        SDL_RenderCopy(app.ren, start,  nullptr, &drc);
        int cx = 0, cy = 0;
        cx = 250;
        cy = 170 + (cursor_pos * 70) - 16;
        SDL_Rect c_rct = { drc.x + cx, drc.y + cy, 64, 64 };
        SDL_RenderCopy(app.ren, cursor, nullptr, &c_rct);
    }
}