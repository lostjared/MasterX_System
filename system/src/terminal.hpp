#ifndef __TERM_H_
#define __TERM_H_

#include"window.hpp"
#include"mx_window.hpp"
#ifdef _WIN32
#include <windows.h>
#elif !defined(FOR_WASM)
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <pty.h>
#include <utmp.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#endif
#include<thread>
#include<mutex>
#include<atomic>
#include<regex>

namespace mx {

        std::string getLastDirectory(const std::string& fullPath);
        class Terminal :  public Window {
        public:
                Terminal(mxApp  &app);
                virtual ~Terminal();
                void draw(mxApp  &app);
                bool event(mxApp &app, SDL_Event  &e);
                void print(const std::string &s);
                void scroll();
                void stateChanged(bool min, bool max, bool closed);
                void setWallpaper(SDL_Texture *tex);
                void drawCursor(mxApp &, int, int, bool);
                bool atBottom();
                std::string parseTerminalData(const std::string &);
                void sendCommand(const std::string &cmd);
        private:
                std::string prompt = "$ ";
#if !defined(FOR_WASM) && !defined(WIN32)
                int master_fd, slave_fd;
                bool cursorVisible = true;
#endif
                std::string handleBackspaces(const std::string &str); 
                std::string inputText;
                int cursorPosition = 0;
                std::vector<std::string> outputLines;
                void renderText(mxApp &app, const std::string &text, int x, int y);
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
        #ifdef _WIN32
                HANDLE hChildStdinRd, hChildStdinWr;  
                HANDLE hChildStdoutRd, hChildStdoutWr;  
                PROCESS_INFORMATION procInfo;  
                HANDLE bashThread;
                static DWORD WINAPI bashReaderThread(LPVOID param);
                std::mutex outputMutex;
        #elif !defined(FOR_WASM)
                pid_t bashPID;
                int pipe_in[2];  
                int pipe_out[2]; 
                SDL_Thread *bashThread;
                static int bashReaderThread(void *ptr);
                std::mutex outputMutex;
        #endif
        };
}

#endif
