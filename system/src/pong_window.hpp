#ifndef _PONG_WINDOW_H_
#define _PONG_WINDOW_H_

#include"mx_window.hpp"
#include"window.hpp"

namespace mx {

    class Dimension;

    class PongWindow : public Window {
    public:
        PongWindow(mxApp &app);
        virtual ~PongWindow();

        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void screenResize(int w, int h) override;

        static void main(mxApp &app, Dimension *dim);
        static DimensionContainer *dim_c;
        static PongWindow *pong_window;

        void newGame();

    private:
        SDL_Texture *draw_tex = nullptr;
        void drawGame(mxApp &app);
        int score = 0;
    };
}

#endif