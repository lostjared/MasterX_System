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
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  
        };

         pellet_grid = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, 
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  
        };
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
        int offset_x = 13;
        int offset_y = 20;
        int wallThickness = 3;
 
        for (int y = 0; y < static_cast<int>(grid.size()); ++y) {
            for (int x = 0; x < static_cast<int>(grid[y].size()); ++x) {
                SDL_Rect cell = {(x * cellWidth) + offset_x, (y * cellHeight) + offset_y, cellWidth, cellHeight};

                if (grid[y][x] == 1) {
                    SDL_SetRenderDrawColor(app.ren, 0, 0, 255, 255);
                    if (y > 0 && grid[y - 1][x] != 1) {
                        SDL_Rect topWall = {cell.x, cell.y, cell.w, wallThickness};
                        SDL_RenderFillRect(app.ren, &topWall);
                    }
                    if (y < static_cast<int>(grid.size()) - 1 && grid[y + 1][x] != 1) {
                        SDL_Rect bottomWall = {cell.x, cell.y + cell.h - wallThickness, cell.w, wallThickness};
                        SDL_RenderFillRect(app.ren, &bottomWall);
                    }
                    if (x > 0 && grid[y][x - 1] != 1) {
                        SDL_Rect leftWall = {cell.x, cell.y, wallThickness, cell.h};
                        SDL_RenderFillRect(app.ren, &leftWall);
                    }
                    if (x < static_cast<int>(grid[y].size()) - 1 && grid[y][x + 1] != 1) {
                        SDL_Rect rightWall = {cell.x + cell.w - wallThickness, cell.y, wallThickness, cell.h};
                        SDL_RenderFillRect(app.ren, &rightWall);
                    }
                }
            }
        }

        for (int y = 0; y < static_cast<int>(grid.size()); ++y) {
            for (int x = 0; x < static_cast<int>(grid[y].size()); ++x) {
                if (pellet_grid[y][x] == 0) {
                    int centerX = offset_x+((x * cellWidth) + (cellWidth / 2));
                    int centerY = offset_y+((y * cellHeight) + (cellHeight / 2));
                    SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);
                    drawCircle(app.ren, centerX, centerY, 2);
                }
            }
        }

        SDL_SetRenderDrawColor(app.ren, 150, 150, 0, 255);
        drawCharacter(app.ren, playerX, playerY, 8);
        static Uint32 last = SDL_GetTicks();
        Uint32 current = SDL_GetTicks();
        if(current-last >= 100) {
            movementLogic();  
            last = current;  
        }
    }

    void PacWindow::drawCharacter(SDL_Renderer* renderer, int playerX, int playerY, int radius) {

        static bool mouthOpening = true;
        static int  angle = 0;
        if (mouthOpening) {
            angle += 2; 
            if (angle >= 45) {
                mouthOpening = false; 
            }
        } else {
            angle -= 2; 
            if (angle <= 0) {
                mouthOpening = true; 
            }
        }
        int cellWidth = 22; 
        int cellHeight = 15;
        int offset_x = 13;
        int offset_y = 20;

        int centerX = (playerX * cellWidth) + (cellWidth / 2);
        int centerY = (playerY * cellHeight) + (cellHeight / 2);
        centerX += offset_x;
        centerY += offset_y;
        int drawX = centerX - radius;
        int drawY = centerY - radius;

        double startAngle = 0.0, endAngle = 0.0;

        switch(direction) {
            case Direction::DIR_LEFT:
                startAngle = -angle / 2;
                endAngle = angle / 2;
                break;
            case Direction::DIR_RIGHT:
                startAngle = 180 - (angle / 2);
                endAngle = 180 + (angle / 2);
                break;
            case Direction::DIR_UP:
                startAngle = 90 - (angle / 2);
                endAngle = 90 + (angle / 2);
                break;
            case Direction::DIR_DOWN:
                startAngle = -90 - (angle / 2);
                endAngle = -90 + (angle / 2);
                break;
        }

        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    double theta = atan2(dy, dx) * 180 / M_PI;
                    if (theta < startAngle || theta > endAngle) {
                        SDL_RenderDrawPoint(renderer, drawX + w, drawY + h);
                    }
                }
            }
        }
    }

    void PacWindow::drawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius * 2; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }


    bool PacWindow::event(mxApp &app, SDL_Event &e) {
        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_UP:
                    nextDirection = Direction::DIR_UP;
                    break;
                case SDLK_DOWN:
                    nextDirection = Direction::DIR_DOWN;
                    break;
                case SDLK_LEFT:
                    nextDirection = Direction::DIR_LEFT;
                    break;
                case SDLK_RIGHT:
                    nextDirection = Direction::DIR_RIGHT;
                    break;
            }
        }
        return Window::event(app, e);
    }

    bool PacWindow::movePlayer(int dx, int dy) {
        int newX = playerX + dx;
        int newY = playerY + dy;

        if (grid[newY][newX] != 1) {
            playerX = newX;
            playerY = newY;
            if(pellet_grid[playerY][playerX] == 0) {
                pellet_grid[playerY][playerX] = -1;
            }
            return true;
        }
        return false;
    }

    void PacWindow::movementLogic() {

         if (nextDirection != direction) {
            switch(nextDirection) {
                case Direction::DIR_LEFT:
                    if (movePlayer(-1, 0)) {
                        direction = Direction::DIR_LEFT;
                        return;
                    }
                    break;
                case Direction::DIR_RIGHT:
                    if (movePlayer(1, 0)) {
                        direction = Direction::DIR_RIGHT;
                        return;
                    }
                    break;
                case Direction::DIR_UP:
                    if (movePlayer(0, -1)) {
                        direction = Direction::DIR_UP;
                        return;
                    }
                    break;
                case Direction::DIR_DOWN:
                    if (movePlayer(0, 1)) {
                        direction = Direction::DIR_DOWN;
                        return;
                    }
                    break;
            }
        }
        int dx = 0, dy = 0;

        switch(direction) {
            case Direction::DIR_LEFT:
                dx = -1;
                break;
            case Direction::DIR_RIGHT:
                dx = 1;
                break;
            case Direction::DIR_UP:
                dy = -1;
                break;
            case Direction::DIR_DOWN:
                dy = 1;
                break;
        }

        if (movePlayer(dx, dy)) {
            return;  
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