#ifndef TETRIS_WINDOW_H
#define TETRIS_WINDOW_H

#include "mx_window.hpp"
#include <vector>
#include <ctime>
#include <cstdlib>

namespace mx {

    class TetrisWindow : public Window {
    public:
        TetrisWindow(mxApp &app);
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        void resetGame();
        void spawnPiece();
        void movePieceDown();
        void movePieceLeft();
        void movePieceRight();
        void rotatePiece();
        void dropPiece();
        bool checkCollision(int newX, int newY, const std::vector<std::vector<int>>& piece);
        void lockPiece();
        void clearLines();
        void gameOver();

    private:
        const int rows = 20;
        const int cols = 10;
        std::vector<std::vector<int>> grid;
        std::vector<std::vector<int>> currentPiece;
        std::vector<std::vector<int>> nextPiece;
        int pieceX, pieceY;
        bool isGameOver = false;
        int score = 0;

        std::vector<std::vector<int>> getRandomPiece();
        std::vector<std::vector<int>> rotate(const std::vector<std::vector<int>>& piece);
    };

}

#endif