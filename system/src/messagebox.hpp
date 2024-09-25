#ifndef _MSGBOX_H__
#define _MSGBOX_H__

#include"window.hpp"
#include"mx_window.hpp"


namespace mx {

    class DimensionContainer;

    using EventCallbackMsg = std::function<bool(mxApp &app, Window *window, int button)>;

    class MessageBox : public Window {
    public:
        MessageBox(mxApp &app);
        virtual ~MessageBox();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
        void createControls(mxApp &app);
        void createControlsOkCancel(mxApp &app);
        void resizeControls();
        static void OkMessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string  &text);
        static void OkCancelMessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string &text, EventCallbackMsg event_cb);

    protected:
        std::string text;
        Button *ok, *cancel;
        EventCallbackMsg event_ = 0;
    };

}





#endif