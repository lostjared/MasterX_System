#include"mx_event.hpp"
#include<vector>
#include"mx_window.hpp"
#include"mx_menu.hpp"

namespace mx {

    bool EventHandler::pumpEvent(SDL_Event &e) {

         if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
            int x = e.button.x;
            int y = e.button.y;
            checkWindowClick(x, y);
        }

        for (auto it = window_stack.rbegin(); it != window_stack.rend(); ++it) {
            Window *window = *it;
            if (window->event(app_, e)) {
                return true;
            }
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

    void EventHandler::sendDrawMessage() {
        for (auto &window : window_stack) {
            window->draw(app_);
        }
        window_stack[cur_focus]->menu.draw(app_);
    }

    bool EventHandler::checkWindowClick(int x, int y) {
        for (auto it = window_stack.rbegin(); it != window_stack.rend(); ++it) {
            Window *window = *it;
            if (window->isPointInside(x, y)) {
                int index = static_cast<int>(std::distance(it, window_stack.rend())) - 1;
                setFocus(index);
                return true;
            }
        }
        return false;
    }

    void EventHandler::setFocus(int index) {
        if (index >= 0 && index < static_cast<int>(window_stack.size())) {
            cur_focus = index;
            Window *focused_window = window_stack[index];
            window_stack.erase(window_stack.begin() + index);
            window_stack.push_back(focused_window);
        }
    }

    void EventHandler::setFocus(Window *window) {
        int index = 0;
        for(auto it = window_stack.begin(); it != window_stack.end(); ++it) {
            Window *win = *it;
            if(win == window) {
                window_stack.erase(window_stack.begin() + index);
                window_stack.push_back(window);
            }
            index ++;
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
        window_stack.push_back(window);
    }
}