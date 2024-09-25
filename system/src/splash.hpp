#ifndef _SPLASH_H_
#define _SPLASH_H_

#include "window.hpp"

namespace mx {


    class Splash : public Screen {
    public:
        Splash(mxApp &app);
        virtual ~Splash();
        virtual void draw(mx::mxApp &app) override;
        virtual bool event(mxApp &app, SDL_Event &e) override;
    private:
        SDL_Texture *bg = 0;
        TTF_Font *font = 0;
    };


}

#endif