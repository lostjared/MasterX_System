#include"masterpiece.hpp"

namespace mx {

    MasterPiece::MasterPiece(mxApp &app) : Window(app) {
        load_gfx(app);
        draw_tex = SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
        if(!draw_tex) {
            mx::system_err << "MasterX System: couuld not create texture..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        setCanResize(true);
    }

    MasterPiece::~MasterPiece() {

        if(draw_tex != nullptr)
            SDL_DestroyTexture(draw_tex);

        release_gfx();
    }

    void MasterPiece::activate() {
        resetGame();
    }

    void MasterPiece::newGame() {
        cur_screen = 2;
        matrix.init_matrix();
        waitTime2 = 1000;
    }

    void MasterPiece::resetGame() {
        startTime = 0;
        waitTime = 3000;
        cur_screen = 0;
        cursor_pos = 0;
    }
        
    bool MasterPiece::event(mxApp &app, SDL_Event &e) {

        switch(e.type) {
            case SDL_KEYDOWN:
            switch(e.key.keysym.sym) {
                case SDLK_UP:
                    switch(cur_screen) {
                        case 1:
                        if(cursor_pos > 0) cursor_pos --;
                        break;
                        case 2:
                            matrix.block.color.shiftcolor(true);
                        break;
                    }
                break;
                case SDLK_DOWN:
                    switch(cur_screen) {
                        case 1:
                        if(cursor_pos < 3) cursor_pos++;
                        break;
                        case 2:
                            matrix.block.MoveDown();
                        break;
                    }
                break;
                case SDLK_LEFT:
                    switch(cur_screen) {
                        case 2:
                        matrix.block.MoveLeft();
                        break;
                    }
                break;
                case SDLK_RIGHT:
                    switch(cur_screen) {
                        case 2:
                        matrix.block.MoveRight();
                        break;
                    }
                break;
                case SDLK_RETURN:
                    switch(cur_screen) {
                        case 1:
                        switch(cursor_pos) {
                            case 0:
                            newGame();
                            break;
                            case 1:
                            break;
                            case 2:
                            break;
                            case 3:
                            show(false);
                            break;
                        }
                        break;
                    }
                break;
            }
            break;
        }

        return Window::event(app, e);
    }

    void MasterPiece::draw(mxApp &app) {

        if(!Window::isVisible()) 
            return;

        if(!Window::isDraw())
            return;

        Window::draw(app);  
        Window::drawMenubar(app);        

        SDL_SetRenderTarget(app.ren, draw_tex);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
        SDL_RenderClear(app.ren);
        switch(cur_screen) {
            case 0:
            draw_intro(app);
            break;
            case 1:
            draw_start(app);
            break;
            case 2:
            draw_game(app);
            break;
        }

        SDL_SetRenderTarget(app.ren, app.tex);
        SDL_Rect drc;
        Window::getDrawRect(drc);
        SDL_RenderCopy(app.ren, draw_tex, nullptr, &drc);
    }

    void MasterPiece::load_gfx(mxApp &app) {
        logo = loadTexture(app, "images/mp_dat/intro.png");
        start = loadTexture(app, "images/mp_dat/start.png");
        SDL_Color color = {255,0,255};
        int w, h;
        cursor = loadTexture(app, "images/mp_dat/cursor.png", w, h, true, color);
        gamebg = loadTexture(app, "images/mp_dat/gamebg.png");

        static const char* grid_block_paths[] = {
            "images/mp_dat/block_black.png",
            "images/mp_dat/block_yellow.png",
            "images/mp_dat/block_orange.png",
            "images/mp_dat/block_ltblue.png",
            "images/mp_dat/block_dblue.png",
            "images/mp_dat/block_purple.png",
            "images/mp_dat/block_pink.png",
            "images/mp_dat/block_gray.png",
            "images/mp_dat/block_red.png",
            "images/mp_dat/block_green.png",
            "images/mp_dat/block_clear.png",
            nullptr
        };

        for(int i = 0; grid_block_paths[i] != nullptr; ++i) {
            blocks[i] = loadTexture(app, grid_block_paths[i]);
        }

    }

    void MasterPiece::release_gfx() {
        if(logo != nullptr)
            SDL_DestroyTexture(logo);
        if(start != nullptr)
            SDL_DestroyTexture(start);
        if(cursor != nullptr) 
            SDL_DestroyTexture(start);
        if(gamebg != nullptr) 
            SDL_DestroyTexture(gamebg);

        for(int i = 0; i < 11; ++i) {
            if(blocks[i] != nullptr) 
                SDL_DestroyTexture(blocks[i]);
        }
    }

    void MasterPiece::draw_intro(mxApp &app) {
        SDL_Rect rc = {0, 0, 640, 480 };
 
         if (startTime == 0) {    
            startTime = SDL_GetTicks();
        }

        Uint32 elapsedTime = SDL_GetTicks() - startTime;
        if (elapsedTime > waitTime) {
            elapsedTime = waitTime;
        }

        Uint8 alphaLogo = 255 - (255 * elapsedTime / waitTime);
        Uint8 alphaStart = 255 * elapsedTime / waitTime;

        SDL_SetTextureAlphaMod(logo, alphaLogo);
        SDL_RenderCopy(app.ren, logo, nullptr, &rc);

        SDL_SetTextureAlphaMod(start, alphaStart);
        SDL_RenderCopy(app.ren, start, nullptr, &rc);

        if (elapsedTime >= waitTime) {
            cur_screen = 1;
            startTime = 0;
        }
    }

    void MasterPiece::draw_start(mxApp &app) {
        SDL_Rect drc = {0, 0, 640, 480};
        SDL_RenderCopy(app.ren, start,  nullptr, &drc);
        int cx = 0, cy = 0;
        cx = 250;
        cy = 170 + (cursor_pos * 70);
        SDL_Rect c_rct = { drc.x + cx, drc.y + cy, 64, 64 };
        SDL_RenderCopy(app.ren, cursor, nullptr, &c_rct);
    }

    const int BLOCK_FADE = -1;
    const int BLOCK_BLACK = 0;
    const int STARTX = 185;
    const int STARTY = 95;

    void MasterPiece::draw_game(mxApp &app) {
        SDL_Rect rc = {0, 0, 640, 480};
        SDL_RenderCopy(app.ren, gamebg, nullptr, &rc);

        const int STARTPOSX = STARTX;
        const int STARTPOSY = STARTY;

        int x = STARTPOSX, y = STARTPOSY;
        int blockWidth = 32;
        int blockHeight = 16;

        for (int i = 0; i < 17; i++) {
            for (int j = 0; j < 8; j++) {
                if (matrix.Tiles[i][j] != BLOCK_FADE) {
                    if (matrix.Tiles[i][j] != BLOCK_BLACK) {
                        SDL_Rect dstRect = { x, y, blockWidth, blockHeight };
                        SDL_RenderCopy(app.ren, blocks[matrix.Tiles[i][j]], nullptr, &dstRect);
                    }
                    x += blockWidth;
                } else {
                    SDL_Rect dstRect = {x,y,blockWidth,blockHeight };
                    SDL_RenderCopy(app.ren, blocks[1+(rand()%8)], nullptr, &dstRect);
                    x += blockWidth;
                }
            }
            x = STARTPOSX;
            y += blockHeight;
        }

        int bx = 510 , by = 200;

        SDL_Rect nextBlockRect1 = { bx, by, blockWidth, blockHeight };
        SDL_RenderCopy(app.ren, blocks[matrix.block.nextcolor.c1], nullptr, &nextBlockRect1);

        SDL_Rect nextBlockRect2 = { bx, by + blockHeight, blockWidth, blockHeight };
        SDL_RenderCopy(app.ren, blocks[matrix.block.nextcolor.c2], nullptr, &nextBlockRect2);

        SDL_Rect nextBlockRect3 = { bx, by + 2 * blockHeight, blockWidth, blockHeight };
        SDL_RenderCopy(app.ren, blocks[matrix.block.nextcolor.c3], nullptr, &nextBlockRect3);

        auto getcords = [rc](int r, int c, int& rx, int& ry) {
            const int STARTPOSX = STARTX;
            const int STARTPOSY = STARTY;
            rx = STARTPOSX + c * 32;
            ry = STARTPOSY + r * 16;
        };

        getcords(matrix.block.y, matrix.block.x, bx, by);
        SDL_Rect blockRect1 = { bx, by, blockWidth, blockHeight };
        SDL_RenderCopy(app.ren, blocks[matrix.block.color.c1], nullptr, &blockRect1);

        getcords(matrix.block.y + 1, matrix.block.x, bx, by);
        SDL_Rect blockRect2 = { bx, by, blockWidth, blockHeight };
        SDL_RenderCopy(app.ren, blocks[matrix.block.color.c2], nullptr, &blockRect2);

        getcords(matrix.block.y + 2, matrix.block.x, bx, by);
        SDL_Rect blockRect3 = { bx, by, blockWidth, blockHeight };
        SDL_RenderCopy(app.ren, blocks[matrix.block.color.c3], nullptr, &blockRect3);

        SDL_Color white {255,255,255,255};
        app.printText(200, 60, "Score: " + std::to_string(matrix.Game.score), white);
        app.printText(310, 60, "Lines: " + std::to_string(matrix.Game.lines), white);
        blockProc();
         if (startTime2 == 0) {    
            startTime2 = SDL_GetTicks();
        }

        waitTime2 = matrix.Game.speed;

        Uint32 elapsedTime = SDL_GetTicks() - startTime2;
        if (elapsedTime > waitTime2) {
            elapsedTime = waitTime2;
        }
        if (elapsedTime >= waitTime2) {
            matrix.block.MoveDown();
            startTime2 = 0;
            matrix.clearBlocks();
        }

        if(matrix.Game.isGameOver()) 
            cur_screen = 0;
    }

    void MasterPiece::blockProc() {
        if (matrix.block.y + 2 >= 16) {
            matrix.setblock();
            return;
        }

        if (matrix.block.y + 3 < 17 && matrix.Tiles[matrix.block.y + 3][matrix.block.x] != 0) {
            matrix.setblock();
            return;
        }

        for (int i = 0; i < 17; i++) {
            for (int j = 0; j < 8 - 2; j++) {
                int cur_color = matrix.Tiles[i][j];
                if (cur_color != BLOCK_BLACK && cur_color != BLOCK_FADE) {
                    if (matrix.Tiles[i][j + 1] == cur_color && matrix.Tiles[i][j + 2] == cur_color) {
                        matrix.Game.addline();
                        matrix.Tiles[i][j] = BLOCK_FADE;
                        matrix.Tiles[i][j + 1] = BLOCK_FADE;
                        matrix.Tiles[i][j + 2] = BLOCK_FADE;
                        return;
                    }
                }
            }
        }

        for (int z = 0; z < 8; z++) {
            for (int q = 0; q < 17 - 2; q++) {
                int cur_color = matrix.Tiles[q][z];
                if (cur_color != BLOCK_BLACK && cur_color != BLOCK_FADE) {
                    if (matrix.Tiles[q + 1][z] == cur_color && matrix.Tiles[q + 2][z] == cur_color) {
                        matrix.Tiles[q][z] = BLOCK_FADE;
                        matrix.Tiles[q + 1][z] = BLOCK_FADE;
                        matrix.Tiles[q + 2][z] = BLOCK_FADE;
                        matrix.Game.addline();
                        return;
                    }
                }
            }
        }

        for (int p = 0; p < 8; p++) {
            for (int w = 0; w < 17; w++) {
                int cur_color = matrix.Tiles[w][p];

                if (cur_color != 0 && cur_color != BLOCK_FADE) {
                    if (w + 2 < 17 && p + 2 < 8) {
                        if (matrix.Tiles[w + 1][p + 1] == cur_color && matrix.Tiles[w + 2][p + 2] == cur_color) {
                            matrix.Tiles[w][p] = BLOCK_FADE;
                            matrix.Tiles[w + 1][p + 1] = BLOCK_FADE;
                            matrix.Tiles[w + 2][p + 2] = BLOCK_FADE;
                            matrix.Game.addline();
                        }
                    }

                    if (w - 2 >= 0 && p - 2 >= 0) {
                        if (matrix.Tiles[w - 1][p - 1] == cur_color && matrix.Tiles[w - 2][p - 2] == cur_color) {
                            matrix.Tiles[w][p] = BLOCK_FADE;
                            matrix.Tiles[w - 1][p - 1] = BLOCK_FADE;
                            matrix.Tiles[w - 2][p - 2] = BLOCK_FADE;
                            matrix.Game.addline();
                        }
                    }

                    if (w - 2 >= 0 && p + 2 < 8) {
                        if (matrix.Tiles[w - 1][p + 1] == cur_color && matrix.Tiles[w - 2][p + 2] == cur_color) {
                            matrix.Tiles[w][p] = BLOCK_FADE;
                            matrix.Tiles[w - 1][p + 1] = BLOCK_FADE;
                            matrix.Tiles[w - 2][p + 2] = BLOCK_FADE;
                            matrix.Game.addline();
                        }
                    }

                    if (w + 2 < 17 && p - 2 >= 0) {
                        if (matrix.Tiles[w + 1][p - 1] == cur_color && matrix.Tiles[w + 2][p - 2] == cur_color) {
                            matrix.Tiles[w][p] = BLOCK_FADE;
                            matrix.Tiles[w + 1][p - 1] = BLOCK_FADE;
                            matrix.Tiles[w + 2][p - 2] = BLOCK_FADE;
                            matrix.Game.addline();
                        }
                    }
                }
            }
        }

        matrix.ProccessBlocks();
    }

}