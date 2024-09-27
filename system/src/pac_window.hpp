#ifndef __PAC_WINDOW_H__
#define __PAC_WINDOW_H__

#include "mx_window.hpp"
#include <vector>

namespace mx {

    class DimensionContainer;
    class Dimension;

    class PacWindow : public mx::Window {
    public:
        PacWindow(mxApp &app);
        virtual ~PacWindow();
        
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void screenResize(int w, int h) override;

        void initializeGame();
        void update();
        void drawGrid(mxApp &app);
        void movePlayer(int dx, int dy);

        static void main(mxApp &app, Dimension *dim);
        static DimensionContainer *dim_c;
        static PacWindow *pac_window;

    private:
        int playerX, playerY;
        std::vector<std::vector<int>> grid;
        SDL_Texture *draw_tex;
    };

}

#endif