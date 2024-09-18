#include "dimension.hpp"
#include "terminal.hpp"
#include "mx_controls.hpp"
#include "window.hpp"
#include "SDL_rect.h"
#include"mx_window.hpp"
#include"mx_system_bar.hpp"
#include"mx_abstract_control.hpp"
#include"masterpiece.hpp"
#include<algorithm>
#include<unordered_set>
#include<random>

namespace mx {

    bool cursor_shown = false;
  
    DimensionContainer::DimensionContainer(mxApp &app) : wallpaper{nullptr} , events{app}, active{false} {}

    DimensionContainer::~DimensionContainer() {
        mx::system_out << "MasterX: Releasing Dimension: " << name << "\n";
        if(wallpaper) {
            SDL_DestroyTexture(wallpaper);
        }
        if(icon) {
            SDL_DestroyTexture(icon);
        }
    }

    void DimensionContainer::destroyWindow(Window *win) {
        events.removeWindow(win);
        for(auto it = mini_win.begin(); it != mini_win.end(); ++it) {
            Window *window = *it;
            if(window == win) {
                mini_win.erase(it);
                break;
            }
        }

        for(auto r = objects.begin(); r != objects.end(); ++r) {
            if(auto w = dynamic_cast<Window *>(r->get())) {
                if(w == win) {
                    objects.erase(r);
                    mx::system_out << "MasterX System: Destroyed Window..\n";
                    return;
                }
            }
        }
    }

    Window *DimensionContainer::createWindow(mxApp &app) {
        objects.push_back(std::make_unique<Window>(app));
        Window *win = dynamic_cast<Window *>(objects[objects.size()-1].get());
        if(win != nullptr) {
            events.addWindow(win);
        }
        return win;
    }

    bool DimensionContainer::isVisible() const { return visible; }

    void DimensionContainer::init(SystemBar *sbar, const std::string &name_, SDL_Texture *wallpaperx) {
        name = name_;
        wallpaper = wallpaperx;
        system_bar = sbar;
    }
    
    void DimensionContainer::setActive(bool a) {
        active = a;
        if(active) {
            for (auto &i : objects) {
                if(auto win = dynamic_cast<Window *>(i.get())) {
                    if(win->reload()) {
                        if(win->isVisible() == false) {
                            win->activate();
                        }
                        win->show(true);
                    }
                }
            }
        }
    }

    bool DimensionContainer::isActive() const {
        return active;
    }

    void DimensionContainer::draw(mxApp &app) {
        if (!active) return;

        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);

        if (wallpaper != nullptr) {
            if (transitioning && nextWallpaper != nullptr) {
                SDL_SetTextureBlendMode(wallpaper, SDL_BLENDMODE_BLEND);
                SDL_SetTextureBlendMode(nextWallpaper, SDL_BLENDMODE_BLEND);
                SDL_SetTextureAlphaMod(wallpaper, 255 - transitionAlpha);
                SDL_SetTextureAlphaMod(nextWallpaper, transitionAlpha);
                SDL_RenderCopy(app.ren, wallpaper, nullptr, nullptr);
                SDL_RenderCopy(app.ren, nextWallpaper, nullptr, nullptr);
                transitionAlpha -= 5;
                if (transitionAlpha <= 0) {
                    transitioning = false;
                    SDL_SetTextureBlendMode(wallpaper, SDL_BLENDMODE_NONE);
                }
            } else {
                SDL_SetTextureBlendMode(wallpaper, SDL_BLENDMODE_NONE);
                SDL_RenderCopy(app.ren, wallpaper, nullptr, nullptr);
            }
        }

        if (visible) {
            events.sendDrawMessage();
        }
    }
    
    void DimensionContainer::startTransition(SDL_Texture* nextWp) {
        if (!transitioning) {
            nextWallpaper = nextWp;
            transitionAlpha = 255;
            transitioning = true;
        }
    }
    void DimensionContainer::updateTransition() {
        transitionAlpha -= transitionSpeed;  
        if (transitionAlpha <= 0) {
            transitionAlpha = 255;
            transitioning = false;
        }
    }
    
    void DimensionContainer::setIcon(SDL_Texture *icon) {
        this->icon = icon;
    }

    bool DimensionContainer::event(mxApp &app, SDL_Event &e) {

        if(active == false || visible == false) return false;
        if(events.pumpEvent(e))
            return true;

        return false;
    }

    void DimensionContainer::setVisible(bool v) {
        visible = v;
    }

    std::string Dimension::selectRandomImage(std::vector<std::string>& logos, std::mt19937& gen) {
        if (logos.empty()) {
            return "";  
        }
        std::uniform_int_distribution<> dis(0, logos.size() - 1);
        int random_index = dis(gen);
        std::string selected_image = logos[random_index];
        logos.erase(logos.begin() + random_index);
        return selected_image;
    }

    Dimension::Dimension(mxApp &app) : config(getPath("mx.cfg")) {
        wallpaper = loadTexture(app, config.itemAtKey("desktop", "wallpaper").value);
        objects.push_back(std::make_unique<SystemBar>(app));
        system_bar = dynamic_cast<SystemBar *>(objects[0].get());
        std::vector<std::string> logos = config.splitByComma(config.itemAtKey("desktop", "backgrounds").value);
        std::random_device rd;
        std::mt19937 gen(rd());
        dimensions.push_back(std::make_unique<DimensionContainer>(app));
        dash = dynamic_cast<DimensionContainer *>(getDimension());
        dash->init(system_bar, "Dashboard", loadTexture(app, "images/desktop.png"));
        dash->setActive(true);
        dash->setVisible(false);
        settings_window = dash->createWindow(app);
        settings_window->create(dash, "Settings", 25, 25, 320, 240);   
        settings_window->show(true);
        settings_window->setReload(true);
        settings_window->setCanResize(false);
        settings_window->removeAtClose(true);

        settings_window->children.push_back(std::make_unique<Button>(app));
        toggle_fullscreen = dynamic_cast<Button *>(settings_window->getControl());
        toggle_fullscreen->create(settings_window, "Toggle Fullscreen", 25, 50, 150, 20);
        toggle_fullscreen->setShow(true);
        toggle_fullscreen->setCallback([](mxApp &app, Window *parent, SDL_Event &e) -> bool {
                app.set_fullscreen(app.win, !app.full);
                return true;
        });
        dimensions.push_back(std::make_unique<DimensionContainer>(app));
        welcome = dynamic_cast<DimensionContainer *>(getDimension());
        welcome->init(system_bar, "Welcome", loadTexture(app, selectRandomImage(logos, gen)));
        welcome->setActive(false);
        welcome->setVisible(false);
        welcome_window = welcome->createWindow(app);
        welcome_window->create(welcome, "Welcome", 45, 25, 640, 480);
        welcome_window->show(true);
        welcome_window->setReload(false);
        welcome_window->setCanResize(true);
        welcome_window->removeAtClose(true);
        welcome_window->children.push_back(std::make_unique<Image>(app));
        welcome_image = dynamic_cast<Image *>(welcome_window->getControl());
        welcome_image->create(app, welcome_window, "images/welcome_logo.png", 45, 45);
        welcome_image->setGeometry(5, 8, 640-10, 480-38);
        welcome_window->children.push_back(std::make_unique<Button>(app));
        welcome_ok = dynamic_cast<Button *>(welcome_window->getControl());
        SDL_Rect i_rc;
        welcome_window->getRect(i_rc);
        welcome_ok->create(welcome_window, "Dismiss", i_rc.w-110, i_rc.h-35, 100, 25);
        welcome_ok->setShow(true);
        welcome_ok->setCallback([](mxApp &app, Window *parent, SDL_Event &e) -> bool {
            parent->show(false);
            return true;
        });
        welcome_ok->setResizeCallback([&](Window *parent, int x, int y) -> void {
            SDL_Rect rc,src;
            parent->getRect(rc);
            welcome_ok->setGeometry(rc.w - 110, rc.h - 40, 100, 30);
            welcome_image->getRect(src);
            welcome_image->setGeometry(src.x, src.y, rc.w-10, rc.h-38);    
            welcome_image->setWindowPos(rc.x, rc.y);
        });

        welcome_help = welcome->createWindow(app);
        welcome_help->create(welcome, "Info", 1280-360, 25, 320, 240);
        welcome_help->show(true);
        welcome_help->setReload(false);
        welcome_help->setCanResize(false);
        welcome_help->removeAtClose(true);

        welcome_help->children.push_back(std::make_unique<Label>(app));
        welcome_help_info = dynamic_cast<Label *>(welcome_help->getControl());
        std::vector<std::string> info_help {"MasterX System", "Created by Jared Bruni", "Virtual Environment", "written in C++20", "https://lostsidedead.biz"};
        welcome_help_info->create_multi(welcome_help, info_help, { 0, 0, 0, 255}, 25, 25);
        welcome_help_info->loadFont("fonts/arial.ttf", 16);
        welcome_help_info->linkMode(false);
        

        dimensions.push_back(std::make_unique<DimensionContainer>(app));
        about = dynamic_cast<DimensionContainer *>(getDimension());
        about->init(system_bar, "About", loadTexture(app, selectRandomImage(logos, gen)));
        about->setActive(false);
        about->setVisible(false);
        about_window = about->createWindow(app);
        int centered_x = (app.width - 800) / 2;
        int centered_y = (app.height - 600) / 2;
        about_window->create(about, "About", centered_x, centered_y-35, 800, 600);
        about_window->show(true);
        about_window->setReload(false);
        about_window->removeAtClose(true);
        about_window->children.push_back(std::make_unique<Image>(app));
        Image *image = dynamic_cast<Image *>(about_window->getControl());
        image->create(app, about_window, "images/logo.png", 0, 0);
        image->setGeometry(5, 8, 800-10, 600-38);

        about_window->children.push_back(std::make_unique<Button>(app));
        about_window_ok = dynamic_cast<Button *>(about_window->getControl());

        about_window_ok->create(about_window, "Ok", 800-110, 600-35, 100, 25);
        about_window_ok->setShow(true);
        about_window_ok->setCallback([](mxApp &app, Window *parent, SDL_Event &e) -> bool {
            parent->show(false);
            return false;
        });
        about_window->children.push_back(std::make_unique<Label>(app));
        about_window_info = dynamic_cast<Label *>(about_window->getControl());
        std::vector<std::string> info_text {"MasterX System", "written by Jared Bruni", "(C) 2024 LostSideDead Software", "https://lostsidedead.biz", "\"Open Source, Open Mind\""};
        about_window_info->create_multi(about_window, info_text, { 255,255,255,255}, 25, 25 );
        about_window_info->loadFont("fonts/arial.ttf", 36);
        about_window_info->linkMode(false);
        about_window->setCanResize(false);
        dimensions.push_back(std::make_unique<DimensionContainer>(app));
        term = dynamic_cast<DimensionContainer *>(getDimension());
        SDL_Texture *term_tex = loadTexture(app, selectRandomImage(logos, gen));
        term->init(system_bar, "Terminal", term_tex);
        term->setActive(false);
        term->setVisible(false);
        system_bar->activateDimension(1);
        system_bar->activateDimension(0);
        term->objects.push_back(std::make_unique<Terminal>(app)); 
        termx = dynamic_cast<Terminal*>(term->objects[0].get());
        term->events.addWindow(termx);
        term->setIcon(loadTexture(app, "images/term.png"));
        termx->create(term, "mXTerm", (1280 - 800) / 2, (720 - 600) / 2, 800, 505);
        termx->show(true);
        termx->setReload(true);
        termx->setIcon(loadTexture(app, "images/term.png"));
        dimensions.push_back(std::make_unique<DimensionContainer>(app));
        piece_cont = dynamic_cast<DimensionContainer *>(getDimension());
        SDL_Texture *rtex = loadTexture(app, "images/mp_dat/mp_wall.png");
        piece_cont->init(system_bar, "MastePiece", rtex);
        piece_cont->setActive(false);
        piece_cont->setVisible(false);
        piece_cont->objects.push_back(std::make_unique<MasterPiece>(app));
        piece = dynamic_cast<MasterPiece *>(piece_cont->objects[0].get());
        piece_cont->events.addWindow(piece);
        piece_cont->setIcon(loadTexture(app, "images/mp_dat/block_red.png"));
        piece->create(piece_cont, "MasterPiece", (1280/2) -(640/2), (720/2) - (480/2) - 35, 640, 480);
        piece->show(true);
        piece->setReload(true);
        piece->setIcon(loadTexture(app, "images/mp_dat/block_dblue.png"));

        piece->setSystemBar(system_bar);
        termx->setWallpaper(term_tex);
        system_bar->setDimensions(&dimensions);
        welcome_window->setSystemBar(system_bar);
        welcome_help->setSystemBar(system_bar);
        about_window->setSystemBar(system_bar);
        termx->setSystemBar(system_bar);
        setCurrentDimension(1);
        system_bar->activateDimension(1);
        SDL_Surface *hand_cursor_surf = SDL_LoadBMP(getPath("images/hand.bmp").c_str());
        if(hand_cursor_surf == nullptr) {
            mx::system_err << "MasterX: Error loading cursor..\n";
            exit(EXIT_FAILURE);
        }
        SDL_SetColorKey(hand_cursor_surf, SDL_TRUE, SDL_MapRGB(hand_cursor_surf->format, 0, 128, 128));
        hand_cursor = SDL_CreateTextureFromSurface(app.ren, hand_cursor_surf);
        SDL_FreeSurface(hand_cursor_surf);
        if(hand_cursor == nullptr) {
            mx::system_err << "MasterX System: Error creating texture from surface...\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        SDL_Surface *cursor_surf = SDL_LoadBMP(getPath("images/cursor.bmp").c_str());
        if(cursor_surf == nullptr) {
            mx::system_err << "MasterX: Error loading cursor..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        SDL_SetColorKey(cursor_surf, SDL_TRUE, SDL_MapRGB(cursor_surf->format, 0, 128, 128));
        reg_cursor = SDL_CreateTextureFromSurface(app.ren, cursor_surf);
        SDL_FreeSurface(cursor_surf);
        if(reg_cursor == nullptr) {
            mx::system_err << "MasterX System: Error creating texture from surface...\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
    }

    void Dimension::setCurrentDimension(int dim) {
        system_bar->setCurrentDimension(dim);
    }
        
    int  Dimension::getCurrentDimension() const {
        return system_bar->getCurrentDimension();
    }
        

    Dimension::~Dimension() {
        if(wallpaper != nullptr) {
            SDL_DestroyTexture(wallpaper);
        }
        if(hand_cursor != nullptr) {
            SDL_DestroyTexture(hand_cursor);
        }
        if(reg_cursor != nullptr) {
            SDL_DestroyTexture(reg_cursor);
        }
        mx::system_out << "MasterX: Releasing Dimensions\n";
    }

    void Dimension::drawDash(mxApp &app) {
        SDL_SetRenderTarget(app.ren, app.tex);
        SDL_RenderCopy(app.ren, wallpaper, nullptr, nullptr);
    }

    void Dimension::draw(mxApp &app) {
        cursor_shown = false;
        int cur = getCurrentDimension();
        if(cur >= 0 && cur < static_cast<int>(dimensions.size())) {
            if(system_bar->empty()) 
                drawDash(app);
            else
                dimensions[cur]->draw(app);
        }
        for (auto &i : objects) {
            i->draw(app);
        }
        SDL_SetRenderTarget(app.ren, app.tex);
        SDL_Rect rc = { cursor_x, cursor_y, 32, 32 };
        if(cursor_shown) { 
            SDL_RenderCopy(app.ren, hand_cursor, nullptr, &rc);
        } else {
            SDL_RenderCopy(app.ren, reg_cursor, nullptr, &rc);
        }
        SDL_SetRenderTarget(app.ren, nullptr);
    }

   Screen *Dimension::getDimension() {
        if(dimensions.size()>0)
            return dimensions[dimensions.size()-1].get();
        mx::system_err << "MasterX System: Trying to access out of bounds dimension.\n";
        return nullptr;
   }
    
    Screen *Dimension::getDimension(int index) {
        if(index >= 0 && index < static_cast<int>(dimensions.size()))
            return dimensions[index].get();

        mx::system_err << "MasterX System: Trying to access out of bounds dimension.\n";
        return nullptr;
    }

    bool Dimension::event(mxApp &app, SDL_Event &e) {
        for (auto &i : objects) {
            if(i->event(app, e))
                return true;
        }
        int cur = getCurrentDimension();
        if(cur >= 0 && cur < static_cast<int>(dimensions.size())) {
            dimensions[cur]->event(app, e);
        }
        if(e.type == SDL_MOUSEMOTION) {
            cursor_x = e.motion.x;
            cursor_y = e.motion.y;
        }
        return false;
    }
}