#ifndef _MXSYSTEM_BAR_H_
#define _MXSYSTEM_BAR_H_

#include"window.hpp"
#include<vector>
#include<memory>
#include<algorithm>

namespace mx {
  class MenuBar : public Screen {
    public:
        MenuBar(mxApp &app);
        virtual ~MenuBar();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp  &app, SDL_Event &e) override;
        bool menuOpen;
        bool animating;
        int currentY;
        int targetY;
        int itemClicked(mxApp &app, int x, int y);
    private:
        TTF_Font *font;
        SDL_Surface *itemSurface;
        SDL_Texture *itemTexture;
        SDL_Color white;
    };

    class Window;

    class SystemBar : public Screen {
    public:
        SystemBar(mxApp &app);
        virtual ~SystemBar();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        void setDimensions(std::vector<std::unique_ptr<Screen>> *dim);
        void setCurrentDimension(int dim);
        int  getCurrentDimension() const;
        void activateDimension(int dim);
        void deactivateDimension(int dim);
        bool empty() const;
        void addMinimizedWindow(Window *win);
        void performAction(mxApp &app, int action);
        void loadDimension(int id);
        void restoreWindow(Window *window);
        void drawMinimizedMenu(mxApp &app, int button_x, int button_y, int button_width);
        friend class Window;
    private:
        bool holdingDimension = false;
        bool showMinimizedMenu = false;
        Uint32 holdStartTime = 0;
        int yPos = 0;
        bool animationComplete = false;
        TTF_Font  *font;
        SDL_Color start_bg = { 0,0,0,225 };
        bool isHovering = false;
        std::vector<std::unique_ptr<Screen>> objects;
        std::vector<std::unique_ptr<Screen>> *dimensions;
        MenuBar *menu;
        int cur_dim = 0;
        int prev_dim = 0;
        int hoverIndex = 0;
        bool menuHover = false;
        void drawDimensions(mxApp &app);
        std::vector<int> activeDimensionsStack;
        std::vector<Window *> minimizedWindows;
     
    };  
}





#endif
