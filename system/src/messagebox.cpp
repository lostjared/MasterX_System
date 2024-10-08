#include"messagebox.hpp"
#include"dimension.hpp"
#include<memory>


namespace mx {


    std::vector<MX_MessageBox *> MX_MessageBox::boxes;
    MX_MessageBox *MX_MessageBox::box = nullptr;

    MX_MessageBox::MX_MessageBox(mxApp &app) : Window(app) {
        menu.menu[0].window_menu = false;
    }
    MX_MessageBox::~MX_MessageBox() { 
        if(box != nullptr) {
            auto it = boxes.begin();
            boxes.erase(it);
            if(!boxes.empty()) {
                box = boxes[0];
            } else {
                box = nullptr;
            }
        }
    }
  
    void MX_MessageBox::draw(mxApp &app) {
        if(MX_MessageBox::box != nullptr)
            MX_MessageBox::box->drawBox(app);
    }

    void MX_MessageBox::drawBox(mxApp &app) {
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

    bool MX_MessageBox::event(mxApp &app, SDL_Event &e) {
        if(MX_MessageBox::box != nullptr)
            return MX_MessageBox::box->eventBox(app, e);

        return Window::event(app, e);
    }

    bool MX_MessageBox::eventBox(mxApp &app, SDL_Event &e) {
        for(auto &c : children) {
            if(c->event(app, e))
                return true;
        }
        return Window::event(app, e);
    }

    void MX_MessageBox::createControlsOkCancel(mxApp &app) {
        children.push_back(std::make_unique<Button>(app));
        ok = dynamic_cast<Button *>(getControl());
        if(!ok) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        children.push_back(std::make_unique<Button>(app));
        cancel = dynamic_cast<Button *>(getControl());
        if(!cancel) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        SDL_Rect drawRect;
        Window::getDrawRect(drawRect);
        int totalButtonWidth = 200; 
        int gap = 20; 
        int totalWidth = totalButtonWidth + gap;
        int xCenter = drawRect.w / 2 - totalWidth / 2;
        int yCenter = drawRect.h / 2 + 50; 
        ok->create(this, "Ok", xCenter, yCenter, 100, 25);
        cancel->create(this, "Cancel", xCenter + 100 + gap, yCenter, 100, 25);
        ok->setCallback([](mxApp &app, Window *win, SDL_Event   &e) -> bool {
            MX_MessageBox *msg = dynamic_cast<MX_MessageBox *>(win);
            if(!msg) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            msg->event_(app, win, 1);
            msg->destroyWindow();
            return true;
        });
        cancel->setCallback([](mxApp &app, Window *win, SDL_Event &e) -> bool {
            MX_MessageBox *msg = dynamic_cast<MX_MessageBox *>(win);
            if(!msg) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            msg->event_(app, win, 2);
            msg->destroyWindow();
            return true;
        });
    }

    void MX_MessageBox::createControls(mxApp &app) {
        children.push_back(std::make_unique<Button>(app));
        ok = dynamic_cast<Button *>(getControl());
        if(!ok) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        SDL_Rect drawRect;
        Window::getDrawRect(drawRect);
        int buttonWidth = 100;
        int xCenter = drawRect.w / 2 - buttonWidth / 2;
        int yCenter = drawRect.h / 2 + 50; 
        ok->create(this, "Ok", xCenter, yCenter, buttonWidth, 25);
        ok->setCallback([](mxApp &app, Window *win, SDL_Event &e) -> bool {
            MX_MessageBox *box = dynamic_cast<MX_MessageBox *>(win);
            if(!box) {
                mx::system_err << "MasterX System: Bad cast..\n";
                mx::system_err.flush();
                exit(EXIT_FAILURE);
            }
            box->destroyWindow();
            return true;
        });
    }

    void MX_MessageBox::resizeControls() {

    }

    void MX_MessageBox::OkMX_MessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string  &text) {
        dim->objects.push_back(std::make_unique<MX_MessageBox>(app));
        MX_MessageBox *msgbox = dynamic_cast<MX_MessageBox *>(dim->objects[dim->objects.size()-1].get());
        if(!msgbox) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
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
        boxes.push_back(msgbox);
        box = boxes[0];
    }

    void MX_MessageBox::OkCancelMX_MessageBox(mxApp &app, DimensionContainer *dim, const std::string &title, const std::string &text, EventCallbackMsg event_cb) {
        dim->objects.push_back(std::make_unique<MX_MessageBox>(app));
        MX_MessageBox *msgbox = dynamic_cast<MX_MessageBox *>(dim->objects[dim->objects.size()-1].get());
        if(!msgbox) {
            mx::system_err << "MasterX System: Bad cast..\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }
        int box_w = 400;
        int box_h = 175;
        msgbox->create(dim, title, (app.width/2) - (box_w/2), (app.height/2) - (box_h/2), box_w, box_h);
        dim->events.addWindow(msgbox);
        msgbox->setSystemBar(dim->system_bar);
        msgbox->menu.menu[0].is_messagebox = true;
        msgbox->text = text;
        msgbox->createControlsOkCancel(app);
        msgbox->removeAtClose(true);
        msgbox->setCanResize(false);
        msgbox->show(true);
        msgbox->event_ = event_cb;
        boxes.push_back(msgbox);
        box = boxes[0];
    }
}