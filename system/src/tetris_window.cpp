#include "tetris_window.hpp"

namespace mx {

    TetrisWindow::TetrisWindow(mxApp &app)
    : Window(app), pieceX(0), pieceY(0) {
        resetGame();
    }

    void TetrisWindow::resetGame() {
        srand(static_cast<unsigned>(time(nullptr)));
        grid.clear();
        grid.resize(rows, std::vector<int>(cols, 0));
        currentPiece = getRandomPiece();
        nextPiece = getRandomPiece();
        pieceX = cols / 2 - static_cast<int>(currentPiece[0].size()) / 2;
        pieceY = 0;
        isGameOver = false;
        score = 0;
    }

    std::vector<std::vector<int>> TetrisWindow::getRandomPiece() {
        const std::vector<std::vector<std::vector<int>>> pieces = {
            {{1, 1, 1, 1}},
            {{1, 1}, {1, 1}},
            {{0, 1, 0}, {1, 1, 1}},
            {{1, 1, 0}, {0, 1, 1}},
            {{0, 1, 1}, {1, 1, 0}},
            {{1, 1, 1}, {1, 0, 0}},
            {{1, 1, 1}, {0, 0, 1}}
        };
        int index = rand() % static_cast<int>(pieces.size());
        return pieces[index];
    }

    std::vector<std::vector<int>> TetrisWindow::rotate(const std::vector<std::vector<int>>& piece) {
        int rows = static_cast<int>(piece.size());
        int cols = static_cast<int>(piece[0].size());
        std::vector<std::vector<int>> rotatedPiece(cols, std::vector<int>(rows, 0));

        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                rotatedPiece[col][rows - 1 - row] = piece[row][col];
            }
        }

        return rotatedPiece;
    }

    bool TetrisWindow::checkCollision(int newX, int newY, const std::vector<std::vector<int>>& piece) {
        for (int row = 0; row < static_cast<int>(piece.size()); ++row) {
            for (int col = 0; col < static_cast<int>(piece[0].size()); ++col) {
                if (piece[row][col] == 1) {
                    int gridX = newX + col;
                    int gridY = newY + row;
                    if (gridX < 0 || gridX >= cols || gridY >= rows || (gridY >= 0 && grid[gridY][gridX] != 0)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void TetrisWindow::lockPiece() {
        for (int row = 0; row < static_cast<int>(currentPiece.size()); ++row) {
            for (int col = 0; col < static_cast<int>(currentPiece[0].size()); ++col) {
                if (currentPiece[row][col] == 1) {
                    grid[pieceY + row][pieceX + col] = 1;
                }
            }
        }
        clearLines();
        currentPiece = nextPiece;
        nextPiece = getRandomPiece();
        pieceX = cols / 2 - static_cast<int>(currentPiece[0].size()) / 2;
        pieceY = 0;
        if (checkCollision(pieceX, pieceY, currentPiece)) {
            gameOver();
        }
    }

    void TetrisWindow::clearLines() {
        for (int row = rows - 1; row >= 0; --row) {
            bool isFull = true;
            for (int col = 0; col < cols; ++col) {
                if (grid[row][col] == 0) {
                    isFull = false;
                    break;
                }
            }
            if (isFull) {
                grid.erase(grid.begin() + row);
                grid.insert(grid.begin(), std::vector<int>(cols, 0));
                row++;
                score += 100;
            }
        }
    }

    void TetrisWindow::gameOver() {
        isGameOver = true;
    }

    void TetrisWindow::movePieceDown() {
        if (!checkCollision(pieceX, pieceY + 1, currentPiece)) {
            pieceY++;
        } else {
            lockPiece();
        }
    }

    void TetrisWindow::movePieceLeft() {
        if (!checkCollision(pieceX - 1, pieceY, currentPiece)) {
            pieceX--;
        }
    }

    void TetrisWindow::movePieceRight() {
        if (!checkCollision(pieceX + 1, pieceY, currentPiece)) {
            pieceX++;
        }
    }

    void TetrisWindow::rotatePiece() {
        auto rotatedPiece = rotate(currentPiece);
        if (!checkCollision(pieceX, pieceY, rotatedPiece)) {
            currentPiece = rotatedPiece;
        }
    }

    void TetrisWindow::dropPiece() {
        while (!checkCollision(pieceX, pieceY + 1, currentPiece)) {
            pieceY++;
        }
        lockPiece();
    }

    void TetrisWindow::draw(mxApp &app) {

        if(!Window::isVisible())
            return;

        if(!Window::isDraw())
            return;


        Window::draw(app);
        Window::drawMenubar(app);

        SDL_Rect rc;
        Window::getDrawRect(rc);

        rc.y += 25;

        if (isGameOver) {
            SDL_Surface* surface = TTF_RenderText_Blended(app.font, "Game Over", {255, 0, 0});
            SDL_Texture* texture = SDL_CreateTextureFromSurface(app.ren, surface);
            int textW = 0, textH = 0;
            SDL_QueryTexture(texture, nullptr, nullptr, &textW, &textH);
            SDL_Rect textRect = {rc.x + (rc.w - textW) / 2, rc.y + rc.h / 2, textW, textH};
            SDL_RenderCopy(app.ren, texture, nullptr, &textRect);
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(surface);
            return;
        }

        int blockSize = rc.w / cols;  
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                SDL_Rect rect = {rc.x + col * blockSize, rc.y + row * blockSize, blockSize, blockSize};
                if (grid[row][col] == 1) {
                    SDL_SetRenderDrawColor(app.ren, 255, 0, 0, 255);
                    SDL_RenderFillRect(app.ren, &rect);
                } else {
                    SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
                    SDL_RenderDrawRect(app.ren, &rect);
                }
            }
 
            static Uint32 fallTime = SDL_GetTicks();
            static Uint32 fallInterval= 1000;

            Uint32 currentTime = SDL_GetTicks(); 
            if (currentTime - fallTime >= fallInterval) {
                movePieceDown();
                fallTime = currentTime;
            }

            app.printText(rc.x+3, rc.y-23, "Score: " + std::to_string(score), {0,0,0,255});
         }

        
        for (int row = 0; row < static_cast<int>(currentPiece.size()); ++row) {
            for (int col = 0; col < static_cast<int>(currentPiece[0].size()); ++col) {
                if (currentPiece[row][col] == 1) {
                    SDL_Rect rect = {rc.x + (pieceX + col) * blockSize, rc.y + (pieceY + row) * blockSize, blockSize, blockSize};
                    SDL_SetRenderDrawColor(app.ren, 0, 255, 0, 255);
                    SDL_RenderFillRect(app.ren, &rect);
                }
            }
        }
    }

    bool TetrisWindow::event(mxApp &app, SDL_Event &e) {
        if (e.type == SDL_KEYDOWN && !isGameOver) {
            switch (e.key.keysym.sym) {
                case SDLK_DOWN:
                    movePieceDown();
                    break;
                case SDLK_LEFT:
                    movePieceLeft();
                    break;
                case SDLK_RIGHT:
                    movePieceRight();
                    break;
                case SDLK_UP:
                    rotatePiece();
                    break;
                case SDLK_SPACE:
                    dropPiece();
                    break;
                default:
                    break;
            }
        }

        return Window::event(app, e);
    }

}