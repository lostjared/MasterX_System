#ifndef _MSGBOX_H__
#define _MSGBOX_H__

#include"window.hpp"
#include"mx_window.hpp"
#include<memory>


namespace mx {

    class DimensionContainer;

    using EventCallbackMsg = std::function<bool(mxApp &app, Window *window, int button)>;

    class MX_MessageBox : public Window {
    public:
        MX_MessageBox(mxApp &app);
        virtual ~MX_MessageBox();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;

        void drawBox(mxApp &app);
        bool eventBox(mxApp &app, SDL_Event &e);
        
        void createControls(mxApp &app);
        void createControlsOkCancel(mxApp &app);
        void resizeControls();
        static void OkMX_MessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string  &text);
        static void OkCancelMX_MessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string &text, EventCallbackMsg event_cb);
    protected:
        std::string text;
        Button *ok = 0, *cancel = 0;
        EventCallbackMsg event_ = 0;
        static std::vector<MX_MessageBox *> boxes;
        static MX_MessageBox *box;
    };
}





#endif