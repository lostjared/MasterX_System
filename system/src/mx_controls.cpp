#include "mx_controls.hpp"
#include "mx_window.hpp"

namespace mx {

       Label::Label(mxApp &app)
        : font_(nullptr), name_(""), text_(""), x(0), y(0), size_(0), wx(0), wy(0), w(0), h(0) {
            parent = nullptr;
        }

    Label::~Label() {
        if(font_ != nullptr)
            TTF_CloseFont(font_);
    }

    void Label::setWindowPos(int xx, int yy) {
        wx = xx;
        wy = yy;
    }

    void Label::draw(mxApp &app) {

        if(!show)
            return;


        if(font_ != nullptr && text_.length() > 0 && multi_lined == false) {
            if(mode == false || under_ == false) {
                TTF_SetFontStyle(font_, TTF_STYLE_NORMAL);
            } else if(under_ == true) {
                cursor_shown = true;
                TTF_SetFontStyle(font_, TTF_STYLE_UNDERLINE);
            }

            SDL_Surface *surf = TTF_RenderText_Blended(font_, text_.c_str(), color_);
            if(surf == nullptr) {
                mx::system_err << "MasterX System: Error creating surface.\n";
                mx::system_err.flush();
                return;
            }
            int sw = surf->w;
            int sh = surf->h;
            w = sw;
            h = sh;
            SDL_Texture *t = SDL_CreateTextureFromSurface(app.ren, surf);
            if(t == nullptr) {
                mx::system_err << "MasterX System: Error creating texture.\n";
                mx::system_err.flush();
                return;
            }
            SDL_FreeSurface(surf);
            SDL_SetRenderTarget(app.ren, app.tex);
            SDL_Rect point = {x+wx,y+wy+25,sw,sh};
            SDL_RenderCopy(app.ren, t, nullptr, &point);
            SDL_DestroyTexture(t);

        } else if(font_ != nullptr && multi_text.size()>0 && multi_lined == true) {
            int off_y = 0;
            int height = TTF_FontHeight(font_);
                
            for(int i = 0; i < static_cast<int>(multi_text.size()); ++i) {
                if(multi_text[i].length()==0) continue;
                SDL_Surface *surf = TTF_RenderText_Blended(font_, multi_text[i].c_str(), color_);
                if(surf == nullptr) {
                    mx::system_err << "MasterX System: Error creating surface.\n";
                    mx::system_err.flush();
                    return;
                }
                int sw = surf->w;
                int sh = surf->h;
                w = sw;
                h = sh;
                SDL_Texture *t = SDL_CreateTextureFromSurface(app.ren, surf);
                if(t == nullptr) {
                    mx::system_err << "MasterX System: Error creating texture.\n";
                    mx::system_err.flush();
                    return;
                }
                SDL_FreeSurface(surf);
                SDL_SetRenderTarget(app.ren, app.tex);
                SDL_Rect point = {x+wx,y+wy+25+off_y+height,sw,sh};
                SDL_RenderCopy(app.ren, t, nullptr, &point);
                SDL_DestroyTexture(t);
                off_y += height;
            }
        }
    }

    void Label::linkMode(bool m) {
        mode = m;
    }

    void Label::create(Window *parent, const std::string &text, SDL_Color col, int xx, int yy) {
        setText(text, col);
        setGeometry(xx, yy);
        this->parent = parent;
        multi_lined = false;
        color_ = col;
    }
    
    void Label::create_multi(Window *parent, std::vector<std::string>  &t, SDL_Color col, int xx,  int yy) {
        setMultiLine(t);
        setGeometry(xx,yy);
        this->parent = parent;
        multi_lined = true;
        color_ = col;
    }

    bool Label::event(mxApp &app, SDL_Event &e) {

        if(!show) return false;

        if(e.type == SDL_MOUSEMOTION) {
            int mx = e.motion.x;
            int my = e.motion.y;
            SDL_Rect rc = {x+wx, y+wy+25, w, h };
            SDL_Point cp = { mx, my };
            if(SDL_PointInRect(&cp, &rc)) {
                cursor_shown = true;
                under_ = true;
                return true;
            } else {
                under_ = false;
            }
        }


        return false;
    }

    void Label::setMultiLine(std::vector<std::string> &v) {
        multi_text = std::move(v);
    }

    void Label::loadFont(const std::string &name, int sizex) {
        name_ = name;
        size_ = sizex;
        font_ = TTF_OpenFont(getPath(name).c_str(), size_);
        if(font_ == nullptr) {
            mx::system_err << "Error opening font: " << getPath(name) << "\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
    }

    void Label::setText(const std::string &text, SDL_Color col) {
        text_ = text;
        color_ = col;
    }

    void Label::setGeometry(int xx, int yy) {
        x = xx;
        y = yy;
    }

    Button::Button(mxApp &app) 
        : x(0), y(0), w(0), h(0),wx(0), wy(0), hover(false), pressed(false) {
        fgColor = { 200, 200, 200, 255 };  
        bgColor = { 240, 240, 240, 255 };  
        hover_fg = { 180, 180, 180, 255 };  
        hover_bg = { 220, 220, 220, 255 };  
        pressed_fg = { 120, 120, 120, 255 };  
        pressed_bg = { 200, 200, 200, 255 };  
        textColor = { 0, 0, 0, 255 };        
        parent = nullptr;
    }

    Button::~Button() {}

    void Button::draw(mxApp &app) {
        if (!show) return;

        SDL_Renderer* renderer = app.ren;
        SDL_Rect rect = { x+wx, y+wy, w, h };
        
        SDL_Color topLeftColor = fgColor;
        SDL_Color bottomRightColor = { 100, 100, 100, 255 };  

        if (pressed) {
            SDL_SetRenderDrawColor(renderer, pressed_bg.r, pressed_bg.g, pressed_bg.b, pressed_bg.a);
            topLeftColor = pressed_fg;
            bottomRightColor = { 150, 150, 150, 255 }; 
        } else if (hover) {
            SDL_SetRenderDrawColor(renderer, hover_bg.r, hover_bg.g, hover_bg.b, hover_bg.a);
        } else {
            SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
        }
        
        SDL_RenderFillRect(renderer, &rect);
        
        int cx = x + wx;
        int cy = y + wy;
        
        SDL_SetRenderDrawColor(renderer, topLeftColor.r, topLeftColor.g, topLeftColor.b, topLeftColor.a);
        SDL_RenderDrawLine(renderer, cx, cy, cx + w - 1, cy);  
        SDL_RenderDrawLine(renderer, cx, cy, cx, cy + h - 1);  
        
        SDL_SetRenderDrawColor(renderer, bottomRightColor.r, bottomRightColor.g, bottomRightColor.b, bottomRightColor.a);
        SDL_RenderDrawLine(renderer, cx + w - 1, cy, cx + w - 1, cy + h - 1);  
        SDL_RenderDrawLine(renderer, cx, cy + h - 1, cx + w - 1, cy + h - 1);  

        if (app.font) {
            SDL_Surface* surface = TTF_RenderText_Blended(app.font, text.c_str(), textColor);
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

            int text_width = surface->w;
            int text_height = surface->h;
            SDL_Rect dstrect = { cx + (w - text_width) / 2, cy + (h - text_height) / 2, text_width, text_height };

            SDL_RenderCopy(renderer, texture, nullptr, &dstrect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }

    void Button::setWindowPos(int xx, int yy) {
        wx = xx;
        wy = yy;
    }

    bool Button::event(mxApp &app, SDL_Event &e) {

        int cx = x + wx;
        int cy = y + wy;

        if (e.type == SDL_MOUSEMOTION) {
            int mouseX = e.motion.x;
            int mouseY = e.motion.y;
            hover = (mouseX >= cx && mouseX <= cx + w && mouseY >= cy && mouseY <= cy + h);
        }

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            if (mouseX >= cx && mouseX <= cx + w && mouseY >= cy && mouseY <= cy + h) {
                pressed = true;
              
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            if (pressed && mouseX >= cx && mouseX <= cx + w && mouseY >= cy && mouseY <= cy + h) {
                pressed = false;
                if(callback && parent) {
                    return callback(app, parent, e);
                }
                return true;  
            }
            pressed = false;
        }
        
        return false;
    }

    void Button::create(Window *parent, const std::string &text, int x, int y, int w, int h) {
        this->text = text;
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
        this->parent = parent;
    }

    void Button::setText(const std::string &t) {
        text = t;
    }

    void Button::setGeometry(int xx, int yy, int ww, int hh) {
        x = xx;
        y = yy;
        w = ww;
        h = hh;
    }
 
    void Button::resizeWindow(int w, int h) {
        if(rcallback && parent) {
            rcallback(parent, w, h);
        }
    }

    Image::Image(mxApp &app) : x(0), y(0), w(0), h(0), image(nullptr) {

    }

    Image::~Image() {
        if(image) {
            SDL_DestroyTexture(image);
        }
    }

    void Image::setWindowPos(int x, int y) {
        wx = x;
        wy = y;
    }

    void Image::draw(mxApp &app) {

        if(!show)
            return;

        if(image) {
            int dw = w, dh = h;
            if(w == 0 || h == 0) {
                dw = sw;
                dh = sh;
            }
            SDL_Rect rc = { x+wx, y+wy+25, dw, dh };
            SDL_RenderCopy(app.ren, image, (use_rect == true) ? &src : nullptr, &rc);
        }
    }

    void Image::getRect(SDL_Rect &rc) {
        rc.x = x;
        rc.y = y;
        rc.w = w;
        rc.h = h;
    }

    bool Image::event(mxApp &app, SDL_Event &e) {
        if(show == false)
            return false;
        int cx = x + wx;
        int cy = y + wy;
        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            if (mouseX >= cx && mouseX <= cx + w && mouseY >= cy && mouseY <= cy + h) {
                if(callback && parent) {
                    return callback(app, parent, e);
                }
                return true;       
            }
        }
        return false;
    }

    void Image::create(mxApp &app, Window *window, const std::string &path, int x, int y) {
        image = loadTexture(app, path, sw, sh);
        parent= window;
        this->x = x;
        this->y = y;
        w = sw;
        h = sh;
    }

    void Image::setSourceRect(int x, int y, int w, int h) {
        src.x = x;
        src.y = y;
        src.w = w;
        src.h = h;
        use_rect = true;
    }

    void Image::setGeometry(int x, int y, int w, int h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }    

    void Image::resizeWindow(int w, int h) {
        setGeometry(x, y, w, h);  
    }
}