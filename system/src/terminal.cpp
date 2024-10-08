#include"terminal.hpp"
#include<sstream>
#include<algorithm>
#include<iostream>
#include<thread>
#include<mutex>
#include"mx_window.hpp"
#include"dimension.hpp"
#include"mx_system_bar.hpp"
#ifdef FOR_WASM
#include "apps/ats/ats.h"
#endif
template<typename T>
T my_max(const T& a, const T& b) {
    return a > b ? a : b;
}

template<typename T>
T my_min(const T& a, const T& b) {
    return a < b ? a : b;
}

namespace mx {

#if defined(__linux__) || defined(__APPLE__)
    int Terminal::is_echo_enabled() {
        struct termios tty;
        tcgetattr(slave_fd, &tty); 
        return (tty.c_lflag & ECHO) != 0; 
    }
#endif

    void Terminal::screenResize(int w, int h) {
        Window::screenResize(w, h);
        const int baseWidth = 1280;
        const int baseHeight = 720;
        int screenWidth = w;
        int screenHeight = h;
        float scaleX = static_cast<float>(screenWidth) / baseWidth;
        float scaleY = static_cast<float>(screenHeight) / baseHeight;
        int windowWidth = static_cast<int>(800 * scaleX);
        int windowHeight = static_cast<int>(505 * scaleY);
        int windowPosX = (screenWidth - windowWidth) / 2;
        int windowPosY = (screenHeight - windowHeight) / 2;
        SDL_Rect rc={windowPosX, windowPosY, windowWidth, windowHeight};
        this->setRect(rc);
    }

    Terminal::Terminal(mxApp  &app) : Window(app) {
        active = true;
        std::vector<std::string> col = app.config.splitByComma(app.config.itemAtKey("terminal", "color").value);
        text_color.r = static_cast<unsigned char>(atoi(col[0].c_str()));
        text_color.g = static_cast<unsigned char>(atoi(col[1].c_str()));
        text_color.b = static_cast<unsigned char>(atoi(col[2].c_str()));
        text_color.a = 255;

        font = TTF_OpenFont(getPath(app.term_font).c_str(), 18);
        if(!font) {
            mx::system_err << "MasterX System Error: could not load system font.\n";
            mx::system_err.flush();
            exit(EXIT_FAILURE);
        }

        Window::setCanResize(true);
        
        SDL_Rect rc;
        Window::getRect(rc);
        scroll();  
#ifdef _WIN32
        SECURITY_ATTRIBUTES saAttr = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
        if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0)) {
            print("Stdout pipe creation failed");
            return;
        }
        if (!CreatePipe(&hChildStdinRd, &hChildStdinWr, &saAttr, 0)) {
            print("Stdin pipe creation failed");
            return;
        }
        STARTUPINFOA siStartInfo = {sizeof(STARTUPINFO)};
        siStartInfo.hStdError = hChildStdoutWr;
        siStartInfo.hStdOutput = hChildStdoutWr;
        siStartInfo.hStdInput = hChildStdinRd;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        std::string command  = "wsl.exe bash";
        
        if (!CreateProcessA(NULL, (LPSTR)command.data(), NULL, NULL, TRUE, 0, NULL, NULL, &siStartInfo, &procInfo)) {
            print("Process creation failed");
            return;
        }

        CloseHandle(hChildStdinRd);
        CloseHandle(hChildStdoutWr);
        bashThread = CreateThread(NULL, 0, bashReaderThread, this, 0, NULL);

 #elif !defined(FOR_WASM)
    if (openpty(&master_fd, &slave_fd, NULL, NULL, NULL) == -1) {
        perror("Failed to create PTY");
        exit(1);
    }
    bashPID = fork();
    if (bashPID == -1) {
        perror("Failed to fork bash");
        exit(1);
    } else if (bashPID == 0) {
        close(master_fd);  
        setsid();  
        ioctl(slave_fd, TIOCSCTTY, 0);  
        dup2(slave_fd, STDIN_FILENO);
        dup2(slave_fd, STDOUT_FILENO);
        dup2(slave_fd, STDERR_FILENO);
        close(slave_fd);  
        execlp("bash", "bash", NULL);
        perror("Failed to exec bash");
        exit(1);
    } else {
        close(slave_fd);  
        std::string ps1 = R"(\[\e]0;BEGIN_PROMPT\u@\h:\WEND_PROMPT\a\]\u@\h:\W$ )";
        sendCommand("export PS1=\"" + ps1 + "\"\n");
    }
    bashThread = SDL_CreateThread(bashReaderThread, "bashReaderThread", this);
    #endif
        print("MasterX System - Logged in...\n");
    }

     void Terminal::setWallpaper(SDL_Texture *tex) {
        wallpaper = tex;
     }

    void Terminal::sendCommand(const std::string &cmd) {
        #if defined(__linux__) || defined(__APPLE__)
            int bytes = 0;
            while(bytes < static_cast<int>(cmd.size())) {
                int wrote = write(master_fd, cmd.c_str(), cmd.size());
                if(wrote < 0) {
                    mx::system_err << "MasterX: System error could write.\n";
                    return;
                }
                bytes += wrote;
            }
        #elif defined(_WIN32)
            DWORD written;
            WriteFile(hChildStdinWr, cmd.c_str(), cmd.length(), &written, NULL);
        #endif
    }

    std::string Terminal::handleBackspaces(const std::string &str) {
        std::string result;
        for (char c : str) {
            if (c == '\b') {
                if (!result.empty()) {
                    result.pop_back();
                }
            } else {
                result += c;
            }
        }
        return result;
    }



    std::string cleanTerminalOutput(const std::string &input) {
        std::regex ps1LineRegex(R"((export PS1=.*))");
        return std::regex_replace(input, ps1LineRegex, "");
    }


    std::string Terminal::parseTerminalData(const std::string &input) {
        std::regex oscRegex(R"(\x1B\].*?(\x07|\x1B\\))");
        std::string inputWithoutOSC = std::regex_replace(input, oscRegex, "");
        std::regex cursorRegex(R"(\x1B\[\?25([hl]))");
        std::smatch cursorMatch;
        std::string tempInput = inputWithoutOSC;
        while (std::regex_search(tempInput, cursorMatch, cursorRegex)) {
            #ifdef __linux__
            cursorVisible = (cursorMatch[1] == "h");
            #endif
            tempInput = cursorMatch.suffix();
        }
        std::string inputWithoutCursor = std::regex_replace(inputWithoutOSC, cursorRegex, "");
        std::regex escapeRegex(R"(\x1B\[[0-9;?]*[ -/]*[@-~])");
        std::string cleanedInput = std::regex_replace(inputWithoutCursor, escapeRegex, "");

        cleanedInput = handleBackspaces(cleanedInput);
        std::regex promptRegex(R"(BEGIN_PROMPT(.*?)END_PROMPT)");
        std::smatch promptMatch;
        std::string prompt = "";
        if (std::regex_search(cleanedInput, promptMatch, promptRegex)) {
            prompt = promptMatch[1].str();
            cleanedInput = cleanedInput.substr(promptMatch.position() + promptMatch.length());
        }
        print(cleanTerminalOutput(cleanedInput));
        return prompt;
    }

    Terminal::~Terminal() {
        active = false;
#ifdef _WIN32
        TerminateProcess(procInfo.hProcess, 0);
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
        CloseHandle(hChildStdinWr);
        CloseHandle(hChildStdoutRd);
#elif !defined(FOR_WASM)
        pid_t fg_pgid = tcgetpgrp(master_fd);
        if (fg_pgid == -1) {
            mx::system_err << "MasterX: Failed to get foreground process group\n";
            mx::system_err.flush();
        }
        if (killpg(fg_pgid, SIGINT) == 0) {
            print("- Sent SIGINT to foreground process\n");
        } else {
            mx::system_err << "MasterX: failed to kill process..\n";
        }
        std::string exit_cmd = "exit\n";
        sendCommand(exit_cmd);
        if (bashPID > 0) {
            kill(bashPID, SIGTERM);
            waitpid(bashPID, nullptr, 0);
        }
        close(master_fd);
      if (bashThread) {
            SDL_WaitThread(bashThread, nullptr);
        }
#endif
        if(font != nullptr)
            TTF_CloseFont(font);
    }

    void Terminal::draw(mxApp &app) {
        if (!isVisible())
            return;

        Window::draw(app);

        if (isDraw() == false)
            return;

 
    #ifndef FOR_WASM
        if (newData == true) {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::string temp = new_data;
#if defined(__linux__) || defined(__APPLE__)
           static int cnt = 0;
            if(cnt == 0) {
                cnt ++;
                sendCommand("\n");
            } else {
                parseTerminalData(temp);
                if(temp.find("password for") != std::string::npos || temp.find("Password:") != std::string::npos) {
                    echo_enabled = false;
                }
            }
#elif !defined(FOR_WASM)
                parseTerminalData(temp);
#endif
            newData = false;
            new_data = "";
            
        }
    #endif
 
    #if defined(__linux__) || defined(__APPLE__)
        if (!outputLines.empty()) {
            prompt = outputLines.back();
        }
    #elif defined(_WIN32) || defined(FOR_WASM)
        prompt = "$ ";
    #endif
        SDL_Rect rc;
        Window::getRect(rc);
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 200);
        SDL_RenderCopy(app.ren, dim->getMatrix() ? dim->matrix_tex : wallpaper, nullptr, nullptr);
        SDL_RenderFillRect(app.ren, &rc);
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
        rc.y += 28;
        Window::drawMenubar(app);
 

        int lineHeight = TTF_FontHeight(font);
        int maxWidth = rc.w - 10;
        int y = rc.y + 5;


        for (int i = scrollOffset; i < static_cast<int>(outputLines.size()); ++i) {
            if (y + lineHeight + lineHeight > rc.y + rc.h) {
                break;
            }
            renderText(app, outputLines[i], rc.x + 5, y);
            y += lineHeight;
        }

        int cx = rc.x + 5;
        int cy = y;
#if defined(__linux__) || defined(__APPLE__)
        if(echo_enabled == true) {
#endif
            renderTextWrapped(app, prompt, inputText, cx, cy, maxWidth);
#if defined(__linux__) || defined(__APPLE__)
        }
#endif


        int totalLines = static_cast<int>(outputLines.size());

        int promptWidth;
        TTF_SizeText(font, prompt.c_str(), &promptWidth, nullptr);
        int totalWrappedLines = calculateWrappedLinesForText(inputText, maxWidth - promptWidth, promptWidth);
        totalLines += totalWrappedLines;

        if (totalLines > maxVisibleLines) {
            int offx = rc.x + rc.w;
            int offy = rc.y;
            int availableHeight = rc.h - 28;

            scrollBarHeight = (maxVisibleLines * availableHeight) / totalLines;

            if (scrollBarHeight < 10) {
                scrollBarHeight = 10;
            }

            scrollBarPosY = offy + (scrollOffset * (availableHeight - scrollBarHeight)) / (totalLines - maxVisibleLines);

            if (scrollBarPosY + scrollBarHeight > rc.y + rc.h) {
                scrollBarPosY = rc.y + rc.h - scrollBarHeight;
            }

            SDL_Rect scrollBarRect = {offx - scrollBarWidth, scrollBarPosY, scrollBarWidth, scrollBarHeight};
            SDL_SetRenderDrawColor(app.ren, 100, 100, 100, 255);
            SDL_RenderFillRect(app.ren, &scrollBarRect);
        }
    }


    void Terminal::renderText(mxApp &app, const std::string &text, int x, int y) {
        if(!text.empty()) {
            SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), text_color);
            if(surface == nullptr) {
                mx::system_err << "MasterX System Error: Render Text failed.\n";
                mx::system_err.flush();
                return;
            }
            SDL_Texture* texture = SDL_CreateTextureFromSurface(app.ren, surface);
            if(texture == nullptr) {
                mx::system_err << "MasterX System Error: Create Texture failed.\n";
                mx::system_err.flush();
                return;
            }
            SDL_Rect dstRect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(app.ren, texture, nullptr, &dstRect);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
    }

    std::vector<std::string> Terminal::splitText(const std::string &text) {
        std::vector<std::string> words;
        std::istringstream ss(text);
        std::string word;

        while (ss >> word) {  
            words.push_back(word);
        }

        return words;
    }
    
    void Terminal::drawCursor(mxApp &app, int x, int y, bool showCursor) {
        #ifdef __linux__
        if(cursorVisible && showCursor) {
        #else
        if (showCursor) {
        #endif
            int textHeight = TTF_FontHeight(font);
            SDL_SetRenderDrawColor(app.ren, text_color.r, text_color.g, text_color.b, 255);
            SDL_RenderDrawLine(app.ren, x, y, x, y + textHeight);  
        }
    }

    void Terminal::renderTextWrapped(mxApp &app, const std::string &prompt, const std::string &inputText, int &x, int &y, int maxWidth) {
        SDL_Rect rc;
        Window::getRect(rc);
        int margin = 5;
        int availableWidth = maxWidth - margin * 2;
        x = rc.x + margin;
        int lineHeight = TTF_FontHeight(font);
        int prompt_w = 0;
        TTF_SizeText(font, prompt.c_str(), &prompt_w, nullptr);
    #if defined(__linux__) || defined(__APPLE__)
        y -= lineHeight;
    #endif
        
        int cursorX = x;
        int cursorY = y;
        int charCount = 0;
        bool cursorDrawn = false;
       
        std::string remainingPrompt = prompt;
        int promptEndX = x;
        int promptEndY = y;

        while (!remainingPrompt.empty()) {
            std::string promptLineToRender;
            int promptCurrentWidth = 0;
            size_t i = 0;
            int promptLineWidth = availableWidth;
 
        
            while (i < remainingPrompt.length()) {
                std::string testLine = promptLineToRender + remainingPrompt[i];
                TTF_SizeText(font, testLine.c_str(), &promptCurrentWidth, nullptr);

                if (promptCurrentWidth > promptLineWidth) {
                    if (!promptLineToRender.empty()) {
                        break;
                    } else {
                        promptLineToRender += remainingPrompt[i++];
                        break;
                    }
                } else {
                    promptLineToRender += remainingPrompt[i++];
                }
            }

#if !defined(__linux__) && !defined(__APPLE__)
            renderText(app, promptLineToRender, x, y);
#endif

            
            promptEndX = x + promptCurrentWidth;
            promptEndY = y;

            
            y += lineHeight;
            x = rc.x + margin;
            remainingPrompt = remainingPrompt.substr(i);
        }

        
        if (promptEndX < rc.x + margin + availableWidth) {
            x = promptEndX;
            y = promptEndY;
        } else {
            x = rc.x + margin;
        }

        bool firstLine = true;
#ifdef _WIN32
        TTF_SizeText(font, "$ ",  &prompt_w, nullptr);
#endif

        int sx = x, sy = y;

        std::string remainingText = inputText;
        while (!remainingText.empty()) {
            std::string lineToRender;
            int currentWidth = 0;
            size_t i = 0;
            int lineWidth = firstLine == true ? availableWidth - prompt_w-10 : availableWidth-10;
            int lineY = y;

            while (i < remainingText.length()) {
                std::string testLine = lineToRender + remainingText[i];
                TTF_SizeText(font, testLine.c_str(), &currentWidth, nullptr);

                if (currentWidth > lineWidth) {
                    if (!lineToRender.empty()) {
                        break;
                    } else {
                        lineToRender += remainingText[i++];
                        break;
                    }
                } else {
                    lineToRender += remainingText[i++];
                }

                charCount++;
                if (!cursorDrawn && charCount == cursorPosition) {
                    cursorX = x + currentWidth;
                    cursorY = lineY;
                    cursorDrawn = true;
                }
            }

            renderText(app, lineToRender, x, lineY);
            y += lineHeight;
            x = rc.x + margin;
            firstLine = false;
            remainingText = remainingText.substr(i);
        }

        if (!cursorDrawn) {
            cursorX = x;
            cursorY = y; 
        }

        if(cursorPosition == 0) {
            cursorX = sx;
            cursorY = sy;
        }


        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - cursorTimer >= cursorBlinkInterval) {
            showCursor = !showCursor;
            cursorTimer = currentTime;
        }
        drawCursor(app, cursorX, cursorY, showCursor);
    }

    bool Terminal::event(mxApp &app, SDL_Event &e) {
        if (!Window::isVisible())
           return false;

        if (e.type == SDL_TEXTINPUT) {
            inputText.insert(cursorPosition, e.text.text);
            cursorPosition += strlen(e.text.text);
            scroll();
            return true;
        }

        if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
                case SDLK_d: {
                    SDL_Keycode keycode = e.key.keysym.sym;
                    Uint16 mod = e.key.keysym.mod;
                    bool altPressed = (mod & KMOD_ALT) != 0;

                    if (altPressed && (keycode == SDLK_d)) {
                        
#if !defined(_WIN32) && !defined(FOR_WASM)
                        char eofChar = 0x04; 
                        if(write(master_fd, &eofChar, 1) < 0) {
                            mx::system_err << "MasterX System: Error could not write...\n";
                            return false;
                        }
                        return true;
#endif
                        
                    }
                }
                break;
                case SDLK_c:
                if (e.key.keysym.mod & KMOD_CTRL) {
                    
                    #if !defined(FOR_WASM) && !defined(_WIN32)
                        echo_enabled = true;
                        pid_t fg_pgid = tcgetpgrp(master_fd);
                        if (fg_pgid == -1) {
                            mx::system_err << "MasterX: Failed to get foreground process group\n";
                            mx::system_err.flush();
                            return true;
                        }
                        if (killpg(fg_pgid, SIGINT) == 0) {
                            print("- Sent SIGINT to foreground process\n");
                        } else {
                            mx::system_err << "MasterX: failed to kill process..\n";
                            return true;
                        }
                    #endif
                }
                break;
                case SDLK_BACKSPACE:
                    if (!inputText.empty() && cursorPosition > 0) {
                        inputText.erase(cursorPosition - 1, 1);
                        cursorPosition--;
                    }
                    break;

                case SDLK_LEFT:
                    if (cursorPosition > 0) {
                        cursorPosition--;
                    }
                    break;

                case SDLK_RIGHT:
                    if (cursorPosition < static_cast<int>(inputText.length())) {
                        cursorPosition++;
                    }
                    break;
               case SDLK_UP:
                    if (!stored_commands.empty()) {
                        if (!cyclingThroughHistory) {
                            savedInputText = inputText; 
                            cyclingThroughHistory = true; 
                        }
                        if (store_offset > 0) {
                            store_offset--;
                        } else {
                            store_offset = stored_commands.size() - 1;  
                        }
                        inputText = stored_commands[store_offset];
                        cursorPosition = inputText.length();  
                    }
                    break;

            case SDLK_DOWN:
                    if (!stored_commands.empty()) {
                        if (store_offset < static_cast<int>(stored_commands.size() - 1)) {
                            store_offset++;
                            inputText = stored_commands[store_offset];
                            cursorPosition = inputText.length();  
                        } else {
                            inputText = savedInputText;
                            cursorPosition = inputText.length();  
                            cyclingThroughHistory = false;  
                        }
                    }
                    break;
            case SDLK_RETURN:
                if (!inputText.empty()) {
                    processCommand(app, inputText);            
                    store_offset = stored_commands.size();  
                    inputText.clear();
                    cursorPosition = 0;
                    scroll();
                }
                break;
                default:
                    break;
            }
        }

        SDL_Rect rc;
        Window::getRect(rc);
        rc.y += 28;
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            int mouseX = e.button.x;
            int mouseY = e.button.y;
            
            if (mouseX >= rc.x+rc.w - scrollBarWidth && mouseY >= scrollBarPosY && mouseY <= scrollBarPosY + scrollBarHeight) {
                isScrolling = true;
                scrollBarDragOffset = mouseY - scrollBarPosY;
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP) {
            isScrolling = false;
        }

        int totalLines = total_Lines();

        if (e.type == SDL_MOUSEMOTION && isScrolling) {
            int mouseY = e.motion.y;
            int newScrollPosY = mouseY - scrollBarDragOffset;
            scrollOffset = (newScrollPosY * totalLines - maxVisibleLines) / (rc.y+rc.h- scrollBarHeight);
            scrollOffset = my_max(0, my_min(scrollOffset, (totalLines - maxVisibleLines)));
            render_text = false;
        }

        if (e.type == SDL_MOUSEWHEEL) {
            handleScrolling(e.wheel.y);
            render_text = false;
            return true;
        }
        
        if(Window::event(app, e))
            return true;

        return false; 
    }

    void Terminal::handleScrolling(int direction) {
        scrollOffset -= direction;
        int totalLines = static_cast<int>(outputLines.size());
         scrollOffset = my_max(0, my_min(scrollOffset, (totalLines - maxVisibleLines)));
    }

    void Terminal::processCommand(mxApp &app, std::string command) {
        if(command.empty()) return;
#ifdef FOR_WASM        
        bool clear = false;
#endif



#if defined(__linux__) || defined(__APPLE__)
    if(echo_enabled) {
        stored_commands.push_back(command);
        store_offset = stored_commands.size()-1;
    }
     echo_enabled = true;
#else
        stored_commands.push_back(command);
        store_offset = stored_commands.size()-1;

#endif


#if defined(_WIN32) || defined(FOR_WASM)
        print("\n$ " + command + "\n");
#endif
        std::vector<std::string> words;
        words = splitText(command);

        if(words.size()==0)
            return;
        
        if(command == "matrix") {
            dim->setMatrix(app, dim->matrix_tex, !dim->getMatrix());
            print(command + "\nNeo..\n");
            command.clear();
        } else if(command == "exit") {
            app.shutdown();
        } else if (words.size()==2 && words[0] == "setfull" && words[1] == "true") {
            app.set_fullscreen(app.win, true);
            print(command + "\nMasterX System: full screen is true\n");
            command.clear();
        } else if (words.size()==2 && words[0] == "setfull" && words[1] == "false") {
           app.set_fullscreen(app.win, false);
           print(command + "\nMasterX System: full screen is false\n");
           command.clear();
        } else if(words.size()==4 && words[0] == "setcolor") {
            text_color.r = atoi(words[1].c_str());
            text_color.g = atoi(words[2].c_str());
            text_color.b = atoi(words[3].c_str());
            text_color.a = 255;
            print(command + "\nMasterX System: - set text color\n");
            command.clear();
        } else if(words.size() == 1 && words[0] == "about") {
            print(command + "\nMasterX System written by Jared Bruni\n(C) 2024 LostSideDead Software.\nhttps://lostsidedead.biz\n");
            command.clear();
        } else if(words.size() == 1  && words[0] == "clear") {
            orig_text = "";   
            sendCommand("\n");
            print("");
            scroll(); 
#ifdef FOR_WASM
        clear = true;
#endif
#ifndef _WIN32
            print("");
#else
             command.clear();
#endif
        }
        
#ifdef _WIN32
    std::string cmd = command + "\n";



    DWORD written;
    if (hChildStdinWr == INVALID_HANDLE_VALUE) {
        mx::system_err << "MasterX System: Invalid handle for stdin.\n";
    }

    mx::system_out << "MasterX: commad [ "  << command << " ]\n";

    WriteFile(hChildStdinWr, cmd.c_str(), cmd.length(), &written, NULL);
    if(written == 0) {
        mx::system_err << "MasterX System: Error wrote zero bytes..\n";
    } 
#elif !defined(FOR_WASM) 
    std::string cmd = command + "\n";
    if(command != "clear")
        if(write(master_fd, cmd.c_str(), cmd.size()) < 0) {
            mx::system_err << "MasterX System: Error on write..\n";
        }
#else
    if(command.length()>0 && clear == false) {
        print(scanATS(command));
    }
#endif
        scroll();
    }

    bool isAscii(char c) {
        if(c == ' ')    
            return true;
        if(c == '\t')
            return false;
        return isprint(static_cast<unsigned char>(c)) && c >= 32 && c <= 126;
    }

    std::string trimR(const std::string &s) {
        std::string temp;
        temp.reserve(s.length());
        for(char c : s) {
            if(c == '\t') {
                temp += "    ";
            } else if(isAscii(c))
                temp += c;
        }
        return temp;
    }


    void Terminal::updateText(const std::string &text) {                                               
        if(!text.empty()) 
            orig_text += text;

        if(orig_text.length() > 4096)
             orig_text = orig_text.substr(orig_text.size() - 4096);
    }

    void Terminal::print(const std::string &s) {
        updateText(s);
        std::string line;
        SDL_Rect rc;
        Window::getRect(rc);
        int maxWidth = rc.w - 10;  
        int w, h;
        outputLines.clear();
	    std::string total = orig_text;
        std::istringstream stream(total);
        while(std::getline(stream, line)) {
            if (line.length() > 0) {
                std::string currentLine;
                for (size_t i = 0; i < line.length(); ++i) {
                    currentLine += line[i];
                    TTF_SizeText(font, currentLine.c_str(), &w, &h);
                    if (w > maxWidth) {
                    
                        size_t lastSpace = currentLine.find_last_of(' ');
                        if (lastSpace != std::string::npos) {
                    
                            std::string part = currentLine.substr(0, lastSpace);
                            if(!part.empty())
                            outputLines.push_back(trimR(part));
                            currentLine = currentLine.substr(lastSpace + 1);  
                        } else {
                    
                            if(!currentLine.empty())
                            outputLines.push_back(trimR(currentLine));
                            currentLine.clear();
                        }
                    }
                }
                if (!currentLine.empty()) {
                    outputLines.push_back(trimR(currentLine));
                }
                scroll();  
            }
        
        }
        scroll();
    }

    int Terminal::calculateWrappedLinesForText(const std::string &text, int maxWidth, int promptWidth) {
        int lineCount = 0;  
        std::string lineToRender;
        int currentWidth = 0;
        bool isFirstLine = true;
        for (size_t i = 0; i < text.length(); ++i) {
            lineToRender += text[i];
            TTF_SizeText(font, lineToRender.c_str(), &currentWidth, nullptr);
            int currentMaxWidth = isFirstLine ? maxWidth - promptWidth : maxWidth;
            if (currentWidth > currentMaxWidth) {
                lineCount++;  
                lineToRender.clear();  
                lineToRender += text[i]; 
                TTF_SizeText(font, lineToRender.c_str(), &currentWidth, nullptr);
                isFirstLine = false; 
            }
        }
        if (!lineToRender.empty()) {
            lineCount++;

        }
//#if defined (_WIN32) || defined(FOR_WASM)
        if(inputText.empty())
            lineCount++;               
//#endif
        return lineCount;
    }

    int Terminal::calculateTotalWrappedLines() {
        int totalWrappedLines = 0;
        SDL_Rect rc;
        Window::getRect(rc);
        int maxWidth = rc.w - 10;

        for (const std::string &line : outputLines) {
            int w, h;
            TTF_SizeText(font, line.c_str(), &w, &h);

            
            if (w <= maxWidth) {
                totalWrappedLines++;
            } else {
            
                int wrappedLines = (w + maxWidth - 1) / maxWidth;  
                totalWrappedLines += wrappedLines;
            }
        }

        return totalWrappedLines;
    }

    int Terminal::total_Lines() {
        int totalLines = static_cast<int>(outputLines.size());
        SDL_Rect rc;
        Window::getRect(rc);

        std::string prompt;
        prompt = "$ ";
        int promptWidth;
        TTF_SizeText(font,prompt.c_str(), &promptWidth, nullptr);
        int total = calculateWrappedLinesForText(inputText, rc.w - 20, promptWidth);
        total += totalLines;
        return total;
    }

    void Terminal::scroll() {
        int totalLines = total_Lines();  
        SDL_Rect rc;
        Window::getRect(rc);
        int lineHeight = TTF_FontHeight(font);
        maxVisibleLines = (rc.h - 28) / lineHeight;
        if (totalLines > maxVisibleLines) {
            if (scrollOffset < totalLines - maxVisibleLines) {
                scrollOffset = my_max(0, totalLines - maxVisibleLines);
            }
        } else {
            scrollOffset = 0;
        }
    }


    void Terminal::stateChanged(bool min, bool max, bool closed) {
        isMaximized = max;
        print("");
        Window::dragging = false;
    }

#ifdef _WIN32
    DWORD WINAPI Terminal::bashReaderThread(LPVOID param) {
        Terminal* terminal = static_cast<Terminal*>(param);
        char buffer[1024];
        DWORD bytesRead;
        while (terminal->active) {
            while (ReadFile(terminal->hChildStdoutRd, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                std::lock_guard<std::mutex> lock(terminal->outputMutex);
                terminal->new_data += buffer;
                terminal->newData = true;
            }
            if (bytesRead == 0 || GetLastError() != ERROR_MORE_DATA) {
                break;
            }
            Sleep(10);
        }
        return 0;
    }
#elif !defined(FOR_WASM)
    int Terminal::bashReaderThread(void *ptr) {
        Terminal *terminal = static_cast<Terminal *>(ptr);
            char buffer[1024];
            std::string output;
            std::string pwdOutput;
            while (terminal->active) {
                ssize_t count = 0;
#ifdef __linux__
                while (terminal->active && (count = read(terminal->master_fd, buffer, sizeof(buffer) - 1)) > 0) {
#elif defined(__APPLE__)
                while ((count = read(terminal->master_fd, buffer, sizeof(buffer) - 1)) > 0) {
#endif
                       buffer[count] = '\0';
                       std::lock_guard<std::mutex> lock(terminal->outputMutex);
                       terminal->new_data += buffer;    
                       terminal->newData = true;
                }       
                if (count == -1 && errno != EAGAIN) {
                    break;
                }
            }
            return 0;
    }
                
#endif    
}
 