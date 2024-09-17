#ifndef __MASTERPIECE__H_
#define __MASTERPIECE__H_

#include"window.hpp"
#include"dimension.hpp"
#include"mx_window.hpp"

namespace mx {

    class MasterPiece : public Window {
    public:
        MasterPiece(mxApp &);
        virtual ~MasterPiece();
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void draw(mxApp &app) override;
        virtual void activate() override;
        void newGame();
        void resetGame();
    private:
        void load_gfx(mxApp &);
        void release_gfx();
        void draw_intro(mxApp &);
        void draw_start(mxApp &);
        int cur_screen = 0;
        SDL_Texture *logo, *start, *cursor;
        Uint32 startTime = 0; 
        Uint32 waitTime = 3000;
        int cursor_pos = 0;

    };



}





#endif