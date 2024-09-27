#include "pac_window.hpp"
#include "dimension.hpp"

namespace mx {

    PacWindow::PacWindow(mxApp &app) : Window(app), playerX(5), playerY(5) {
        initializeGame();
        draw_tex = SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
    }

    PacWindow::~PacWindow(){
        // clean up here
        if(draw_tex != nullptr)
            SDL_DestroyTexture(draw_tex);
    }

    void PacWindow::initializeGame() {
        grid = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Row 1
            {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}, // Row 2
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1}, // Row 3
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, // Row 4
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 5
            {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, // Row 6
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 7
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, // Row 8
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, // Row 9
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, // Row 10
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 11
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, // Row 12
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 13
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, // Row 14
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 15
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Row 16
            {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1}, // Row 17
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, // Row 18
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, // Row 19
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 20
            {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, // Row 21
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}, // Row 22
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, // Row 23
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, // Row 24
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, // Row 25
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 26
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, // Row 27
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, // Row 28
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // Row 29
        };

        grid[21][15] = 0;
    }
    
    void PacWindow::draw(mxApp &app) {
        
        if(!isVisible())
            return;
        if(!isDraw())
            return;

        Window::draw(app);
        Window::drawMenubar(app);
        SDL_SetRenderTarget(app.ren, draw_tex);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
        SDL_RenderClear(app.ren);
        
        drawGrid(app);
        
        SDL_SetRenderTarget(app.ren, app.tex);
        SDL_Rect drc;
        Window::getDrawRect(drc);
        SDL_RenderCopy(app.ren, draw_tex, nullptr, &drc);
    }
    void PacWindow::drawGrid(mxApp &app) {
        int cellWidth = 22;
        int cellHeight = 15;
        int offset_x = 25;
        int offset_y = 25;

        for (int y = 0; y < static_cast<int>(grid.size()); ++y) {
            for (int x = 0; x < static_cast<int>(grid[y].size()); ++x) {
                SDL_Rect cell = {(x * cellWidth)+offset_x, (y * cellHeight)+offset_y, cellWidth, cellHeight};
                if (grid[y][x] == 1) {
                    SDL_SetRenderDrawColor(app.ren, 255, 0, 0, 255);                 
                } else if (grid[y][x] == 2) {
                    SDL_SetRenderDrawColor(app.ren, 255, 255, 0, 255); 
                } else {
                    SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255); 
                }
                SDL_RenderFillRect(app.ren, &cell);
            }
        }

        SDL_Rect playerRect = {(playerX * cellWidth)+offset_x, (playerY * cellHeight)+offset_y, cellWidth, cellHeight};
        SDL_SetRenderDrawColor(app.ren, 0, 0, 255, 255); 
        SDL_RenderFillRect(app.ren, &playerRect);

        app.printText(10, 10, "Poss :" + std::to_string(playerX) + " y: " + std::to_string(playerY), {255,255,255,255});
    }

    bool PacWindow::event(mxApp &app, SDL_Event &e) {
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    movePlayer(0, -1);
                    break;
                case SDLK_DOWN:
                    movePlayer(0, 1);
                    break;
                case SDLK_LEFT:
                    movePlayer(-1, 0);
                    break;
                case SDLK_RIGHT:
                    movePlayer(1, 0);
                    break;
            }
        }
        return Window::event(app, e);
    }

    void PacWindow::movePlayer(int dx, int dy) {
        int newX = playerX + dx;
        int newY = playerY + dy;

        if (grid[newY][newX] != 1) {
            playerX = newX;
            playerY = newY;
        }
    }

    void PacWindow::screenResize(int w, int h) {
        Window::screenResize(w, h);
        const int baseWidth = 1280;
        const int baseHeight = 720;
        int screenWidth = w;
        int screenHeight = h;
        float scaleX = static_cast<float>(screenWidth) / baseWidth;
        float scaleY = static_cast<float>(screenHeight) / baseHeight;
        int windowWidth = static_cast<int>(640 * scaleX);
        int windowHeight = static_cast<int>(480 * scaleY);
        int windowPosX = (screenWidth - windowWidth) / 2;
        int windowPosY = (screenHeight - windowHeight) / 2;
        SDL_Rect rc={windowPosX, windowPosY, windowWidth, windowHeight};
        this->setRect(rc);
    }

    DimensionContainer *PacWindow::dim_c = nullptr;
    PacWindow *PacWindow::pac_window = nullptr;

    void PacWindow::main(mxApp &app, Dimension *dim) {
        dim->dimensions.push_back(std::make_unique<DimensionContainer>(app));
        dim_c = dynamic_cast<DimensionContainer *>(dim->dimensions[dim->dimensions.size()-1].get());
        dim_c->init(dim->system_bar, "PacAttack", loadTexture(app, "images/codespiral.png"));
        dim_c->setActive(false);
        dim_c->setVisible(false);
        dim_c->setIcon(loadTexture(app, "images/xicon.png"));
        dim_c->objects.push_back(std::make_unique<PacWindow>(app));
        pac_window = dynamic_cast<PacWindow *>(dim_c->objects[dim_c->objects.size()-1].get());
        const int baseWidth = 1280;
        const int baseHeight = 720;
        int screenWidth = app.width;
        int screenHeight = app.height;
        float scaleX = static_cast<float>(screenWidth) / baseWidth;
        float scaleY = static_cast<float>(screenHeight) / baseHeight;
        int windowWidth = static_cast<int>(640 * scaleX);
        int windowHeight = static_cast<int>(480 * scaleY);
        int windowPosX = (screenWidth - windowWidth) / 2;
        int windowPosY = (screenHeight - windowHeight) / 2;
        pac_window->create(dim_c, "PacAttack", windowPosX, windowPosY, windowWidth, windowHeight);
        dim_c->events.addWindow(pac_window);
        pac_window->setSystemBar(dim->system_bar);
        pac_window->show(true);
    }

}