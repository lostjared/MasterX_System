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
        void resetGame();

    private:
        SDL_Texture *draw_tex = nullptr;
        void drawGame(mxApp &app);
        int ballX = 0;
        int ballY = 0;
        int ballVelX = 0; 
        int ballVelY = 0;
        int ballSpeed = 5;
        int playerPaddleX = 0;
        int playerPaddleY = 0;
        int aiPaddleX = 0;
        int aiPaddleY = 0;
        int paddleWidth = 10;
        int paddleHeight = 100;
        int paddleSpeed = 5;
        int aiPaddleSpeed = 4;
        int playerScore = 0;
        int aiScore = 0;
        int windowWidth = 640;  
        int windowHeight = 480;
        int ballSize = 5;

        Uint32 lastTime = 0, currentTime = 0;
        float deltaTime = 0.0f;
    };
}

#endif