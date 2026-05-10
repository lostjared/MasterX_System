#ifndef MX_EDITOR_HPP
#define MX_EDITOR_HPP

#include "window.hpp"
#include <string>
#include <vector>

namespace mx {

// A small in-process text editor (nano-style) that renders inside a
// supplied SDL_Rect using the supplied TTF_Font. It is owned by the
// Terminal and consumes SDL events while active. It does not depend
// on the host shell, so it works under Linux, Windows and the
// FOR_WASM build.
class TextEditor {
public:
  TextEditor(TTF_Font *font, SDL_Color color);
  ~TextEditor() = default;

  // Open the given file. If it does not exist the editor starts
  // empty with that filename. Returns true on success / file
  // created, false on read error.
  bool open(const std::string &filename);

  // Save buffer to current filename. Returns true on success.
  bool save();

  // Render the editor inside the given content rectangle (the
  // rectangle below the window's title bar).
  void draw(mxApp &app, const SDL_Rect &area);

  // Process a single SDL event. Returns true while the editor is
  // still active. When it returns false the caller should
  // destroy / hide the editor.
  bool event(mxApp &app, SDL_Event &e);

  bool isActive() const { return active; }
  bool isModified() const { return modified; }
  const std::string &getFilename() const { return filename; }

private:
  enum class PromptMode {
    None,
    ExitConfirm,  // "Save modified buffer? (Y/N)"
    ExitFilename, // "File name to write: " (after Y on exit)
    SaveFilename  // Ctrl+O: "File name to write: "
  };

  TTF_Font *font;
  SDL_Color color;
  std::string filename;
  std::vector<std::string> lines;
  int cursorRow = 0;
  int cursorCol = 0; // byte offset in line (ASCII assumed)
  int topRow = 0;    // first visible row
  int leftCol = 0;   // first visible column (in characters)
  bool modified = false;
  bool active = true;

  std::string statusMsg;
  Uint32 statusUntil = 0;

  PromptMode prompt = PromptMode::None;
  std::string promptInput;
  int promptCursor = 0;
  bool exitAfterSave = false;

  // Layout cache (recomputed each draw).
  int charW = 8;
  int lineH = 16;
  int viewRows = 1;
  int viewCols = 1;

  // Helpers.
  void setStatus(const std::string &msg);
  void clampCursor();
  void ensureVisible();
  void insertChar(char c);
  void insertString(const std::string &s);
  void backspace();
  void deleteChar();
  void newline();
  void moveLeft();
  void moveRight();
  void moveUp();
  void moveDown();
  void pageUp();
  void pageDown();
  bool saveAs(const std::string &name);

  void renderLine(mxApp &app, const std::string &text, int x, int y);
  void fillRect(mxApp &app, const SDL_Rect &r, Uint8 r8, Uint8 g8, Uint8 b8,
                Uint8 a8);
};

} // namespace mx

#endif
