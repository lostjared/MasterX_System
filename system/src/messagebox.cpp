#include"messagebox.hpp"


namespace mx {

        MessageBox::MessageBox(mxApp &app) : Window(app) {

        }
        MessageBox::~MessageBox() { 

        }
        
        void MessageBox::draw(mxApp &app) {

        }
    
        bool MessageBox::event(mxApp &app, SDL_Event &e) {

            return false;
        }
}