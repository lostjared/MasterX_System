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
            app.printText(25, 25, "Score: " + std::to_string(score), {255,255,255,255});
        }

        void PongWindow::newGame() {
            score = 0;
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
            dim_c = dim->createDimension(app, "Pong", false, false, loadTexture(app, "images/alienchip.png"), loadTexture(app, "images/xicon.png"));
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
            pong_window->setIcon(loadTexture(app, "images/xicon.png"));
            pong_window->setSystemBar(dim->system_bar);
            pong_window->setReload(true);
            pong_window->setCanResize(false);

            Menu_ID game_id = pong_window->menu.addHeader(create_header("Game"));
            pong_window->menu.addItem(game_id, pong_window->menu.addIcon(loadTexture(app, "images/xicon.png")), create_menu_item("New Game", [](mxApp &app, Window *win, SDL_Event &e) -> bool {
                static PongWindow *p = nullptr;
                p = dynamic_cast<PongWindow *>(win);
                if(!p) {
                    mx::system_err << "MasterX System: Bad cast..\n";
                    mx::system_err.flush();
                    exit(EXIT_FAILURE);
                }
                MX_MessageBox::OkCancelMX_MessageBox(app, win->dim, "Start a new game?", "Start a new game?", [&](mxApp &app, Window *win, int ok) -> bool {
                   // p->lives = 3;
                   // p->level = 1;
                   // p->newGame();
                    return true;
                });
                return true;
            }));
            Menu_ID help_id = pong_window->menu.addHeader(create_header("Help"));
            pong_window->menu.addItem(help_id, pong_window->menu.addIcon(loadTexture(app, "images/xicon.png")), create_menu_item("About", [](mxApp &app, Window *win, SDL_Event &e) -> bool {
                MX_MessageBox::OkMX_MessageBox(app, win->dim, "About Pong", "Pong created by Jared Bruni");
                return true;
            }));
            pong_window->show(true);
        }


}