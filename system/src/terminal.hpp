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
                void insertText(const std::string &text);
                std::string getInput();
                bool isWaitingForInput() const { return waitingForInput; }
        private:
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
                std::vector<std::vector<SDL_Color>> outputLineColors;
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
                SDL_Texture *wallpaper;
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
                std::vector<std::vector<SDL_Color>> ansiLineColors;
                int ansiCursorRow = 0;
                int ansiCursorCol = 0;
                int ansiSavedRow = 0;
                int ansiSavedCol = 0;
                bool ansiInitialized = false;
                SDL_Color ansiCurrentColor{255, 255, 255, 255};
                void initAnsiState();
                void syncAnsiToOutput();
                void applyAnsiData(const std::string &input);

                std::atomic<bool> waitingForInput{false};
                std::string inputResult;

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
