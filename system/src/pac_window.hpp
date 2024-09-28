#ifndef __PAC_WINDOW_H__
#define __PAC_WINDOW_H__

#include "mx_window.hpp"
#include <vector>

namespace mx {

    class DimensionContainer;
    class Dimension;

    enum class Direction { DIR_LEFT, DIR_RIGHT, DIR_UP, DIR_DOWN };

    class PacWindow : public mx::Window {
    public:
        PacWindow(mxApp &app);
        virtual ~PacWindow();
        
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void screenResize(int w, int h) override;

        void newGame();
        void initializeGame();
        void update();
        void drawGrid(mxApp &app);
        bool movePlayer(int dx, int dy);

        static void main(mxApp &app, Dimension *dim);
        static DimensionContainer *dim_c;
        static PacWindow *pac_window;

    private:
        int playerX = 0, playerY = 0;
        std::vector<std::vector<int>> grid;
        std::vector<std::vector<int>> pellet_grid;
        SDL_Texture *draw_tex;
        Direction direction = Direction::DIR_RIGHT;
        Direction nextDirection = Direction::DIR_RIGHT;
        Uint32 time_remaining = 0;
        Uint32 score = 0;
        Uint32 lives = 3;
        void movementLogic();
        void drawCharacter(SDL_Renderer* renderer, int x, int y, int radius);
        void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
    };

}

#endif