#ifndef __TERM_H_
#define __TERM_H_

#include"window.hpp"
#include"mx_window.hpp"
#include"mx_editor.hpp"
#include<memory>
#ifdef _WIN32
#include <windows.h>
#elif !defined(FOR_WASM)
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <termios.h>
#ifdef __linux__
#include <pty.h>
#elif defined(__APPLE__)
#include <stdlib.h>
#include <unistd.h>
#include <util.h>
#include <sys/ioctl.h>
#endif
#include <utmp.h>
#include <fcntl.h>
#endif
#include<thread>
#include<mutex>
#include<atomic>
#include<regex>
#include<condition_variable>

namespace mx {

        // Per-character visual style for ANSI-aware rendering.
        struct CharStyle {
            SDL_Color fg{255, 255, 255, 255};
            SDL_Color bg{0, 0, 0, 0};  // a==0 means transparent (no background)
            bool bold{false};
            bool underline{false};
        };

        std::string getLastDirectory(const std::string& fullPath);
        class Terminal :  public Window {
        public:
                Terminal(mxApp  &app);
                virtual ~Terminal();
                void draw(mxApp  &app) override;
                bool event(mxApp &app, SDL_Event  &e) override;
                virtual void screenResize(int w, int h) override;
                void print(const std::string &s);
                void scroll();
                void stateChanged(bool min, bool max, bool closed) override;
                void setWallpaper(SDL_Texture *tex);
                void drawCursor(mxApp &, int, int, bool);
                bool atBottom();
                std::string parseTerminalData(const std::string &);
                void sendCommand(const std::string &cmd);
                void pasteFromClipboard();
                void copyToClipboard();
                void copySelectionToClipboard();
                bool hasSelectedText() const { return hasSelection; }
                void insertText(const std::string &text);
                std::string getInput();
                bool isWaitingForInput() const { return waitingForInput; }
                bool adjustFontSize(int delta);
                bool setFontSize(int size);
                int getFontSize() const { return font_size_; }

                // Background shader effects
                enum class TermEffect : int {
                    None = 0,
                    Plasma,
                    Vortex,
                    ChromaticRipple,
                    NeonGrid,
                    Starfield,
                    LiquidWave,
                    Fractal,
                    AcidSpiral,
                    Aurora,
                    Tunnel,
                    Crystal,
                    Fire,
                    Hyperspace,
                    kCount
                };
                void setEffect(TermEffect e, mxApp &app);
                // Draw only the shader/wallpaper background without any
                // terminal chrome or content — used when the owning
                // TerminalTabs window is minimized.
                void drawBackgroundOnly(mxApp &app);

                // Embedded mode: when true, the Terminal does not draw its
                // own window chrome (title bar / menu bar) and does not
                // consume window-level events (drag / resize / close /
                // min / max). The owning container (TerminalTabs) is
                // responsible for those, and for sizing this Terminal's
                // rect via setRect() so its content lands in the area it
                // allocated.
                void setEmbedded(bool b) { embedded_ = b; }
                bool isEmbedded() const { return embedded_; }

                // Bash session liveness check used by the owning
                // TerminalTabs to detect when a child shell exited (e.g.
                // user typed `exit`) so the tab can be auto-closed.
                bool sessionAlive() const;

                // Public access to the prompt string so the owning
                // TerminalTabs can show a meaningful tab title.
                const std::string &currentPrompt() const { return prompt; }
        private:
                bool embedded_ = false;
                std::string prompt = "$ ";
#if !defined(FOR_WASM) && !defined(WIN32)
                int master_fd, slave_fd;
                bool cursorVisible = true;
                int is_echo_enabled();
                bool echo_enabled = true;
#endif
                std::string handleBackspaces(const std::string &str); 
                std::string inputText;
                int cursorPosition = 0;
                std::vector<std::string> outputLines;
                std::vector<std::vector<CharStyle>> outputLineColors;
                void renderText(mxApp &app, const std::string &text, int x, int y);
                void renderOutputLine(mxApp &app, int lineIndex, int x, int y);
                void renderTextWrapped(mxApp &app, const std::string &prompt, const std::string &text, int &x, int &y, int maxWidth);
                void processCommand(mxApp &app, std::string cmd);
                void handleScrolling(int);
                void updateText(const std::string &text);
                std::string orig_text;
                std::vector<std::string> splitText(const std::string &text);
                std::vector<std::string> stored_commands;
                int store_offset = 0;
                int scrollOffset = 0; 
                int maxVisibleLines = 10; 
                bool showCursor = true;
                Uint32 cursorTimer = 0, cursorBlinkInterval = 100;  
                SDL_Color text_color;
                bool isMaximized = false;
                TTF_Font *font;
                std::string font_name_;
                int font_size_ = 18;
                static constexpr int kMinFontSize = 8;
                static constexpr int kMaxFontSize = 48;
                SDL_Texture *wallpaper;

                // --- Background shader effect state (private) ---
                static constexpr int kEffectCount = static_cast<int>(TermEffect::kCount);
                unsigned int effectProgs_[kEffectCount] = {};
                TermEffect   activeEffect_  = TermEffect::None;
                float        shaderTime_    = 0.0f;
                Uint32       lastEffectMs_  = 0;
                void drawEffectBackground(mxApp &app);
                bool isScrolling = false;
                int scrollBarWidth = 10;
                int scrollBarHeight = 0;
                int scrollBarPosY = 0;
                int scrollBarDragOffset = 0;
                std::string savedInputText;
                bool cyclingThroughHistory = false;
                int calculateTotalWrappedLines();
                int calculateWrappedLinesForText(const std::string &text, int, int);
                std::atomic<bool> active;
                int total_Lines();
                void scrollToBottom();
                bool render_text = true;
                void requestCurrentDirectory();
                std::string new_output;
                std::string new_data;
                std::atomic<bool> newData;

                // ANSI/VT100 stream state for shell output rendering.
                std::vector<std::string> ansiLines;
                std::vector<std::vector<CharStyle>> ansiLineColors;
                int ansiCursorRow = 0;
                int ansiCursorCol = 0;
                int ansiSavedRow = 0;
                int ansiSavedCol = 0;
                bool ansiInitialized = false;
                SDL_Color ansiCurrentColor{255, 255, 255, 255};
                SDL_Color ansiCurrentBg{0, 0, 0, 0};
                bool ansiBold = false;
                bool ansiUnderline = false;
                void initAnsiState();
                void syncAnsiToOutput();
                void applyAnsiData(const std::string &input);

                // TUI / full-screen application support (vim, nano, less, top, ...).
                bool altScreen = false;     // alternate screen buffer active
                bool decckm = false;        // DECCKM cursor-key application mode
                bool keypadApp = false;     // keypad application mode (\e= / \e>)
                int  scrollTop = 0;         // 0-based inclusive top of scroll region
                int  scrollBot = -1;        // 0-based inclusive bottom; -1 = whole screen
                int  termRows = 24;
                int  termCols = 80;
                int  lastReportedRows = -1;
                int  lastReportedCols = -1;
                // Saved primary screen state (restored on alt-screen exit).
                std::vector<std::string>              savedLines;
                std::vector<std::vector<CharStyle>>   savedLineColors;
                int savedAltRow = 0;
                int savedAltCol = 0;
                SDL_Color savedAltFg{255,255,255,255};
                SDL_Color savedAltBg{0,0,0,0};
                bool savedAltBold = false;
                bool savedAltUnderline = false;

                void enterAltScreen();
                void leaveAltScreen();
                void writeToPty(const std::string &data);
                void updatePtySize();
                bool handleRawKeyEvent(mxApp &app, SDL_Event &e);
                std::string keyToPtyBytes(SDL_Keycode sym, Uint16 mod);

                std::atomic<bool> waitingForInput{false};
                std::string inputResult;

                // Mouse text selection state. Selection coordinates are
                // expressed as (row, col) inside outputLines using a fixed
                // cell grid identical to the renderer.
                bool selecting = false;       // mouse currently dragging
                bool hasSelection = false;    // a finalized / live selection exists
                int  selAnchorRow = 0;
                int  selAnchorCol = 0;
                int  selFocusRow  = 0;
                int  selFocusCol  = 0;
                bool pointToCell(int px, int py, int &row, int &col) const;
                void normalizedSelection(int &r0, int &c0, int &r1, int &c1) const;
                std::string getSelectedText() const;
                void clearSelection();
                void drawSelection(mxApp &app, const SDL_Rect &contentRect, int lineHeight, int cellW);
                bool hasInlineSelectableInput() const;
                std::vector<std::string> buildSelectableInputLines(int maxWidth) const;
                std::string selectableLineText(int row, int maxWidth) const;
                int selectableLineCount(int maxWidth) const;
                int visibleOutputRowCapacity(int lineHeight, int maxWidth) const;

                // Built-in nano-style text editor. When non-null the
                // terminal routes events / drawing to it.
                std::unique_ptr<TextEditor> editor;
                void launchEditor(const std::string &filename);
#ifndef FOR_WASM
                std::mutex inputMutex;
                std::condition_variable inputCondition;
#endif
#ifdef FOR_WASM
                // Multi-line input support
                std::string multiLineBuffer;
                bool isMultiLineInput = false;
                int blockDepth = 0;
                std::string continuationPrompt = ".. ";
#endif
        #ifdef _WIN32
                HANDLE hChildStdinRd, hChildStdinWr;  
                HANDLE hChildStdoutRd, hChildStdoutWr;  
                PROCESS_INFORMATION procInfo;  
                HANDLE bashThread;
                static DWORD WINAPI bashReaderThread(LPVOID param);
                std::mutex outputMutex;
        #elif !defined(FOR_WASM)
                pid_t bashPID = 0;
                int pipe_in[2] = {0};  
                int pipe_out[2] = {0}; 
                SDL_Thread *bashThread = 0;
                static int bashReaderThread(void *ptr);
                std::mutex outputMutex;
        #endif
        };
}

#endif
