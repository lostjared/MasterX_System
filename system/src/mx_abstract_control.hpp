#ifndef __MX_CONTROL_H_X
#define __MX_CONTROL_H_X

#include "window.hpp"
#include<functional>

namespace mx {

    class Window;

    using EventCallback = std::function<bool(mxApp &app, Window *window, SDL_Event &e)>;
    using ResizeCallback = std::function<void(Window*, int, int)>;

    class Control : public Screen {
    public:
        virtual ~Control() = default;
        virtual void setWindowPos(int x, int y) = 0;
        virtual void resizeWindow(int w, int h) {}

        template<typename F>
        void setCallback(F callb) { callback = callb; }  
        template<typename F>
        void setResizeCallback(F callb) { rcallback = callb; }
        Window *parent = nullptr;
        EventCallback callback = nullptr;
        ResizeCallback rcallback = nullptr;
        bool show = true;
        void setShow(bool s) { show = s; }
    };
}



#endif