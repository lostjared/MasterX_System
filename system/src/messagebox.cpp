#include"messagebox.hpp"
#include"dimension.hpp"
#include<memory>


namespace mx {

        MessageBox::MessageBox(mxApp &app) : Window(app) {
            menu.menu[0].window_menu = false;
        }
        MessageBox::~MessageBox() { 
        }
        
        void MessageBox::draw(mxApp &app) {
            if(!Window::isVisible())
                return;

            if(!Window::isDraw())
                return;

            Window::draw(app);
            Window::drawMenubar(app);
            SDL_Rect rc;
            Window::getDrawRect(rc);
            int textWidth = 0, textHeight = 0;
            TTF_SizeText(app.font, text.c_str(), &textWidth, &textHeight); 
            int pos_x = rc.x + (rc.w / 2) - (textWidth / 2);
            int pos_y = rc.y + 50; 
            app.printText(pos_x, pos_y, text, {0,0,0,255});
        }
    
        bool MessageBox::event(mxApp &app, SDL_Event &e) {
            for(auto &c : children) {
                if(c->event(app, e))
                    return true;
            }
            return Window::event(app, e);
        }

        void MessageBox::createControlsOkCancel(mxApp &app) {
            children.push_back(std::make_unique<Button>(app));
            ok = dynamic_cast<Button *>(getControl());
            children.push_back(std::make_unique<Button>(app));
            cancel = dynamic_cast<Button *>(getControl());
            SDL_Rect drawRect;
            Window::getDrawRect(drawRect);
            int totalButtonWidth = 200; 
            int gap = 20; 
            int totalWidth = totalButtonWidth + gap;
            int xCenter = drawRect.w / 2 - totalWidth / 2;
            int yCenter = drawRect.h / 2 + 50; 
            ok->create(this, "Ok", xCenter, yCenter, 100, 25);
            cancel->create(this, "Cancel", xCenter + 100 + gap, yCenter, 100, 25);
        }

        void MessageBox::createControls(mxApp &app) {
            children.push_back(std::make_unique<Button>(app));
            ok = dynamic_cast<Button *>(getControl());
            SDL_Rect drawRect;
            Window::getDrawRect(drawRect);
            int buttonWidth = 100;
            int xCenter = drawRect.w / 2 - buttonWidth / 2;
            int yCenter = drawRect.h / 2 + 50; 
            ok->create(this, "Ok", xCenter, yCenter, buttonWidth, 25);
            ok->setCallback([](mxApp &app, Window *win, SDL_Event &e) -> bool {
                MessageBox *box = dynamic_cast<MessageBox *>(win);
                box->destroyWindow();
                return true;
            });
        }
 
        void MessageBox::resizeControls() {

        }

        void MessageBox::OkMessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string  &text) {
            dim->objects.push_back(std::make_unique<MessageBox>(app));
            MessageBox *msgbox = dynamic_cast<MessageBox *>(dim->objects[dim->objects.size()-1].get());
            int box_w = 400;
            int box_h = 175;
            msgbox->create(dim, title, (app.width/2) - (box_w/2), (app.height/2) - (box_h/2), box_w, box_h);
            dim->events.addWindow(msgbox);
            msgbox->setSystemBar(dim->system_bar);
            msgbox->menu.menu[0].is_messagebox = true;
            msgbox->text = text;
            msgbox->createControls(app);
            msgbox->removeAtClose(true);
            msgbox->setCanResize(false);
            msgbox->show(true);
        }
}