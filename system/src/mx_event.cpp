#include"mx_event.hpp"
#include<vector>
#include"mx_window.hpp"
#include"mx_menu.hpp"

namespace mx {

    bool EventHandler::pumpEvent(SDL_Event &e) {
        Window *chk = nullptr;
         if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;
            chk = checkWindowClick(x, y);
         }

        Window *win = currentWindow();

        if(win != nullptr) {
            if(win->menu.event(app_, e))
                return true;
                
            if(win->event(app_, e)) 
                return true;
        }

        if(chk != nullptr && win != nullptr && chk != win) {
            chk->event(app_, e);
        }

        if(chk) {
            setFocus(chk);
        }
        return false;
    }

    void EventHandler::removeWindow(Window *window) {
        for(auto it = window_stack.begin(); it != window_stack.end(); ++it) {
            Window *win = *it;
            if(win == window) {
                mx::system_out << "MasterX: Removing Window: " << window->title << "\n";
                window_stack.erase(it);
                if(!window_stack.empty()) {
                    setFocus(window_stack.back());
                } else {
                    clearFocus();
                }
                return;
            }
        }
    }
    void EventHandler::setNextVisible() {
        for(auto it = window_stack.rbegin(); it != window_stack.rend(); ++it) {
            Window *win = *it;
            if(win->isVisible()) {
                setFocus(win);
                return;
            }
        }
    }

    bool EventHandler::allHidden() const {
        for(auto &i : window_stack) {
            if(i->isVisible() == true)
                return false;
        }
        return true;
    }

    void EventHandler::sendDrawMessage() {
        for (auto &window : window_stack) {
            if(window != nullptr)
                window->draw(app_);
        }
        if(!window_stack.empty() && !allHidden() && window_stack[cur_focus] != nullptr)
            window_stack[cur_focus]->menu.draw(app_);
    }

    Window *EventHandler::checkWindowClick(int x, int y) {
        for (auto it = window_stack.rbegin(); it != window_stack.rend(); ++it) {
            Window *window = *it;
            if (window->isPointInside(x, y)) {
               return window;
            }
        }
        return nullptr;
    }

    void EventHandler::setFocus(int index) {
        if (index >= 0 && index < static_cast<int>(window_stack.size())) {
            if(cur_focus == index)
                return;
            cur_focus = index;
        }
    }

    void EventHandler::setFocus(Window *window) {
        auto it = std::find(window_stack.begin(), window_stack.end(), window);
        if(it != window_stack.end()) {
            window_stack.erase(it);
            window_stack.push_back(window);
            cur_focus = window_stack.size()-1;
        }
    }

    void EventHandler::clearFocus() {
        if(window_stack.empty()) {
            cur_focus = 0;
        }
    }

    Window *EventHandler::currentWindow() {
        if (!window_stack.empty()) {
            return window_stack.back();
        }
        return nullptr;
    }

    void EventHandler::addWindow(Window *window) {
        if(window != nullptr)
            window_stack.push_back(window);
    }
}