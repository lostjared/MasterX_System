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

    void MasterPiece::screenResize(int w, int h) {
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

    void MasterPiece::setScreen(int scr) {
        cur_screen = scr;
    }   

    bool MasterPiece::event(mxApp &app, SDL_Event &e) {
        SDL_Rect ir;
        Window::getRect(ir);
        static int x1 = 0, y1x = 0, x2 = 0, y2 = 0;
        static const int drag_threshold = 15;
        if (cur_screen == 2 && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            x1 = e.button.x - ir.x;
            y1x = e.button.y - ir.y;
        }
        else if (cur_screen == 2 && e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            SDL_Point p = { e.button.x, e.button.y };
            if(!SDL_PointInRect(&p, & ir)) {
                return false;
            }
            x2 = e.button.x - ir.x;
            y2 = e.button.y - ir.y;
            if (abs(y1x - y2) > abs(x1 - x2)) {
                if (y1x < y2 && abs(y1x - y2) > drag_threshold) {
                    x1 = x2;
                    y1x = y2;
                    matrix.block.MoveDown();
                }
                else if (y1x > y2 && abs(y2 - y1x) > drag_threshold) {
                    matrix.block.color.shiftcolor(true);
                    x1 = x2;
                    y1x = y2;
                }
            } else {
                if (x1 < x2 && abs(x1 - x2) > drag_threshold) {
                    x1 = x2;
                    y1x = y2;
                    matrix.block.MoveRight();
                }
                else if (x1 > x2 && abs(x2 - x1) > drag_threshold) {
                    matrix.block.MoveLeft();
                    x1 = x2;
                    y1x = y2;
                }
            }
        }

        switch(e.type) {
            case SDL_MOUSEMOTION: {

                switch(cur_screen) {
                    case 1: {
                        SDL_Rect startMeRect;
                        int rectWidth = static_cast<int>(ir.w * 0.2); 
                        int rectHeight = static_cast<int>(ir.h * 0.1);
                        startMeRect.x = (ir.w / 2) - (rectWidth / 2);
                        startMeRect.y = (ir.h / 2) - (rectHeight / 2);
                        startMeRect.w = rectWidth;
                        startMeRect.h = rectHeight;
                        SDL_Point p = {e.motion.x - ir.x, e.motion.y - ir.y};
                        if(SDL_PointInRect(&p, &startMeRect)) {
                            show_cursor = true;
                        } else {
                            show_cursor = false;
                        }
                    }
                    default:
                    break;
                }
            }
                break;
            case SDL_MOUSEBUTTONUP:
                    if(cur_screen != 1) {
                        if(show_cursor){
                            show_cursor = false;
                        }
                    }
            break;
            case SDL_MOUSEBUTTONDOWN: {
                if(e.button.button == SDL_BUTTON_LEFT && isPointInside(e.button.x, e.button.y)) {
                    switch(cur_screen) {
                        case 1:
                        switch(cur_screen) {
                            case 1: {
                                SDL_Rect startMeRect;
                                int rectWidth = static_cast<int>(ir.w * 0.2); 
                                int rectHeight = static_cast<int>(ir.h * 0.1);
                                startMeRect.x = (ir.w / 2) - (rectWidth / 2);
                                startMeRect.y = (ir.h / 2) - (rectHeight / 2);
                                startMeRect.w = rectWidth;
                                startMeRect.h = rectHeight;

                                SDL_Point p = {e.button.x - ir.x, e.button.y - ir.y};
                                if(SDL_PointInRect(&p, &startMeRect)) {
                                    newGame();
                                }
                            }
                            break;
                        }
                        break;
                    }
                }
            }
                break;
            case SDL_KEYDOWN:
            switch(e.key.keysym.sym) {
                case SDLK_UP:
                    switch(cur_screen) {
                        case 2:
                            matrix.block.color.shiftcolor(true);
                        break;
                    }
                break;
                case SDLK_DOWN:
                    switch(cur_screen) {
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
                case SDLK_SPACE:
                    switch(cur_screen) {
                        case 4:
                        case 3:
                        cur_screen = 1;
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

        if(cur_screen == 1 && show_cursor) {
            cursor_shown = true;
        }

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
            case 4:
            draw_credits(app);
            break;
            case 3:
            draw_options(app);
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

        static const char* grid_block_paths[12] = {
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
        alien = loadTexture(app, "images/mp_dat/mp.alien.png");
    }

    void MasterPiece::release_gfx() {
        if(logo != nullptr)
            SDL_DestroyTexture(logo);
        if(start != nullptr)
            SDL_DestroyTexture(start);
        if(cursor != nullptr) 
            SDL_DestroyTexture(cursor);
        if(gamebg != nullptr) 
            SDL_DestroyTexture(gamebg);

        for(int i = 0; i < 11; ++i) {
            if(blocks[i] != nullptr) 
                SDL_DestroyTexture(blocks[i]);
        }
        if(alien != nullptr) {
            SDL_DestroyTexture(alien);
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
            startTime        = 0;
        }
    }

    void MasterPiece::draw_credits(mxApp &app) {
        SDL_Rect rc = {0, 0, 640, 480};
        SDL_RenderCopy(app.ren, start, nullptr, &rc);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
        SDL_Rect rc2 = { 35, 92,621-35,480-130 };
        SDL_RenderFillRect(app.ren, &rc2);
        SDL_Color color = { 255, 255, 255, 255 };
        SDL_Color color2 = { 255, 0, 0, 255 };
        SDL_Color color3 = {0,0,255, 255};

        app.printText( 60, 110, "MasterPiece MX Edition", color);
        app.printText( 60, 135, "written by Jared Bruni", color2);
        app.printText( ( 640 /2 ) - (200/2) + 15, 200, "[ Press Space to Return ]", color3);

        SDL_Rect rc3 = { (640/2)-(150/2), 225, 150, 150 };
        SDL_RenderCopy(app.ren, alien, nullptr, &rc3);
    }
    void MasterPiece::draw_options(mxApp &app) {
        SDL_Rect rc = {0, 0, 640, 480};
        SDL_RenderCopy(app.ren, start, nullptr, &rc);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
        SDL_Rect rc2 = { 35, 92,621-35,480-130 };
        SDL_RenderFillRect(app.ren, &rc2);
        SDL_Color color = { 255, 255, 255, 255 };
        app.printText( 60, 110, "[ Options Menu - Space to Return ]", color);
    }

    void MasterPiece::draw_start(mxApp &app) {
        SDL_Rect drc = {0, 0, 640, 480};
        SDL_RenderCopy(app.ren, start,  nullptr, &drc);
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

        auto getcords = [](int r, int c, int& rx, int& ry) {
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