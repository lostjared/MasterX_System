#ifndef __MX_WINDOW_H_
#define __MX_WINDOW_H_

#include "mx_menu.hpp"
#include "window.hpp"
#include <string>
#include <vector>
namespace mx {

// Height of the window title bar in pixels. Shared with controls and
// terminal windows so every file uses the same authoritative value.
inline constexpr int kTitleBarHeight = 30;

class Control;
class SystemBar;
class DimensionContainer;

class Window : public Screen {
public:
  friend class SystemBar;
  friend class DimensionContainer;
  Window(mxApp &app);
  virtual ~Window();
  virtual void draw(mxApp &app) override;
  virtual bool event(mxApp &app, SDL_Event &e) override;
  virtual void stateChanged(bool min, bool max, bool close) {}
  virtual void activate() {}
  virtual void screenResize(int w, int h) {
    dim_w = w;
    dim_h = h;
  }
  void create(DimensionContainer *dim, const std::string &name, const int x,
              const int y, const int w, const int h);
  void destroyWindow();
  void removeAtClose(bool b);
  void show(bool b);
  void minimize(bool m);
  void maximize(bool m);
  void getRect(SDL_Rect &rc);
  void getDrawRect(SDL_Rect &rc);
  void setRect(const SDL_Rect &rc);
  Control *getControl();
  Control *getControl(int index);
  bool isVisible() const;
  bool isDraw() const;
  bool reload() const;
  void setReload(bool r);
  void setCanResize(bool r);
  bool canResize() const;
  bool isMaximized() const { return maximized; }
  void setSystemBar(SystemBar *s);
  void drawMenubar(mxApp &app);
  bool isPointInside(int x, int y);
  void setIcon(SDL_Texture *icon);
  SystemBar *systemBar = nullptr;
  DimensionContainer *dim = nullptr;
  bool minimized = false;
  bool dragging = false;

private:
  // Edge-drag resize state (active only when canResize() is true).
  // resizeEdges_ is a bitmask of (1=left, 2=right, 4=top, 8=bottom).
  int resizeEdges_ = 0;
  int resizeStartX_ = 0, resizeStartY_ = 0;
  int resizeStartW_ = 0, resizeStartH_ = 0;
  int resizeMouseX_ = 0, resizeMouseY_ = 0;
  bool resizing_ = false;
  static constexpr int kResizeBorder = 6;
  static constexpr int kMinW = 160;
  static constexpr int kMinH = 80;
  int hitTestEdges(int mx, int my) const;
  void applyEdgeCursor(int edges) const;
  int x, y, w, h;
  int dim_w = 0, dim_h = 0;
  bool shown = false;
  bool remove_on = false;
  bool maximized = false;
  int dragOffsetX = 0, dragOffsetY = 0;
  int oldX = 0, oldY = 0, oldW = 0, oldH = 0;
  bool is_visible = true;
  bool reload_window = false;
  bool can_resize = false;
  bool isMinimizing = false;
  int minTargetX = 0, minTargetY = 0;
  int minTargetW = 0, minTargetH = 0;
  int minAnimationStep = 1;
  int restoreTotalFrames = 12;
  int restoreFrame = 0;
  int restoreStartX = 0, restoreStartY = 0;
  int restoreStartW = 0, restoreStartH = 0;
  void toggleMaximize(mxApp &app);
  Uint32 lastMaxToggleTicks_ = 0;
  int originalX;
  int originalY;
  int originalWidth;
  int originalHeight;
  bool isRestoring = false;
  int restoreTargetX;
  int restoreTargetY;
  int restoreTargetW;
  int restoreTargetH;
  int orig_x = 0, orig_y = 0;

public:
  SDL_Texture *icon = nullptr;
  std::vector<std::unique_ptr<Control>> children;
  SDL_Rect minimizeButton, closeButton, maximizeButton;
  SDL_bool minimizeHovered, closeHovered, maximizeHovered;
  std::string title = "Window";
  Menu menu;
};

} // namespace mx

#endif