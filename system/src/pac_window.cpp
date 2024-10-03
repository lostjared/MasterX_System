#include "pac_window.hpp"
#include "dimension.hpp"
#include "messagebox.hpp"

namespace mx {

    PacWindow::PacWindow(mxApp &app) : Window(app), playerX(5), playerY(5) {
        draw_tex = SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 875, 640);
        if(!draw_tex) {
            mx::system_err << "MasterX System: Error creating texture in PacAttack.\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        game_over_font = TTF_OpenFont(getPath("fonts/arial.ttf").c_str(), 44);
        if(!game_over_font) {
            mx::system_err << "MasterX System: Error loading PacAttack font.\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        game_score_font = app.loadFont("fonts/arial.ttf", 24);
        game_over_tex = loadTexture(app, "images/pacgameover.png");
        lives = 3;
        newGame();
    }

    PacWindow::~PacWindow(){
        // clean up here
        if(draw_tex != nullptr)
            SDL_DestroyTexture(draw_tex);
        if(game_over_font != nullptr) {
            TTF_CloseFont(game_over_font);
        }
        if(game_score_font != nullptr) {
            TTF_CloseFont(game_score_font);
        }
    }

    void PacWindow::initializeGame() {
        grid.clear();
        pellet_grid.clear();
        static const std::vector<std::vector<int>> cgrid {
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

         static const std::vector<std::vector<int>> cpellet_grid {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, 
            {1, 2, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 2, 1}, 
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
            {1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1}, 
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
            {1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1}, 
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  
        };
        
        grid = cgrid;
        pellet_grid = cpellet_grid;
        clearGhosts();
        initGhosts();
    }
    
    bool PacWindow::pelletsEmpty() const {

        for(const auto &i : pellet_grid) {
            for(const auto &z : i) {
                if(z == 0 || z == 2)
                    return false;
            }
        }
        return true;
    }

    void PacWindow::draw(mxApp &app) {
        
        if(!isVisible())
            return;
        if(!isDraw())
            return;

        if(pelletsEmpty()) {
            nextLevel();
        }

        Window::draw(app);
        Window::drawMenubar(app);
        SDL_SetRenderTarget(app.ren, draw_tex);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
        SDL_RenderClear(app.ren);

        if(game_over == false)
            drawGrid(app);
        else {
            SDL_RenderCopy(app.ren, game_over_tex, nullptr, nullptr);
            int w = 0, h = 0;
            TTF_SizeText(game_over_font, "Game Over you Lose", &w, &h);
            app.font_printText_Solid(game_over_font, (875/2)-(w/2), (640/2)-(h/2), "Game Over you Lose", {255, 255, 255, 255});
            app.font_printText_Solid(game_over_font, ((875/2)-(w/2))+3, ((640/2)-(h/2))+3, "Game Over you Lose", {175, 0, 175, 255});
        }
        SDL_SetRenderTarget(app.ren, app.tex);
        SDL_Rect drc;
        Window::getDrawRect(drc);
        SDL_RenderCopy(app.ren, draw_tex, nullptr, &drc);
    }
    void PacWindow::drawGrid(mxApp &app) {
        int wallThickness = 2;
        int halfWall = 1;  

        for (int y = 0; y < static_cast<int>(grid.size()); ++y) {
            for (int x = 0; x < static_cast<int>(grid[y].size()); ++x) {
                if (grid[y][x] == 1) {
                    SDL_SetRenderDrawColor(app.ren, 0, 0, 255, 255);    
                    int cellX = (x * cellWidth) + offset_x;
                    int cellY = (y * cellHeight) + offset_y;

                    if (y == 0 || grid[y - 1][x] != 1) {
                        SDL_Rect topWall = { cellX - halfWall, cellY - halfWall, cellWidth + wallThickness, wallThickness };
                        SDL_RenderFillRect(app.ren, &topWall);
                    }

                    if (x == 0 || grid[y][x - 1] != 1) {
                        SDL_Rect leftWall = { cellX - halfWall, cellY - halfWall, wallThickness, cellHeight + wallThickness };
                        SDL_RenderFillRect(app.ren, &leftWall);
                    }

                    if (y == static_cast<int>(grid.size()) - 1 || grid[y + 1][x] != 1) {
                        SDL_Rect bottomWall = { cellX - halfWall, cellY + cellHeight - halfWall, cellWidth + wallThickness, wallThickness };
                        SDL_RenderFillRect(app.ren, &bottomWall);
                    }

                    if (x == static_cast<int>(grid[y].size()) - 1 || grid[y][x + 1] != 1) {
                        SDL_Rect rightWall = { cellX + cellWidth - halfWall, cellY - halfWall, wallThickness, cellHeight + wallThickness };
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
                } else if(pellet_grid[y][x] == 2) {
                    int centerX = offset_x+((x * cellWidth) + (cellWidth / 2));
                    int centerY = offset_y+((y * cellHeight) + (cellHeight / 2));
                    SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255);
                    drawCircle(app.ren, centerX, centerY, 4);
                }
            }
        }

        drawGhosts(app);

        if(time_remaining == 0)
            SDL_SetRenderDrawColor(app.ren, 150, 150, 0, 255);
        else if(time_remaining > 0)
            SDL_SetRenderDrawColor(app.ren,rand()%255, rand()%255, rand()%255, 255);
        
        drawCharacter(app.ren, playerX, playerY, 8);
         
        static Uint32 last = SDL_GetTicks();
        Uint32 current = SDL_GetTicks();
        if(current-last >= 100) {
            movementLogic();  
            last = current;  
            if(time_remaining > 0)
                time_remaining --;
        }
        checkColide();
        app.font_printText_Solid(game_score_font, 15, 15, "Lives: " + std::to_string(lives) + " Score: " + std::to_string(score) + " Level: " + std::to_string(level), {static_cast<unsigned char>(rand()%255),static_cast<unsigned char>(rand()%255),static_cast<unsigned char>(rand()%255),255});
    }

    void PacWindow::checkColide() {
        SDL_Rect pacmanRect = {playerX * cellWidth + offset_x, playerY * cellHeight + offset_y, cellWidth, cellHeight};
        for(auto g = ghosts.begin(); g != ghosts.end(); ++g) {

            if(g->x == playerX && g->y == playerY) {
                if(time_remaining > 0) {
                    ghosts.erase(g);
                    return;
                } else {
                    lives--;
                    if(lives == 0) {
                        game_over = true;
                        return;
                    } else {
                        clearGhosts();
                        initGhosts();
                        playerX = 5;
                        playerY = 5;
                        return;
                    }
                }
            }

            SDL_Rect ghostRect = {g->x * cellWidth + offset_x, g->y * cellHeight + offset_y, cellWidth, cellHeight};
            if (SDL_HasIntersection(&pacmanRect, &ghostRect)) {
                if(time_remaining > 0) {
                    ghosts.erase(g);
                    return;
                }
                else {
                    lives--;
                    if(lives == 0) {
                        game_over = true;
                        return;
                    } else {
                        clearGhosts();
                        initGhosts();
                        playerX = 5;
                        playerY = 5;
                        return;
                    }
                }
                return;
            }
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
            return true;
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
                score += 1;
            } else if(pellet_grid[playerY][playerX] == 2) {
                pellet_grid[playerY][playerX] = -1;
                time_remaining = 50;
                score += 20;
            }
            return true;
        }
        return false;
    }

    void PacWindow::movementLogic() {
        moveGhosts();
        checkColide();

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
            checkColide();
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
        int windowWidth = static_cast<int>(800 * scaleX);
        int windowHeight = static_cast<int>(600 * scaleY);
        int windowPosX = (screenWidth - windowWidth) / 2;
        int windowPosY = (screenHeight - windowHeight) / 2;
        SDL_Rect rc={windowPosX, windowPosY-25, windowWidth, windowHeight};
        this->setRect(rc);
    }

    DimensionContainer *PacWindow::dim_c = nullptr;
    PacWindow *PacWindow::pac_window = nullptr;

    void PacWindow::main(mxApp &app, Dimension *dim) {
        dim->dimensions.push_back(std::make_unique<DimensionContainer>(app));
        dim_c = dynamic_cast<DimensionContainer *>(dim->dimensions[dim->dimensions.size()-1].get());
        if(!dim_c) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        dim_c->init(dim->system_bar, "PacAttack", loadTexture(app, "images/pacwall.png"));
        dim_c->setActive(false);
        dim_c->setVisible(false);
        dim_c->setIcon(loadTexture(app, "images/ghost.png"));
        dim_c->objects.push_back(std::make_unique<PacWindow>(app));
        pac_window = dynamic_cast<PacWindow *>(dim_c->objects[dim_c->objects.size()-1].get());
        if(!pac_window) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        const int baseWidth = 1280;
        const int baseHeight = 720;
        int screenWidth = app.width;
        int screenHeight = app.height;
        float scaleX = static_cast<float>(screenWidth) / baseWidth;
        float scaleY = static_cast<float>(screenHeight) / baseHeight;
        int windowWidth = static_cast<int>(800 * scaleX);
        int windowHeight = static_cast<int>(600 * scaleY);
        int windowPosX = (screenWidth - windowWidth) / 2;
        int windowPosY = (screenHeight - windowHeight) / 2;
        pac_window->create(dim_c, "PacAttack", windowPosX, windowPosY-50, windowWidth, windowHeight);
        dim_c->events.addWindow(pac_window);
        pac_window->setIcon(loadTexture(app, "images/ghost.png"));
        pac_window->setSystemBar(dim->system_bar);
        pac_window->setReload(true);
        pac_window->setCanResize(false);

        Menu_ID game_id = pac_window->menu.addHeader(create_header("Game"));
        pac_window->menu.addItem(game_id, pac_window->menu.addIcon(loadTexture(app, "images/ghost.png")), create_menu_item("New Game", [](mxApp &app, Window *win, SDL_Event &e) -> bool {
            static PacWindow *p = nullptr;
            p = dynamic_cast<PacWindow *>(win);
            if(!p) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            MX_MessageBox::OkCancelMX_MessageBox(app, win->dim, "Start a new game?", "Start a new game?", [&](mxApp &app, Window *win, int ok) -> bool {
                p->lives = 3;
                p->level = 1;
                p->newGame();
                return true;
            });
            return true;
        }));
        Menu_ID help_id = pac_window->menu.addHeader(create_header("Help"));
        pac_window->menu.addItem(help_id, pac_window->menu.addIcon(loadTexture(app, "images/ghost.png")), create_menu_item("About", [](mxApp &app, Window *win, SDL_Event &e) -> bool {
            MX_MessageBox::OkMX_MessageBox(app, win->dim, "About PacAttack", "PacAttack created by Jared Bruni");
            return true;
        }));
        pac_window->show(true);
    }

    void PacWindow::newGame() {
        score = 0;
        time_remaining = 0;
        initializeGame();
        playerX = 5;
        playerY = 5;
        game_over = false;
    }

    void PacWindow::nextLevel() {
        time_remaining = 0;
        initializeGame();
        playerX = 5;
        playerY = 5;
        game_over = false;
        level++;
    }

    void PacWindow::initGhosts() {
        Ghost ghost1 = {13, 12, Direction::DIR_DOWN, 1, {0, 0, 255, 255}};   // Inky (Blue)
        Ghost ghost2 = {14, 12, Direction::DIR_DOWN, 1, {255, 0, 0, 255}};   // Blinky (Red)
        Ghost ghost3 = {13, 8, Direction::DIR_UP, 1, {255, 184, 255, 255}};  // Pinky (Pink)
        Ghost ghost4 = {14, 8, Direction::DIR_UP, 1, {255, 184, 82, 255}};   // Clyde (Orange)        
        ghosts.push_back(ghost1);
        ghosts.push_back(ghost2);
        ghosts.push_back(ghost3);
        ghosts.push_back(ghost4);
    }
    void PacWindow::clearGhosts() {
        ghosts.clear();
    }

    void PacWindow::moveGhosts() {
        for (auto &g : ghosts) {
            int newX = g.x, newY = g.y;
            
            switch (g.direction) {
                case Direction::DIR_UP:
                    newY -= g.speed;
                    break;
                case Direction::DIR_DOWN:
                    newY += g.speed;
                    break;
                case Direction::DIR_LEFT:
                    newX -= g.speed;
                    break;
                case Direction::DIR_RIGHT:
                    newX += g.speed;
                    break;
            }

            if (grid[newY][newX] != 1) {
                g.x = newX;
                g.y = newY;
            } else {
                g.direction = static_cast<Direction>(rand()%4);
            }
        }
    }

    void PacWindow::drawGhosts(mxApp &app) {
        for(auto &g : ghosts) {
            int centerX = (g.x * cellWidth) + (20 / 2) + offset_x;
            int centerY = (g.y * cellHeight) + (14 / 2) + offset_y;
            drawGhost(app.ren, centerX, centerY, 20, 14, g.color);
        }
    }

    void PacWindow::drawGhost(SDL_Renderer* renderer, int x, int y, int width, int height, SDL_Color color) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        int radius = width / 2;
        for (int w = 0; w < radius * 2; w++) {
            for (int h = 0; h < radius; h++) {
                int dx = radius - w;
                int dy = radius - h;
                if ((dx * dx + dy * dy) <= (radius * radius)) {
                    SDL_RenderDrawPoint(renderer, x + w, y + h);
                }
            }
        }
        SDL_Rect bodyRect = {x, y + radius, width, height - radius};
        SDL_RenderFillRect(renderer, &bodyRect);
        int zigZagHeight = height / 6;
        int numZigs = 3;
        for (int i = 0; i < numZigs; i++) {
            int startX = x + (i * (width / numZigs));
            int endX = startX + (width / numZigs);
            int topY = y + height - zigZagHeight;
            int bottomY = y + height;
            SDL_RenderDrawLine(renderer, startX, topY, (startX + endX) / 2, bottomY);
            SDL_RenderDrawLine(renderer, (startX + endX) / 2, bottomY, endX, topY);
        }
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int eyeRadius = width / 8;
        int eyeOffsetX = width / 4;
        int eyeOffsetY = height / 6;
        for (int w = 0; w < eyeRadius * 2; w++) {
            for (int h = 0; h < eyeRadius * 2; h++) {
                int dx = eyeRadius - w;
                int dy = eyeRadius - h;
                if ((dx * dx + dy * dy) <= (eyeRadius * eyeRadius)) {
                    SDL_RenderDrawPoint(renderer, x + eyeOffsetX + w, y + eyeOffsetY + h);
                }
            }
        }
        for (int w = 0; w < eyeRadius * 2; w++) {
            for (int h = 0; h < eyeRadius * 2; h++) {
                int dx = eyeRadius - w;
                int dy = eyeRadius - h;
                if ((dx * dx + dy * dy) <= (eyeRadius * eyeRadius)) {
                    SDL_RenderDrawPoint(renderer, x + width - eyeOffsetX - eyeRadius + w, y + eyeOffsetY + h);
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        int pupilRadius = width / 16;
        for (int w = 0; w < pupilRadius * 2; w++) {
            for (int h = 0; h < pupilRadius * 2; h++) {
                int dx = pupilRadius - w;
                int dy = pupilRadius - h;
                if ((dx * dx + dy * dy) <= (pupilRadius * pupilRadius)) {
                    SDL_RenderDrawPoint(renderer, x + eyeOffsetX + eyeRadius / 2 + w, y + eyeOffsetY + pupilRadius + h);
                }
            }
        }
        for (int w = 0; w < pupilRadius * 2; w++) {
            for (int h = 0; h < pupilRadius * 2; h++) {
                int dx = pupilRadius - w;
                int dy = pupilRadius - h;
                if ((dx * dx + dy * dy) <= (pupilRadius * pupilRadius)) {
                    SDL_RenderDrawPoint(renderer, x + width - eyeOffsetX - eyeRadius + eyeRadius / 2 + w, y + eyeOffsetY + pupilRadius + h);
                }
            }
        }
    }
}