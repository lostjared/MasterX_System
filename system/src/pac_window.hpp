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
        TTF_Font  *game_over_font = nullptr;
        TTF_Font  *game_score_font = nullptr;
        SDL_Texture *game_over_tex = nullptr;
        bool game_over = false;
        int playerX = 0, playerY = 0;
        std::vector<std::vector<int>> grid;
        std::vector<std::vector<int>> pellet_grid;
        struct Ghost {
            int x = 0;
            int y = 0;
            Direction direction = Direction::DIR_LEFT;
            int speed = 0;
            SDL_Color color = {0};
        };
        std::vector<Ghost> ghosts;
        SDL_Texture *draw_tex;
        Direction direction = Direction::DIR_RIGHT;
        Direction nextDirection = Direction::DIR_RIGHT;
        Uint32 time_remaining = 0;
        Uint32 score = 0;
        Uint32 lives = 3;
        int cellWidth = 30;
        int cellHeight = 20;
        int offset_x = 16;
        int offset_y = 25;
  
        void movementLogic();
        void drawCharacter(SDL_Renderer* renderer, int x, int y, int radius);
        void drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius);
        void drawGhost(SDL_Renderer* renderer, int x, int y, int width, int height, SDL_Color color);
        void initGhosts();
        void clearGhosts();
        void drawGhosts(mxApp &app);
        void moveGhosts();
        void checkColide();
    };

}

#endif