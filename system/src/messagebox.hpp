#ifndef _MSGBOX_H__
#define _MSGBOX_H__

#include"window.hpp"
#include"mx_window.hpp"


namespace mx {

    class MessageBox : public Window {
    public:
        MessageBox(mxApp &app);
        virtual ~MessageBox();
        virtual void draw(mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
    };

}





#endif