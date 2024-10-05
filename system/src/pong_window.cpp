#include "pong_window.hpp"
#include "dimension.hpp"
#include "messagebox.hpp"

namespace mx {

        PongWindow::PongWindow(mxApp &app) : Window(app) {
            draw_tex = SDL_CreateTexture(app.ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 640, 480);
            if(!draw_tex) {
                mx::system_err << "MasterX System: Error creating texture in PacAttack.\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }

            playerPaddleX = 50;  
            playerPaddleY = (windowHeight / 2) - (paddleHeight / 2);  
            aiPaddleX = windowWidth - 50 - paddleWidth;  
            aiPaddleY = (windowHeight / 2) - (paddleHeight / 2); 
            ballX = windowWidth / 2;
            ballY = windowHeight / 2;
            ballVelX = (rand() % 2 == 0) ? -ballSpeed : ballSpeed;
            ballVelY = (rand() % 2 == 0) ? -ballSpeed : ballSpeed;
            playerScore = 0;
            aiScore = 0;
        }

        PongWindow::~PongWindow() {
            if(draw_tex != nullptr) {
                SDL_DestroyTexture(draw_tex);
            }
        }

        void PongWindow::draw(mxApp &app) {
            if(!Window::isVisible())
                return;

            if(!Window::isDraw())
                return;

            Window::draw(app);
            Window::drawMenubar(app);
            SDL_SetRenderTarget(app.ren, draw_tex);
            SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 255);
            SDL_RenderClear(app.ren);
            drawGame(app);
            SDL_SetRenderTarget(app.ren, app.tex);
            SDL_Rect drc;
            Window::getDrawRect(drc);
            SDL_RenderCopy(app.ren, draw_tex, nullptr, &drc);
        }

        void PongWindow::drawGame(mxApp &app) {

        
            SDL_Rect playerPaddle = {playerPaddleX, playerPaddleY, paddleWidth, paddleHeight};
            SDL_Rect aiPaddle = {aiPaddleX, aiPaddleY, paddleWidth, paddleHeight};
            SDL_Rect ball = {ballX, ballY, ballSize, ballSize};

            SDL_SetRenderDrawColor(app.ren, 255, 255, 255, 255); 
            SDL_RenderFillRect(app.ren, &playerPaddle); 
            SDL_RenderFillRect(app.ren, &aiPaddle);    
            SDL_RenderFillRect(app.ren, &ball);        

            static Uint32 last = SDL_GetTicks();
            Uint32 current = SDL_GetTicks();
            if(current-last >= 25) {
                ballX += ballVelX;
                ballY += ballVelY;

                if (ballY <= 0 || ballY >= windowHeight - ballSize) {
                    ballVelY = -ballVelY;
                }

                if (ballVelX < 0 &&  
                ballX <= playerPaddleX + paddleWidth &&  
                ballX + ballSize >= playerPaddleX &&     
                ballY + ballSize >= playerPaddleY &&     
                ballY <= playerPaddleY + paddleHeight) { 
                    ballVelX = -ballVelX;  
                    ballX = playerPaddleX + paddleWidth;
                }

                if (ballVelX > 0 &&  
                ballX + ballSize >= aiPaddleX &&  
                ballX <= aiPaddleX + paddleWidth &&  
                ballY + ballSize >= aiPaddleY &&     
                ballY <= aiPaddleY + paddleHeight) { 
                    ballVelX = -ballVelX;  
                    ballX = aiPaddleX - ballSize;
                }
                
                if (ballX <= 0) {
                    aiScore++;
                    resetGame();
                } else if (ballX >= windowWidth) {
                    playerScore++;
                    resetGame();
                }

                const Uint8* state = SDL_GetKeyboardState(NULL);
                if (state[SDL_SCANCODE_UP] && playerPaddleY > 0) {
                    playerPaddleY -= paddleSpeed;
                } else if (state[SDL_SCANCODE_DOWN] && playerPaddleY < windowHeight - paddleHeight) {
                    playerPaddleY += paddleSpeed;
                }

                if (ballY < aiPaddleY && aiPaddleY > 0) {
                    aiPaddleY -= aiPaddleSpeed;
                } else if (ballY > aiPaddleY + paddleHeight && aiPaddleY < windowHeight - paddleHeight) {
                    aiPaddleY += aiPaddleSpeed;
                }
                last = current;
            }

            app.printText(125, 25, "Player Score: " + std::to_string(playerScore), {255,255,255,255});
            app.printText(350, 25, "AI Score: " + std::to_string(aiScore), {255,255,255,255});
      
        }

        void PongWindow::newGame() {
            playerScore = 0;
            aiScore = 0;
            resetGame();
        }

        void PongWindow::resetGame() {
            ballX = 640 / 2;
            ballY = 480 / 2;
            ballVelX = (rand() % 2 == 0) ? -ballSpeed : ballSpeed; 
            ballVelY = (rand() % 2 == 0) ? -ballSpeed : ballSpeed;
        }

        bool PongWindow::event(mxApp &app, SDL_Event &e) {
            return Window::event(app, e);
        }

        void PongWindow::screenResize(int w, int h) {
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
            SDL_Rect rc={windowPosX, windowPosY-25, windowWidth, windowHeight};
            this->setRect(rc);
        }

        DimensionContainer *PongWindow::dim_c = nullptr;
        PongWindow *PongWindow::pong_window = nullptr;

        void PongWindow::main(mxApp &app, Dimension *dim) {
            dim_c = dim->createDimension(app, "Pong", false, false, loadTexture(app, "images/pong.png"), loadTexture(app, "images/pongico.png"));
            if(!dim_c) {
                mx::system_err << "MasterX: Failed to create dimension.\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            dim_c->objects.push_back(std::make_unique<PongWindow>(app));
            pong_window = dynamic_cast<PongWindow *>(dim_c->objects[dim_c->objects.size()-1].get());
            if(!pong_window) {
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
            int windowWidth = static_cast<int>(640 * scaleX);
            int windowHeight = static_cast<int>(480 * scaleY);
            int windowPosX = (screenWidth - windowWidth) / 2;
            int windowPosY = (screenHeight - windowHeight) / 2;
            pong_window->create(dim_c, "Pong", windowPosX, windowPosY-50, windowWidth, windowHeight);
            dim_c->events.addWindow(pong_window);
            pong_window->setIcon(loadTexture(app, "images/pongico.png"));
            pong_window->setSystemBar(dim->system_bar);
            pong_window->setReload(true);
            pong_window->setCanResize(false);

            Menu_ID game_id = pong_window->menu.addHeader(create_header("Game"));
            pong_window->menu.addItem(game_id, pong_window->menu.addIcon(loadTexture(app, "images/pongico.png")), create_menu_item("New Game", [](mxApp &app, Window *win, SDL_Event &e) -> bool {
                static PongWindow *p = nullptr;
                p = dynamic_cast<PongWindow *>(win);
                if(!p) {
                    mx::system_err << "MasterX System: Bad cast..\n";
                    mx::system_err.flush();
                    exit(EXIT_FAILURE);
                }
                MX_MessageBox::OkCancelMX_MessageBox(app, win->dim, "Start a new game?", "Start a new game?", [&](mxApp &app, Window *win, int ok) -> bool {
                    p->newGame();
                    return true;
                });
                return true;
            }));
            Menu_ID help_id = pong_window->menu.addHeader(create_header("Help"));
            pong_window->menu.addItem(help_id, pong_window->menu.addIcon(loadTexture(app, "images/pongico.png")), create_menu_item("About", [](mxApp &app, Window *win, SDL_Event &e) -> bool {
                MX_MessageBox::OkMX_MessageBox(app, win->dim, "About Pong", "Pong created by Jared Bruni");
                return true;
            }));
            pong_window->show(true);
        }


}