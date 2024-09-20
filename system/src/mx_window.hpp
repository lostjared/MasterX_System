#ifndef __MX_WINDOW_H_
#define __MX_WINDOW_H_

#include "window.hpp"
#include<vector>
#include<string>
#include"mx_menu.hpp"
namespace mx {

  class Control;
  class SystemBar;
  class DimensionContainer;
  
  class Window : public Screen {
    public:
        friend class SystemBar;
        friend class DimensionContainer;
        Window(mxApp &app);
        virtual ~Window();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void stateChanged(bool min, bool max, bool close) {}
        virtual void activate() {}
        void create(DimensionContainer *dim, const std::string &name, const int x, const int y, const int w, const int h);
        void destroyWindow();
        void removeAtClose(bool b);
        void show(bool b);
        void minimize(bool m);
        void maximize(bool m);
        void getRect(SDL_Rect &rc);
        void getDrawRect(SDL_Rect &rc);
        void setRect(const SDL_Rect &rc);
        Control *getControl();
        Control *getControl(int index);
        bool isVisible() const;
        bool isDraw() const;
        bool reload() const;
        void setReload(bool r);
        void setCanResize(bool r);
        bool canResize() const;
        void setSystemBar(SystemBar *s);
        void drawMenubar(mxApp &app);
        bool isPointInside(int x, int y);
        void setIcon(SDL_Texture *icon);
        SystemBar *systemBar = nullptr;
        DimensionContainer *dim = nullptr;
        bool minimized = false;
        bool dragging = false;
    private:
        int x,y,w,h;
        int dim_w = 0, dim_h = 0;
        bool shown = false;
        bool remove_on = false;
        bool maximized = false;
          int dragOffsetX = 0, dragOffsetY = 0;
        int oldX = 0, oldY = 0, oldW = 0, oldH = 0;
        bool is_visible = true;
        bool reload_window = false;
        bool can_resize = false;
        bool isMinimizing = false;
        int minTargetX = 0, minTargetY = 0;  
        int minTargetW = 0, minTargetH = 0;  
        int minAnimationStep = 1;   
        int restoreAnimationStep = 5;
        int originalX;    
        int originalY;    
        int originalWidth;  
        int originalHeight; 
        bool isRestoring = false;
        int restoreTargetX; 
        int restoreTargetY; 
        int restoreTargetW;  
        int restoreTargetH;
        int orig_x = 0, orig_y = 0;
    public:
        SDL_Texture *icon = nullptr;
        std::vector<std::unique_ptr<Control>> children;
        SDL_Rect minimizeButton, closeButton, maximizeButton;
        SDL_bool minimizeHovered, closeHovered, maximizeHovered;
        std::string title = "Window";
        Menu menu;
    };

}

#endif