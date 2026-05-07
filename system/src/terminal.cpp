#include"terminal.hpp"
#include<sstream>
#include<algorithm>
#include<iostream>
#include<thread>
#include<mutex>
#include<cctype>
#include<cstdlib>
#include"mx_window.hpp"
#include"dimension.hpp"
#include"mx_system_bar.hpp"
#ifdef FOR_WASM
#include "apps/cmd/cmd_shell.h"
#include "ast.hpp"
#include <emscripten.h>
extern "C" void forceFrameRender();

static mx::Terminal* g_activeTerminal = nullptr;

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void terminalPasteText(const char* text) {
        if (g_activeTerminal && text) {
            std::string pasteText(text);
            g_activeTerminal->insertText(pasteText);
        }
    }
}
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
        if (embedded_) {
            // The owning TerminalTabs controls the rect; only update the
            // PTY size so the running shell sees the new cell grid.
            updatePtySize();
            return;
        }
        SDL_Rect rc;
        Window::getRect(rc);
        const int topLimit    = 26;
        const int bottomLimit = h - 50;
        const int maxH        = std::max(1, bottomLimit - topLimit);
        if (Window::isMaximized()) {
            // Fill the host window (under the menu bar, above the system bar).
            rc = { 0, topLimit, w, h - 76 };
        } else if (rc.w <= 0 || rc.h <= 0) {
            // Initial layout: centered default size scaled to host.
            const int baseWidth  = 1280;
            const int baseHeight = 720;
            float scaleX = static_cast<float>(w) / baseWidth;
            float scaleY = static_cast<float>(h) / baseHeight;
            rc.w = static_cast<int>(800 * scaleX);
            rc.h = static_cast<int>(505 * scaleY);
            rc.x = (w - rc.w) / 2;
            rc.y = (h - rc.h) / 2;
            if (rc.y < topLimit) rc.y = topLimit;
        } else {
            // Preserve the user's chosen size; clamp to fit the new host.
            if (rc.w > w)    rc.w = w;
            if (rc.h > maxH) rc.h = maxH;
            if (rc.x < 0)             rc.x = 0;
            if (rc.y < topLimit)      rc.y = topLimit;
            if (rc.x + rc.w > w)      rc.x = std::max(0, w - rc.w);
            if (rc.y + rc.h > bottomLimit)
                rc.y = std::max(topLimit, bottomLimit - rc.h);
        }
        this->setRect(rc);
        updatePtySize();
    }

    void Terminal::writeToPty(const std::string &data) {
        if (data.empty()) return;
#ifdef _WIN32
        if (hChildStdinWr != INVALID_HANDLE_VALUE) {
            DWORD written = 0;
            WriteFile(hChildStdinWr, data.c_str(),
                      static_cast<DWORD>(data.size()), &written, NULL);
        }
#elif !defined(FOR_WASM)
        if (master_fd >= 0) {
            ssize_t r = ::write(master_fd, data.c_str(), data.size());
            (void)r;
        }
#endif
    }

    void Terminal::updatePtySize() {
#if !defined(_WIN32) && !defined(FOR_WASM)
        if (master_fd < 0 || !font) return;
        SDL_Rect rc;
        Window::getRect(rc);
        int cellH = TTF_FontHeight(font);
        int cellW = 0;
        TTF_SizeText(font, "M", &cellW, nullptr);
        if (cellW <= 0) cellW = 8;
        if (cellH <= 0) cellH = 16;
        int contentW = std::max(cellW, rc.w - 10);
        int contentH = std::max(cellH, rc.h - 38);
        termCols = std::max(20, contentW / cellW);
        termRows = std::max(5, contentH / cellH);
        if (termRows == lastReportedRows && termCols == lastReportedCols)
            return;
        lastReportedRows = termRows;
        lastReportedCols = termCols;
        struct winsize ws{};
        ws.ws_row    = static_cast<unsigned short>(termRows);
        ws.ws_col    = static_cast<unsigned short>(termCols);
        ws.ws_xpixel = static_cast<unsigned short>(rc.w);
        ws.ws_ypixel = static_cast<unsigned short>(rc.h);
        ioctl(master_fd, TIOCSWINSZ, &ws);
        if (altScreen) {
            // Resize alternate-screen grid to match new dimensions.
            if (static_cast<int>(ansiLines.size()) < termRows) {
                ansiLines.resize(termRows);
                ansiLineColors.resize(termRows);
            }
            if (scrollBot >= 0 && scrollBot >= termRows)
                scrollBot = termRows - 1;
        }
#endif
    }

    void Terminal::enterAltScreen() {
        if (altScreen) return;
        if (!ansiInitialized) initAnsiState();
        savedLines       = ansiLines;
        savedLineColors  = ansiLineColors;
        savedAltRow      = ansiCursorRow;
        savedAltCol      = ansiCursorCol;
        savedAltFg       = ansiCurrentColor;
        savedAltBg       = ansiCurrentBg;
        savedAltBold     = ansiBold;
        savedAltUnderline= ansiUnderline;

        altScreen = true;
        ansiLines.assign(std::max(5, termRows), std::string{});
        ansiLineColors.assign(ansiLines.size(), std::vector<CharStyle>{});
        ansiCursorRow = 0;
        ansiCursorCol = 0;
        scrollTop = 0;
        scrollBot = static_cast<int>(ansiLines.size()) - 1;
        ansiCurrentColor = text_color;
        ansiCurrentBg = {0,0,0,0};
        ansiBold = false;
        ansiUnderline = false;
        scrollOffset = 0;
        syncAnsiToOutput();
    }

    void Terminal::leaveAltScreen() {
        if (!altScreen) return;
        altScreen = false;
        ansiLines       = savedLines;
        ansiLineColors  = savedLineColors;
        ansiCursorRow   = savedAltRow;
        ansiCursorCol   = savedAltCol;
        ansiCurrentColor= savedAltFg;
        ansiCurrentBg   = savedAltBg;
        ansiBold        = savedAltBold;
        ansiUnderline   = savedAltUnderline;
        savedLines.clear();
        savedLineColors.clear();
        scrollTop = 0;
        scrollBot = -1;
        decckm = false;
        keypadApp = false;
        syncAnsiToOutput();
        scroll();
    }

    std::string Terminal::keyToPtyBytes(SDL_Keycode sym, Uint16 mod) {
        const bool ctrl  = (mod & KMOD_CTRL)  != 0;
        const bool alt   = (mod & KMOD_ALT)   != 0;
        // Build optional ALT prefix (ESC) per xterm convention.
        auto withAlt = [&](const std::string &s) -> std::string {
            return alt ? std::string("\x1b") + s : s;
        };
        auto cursorSeq = [&](char letter) -> std::string {
            return withAlt(decckm ? std::string("\x1bO") + letter
                                  : std::string("\x1b[") + letter);
        };
        switch (sym) {
            case SDLK_RETURN:    return withAlt("\r");
            case SDLK_KP_ENTER:  return withAlt("\r");
            case SDLK_TAB:       return withAlt("\t");
            case SDLK_BACKSPACE: return withAlt("\x7f");
            case SDLK_ESCAPE:    return std::string("\x1b");
            case SDLK_UP:        return cursorSeq('A');
            case SDLK_DOWN:      return cursorSeq('B');
            case SDLK_RIGHT:     return cursorSeq('C');
            case SDLK_LEFT:      return cursorSeq('D');
            case SDLK_HOME:      return withAlt(decckm ? "\x1bOH" : "\x1b[H");
            case SDLK_END:       return withAlt(decckm ? "\x1bOF" : "\x1b[F");
            case SDLK_PAGEUP:    return withAlt("\x1b[5~");
            case SDLK_PAGEDOWN:  return withAlt("\x1b[6~");
            case SDLK_INSERT:    return withAlt("\x1b[2~");
            case SDLK_DELETE:    return withAlt("\x1b[3~");
            case SDLK_F1:        return withAlt("\x1bOP");
            case SDLK_F2:        return withAlt("\x1bOQ");
            case SDLK_F3:        return withAlt("\x1bOR");
            case SDLK_F4:        return withAlt("\x1bOS");
            case SDLK_F5:        return withAlt("\x1b[15~");
            case SDLK_F6:        return withAlt("\x1b[17~");
            case SDLK_F7:        return withAlt("\x1b[18~");
            case SDLK_F8:        return withAlt("\x1b[19~");
            case SDLK_F9:        return withAlt("\x1b[20~");
            case SDLK_F10:       return withAlt("\x1b[21~");
            case SDLK_F11:       return withAlt("\x1b[23~");
            case SDLK_F12:       return withAlt("\x1b[24~");
            default: break;
        }
        if (ctrl && sym >= SDLK_a && sym <= SDLK_z) {
            char c = static_cast<char>((sym - SDLK_a + 1) & 0x1f);
            return withAlt(std::string(1, c));
        }
        if (ctrl) {
            switch (sym) {
                case SDLK_SPACE:     return withAlt(std::string(1, '\0'));
                case SDLK_LEFTBRACKET:  return withAlt("\x1b");
                case SDLK_BACKSLASH:    return withAlt("\x1c");
                case SDLK_RIGHTBRACKET: return withAlt("\x1d");
                case SDLK_6:            return withAlt("\x1e");
                case SDLK_MINUS:        return withAlt("\x1f");
                default: break;
            }
        }
        return std::string{};
    }

    bool Terminal::handleRawKeyEvent(mxApp &app, SDL_Event &e) {
        (void)app;
        if (e.type == SDL_TEXTINPUT) {
            // Send plain typed text directly to the PTY.
            writeToPty(e.text.text);
            return true;
        }
        if (e.type == SDL_KEYDOWN) {
            const Uint16 mod = e.key.keysym.mod;
            const SDL_Keycode sym = e.key.keysym.sym;
            // Let SDL_TEXTINPUT deliver printable characters for non-Ctrl/Alt
            // keypresses so we don't double-send them here.
            const bool printableKey =
                (sym >= SDLK_SPACE && sym <= SDLK_z) || sym == SDLK_RETURN ||
                sym == SDLK_TAB || sym == SDLK_BACKSPACE || sym == SDLK_ESCAPE;
            const bool needsTranslation =
                (mod & (KMOD_CTRL | KMOD_ALT)) != 0 ||
                sym == SDLK_UP || sym == SDLK_DOWN || sym == SDLK_LEFT || sym == SDLK_RIGHT ||
                sym == SDLK_HOME || sym == SDLK_END || sym == SDLK_PAGEUP || sym == SDLK_PAGEDOWN ||
                sym == SDLK_INSERT || sym == SDLK_DELETE ||
                sym == SDLK_RETURN || sym == SDLK_KP_ENTER ||
                sym == SDLK_TAB || sym == SDLK_BACKSPACE || sym == SDLK_ESCAPE ||
                (sym >= SDLK_F1 && sym <= SDLK_F12);
            if (printableKey && !needsTranslation)
                return false;  // wait for SDL_TEXTINPUT
            std::string seq = keyToPtyBytes(sym, mod);
            if (!seq.empty()) {
                writeToPty(seq);
                return true;
            }
        }
        return false;
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
        // Identify ourselves as a real ANSI/xterm-compatible terminal so
        // full-screen programs (vim, nano, less, top, ...) emit proper
        // escape sequences instead of falling back to a dumb terminal.
        setenv("TERM", "xterm-256color", 1);
        setenv("COLORTERM", "truecolor", 1);
        unsetenv("LINES");
        unsetenv("COLUMNS");
        // Set PS1 via the environment so bash's *first* prompt already
        // uses our format -- avoids the double-prompt that resulted from
        // running an `export PS1=...` line after bash had already drawn
        // its default prompt.
        setenv("PS1", "\\u@\\h:\\W\\$ ", 1);
        execlp("bash", "bash", NULL);
        perror("Failed to exec bash");
        exit(1);
    } else {
        close(slave_fd);
        // Push initial PTY window size so child knows our cell grid.
        updatePtySize();
    }
    bashThread = SDL_CreateThread(bashReaderThread, "bashReaderThread", this);
    #endif
    }

     void Terminal::setWallpaper(SDL_Texture *tex) {
        wallpaper = tex;
     }

    void Terminal::pasteFromClipboard() {
#ifdef FOR_WASM
        EM_ASM({
            navigator.clipboard.readText().then(function(text) {
                if (text && text.length > 0) {
                    var cleanText = text.split(String.fromCharCode(13)).join("").split(String.fromCharCode(10)).join("");
                    Module.ccall('terminalPasteText', null, ['string'], [cleanText]);
                }
            }).catch(function(err) {
                console.log('Clipboard read failed:', err);
            });
        });
#else
        char* clipboardText = SDL_GetClipboardText();
        if (clipboardText && clipboardText[0] != '\0') {
            std::string pasteText(clipboardText);
#if defined(__linux__) || defined(__APPLE__)
            // PTY raw mode: send directly to the shell. Normalize CRLF/CR
            // to LF so multi-line pastes are interpreted as Enter presses
            // by the running program (just like xterm/gnome-terminal).
            if (master_fd > 0 && !waitingForInput) {
                std::string normalized;
                normalized.reserve(pasteText.size());
                for (size_t i = 0; i < pasteText.size(); ++i) {
                    char c = pasteText[i];
                    if (c == '\r') {
                        normalized.push_back('\n');
                        if (i + 1 < pasteText.size() && pasteText[i + 1] == '\n')
                            ++i; // skip LF of CRLF
                    } else {
                        normalized.push_back(c);
                    }
                }
                writeToPty(normalized);
                SDL_free(clipboardText);
                return;
            }
#elif defined(_WIN32)
            // Windows: send to child stdin if a PTY-style pipe exists.
            if (hChildStdinWr != INVALID_HANDLE_VALUE && !waitingForInput) {
                std::string normalized;
                normalized.reserve(pasteText.size());
                for (size_t i = 0; i < pasteText.size(); ++i) {
                    char c = pasteText[i];
                    if (c == '\r') {
                        normalized.push_back('\r');
                        normalized.push_back('\n');
                        if (i + 1 < pasteText.size() && pasteText[i + 1] == '\n')
                            ++i;
                    } else if (c == '\n') {
                        normalized.push_back('\r');
                        normalized.push_back('\n');
                    } else {
                        normalized.push_back(c);
                    }
                }
                writeToPty(normalized);
                SDL_free(clipboardText);
                return;
            }
#endif
            // Cooked / built-in input() mode fallback: edit local buffer.
            for (char c : pasteText) {
                if (c != '\r' && c != '\n') {
                    inputText.insert(cursorPosition, 1, c);
                    cursorPosition++;
                }
            }
            scroll();
        }
        SDL_free(clipboardText);
#endif
    }

    void Terminal::copyToClipboard() {
#ifdef FOR_WASM
        std::string allText = orig_text;
        EM_ASM({
            var text = UTF8ToString($0);
            navigator.clipboard.writeText(text).then(function() {
                console.log('Copied to clipboard');
            }).catch(function(err) {
                console.log('Clipboard write failed:', err);
            });
        }, allText.c_str());
#else
        SDL_SetClipboardText(orig_text.c_str());
#endif
    }

    void Terminal::insertText(const std::string &text) {
        for (char c : text) {
            if (c != '\r' && c != '\n') {
                inputText.insert(cursorPosition, 1, c);
                cursorPosition++;
            }
        }
        scroll();
    }

    bool Terminal::pointToCell(int px, int py, int &row, int &col) const {
        if (!font) return false;
        SDL_Rect rc;
        const_cast<Terminal*>(this)->Window::getRect(rc);
        // Content rect: matches draw() (skip 28px title bar + 5px pad).
        int contentX = rc.x + 5;
        int contentY = rc.y + 28 + 5;
        int contentW = rc.w - 10;
        int contentH = rc.h - 28 - 10;
        if (px < rc.x || px >= rc.x + rc.w) return false;
        if (py < rc.y + 28 || py >= rc.y + rc.h) return false;
        int lineH = TTF_FontHeight(const_cast<TTF_Font*>(font));
        if (lineH <= 0) lineH = 16;
        int cellW = 0;
        TTF_SizeText(const_cast<TTF_Font*>(font), "M", &cellW, nullptr);
        if (cellW <= 0) cellW = 8;
        int relY = py - contentY;
        int relX = px - contentX;
        if (relY < 0) relY = 0;
        if (relX < 0) relX = 0;
        int visibleRow = relY / lineH;
        int absCol = relX / cellW;
        if (absCol < 0) absCol = 0;
        int maxWidth = rc.w - 10;

#ifdef FOR_WASM
        if (!hasInlineSelectableInput()) {
            std::vector<std::string> inputLines = buildSelectableInputLines(maxWidth);
            if (!inputLines.empty()) {
                int outputRows = visibleOutputRowCapacity(lineH, maxWidth);
                if (visibleRow >= outputRows) {
                    int inputRow = visibleRow - outputRows;
                    if (inputRow < 0) inputRow = 0;
                    if (inputRow >= static_cast<int>(inputLines.size()))
                        inputRow = static_cast<int>(inputLines.size()) - 1;
                    row = static_cast<int>(outputLines.size()) + inputRow;
                    col = absCol;
                    (void)contentW; (void)contentH;
                    return true;
                }
            }
        }
#endif

        int absRow = visibleRow + (altScreen ? 0 : scrollOffset);
        int total = selectableLineCount(maxWidth);
        if (total == 0) return false;
        if (absRow < 0) absRow = 0;
        if (absRow >= total) absRow = total - 1;
        row = absRow;
        col = absCol;
        (void)contentW; (void)contentH;
        return true;
    }

    bool Terminal::hasInlineSelectableInput() const {
#ifdef FOR_WASM
        return waitingForInput && !orig_text.empty() && orig_text.back() != '\n';
#else
        return false;
#endif
    }

    std::vector<std::string> Terminal::buildSelectableInputLines(int maxWidth) const {
        std::vector<std::string> lines;
#ifdef FOR_WASM
        if (hasInlineSelectableInput()) return lines;
        if (!font) {
            lines.push_back((waitingForInput ? "" : prompt) + inputText);
            return lines;
        }

        const int margin = 5;
        int availableWidth = maxWidth - margin * 2;
        if (availableWidth < 1) availableWidth = 1;

        auto appendWrappedLine = [&](const std::string &prefix, const std::string &text) {
            int prefixWidth = 0;
            TTF_SizeText(const_cast<TTF_Font*>(font), prefix.c_str(), &prefixWidth, nullptr);

            if (text.empty()) {
                lines.push_back(prefix);
                return;
            }

            size_t pos = 0;
            bool firstVisualLine = true;
            while (pos < text.size()) {
                std::string chunk;
                int currentWidth = 0;
                int lineWidth = firstVisualLine ? availableWidth - prefixWidth : availableWidth;
                if (lineWidth < 1) lineWidth = 1;

                while (pos < text.size()) {
                    std::string testChunk = chunk + text[pos];
                    TTF_SizeText(const_cast<TTF_Font*>(font), testChunk.c_str(), &currentWidth, nullptr);
                    if (currentWidth > lineWidth) {
                        if (chunk.empty()) {
                            chunk += text[pos++];
                        }
                        break;
                    }
                    chunk = std::move(testChunk);
                    ++pos;
                }

                lines.push_back(firstVisualLine ? prefix + chunk : chunk);
                firstVisualLine = false;
            }
        };

        if (inputText.find('\n') != std::string::npos) {
            bool firstLogicalLine = true;
            size_t pos = 0;
            while (true) {
                size_t next = inputText.find('\n', pos);
                std::string segment = (next == std::string::npos)
                    ? inputText.substr(pos)
                    : inputText.substr(pos, next - pos);
                appendWrappedLine(firstLogicalLine ? (waitingForInput ? "" : prompt) : continuationPrompt,
                                  segment);
                if (next == std::string::npos) break;
                pos = next + 1;
                firstLogicalLine = false;
            }
        } else {
            appendWrappedLine(waitingForInput ? "" : prompt, inputText);
        }

        if (lines.empty()) {
            lines.push_back(waitingForInput ? "" : prompt);
        }
#else
        (void)maxWidth;
#endif
        return lines;
    }

    std::string Terminal::selectableLineText(int row, int maxWidth) const {
        if (row < 0) return "";

        const int outputCount = static_cast<int>(outputLines.size());
        if (row < outputCount) {
#ifdef FOR_WASM
            if (hasInlineSelectableInput() && row == outputCount - 1) {
                return outputLines[row] + inputText;
            }
#endif
            return outputLines[row];
        }

        std::vector<std::string> inputLines = buildSelectableInputLines(maxWidth);
        int inputRow = row - outputCount;
        if (inputRow < 0 || inputRow >= static_cast<int>(inputLines.size())) return "";
        return inputLines[inputRow];
    }

    int Terminal::selectableLineCount(int maxWidth) const {
        return static_cast<int>(outputLines.size()) + static_cast<int>(buildSelectableInputLines(maxWidth).size());
    }

    int Terminal::visibleOutputRowCapacity(int lineHeight, int maxWidth) const {
        if (lineHeight <= 0) return 0;

        SDL_Rect rc;
        const_cast<Terminal*>(this)->Window::getRect(rc);
        // Mirror draw(): after the title bar adjustment, y starts at
        // (rc.y + 28) + 5 and the loop breaks when
        //   y + lineHeight * (inputRows + 1) > rc.y + rc.h
        // so the maximum drawable output rows is
        //   floor((contentH - 5) / lineHeight) - (inputRows + 1) + 1
        // = floor((contentH - 5) / lineHeight) - inputRows.
        int contentH = rc.h - 28;
        if (contentH < lineHeight) return 0;

#ifdef FOR_WASM
        if (!hasInlineSelectableInput()) {
            int inputRows = static_cast<int>(buildSelectableInputLines(maxWidth).size());
            if (inputRows < 1) inputRows = 1;
            int capacity = (contentH - 5) / lineHeight - inputRows;
            if (capacity < 0) capacity = 0;
            // Number of output rows that draw() will actually render.
            int available = static_cast<int>(outputLines.size()) - scrollOffset;
            if (available < 0) available = 0;
            int drawn = available < capacity ? available : capacity;
            if (drawn < 0) drawn = 0;
            return drawn;
        }
#endif
        (void)maxWidth;
        int visibleRows = contentH / lineHeight;
        if (visibleRows < 1) visibleRows = 1;
        return visibleRows;
    }

    void Terminal::normalizedSelection(int &r0, int &c0, int &r1, int &c1) const {
        r0 = selAnchorRow; c0 = selAnchorCol;
        r1 = selFocusRow;  c1 = selFocusCol;
        if (r1 < r0 || (r1 == r0 && c1 < c0)) {
            std::swap(r0, r1);
            std::swap(c0, c1);
        }
    }

    std::string Terminal::getSelectedText() const {
        if (!hasSelection) return "";
        int r0, c0, r1, c1;
        normalizedSelection(r0, c0, r1, c1);
        std::string out;
        SDL_Rect rc;
        const_cast<Terminal*>(this)->Window::getRect(rc);
        int maxWidth = rc.w - 10;
        int total = selectableLineCount(maxWidth);
        for (int r = r0; r <= r1 && r < total; ++r) {
            std::string line = selectableLineText(r, maxWidth);
            int len = static_cast<int>(line.size());
            int startC = (r == r0) ? c0 : 0;
            int endC   = (r == r1) ? c1 : len;
            if (startC < 0) startC = 0;
            if (endC > len) endC = len;
            if (startC < endC)
                out.append(line, startC, endC - startC);
            if (r != r1) out.push_back('\n');
        }
        return out;
    }

    void Terminal::copySelectionToClipboard() {
        std::string sel = getSelectedText();
        if (sel.empty()) return;
#ifdef FOR_WASM
        EM_ASM({
            var text = UTF8ToString($0);
            navigator.clipboard.writeText(text).then(function() {}).catch(function(err) {});
        }, sel.c_str());
#else
        SDL_SetClipboardText(sel.c_str());
#endif
    }

    void Terminal::clearSelection() {
        selecting = false;
        hasSelection = false;
        selAnchorRow = selAnchorCol = 0;
        selFocusRow  = selFocusCol  = 0;
    }

    void Terminal::drawSelection(mxApp &app, const SDL_Rect &contentRect, int lineHeight, int cellW) {
        if (!hasSelection) return;
        int r0, c0, r1, c1;
        normalizedSelection(r0, c0, r1, c1);
        int maxWidth = contentRect.w;
        int total = selectableLineCount(maxWidth);
        int outputCount = static_cast<int>(outputLines.size());
        int baseRow = altScreen ? 0 : scrollOffset;
        int inputBaseRow = visibleOutputRowCapacity(lineHeight, maxWidth);
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app.ren, 100, 100,200, 110);
        for (int r = r0; r <= r1 && r < total; ++r) {
            int y = 0;
            if (r < outputCount) {
                int screenRow = r - baseRow;
                if (screenRow < 0) continue;
                y = contentRect.y + 5 + screenRow * lineHeight;
            } else {
                int inputRow = r - outputCount;
                y = contentRect.y + 5 + (inputBaseRow + inputRow) * lineHeight;
            }
            if (y + lineHeight <= contentRect.y) continue;
            if (y >= contentRect.y + contentRect.h) break;
            std::string line = selectableLineText(r, maxWidth);
            int lineLen = static_cast<int>(line.size());
            int startC = (r == r0) ? c0 : 0;
            int endC   = (r == r1) ? c1 : lineLen;
            if (startC < 0) startC = 0;
            if (endC < startC) endC = startC;
            if (r != r1 && endC == startC) endC = startC + 1; // visual hint for empty lines mid-selection
            int x = contentRect.x + 5 + startC * cellW;
            int w = (endC - startC) * cellW;
            if (w < 1) w = 1;
            SDL_Rect sr{ x, y, w, lineHeight };
            SDL_RenderFillRect(app.ren, &sr);
        }
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
    }

    std::string Terminal::getInput() {
        waitingForInput = true;
        inputResult.clear();
        
#ifdef FOR_WASM
        scroll();
        forceFrameRender();
        
        while (waitingForInput) {
            forceFrameRender();
            emscripten_sleep(16);
        }
#else
        std::unique_lock<std::mutex> lock(inputMutex);
        inputCondition.wait(lock, [this]() { return !waitingForInput; });
#endif
        
        return inputResult;
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

    static SDL_Color ansiBasicColor(int code, const SDL_Color &fallback) {
        static const SDL_Color normal[8] = {
            {0, 0, 0, 255},       {205, 49, 49, 255},  {13, 188, 121, 255},
            {229, 229, 16, 255},  {36, 114, 200, 255}, {188, 63, 188, 255},
            {17, 168, 205, 255},  {229, 229, 229, 255}};
        static const SDL_Color bright[8] = {
            {102, 102, 102, 255}, {241, 76, 76, 255},  {35, 209, 139, 255},
            {245, 245, 67, 255},  {59, 142, 234, 255}, {214, 112, 214, 255},
            {41, 184, 219, 255},  {255, 255, 255, 255}};

        if (code >= 30 && code <= 37)
            return normal[code - 30];
        if (code >= 90 && code <= 97)
            return bright[code - 90];
        if (code == 39)
            return fallback;
        return fallback;
    }

    static SDL_Color ansiBasicBgColor(int code) {
        static const SDL_Color normal[8] = {
            {0, 0, 0, 255},       {205, 49, 49, 255},  {13, 188, 121, 255},
            {229, 229, 16, 255},  {36, 114, 200, 255}, {188, 63, 188, 255},
            {17, 168, 205, 255},  {229, 229, 229, 255}};
        static const SDL_Color bright[8] = {
            {102, 102, 102, 255}, {241, 76, 76, 255},  {35, 209, 139, 255},
            {245, 245, 67, 255},  {59, 142, 234, 255}, {214, 112, 214, 255},
            {41, 184, 219, 255},  {255, 255, 255, 255}};
        if (code >= 40 && code <= 47)
            return normal[code - 40];
        if (code >= 100 && code <= 107)
            return bright[code - 100];
        return {0, 0, 0, 0};
    }

    void Terminal::initAnsiState() {
        ansiLines = outputLines;
        ansiLineColors.clear();
        ansiLineColors.reserve(ansiLines.size());
        CharStyle defStyle{text_color, {0, 0, 0, 0}, false, false};
        for (const auto &line : ansiLines) {
            ansiLineColors.emplace_back(line.size(), defStyle);
        }
        if (ansiLines.empty()) {
            ansiLines.emplace_back("");
            ansiLineColors.emplace_back();
        }
        ansiCursorRow = static_cast<int>(ansiLines.size()) - 1;
        ansiCursorCol = static_cast<int>(ansiLines.back().size());
        ansiSavedRow = ansiCursorRow;
        ansiSavedCol = ansiCursorCol;
        ansiCurrentColor = text_color;
        ansiCurrentBg = {0, 0, 0, 0};
        ansiBold = false;
        ansiUnderline = false;
        ansiInitialized = true;
    }

    void Terminal::syncAnsiToOutput() {
        outputLines = ansiLines;
        outputLineColors = ansiLineColors;

        std::ostringstream oss;
        for (size_t i = 0; i < outputLines.size(); ++i) {
            oss << outputLines[i];
            if (i + 1 < outputLines.size())
                oss << '\n';
        }
        orig_text = oss.str();
        if (orig_text.length() > 4096)
            orig_text = orig_text.substr(orig_text.size() - 4096);
        scroll();
    }

    void Terminal::applyAnsiData(const std::string &input) {
        if (!ansiInitialized)
            initAnsiState();

        auto ensureRow = [&](int row) {
            while (row >= static_cast<int>(ansiLines.size())) {
                ansiLines.emplace_back("");
                ansiLineColors.emplace_back();
            }
            if (row < 0)
                ansiCursorRow = 0;
        };

        auto ensureCol = [&](int row, int col) {
            if (row < 0)
                return;
            ensureRow(row);
            if (col > static_cast<int>(ansiLines[row].size())) {
                int pad = col - static_cast<int>(ansiLines[row].size());
                ansiLines[row].append(static_cast<size_t>(pad), ' ');
                CharStyle padStyle{text_color, {0, 0, 0, 0}, false, false};
                auto &colors = ansiLineColors[row];
                colors.insert(colors.end(), static_cast<size_t>(pad), padStyle);
            }
            if (static_cast<int>(ansiLineColors[row].size()) < static_cast<int>(ansiLines[row].size())) {
                CharStyle padStyle{text_color, {0, 0, 0, 0}, false, false};
                ansiLineColors[row].resize(ansiLines[row].size(), padStyle);
            }
        };

        auto clearToLineEnd = [&]() {
            ensureRow(ansiCursorRow);
            ensureCol(ansiCursorRow, ansiCursorCol);
            if (ansiCursorCol < static_cast<int>(ansiLines[ansiCursorRow].size())) {
                ansiLines[ansiCursorRow].erase(static_cast<size_t>(ansiCursorCol));
                ansiLineColors[ansiCursorRow].erase(
                    ansiLineColors[ansiCursorRow].begin() + ansiCursorCol,
                    ansiLineColors[ansiCursorRow].end());
            }
        };

        auto regionBounds = [&](int &top, int &bot) {
            top = scrollTop;
            int maxRow = static_cast<int>(ansiLines.size()) - 1;
            if (scrollBot < 0)
                bot = maxRow;
            else
                bot = std::min(scrollBot, maxRow);
            if (top < 0) top = 0;
            if (bot < top) bot = top;
        };

        auto scrollRegionUp = [&](int n) {
            int top, bot; regionBounds(top, bot);
            if (n <= 0) return;
            int span = bot - top + 1;
            if (n > span) n = span;
            for (int k = 0; k < n; ++k) {
                if (top < static_cast<int>(ansiLines.size())) {
                    ansiLines.erase(ansiLines.begin() + top);
                    ansiLineColors.erase(ansiLineColors.begin() + top);
                }
                if (bot < static_cast<int>(ansiLines.size())) {
                    ansiLines.insert(ansiLines.begin() + bot, std::string{});
                    ansiLineColors.insert(ansiLineColors.begin() + bot,
                                          std::vector<CharStyle>{});
                } else {
                    ansiLines.emplace_back();
                    ansiLineColors.emplace_back();
                }
            }
        };

        auto scrollRegionDown = [&](int n) {
            int top, bot; regionBounds(top, bot);
            if (n <= 0) return;
            int span = bot - top + 1;
            if (n > span) n = span;
            for (int k = 0; k < n; ++k) {
                if (bot < static_cast<int>(ansiLines.size())) {
                    ansiLines.erase(ansiLines.begin() + bot);
                    ansiLineColors.erase(ansiLineColors.begin() + bot);
                }
                ansiLines.insert(ansiLines.begin() + top, std::string{});
                ansiLineColors.insert(ansiLineColors.begin() + top,
                                      std::vector<CharStyle>{});
            }
        };

        auto doNewline = [&]() {
            int top, bot; regionBounds(top, bot);
            if (ansiCursorRow >= bot && (altScreen || scrollBot >= 0)) {
                scrollRegionUp(1);
                ansiCursorRow = bot;
            } else {
                ansiCursorRow++;
                ensureRow(ansiCursorRow);
            }
        };

        auto putChar = [&](char ch) {
            ensureCol(ansiCursorRow, ansiCursorCol);
            auto &line = ansiLines[ansiCursorRow];
            auto &colors = ansiLineColors[ansiCursorRow];
            CharStyle cs{ansiCurrentColor, ansiCurrentBg, ansiBold, ansiUnderline};
            if (ansiCursorCol < static_cast<int>(line.size())) {
                line[ansiCursorCol] = ch;
                colors[ansiCursorCol] = cs;
            } else {
                line.push_back(ch);
                colors.push_back(cs);
            }
            ansiCursorCol++;
        };

        auto parseParams = [](const std::string &paramText) {
            std::vector<int> out;
            if (paramText.empty()) {
                out.push_back(0);
                return out;
            }
            std::string token;
            for (char ch : paramText) {
                if (ch == ';') {
                    if (token.empty())
                        out.push_back(0);
                    else
                        out.push_back(std::atoi(token.c_str()));
                    token.clear();
                } else {
                    token.push_back(ch);
                }
            }
            if (token.empty())
                out.push_back(0);
            else
                out.push_back(std::atoi(token.c_str()));
            return out;
        };

        const std::string data = cleanTerminalOutput(input);
        size_t i = 0;
        while (i < data.size()) {
            unsigned char ch = static_cast<unsigned char>(data[i]);
            if (ch == 0x1B) {
                if (i + 1 < data.size() && data[i + 1] == ']') {
                    // OSC: consume until BEL or ST.
                    size_t j = i + 2;
                    while (j < data.size()) {
                        if (data[j] == '\a') {
                            j++;
                            break;
                        }
                        if (data[j] == 0x1B && (j + 1) < data.size() && data[j + 1] == '\\') {
                            j += 2;
                            break;
                        }
                        ++j;
                    }
                    i = j;
                    continue;
                }

                if (i + 1 < data.size() && data[i + 1] != '[') {
                    // Non-CSI ESC sequences: ESC X (single char).
                    char esc2 = data[i + 1];
                    bool consumed = true;
                    switch (esc2) {
                        case 'D': // IND - line feed
                            doNewline();
                            break;
                        case 'M': // RI - reverse line feed
                            if (ansiCursorRow <= scrollTop)
                                scrollRegionDown(1);
                            else {
                                ansiCursorRow--;
                                if (ansiCursorRow < 0) ansiCursorRow = 0;
                            }
                            break;
                        case 'E': // NEL - next line
                            ansiCursorCol = 0;
                            doNewline();
                            break;
                        case '7': // DECSC - save cursor + attrs
                            ansiSavedRow = ansiCursorRow;
                            ansiSavedCol = ansiCursorCol;
                            savedAltFg = ansiCurrentColor;
                            savedAltBg = ansiCurrentBg;
                            savedAltBold = ansiBold;
                            savedAltUnderline = ansiUnderline;
                            break;
                        case '8': // DECRC - restore cursor + attrs
                            ansiCursorRow = ansiSavedRow;
                            ansiCursorCol = ansiSavedCol;
                            ansiCurrentColor = savedAltFg;
                            ansiCurrentBg    = savedAltBg;
                            ansiBold         = savedAltBold;
                            ansiUnderline    = savedAltUnderline;
                            ensureRow(ansiCursorRow);
                            break;
                        case '=': keypadApp = true; break;
                        case '>': keypadApp = false; break;
                        case 'H': /* HTS - tab set, ignore */ break;
                        case 'c': /* RIS - full reset, ignore */ break;
                        case '(': case ')': case '*': case '+':
                            // Charset designation: G0/G1/G2/G3 - consume next byte.
                            if (i + 2 < data.size()) i++;
                            break;
                        default:
                            consumed = false;
                            break;
                    }
                    if (consumed) {
                        i += 2;
                        continue;
                    }
                }

                if (i + 1 < data.size() && data[i + 1] == '[') {
                    size_t j = i + 2;
                    while (j < data.size()) {
                        unsigned char cj = static_cast<unsigned char>(data[j]);
                        if (cj >= 0x40 && cj <= 0x7E)
                            break;
                        ++j;
                    }
                    if (j >= data.size())
                        break;

                    const char cmd = data[j];
                    std::string params = data.substr(i + 2, j - (i + 2));
                    bool privateMode = !params.empty() && params[0] == '?';
                    if (privateMode)
                        params.erase(params.begin());

                    auto nums = parseParams(params);
                    auto n0 = [&]() { return nums.empty() || nums[0] <= 0 ? 1 : nums[0]; };

                    if (privateMode && (cmd == 'h' || cmd == 'l')) {
                        bool set = (cmd == 'h');
                        for (int code : nums) {
                            switch (code) {
                                case 25:
#ifdef __linux__
                                    cursorVisible = set;
#endif
                                    break;
                                case 1:    decckm = set;     break;
                                case 47:
                                case 1047:
                                case 1049:
                                    if (set) enterAltScreen();
                                    else     leaveAltScreen();
                                    break;
                                case 1048: // save/restore cursor
                                    if (set) {
                                        ansiSavedRow = ansiCursorRow;
                                        ansiSavedCol = ansiCursorCol;
                                    } else {
                                        ansiCursorRow = ansiSavedRow;
                                        ansiCursorCol = ansiSavedCol;
                                    }
                                    break;
                                default: break;
                            }
                        }
                    } else if (cmd == 'm') {
                        for (size_t k = 0; k < nums.size(); ++k) {
                            int code = nums[k];
                            if (code == 0) {
                                ansiCurrentColor = text_color;
                                ansiCurrentBg = {0, 0, 0, 0};
                                ansiBold = false;
                                ansiUnderline = false;
                                continue;
                            }
                            if (code == 1) { ansiBold = true; continue; }
                            if (code == 22) { ansiBold = false; continue; }
                            if (code == 4) { ansiUnderline = true; continue; }
                            if (code == 24) { ansiUnderline = false; continue; }
                            if (code == 49) { ansiCurrentBg = {0, 0, 0, 0}; continue; }
                            if ((code >= 40 && code <= 47) || (code >= 100 && code <= 107)) {
                                ansiCurrentBg = ansiBasicBgColor(code);
                                continue;
                            }
                            if (code == 38 && k + 1 < nums.size()) {
                                if (nums[k + 1] == 5 && k + 2 < nums.size()) {
                                    int idx = nums[k + 2];
                                    if (idx >= 16 && idx <= 231) {
                                        idx -= 16;
                                        int r = (idx / 36) % 6;
                                        int g = (idx / 6) % 6;
                                        int b = idx % 6;
                                        ansiCurrentColor = SDL_Color{
                                            static_cast<Uint8>(r == 0 ? 0 : r * 40 + 55),
                                            static_cast<Uint8>(g == 0 ? 0 : g * 40 + 55),
                                            static_cast<Uint8>(b == 0 ? 0 : b * 40 + 55),
                                            255};
                                    } else if (idx >= 232 && idx <= 255) {
                                        Uint8 gray = static_cast<Uint8>((idx - 232) * 10 + 8);
                                        ansiCurrentColor = SDL_Color{gray, gray, gray, 255};
                                    }
                                    k += 2;
                                    continue;
                                }
                                if (nums[k + 1] == 2 && k + 4 < nums.size()) {
                                    ansiCurrentColor = SDL_Color{
                                        static_cast<Uint8>(std::max(0, std::min(255, nums[k + 2]))),
                                        static_cast<Uint8>(std::max(0, std::min(255, nums[k + 3]))),
                                        static_cast<Uint8>(std::max(0, std::min(255, nums[k + 4]))),
                                        255};
                                    k += 4;
                                    continue;
                                }
                            }
                            if (code == 48 && k + 1 < nums.size()) {
                                if (nums[k + 1] == 5 && k + 2 < nums.size()) {
                                    int idx = nums[k + 2];
                                    if (idx >= 16 && idx <= 231) {
                                        idx -= 16;
                                        int r = (idx / 36) % 6;
                                        int g = (idx / 6) % 6;
                                        int b = idx % 6;
                                        ansiCurrentBg = SDL_Color{
                                            static_cast<Uint8>(r == 0 ? 0 : r * 40 + 55),
                                            static_cast<Uint8>(g == 0 ? 0 : g * 40 + 55),
                                            static_cast<Uint8>(b == 0 ? 0 : b * 40 + 55),
                                            255};
                                    } else if (idx >= 232 && idx <= 255) {
                                        Uint8 gray = static_cast<Uint8>((idx - 232) * 10 + 8);
                                        ansiCurrentBg = SDL_Color{gray, gray, gray, 255};
                                    }
                                    k += 2;
                                    continue;
                                }
                                if (nums[k + 1] == 2 && k + 4 < nums.size()) {
                                    ansiCurrentBg = SDL_Color{
                                        static_cast<Uint8>(std::max(0, std::min(255, nums[k + 2]))),
                                        static_cast<Uint8>(std::max(0, std::min(255, nums[k + 3]))),
                                        static_cast<Uint8>(std::max(0, std::min(255, nums[k + 4]))),
                                        255};
                                    k += 4;
                                    continue;
                                }
                            }
                            ansiCurrentColor = ansiBasicColor(code, ansiCurrentColor);
                        }
                    } else if (cmd == 'A') {
                        ansiCursorRow = std::max(0, ansiCursorRow - n0());
                    } else if (cmd == 'B') {
                        ansiCursorRow += n0();
                        ensureRow(ansiCursorRow);
                    } else if (cmd == 'C') {
                        ansiCursorCol += n0();
                    } else if (cmd == 'D') {
                        ansiCursorCol = std::max(0, ansiCursorCol - n0());
                    } else if (cmd == 'G') {
                        ansiCursorCol = std::max(0, n0() - 1);
                    } else if (cmd == 'H' || cmd == 'f') {
                        int row = nums.size() > 0 ? std::max(1, nums[0]) : 1;
                        int col = nums.size() > 1 ? std::max(1, nums[1]) : 1;
                        ansiCursorRow = row - 1;
                        ansiCursorCol = col - 1;
                        ensureRow(ansiCursorRow);
                    } else if (cmd == 'K') {
                        int mode = nums.empty() ? 0 : nums[0];
                        ensureRow(ansiCursorRow);
                        ensureCol(ansiCursorRow, ansiCursorCol);
                        CharStyle padStyle{text_color, {0,0,0,0}, false, false};
                        if (mode == 0) {
                            clearToLineEnd();
                        } else if (mode == 1) {
                            auto &line = ansiLines[ansiCursorRow];
                            auto &colors = ansiLineColors[ansiCursorRow];
                            int end = std::min(ansiCursorCol, static_cast<int>(line.size()));
                            for (int c = 0; c < end; ++c) {
                                line[c] = ' ';
                                colors[c] = padStyle;
                            }
                        } else if (mode == 2) {
                            ansiLines[ansiCursorRow].clear();
                            ansiLineColors[ansiCursorRow].clear();
                            ansiCursorCol = 0;
                        }
                    } else if (cmd == 'J') {
                        int mode = nums.empty() ? 0 : nums[0];
                        ensureRow(ansiCursorRow);
                        if (mode == 2) {
                            ansiLines.assign(1, std::string{});
                            ansiLineColors.assign(1, std::vector<CharStyle>{});
                            ansiCursorRow = 0;
                            ansiCursorCol = 0;
                        } else if (mode == 0) {
                            clearToLineEnd();
                            for (size_t r = static_cast<size_t>(ansiCursorRow + 1); r < ansiLines.size(); ++r) {
                                ansiLines[r].clear();
                                ansiLineColors[r].clear();
                            }
                        } else if (mode == 1) {
                            for (int r = 0; r < ansiCursorRow; ++r) {
                                ansiLines[r].clear();
                                ansiLineColors[r].clear();
                            }
                            auto &line = ansiLines[ansiCursorRow];
                            auto &colors = ansiLineColors[ansiCursorRow];
                            CharStyle padStyle{text_color, {0,0,0,0}, false, false};
                            int end = std::min(ansiCursorCol, static_cast<int>(line.size()));
                            for (int c = 0; c < end; ++c) {
                                line[c] = ' ';
                                colors[c] = padStyle;
                            }
                        }
                    } else if (cmd == 's') {
                        ansiSavedRow = ansiCursorRow;
                        ansiSavedCol = ansiCursorCol;
                    } else if (cmd == 'u') {
                        ansiCursorRow = std::max(0, ansiSavedRow);
                        ansiCursorCol = std::max(0, ansiSavedCol);
                        ensureRow(ansiCursorRow);
                    } else if (cmd == 'r') {
                        // DECSTBM: set scrolling region (1-based, inclusive).
                        int top = (nums.size() > 0 && nums[0] > 0) ? nums[0] - 1 : 0;
                        int bot = (nums.size() > 1 && nums[1] > 0) ? nums[1] - 1 : -1;
                        scrollTop = std::max(0, top);
                        scrollBot = bot;
                        ansiCursorRow = scrollTop;
                        ansiCursorCol = 0;
                    } else if (cmd == 'L') {
                        // IL: insert N lines at cursor (within scroll region).
                        int n = n0();
                        int savedTop = scrollTop;
                        scrollTop = ansiCursorRow;
                        scrollRegionDown(n);
                        scrollTop = savedTop;
                    } else if (cmd == 'M') {
                        // DL: delete N lines at cursor.
                        int n = n0();
                        int savedTop = scrollTop;
                        scrollTop = ansiCursorRow;
                        scrollRegionUp(n);
                        scrollTop = savedTop;
                    } else if (cmd == 'P') {
                        // DCH: delete N characters at cursor.
                        int n = n0();
                        ensureRow(ansiCursorRow);
                        auto &line   = ansiLines[ansiCursorRow];
                        auto &colors = ansiLineColors[ansiCursorRow];
                        if (ansiCursorCol < static_cast<int>(line.size())) {
                            int end = std::min(ansiCursorCol + n, (int)line.size());
                            line.erase(line.begin() + ansiCursorCol, line.begin() + end);
                            colors.erase(colors.begin() + ansiCursorCol, colors.begin() + end);
                        }
                    } else if (cmd == '@') {
                        // ICH: insert N blank characters at cursor.
                        int n = n0();
                        ensureRow(ansiCursorRow);
                        ensureCol(ansiCursorRow, ansiCursorCol);
                        auto &line   = ansiLines[ansiCursorRow];
                        auto &colors = ansiLineColors[ansiCursorRow];
                        CharStyle cs{ansiCurrentColor, ansiCurrentBg, ansiBold, ansiUnderline};
                        line.insert(line.begin() + ansiCursorCol, n, ' ');
                        colors.insert(colors.begin() + ansiCursorCol, n, cs);
                    } else if (cmd == 'X') {
                        // ECH: erase N characters in place using current bg.
                        int n = n0();
                        ensureRow(ansiCursorRow);
                        ensureCol(ansiCursorRow, ansiCursorCol + n);
                        auto &line   = ansiLines[ansiCursorRow];
                        auto &colors = ansiLineColors[ansiCursorRow];
                        CharStyle cs{ansiCurrentColor, ansiCurrentBg, ansiBold, ansiUnderline};
                        for (int k = 0; k < n &&
                             (ansiCursorCol + k) < (int)line.size(); ++k) {
                            line[ansiCursorCol + k]   = ' ';
                            colors[ansiCursorCol + k] = cs;
                        }
                    } else if (cmd == 'S') {
                        scrollRegionUp(n0());
                    } else if (cmd == 'T') {
                        scrollRegionDown(n0());
                    } else if (cmd == 'd') {
                        // VPA: vertical position absolute (1-based).
                        int row = (nums.size() > 0 && nums[0] > 0) ? nums[0] - 1 : 0;
                        ansiCursorRow = std::max(0, row);
                        ensureRow(ansiCursorRow);
                    }

                    i = j + 1;
                    continue;
                }
            }

            if (ch == '\r') {
                ansiCursorCol = 0;
            } else if (ch == '\n') {
                doNewline();
                if (!altScreen)
                    ansiCursorCol = 0;
            } else if (ch == '\b') {
                ansiCursorCol = std::max(0, ansiCursorCol - 1);
            } else if (ch == '\t') {
                int spaces = 4 - (ansiCursorCol % 4);
                for (int s = 0; s < spaces; ++s)
                    putChar(' ');
            } else if (ch >= 32) {
                putChar(static_cast<char>(ch));
            }
            ++i;
        }

        syncAnsiToOutput();
    }

    std::string Terminal::parseTerminalData(const std::string &input) {
        std::regex promptRegex(R"(BEGIN_PROMPT(.*?)END_PROMPT)");
        std::smatch promptMatch;
        std::string promptText;
        if (std::regex_search(input, promptMatch, promptRegex)) {
            promptText = promptMatch[1].str();
        }

        std::string stream = std::regex_replace(input, promptRegex, "");
        applyAnsiData(stream);
        return promptText;
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

#ifdef FOR_WASM
        g_activeTerminal = this;
#endif

        if (!embedded_)
            Window::draw(app);

        if (isDraw() == false)
            return;

        // If the editor is active, render it inside the window content
        // area (below the title bar) and skip the regular terminal UI.
        if (editor && editor->isActive()) {
            SDL_Rect rc;
            Window::getRect(rc);
            rc.y += 28;
            rc.h -= 28;
            if (rc.h < 0) rc.h = 0;
            if (!embedded_)
                Window::drawMenubar(app);
            editor->draw(app, rc);
            return;
        }

 
    #ifndef FOR_WASM
        if (newData == true) {
            std::lock_guard<std::mutex> lock(outputMutex);
            std::string temp = new_data;
            parseTerminalData(temp);
#if defined(__linux__) || defined(__APPLE__)
            if (temp.find("password for") != std::string::npos ||
                temp.find("Password:") != std::string::npos) {
                echo_enabled = false;
            }
#endif
            newData = false;
            new_data = "";
            
        }
    #endif
 
    #if defined(__linux__) || defined(__APPLE__)
        if (!altScreen && !outputLines.empty()) {
            prompt = outputLines.back();
        }
    #elif defined(_WIN32)
        prompt = "$ ";
    #endif
        SDL_Rect rc;
        Window::getRect(rc);
        // Make sure the PTY always reflects the current window size; the
        // initial constructor call ran before the window was sized.
        updatePtySize();
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app.ren, 0, 0, 0, 200);
        if (activeEffect_ != TermEffect::None && !dim->getMatrix()) {
            drawEffectBackground(app);
        } else if (!embedded_) {
            SDL_RenderCopy(app.ren, dim->getMatrix() ? dim->matrix_tex : wallpaper, nullptr, nullptr);
        }
        // Translucent dark overlay for the terminal content area. When
        // embedded, the parent draws title bar / tab strip on top of us
        // afterwards, so restrict the overlay to our content rect to
        // keep the shader/wallpaper showing through above the chrome.
        SDL_Rect fillRect = rc;
        if (embedded_) {
            fillRect.y += 28;
            fillRect.h -= 28;
            if (fillRect.h < 0) fillRect.h = 0;
        }
        SDL_RenderFillRect(app.ren, &fillRect);
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_NONE);
        rc.y += 28;
        rc.h -= 28;
        if (rc.h < 0) rc.h = 0;
        if (!embedded_)
            Window::drawMenubar(app);
        // Clip terminal contents to the window's content rect so text
        // never spills outside when the user shrinks the window.
        SDL_RenderSetClipRect(app.ren, &rc);
 

        int lineHeight = TTF_FontHeight(font);
        int maxWidth = rc.w - 10;
        int y = rc.y + 5;

        int promptWidth = 0;
        TTF_SizeText(font, prompt.c_str(), &promptWidth, nullptr);
        int requiredInputLines = calculateWrappedLinesForText(inputText, maxWidth - promptWidth, promptWidth);
        if (requiredInputLines < 1) {
            requiredInputLines = 1;
        }

        // In alt-screen (vim/nano/...) we draw the program-owned grid only,
        // with no input echo line, no scrollback offset, and a cursor that
        // follows the program's reported (row,col) position.
        if (altScreen) {
            int startRow = 0;
            int rows = static_cast<int>(outputLines.size());
            for (int i = startRow; i < rows; ++i) {
                if (y + lineHeight > rc.y + rc.h) break;
                renderOutputLine(app, i, rc.x + 5, y);
                y += lineHeight;
            }
            // Cursor: place at ansiCursorRow/Col using a fixed cell width.
            int cellW = 0;
            TTF_SizeText(font, "M", &cellW, nullptr);
            if (cellW <= 0) cellW = 8;
            int cur_y = rc.y + 5 + ansiCursorRow * lineHeight;
            int cur_x = rc.x + 5 + ansiCursorCol * cellW;
            Uint32 t = SDL_GetTicks();
            if (t - cursorTimer >= cursorBlinkInterval) {
                showCursor = !showCursor;
                cursorTimer = t;
            }
            drawCursor(app, cur_x, cur_y, showCursor);
            int totalLinesAlt = static_cast<int>(outputLines.size());
            if (totalLinesAlt <= maxVisibleLines) {
                scrollBarHeight = 0;
            }
            SDL_RenderSetClipRect(app.ren, nullptr);
            return;
        }

        for (int i = scrollOffset; i < static_cast<int>(outputLines.size()); ++i) {
            if (y + lineHeight * (requiredInputLines + 1) > rc.y + rc.h) {
                break;
            }
            renderOutputLine(app, i, rc.x + 5, y);
            y += lineHeight;
        }

        // Selection overlay (drawn after text so highlighting blends on top).
        {
            int cellW = 0;
            TTF_SizeText(font, "M", &cellW, nullptr);
            if (cellW <= 0) cellW = 8;
            drawSelection(app, rc, lineHeight, cellW);
        }

        int cx = rc.x + 5;
        int cy = y;

#if defined(__linux__) || defined(__APPLE__)
        // PTY-driven session: bash already echoes typed characters and
        // draws its own prompt as part of its output stream, so all we
        // need to do here is place a blinking cursor at bash's reported
        // cell position. We must NOT redraw `prompt + inputText` -- that
        // produced the doubled prompt the user used to see.
        if (!waitingForInput) {
            int cellW = 0;
            TTF_SizeText(font, "M", &cellW, nullptr);
            if (cellW <= 0) cellW = 8;
            // ansiCursorRow is an absolute row in outputLines; translate
            // to a screen Y by subtracting the current scrollOffset.
            int curRow = ansiCursorRow - scrollOffset;
            int curCol = ansiCursorCol;
            int cur_y = rc.y + 5 + curRow * lineHeight;
            int cur_x = rc.x + 5 + curCol * cellW;
            Uint32 t = SDL_GetTicks();
            if (t - cursorTimer >= cursorBlinkInterval) {
                showCursor = !showCursor;
                cursorTimer = t;
            }
            // Only draw the cursor if its row is within the actually
            // rendered output range (cy is the y just past the last line
            // we drew). Otherwise scrolling away from the prompt would
            // draw a stray cursor in the reserved input area.
            if (cur_y >= rc.y && cur_y + lineHeight <= cy)
                drawCursor(app, cur_x, cur_y, showCursor);
        }
#endif

#ifdef FOR_WASM
        if (waitingForInput && !orig_text.empty() && orig_text.back() != '\n') {
            int lastLineY = y - lineHeight;
            int lastLineWidth = 0;
            if (!outputLines.empty()) {
                TTF_SizeText(font, outputLines.back().c_str(), &lastLineWidth, nullptr);
            }
            int inputStartX = rc.x + 5 + lastLineWidth;
            
            if (!inputText.empty()) {
                renderText(app, inputText, inputStartX, lastLineY);
            }
            
            int inputWidth = 0;
            if (!inputText.empty()) {
                std::string textBeforeCursor = inputText.substr(0, cursorPosition);
                TTF_SizeText(font, textBeforeCursor.c_str(), &inputWidth, nullptr);
            }
            
            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - cursorTimer >= cursorBlinkInterval) {
                showCursor = !showCursor;
                cursorTimer = currentTime;
            }
            drawCursor(app, inputStartX + inputWidth, lastLineY, showCursor);
        } else {
            std::string displayPrompt = waitingForInput ? "" : prompt;
            renderTextWrapped(app, displayPrompt, inputText, cx, cy, maxWidth);
        }
#elif defined(_WIN32)
        // Windows path still uses cooked-mode line editing locally.
        std::string displayPrompt = waitingForInput ? "" : prompt;
        renderTextWrapped(app, displayPrompt, inputText, cx, cy, maxWidth);
#else
        // Linux / macOS: PTY raw mode -- nothing to draw here; bash owns
        // the input line. The block above placed the cursor.
        (void)cx; (void)cy;
#endif


        int totalLines = static_cast<int>(outputLines.size());

        int totalWrappedLines = calculateWrappedLinesForText(inputText, maxWidth - promptWidth, promptWidth);
        totalLines += totalWrappedLines;

        // Recompute the visible-line capacity from the current content rect
        // so a window resize (especially a horizontal-only one that doesn't
        // call scroll()) immediately corrects the scrollbar geometry.
        if (lineHeight > 0) {
            maxVisibleLines = rc.h / lineHeight;
            if (maxVisibleLines < 1) maxVisibleLines = 1;
        }
        // scrollOffset indexes outputLines directly. The drawing loop above
        // reserves the bottom (requiredInputLines + 1) rows for the input
        // line, so the largest scrollOffset that still fills the output
        // area is outputLines.size() - (maxVisibleLines - requiredInputLines).
        // Clamp against that so resizing larger doesn't leave blank space
        // below the last line (which also pushed the cursor and scrollbar
        // mid-window).
        int outputCount = static_cast<int>(outputLines.size());
        int visibleOutputRows = maxVisibleLines - (requiredInputLines + 1);
        if (visibleOutputRows < 1) visibleOutputRows = 1;
        int maxScroll = outputCount - visibleOutputRows;
        if (maxScroll < 0) maxScroll = 0;
        if (scrollOffset > maxScroll) scrollOffset = maxScroll;
        if (scrollOffset < 0) scrollOffset = 0;

        if (totalLines > maxVisibleLines) {
            int offx = rc.x + rc.w;
            int offy = rc.y;
            // rc has already been adjusted (rc.y += 28; rc.h -= 28) earlier
            // so it is the content rect; the available height is just rc.h.
            int availableHeight = rc.h;

            scrollBarHeight = (maxVisibleLines * availableHeight) / totalLines;

            if (scrollBarHeight < 10) {
                scrollBarHeight = 10;
            }

            // Use the same maxScroll that bounds scrollOffset so the bar
            // reaches the bottom of the track exactly when content does.
            int denom = maxScroll > 0 ? maxScroll : 1;
            scrollBarPosY = offy + (scrollOffset * (availableHeight - scrollBarHeight)) / denom;

            if (scrollBarPosY + scrollBarHeight > rc.y + rc.h) {
                scrollBarPosY = rc.y + rc.h - scrollBarHeight;
            }

            SDL_Rect scrollBarRect = {offx - scrollBarWidth, scrollBarPosY, scrollBarWidth, scrollBarHeight};
            SDL_SetRenderDrawColor(app.ren, 100, 100, 100, 255);
            SDL_RenderFillRect(app.ren, &scrollBarRect);
        }
        SDL_RenderSetClipRect(app.ren, nullptr);
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

    void Terminal::renderOutputLine(mxApp &app, int lineIndex, int x, int y) {
        if (lineIndex < 0 || lineIndex >= static_cast<int>(outputLines.size()))
            return;

        const std::string &line = outputLines[lineIndex];
        if (line.empty())
            return;

        if (lineIndex >= static_cast<int>(outputLineColors.size()) ||
            outputLineColors[lineIndex].size() != line.size()) {
            renderText(app, line, x, y);
            return;
        }

        const auto &styles = outputLineColors[lineIndex];
        const int lineH = TTF_FontHeight(font);
        // Use a fixed cell width so column alignment is preserved even
        // when style runs (e.g. bold/colored directory names from `ls`)
        // mix with regular text on the same line.
        int cellW = 0;
        TTF_SizeText(font, "M", &cellW, nullptr);
        if (cellW <= 0) cellW = 8;
        int drawX = x;
        size_t i = 0;
        while (i < line.size()) {
            const CharStyle &run = styles[i];
            size_t j = i + 1;
            while (j < line.size() &&
                   styles[j].fg.r == run.fg.r && styles[j].fg.g == run.fg.g &&
                   styles[j].fg.b == run.fg.b && styles[j].fg.a == run.fg.a &&
                   styles[j].bg.r == run.bg.r && styles[j].bg.g == run.bg.g &&
                   styles[j].bg.b == run.bg.b && styles[j].bg.a == run.bg.a &&
                   styles[j].bold == run.bold && styles[j].underline == run.underline) {
                ++j;
            }

            std::string text = line.substr(i, j - i);
            if (!text.empty()) {
                int ttfStyle = TTF_STYLE_NORMAL;
                if (run.bold)      ttfStyle |= TTF_STYLE_BOLD;
                if (run.underline) ttfStyle |= TTF_STYLE_UNDERLINE;
                TTF_SetFontStyle(font, ttfStyle);

                int runW = static_cast<int>(text.size()) * cellW;

                if (run.bg.a != 0) {
                    SDL_Rect bgRect = {drawX, y, runW, lineH};
                    SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
                    SDL_SetRenderDrawColor(app.ren, run.bg.r, run.bg.g, run.bg.b, run.bg.a);
                    SDL_RenderFillRect(app.ren, &bgRect);
                }

                SDL_Surface *surface = TTF_RenderText_Blended(font, text.c_str(), run.fg);
                if (surface) {
                    SDL_Texture *texture = SDL_CreateTextureFromSurface(app.ren, surface);
                    if (texture) {
                        SDL_Rect dstRect = {drawX, y, surface->w, surface->h};
                        SDL_RenderCopy(app.ren, texture, nullptr, &dstRect);
                        SDL_DestroyTexture(texture);
                    }
                    SDL_FreeSurface(surface);
                }
                drawX += runW;
                TTF_SetFontStyle(font, TTF_STYLE_NORMAL);
            }
            i = j;
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

#ifdef FOR_WASM
        if (inputText.find('\n') != std::string::npos) {
            int cursorX = x;
            int cursorY = y;
            bool cursorDrawn = false;

            auto renderOneLogicalLine = [&](const std::string& linePrompt, const std::string& lineText, int lineStartIndex) {
                int prompt_w = 0;
                TTF_SizeText(font, linePrompt.c_str(), &prompt_w, nullptr);
                renderText(app, linePrompt, x, y);

                int localCursorPos = -1;
                if (cursorPosition >= lineStartIndex && cursorPosition <= lineStartIndex + static_cast<int>(lineText.size())) {
                    localCursorPos = cursorPosition - lineStartIndex;
                }

                int localCount = 0;
                bool firstVisualLine = true;
                int textX = x + prompt_w;
                int textY = y;
                std::string remainingText = lineText;

                while (!remainingText.empty()) {
                    std::string lineToRender;
                    int currentWidth = 0;
                    size_t i = 0;

                    int lineWidth = firstVisualLine ? (availableWidth - prompt_w - 10) : (availableWidth - 10);
                    int drawX = firstVisualLine ? textX : (rc.x + margin);

                    while (i < remainingText.length()) {
                        std::string testLine = lineToRender + remainingText[i];
                        TTF_SizeText(font, testLine.c_str(), &currentWidth, nullptr);

                        if (currentWidth > lineWidth) {
                            if (!lineToRender.empty()) {
                                break;
                            }
                            lineToRender += remainingText[i++];
                            break;
                        }

                        lineToRender += remainingText[i++];
                        localCount++;
                        if (!cursorDrawn && localCursorPos >= 0 && localCount == localCursorPos) {
                            cursorX = drawX + currentWidth;
                            cursorY = textY;
                            cursorDrawn = true;
                        }
                    }

                    renderText(app, lineToRender, drawX, textY);

                    textY += lineHeight;
                    y = textY;
                    remainingText = remainingText.substr(i);
                    firstVisualLine = false;
                }

                if (lineText.empty()) {
                    y += lineHeight;
                }

                x = rc.x + margin;
            };

            int lineStartIndex = 0;
            size_t pos = 0;
            size_t next = 0;
            bool firstLine = true;
            while (true) {
                next = inputText.find('\n', pos);
                std::string line = (next == std::string::npos) ? inputText.substr(pos) : inputText.substr(pos, next - pos);
                renderOneLogicalLine(firstLine ? prompt : continuationPrompt, line, lineStartIndex);
                lineStartIndex += static_cast<int>(line.size()) + 1;
                if (next == std::string::npos) break;
                pos = next + 1;
                firstLine = false;
            }

            if (!cursorDrawn) {
                cursorX = x;
                cursorY = y;
            }
            if (cursorPosition == 0) {
                cursorX = rc.x + margin;
            }

            Uint32 currentTime = SDL_GetTicks();
            if (currentTime - cursorTimer >= cursorBlinkInterval) {
                showCursor = !showCursor;
                cursorTimer = currentTime;
            }
            drawCursor(app, cursorX, cursorY, showCursor);
            return;
        }
#endif

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

        // If the built-in text editor is active, route events to it.
        if (editor && editor->isActive()) {
            // Allow the window chrome (drag, close, resize) to still work.
            if (!embedded_ &&
                (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP ||
                 e.type == SDL_MOUSEMOTION)) {
                if (Window::event(app, e))
                    return true;
            }
            editor->event(app, e);
            if (!editor->isActive()) {
                print("\n[ editor closed ]\n");
                editor.reset();
                print(prompt);
                scroll();
            }
            return true;
        }

        // Terminal-style copy / paste shortcuts. Use Ctrl+Shift+C/V so a
        // bare Ctrl+C still reaches the foreground process via the PTY.
        if (e.type == SDL_KEYDOWN) {
            const Uint16 mod = e.key.keysym.mod;
            const SDL_Keycode sym = e.key.keysym.sym;
            const bool ctrlShift = (mod & KMOD_CTRL) && (mod & KMOD_SHIFT);
            if (ctrlShift && sym == SDLK_c) {
                if (hasSelection) copySelectionToClipboard();
                return true;
            }
            if (ctrlShift && sym == SDLK_v) {
                pasteFromClipboard();
                return true;
            }
        }

        // Pipe every keystroke straight to the PTY so bash's own readline
        // performs line editing, history, tab completion, and signal
        // handling -- exactly like a real terminal emulator. We only fall
        // back to local cooked-mode line editing for built-in input prompts
        // (input()) and on platforms without a PTY (Windows / WASM).
#if defined(__linux__) || defined(__APPLE__)
        if (master_fd > 0 && !waitingForInput &&
            (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT)) {
            if (handleRawKeyEvent(app, e))
                return true;
        }
#else
        if (altScreen &&
            (e.type == SDL_KEYDOWN || e.type == SDL_TEXTINPUT)) {
            if (handleRawKeyEvent(app, e))
                return true;
        }
#endif

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
                    #ifdef FOR_WASM
                        cancelWasmLoop();
                        if (isMultiLineInput) {
                            print("^C\n");
                            multiLineBuffer.clear();
                            isMultiLineInput = false;
                            blockDepth = 0;
                            inputText.clear();
                            cursorPosition = 0;
                            print(prompt);
                        } else {
                            print("^C\n");
                            inputText.clear();
                            cursorPosition = 0;
                            print(prompt);
                        }
                    #elif !defined(_WIN32)
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
                case SDLK_v:
                if (e.key.keysym.mod & KMOD_CTRL) {
                    pasteFromClipboard();
                    return true;
                }
                break;
                case SDLK_INSERT:
                if (e.key.keysym.mod & KMOD_CTRL) {
                    pasteFromClipboard();
                    return true;
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

                case SDLK_HOME:
                    cursorPosition = 0;
                    break;

                case SDLK_END:
                    cursorPosition = static_cast<int>(inputText.length());
                    break;

               case SDLK_UP:
#ifdef FOR_WASM
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
#else
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
#endif
                    break;

            case SDLK_DOWN:
#ifdef FOR_WASM
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
#else
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
#endif
                    break;
            case SDLK_RETURN:
                if (waitingForInput) {
                    print(inputText + "\n");
                    inputResult = inputText;
                    inputText.clear();
                    cursorPosition = 0;
                    waitingForInput = false;
#ifndef FOR_WASM
                    inputCondition.notify_one();
#endif
                    scroll();
                    break;
                }
#ifdef FOR_WASM
                if (!inputText.empty() || isMultiLineInput) {
                    auto trimSpaces = [](const std::string& s) {
                        const size_t start = s.find_first_not_of(" \t\r\n");
                        if (start == std::string::npos) return std::string{};
                        const size_t end = s.find_last_not_of(" \t\r\n");
                        return s.substr(start, end - start + 1);
                    };
                    auto isBlockTerminator = [&](const std::string& s) {
                        const std::string t = trimSpaces(s);
                        return (t == "done" || t == "fi" || t == "end");
                    };

                    auto ensureNewlineBeforeEcho = [&]() {
                        if (!orig_text.empty() && orig_text.back() != '\n') {
                            print("\n");
                            forceFrameRender();
                        }
                    };

                    bool lineContinuation = false;
                    if (!inputText.empty()) {
                        size_t lastNonSpace = inputText.find_last_not_of(" \t");
                        if (lastNonSpace != std::string::npos && inputText[lastNonSpace] == '\\') {
                            lineContinuation = true;
                            inputText = inputText.substr(0, lastNonSpace);
                        }
                    }
                    
                    if (isMultiLineInput) {
                        multiLineBuffer += "\n" + inputText;
                    } else {
                        multiLineBuffer = inputText;
                    }
                    MultiLineState state = checkMultiLineState(multiLineBuffer);
                    
                    if (lineContinuation || state.needsMoreInput) {
                        isMultiLineInput = true;
                        blockDepth = state.blockDepth;
                        ensureNewlineBeforeEcho();
                        print(prompt + inputText + "\n");
                        stored_commands.push_back(inputText);
                        store_offset = stored_commands.size();
                        prompt = continuationPrompt;
                        inputText.clear();
                        cursorPosition = 0;
                        forceFrameRender();
                    } else {
                        ensureNewlineBeforeEcho();
                        print(prompt + inputText + "\n");
                        stored_commands.push_back(inputText);
                        inputText.clear();
                        cursorPosition = 0;
                        forceFrameRender();
                        processCommand(app, multiLineBuffer);
                        store_offset = stored_commands.size();
                        isMultiLineInput = false;
                        multiLineBuffer.clear();
                        blockDepth = 0;
                        prompt = "$ ";
                    }
                    scroll();
                } else {
                    print(prompt + "\n");
                    scroll();
                }
#else
                if (!inputText.empty()) {
                    processCommand(app, inputText);            
                    store_offset = stored_commands.size();  
                    inputText.clear();
                    cursorPosition = 0;
                    scroll();
                }
#endif
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
            } else if (e.button.button == SDL_BUTTON_LEFT) {
                int row, col;
                if (pointToCell(mouseX, mouseY, row, col)) {
                    selecting = true;
                    hasSelection = true;
                    selAnchorRow = selFocusRow = row;
                    selAnchorCol = selFocusCol = col;
                    render_text = false;
                    return true;
                } else {
                    if (hasSelection) { clearSelection(); render_text = false; }
                }
            } else if (e.button.button == SDL_BUTTON_MIDDLE) {
                // X11-style middle-click paste from clipboard.
                pasteFromClipboard();
                return true;
            }
        }

        if (e.type == SDL_MOUSEBUTTONUP) {
            isScrolling = false;
            if (selecting && e.button.button == SDL_BUTTON_LEFT) {
                selecting = false;
                if (selAnchorRow == selFocusRow && selAnchorCol == selFocusCol) {
                    hasSelection = false;
                }
                render_text = false;
            }
        }

        if (e.type == SDL_MOUSEMOTION && selecting) {
            int row, col;
            if (pointToCell(e.motion.x, e.motion.y, row, col)) {
                selFocusRow = row;
                selFocusCol = col;
                hasSelection = true;
                render_text = false;
            }
        }

        if (e.type == SDL_MOUSEMOTION && isScrolling) {
            int mouseY = e.motion.y;
            int newScrollPosY = mouseY - scrollBarDragOffset;
            int availableHeight = rc.h - 28;
            int scrollableRange = availableHeight - scrollBarHeight;
            // Use the same maxScroll the draw and wheel paths use so the
            // bar's drag range matches what the wheel/draw can reach.
            int outputCount = static_cast<int>(outputLines.size());
            int promptW = 0;
            if (font) TTF_SizeText(font, prompt.c_str(), &promptW, nullptr);
            int maxWidth = rc.w - 10;
            int reqInput = calculateWrappedLinesForText(inputText, maxWidth - promptW, promptW);
            if (reqInput < 1) reqInput = 1;
            int visibleOutputRows = maxVisibleLines - (reqInput + 1);
            if (visibleOutputRows < 1) visibleOutputRows = 1;
            int maxScroll = outputCount - visibleOutputRows;
            if (maxScroll < 0) maxScroll = 0;
            if (scrollableRange > 0 && maxScroll > 0) {
                scrollOffset = ((newScrollPosY - rc.y) * maxScroll) / scrollableRange;
                scrollOffset = my_max(0, my_min(scrollOffset, maxScroll));
            }
            render_text = false;
        }

        if (e.type == SDL_MOUSEWHEEL) {
            handleScrolling(e.wheel.y);
            render_text = false;
            return true;
        }
        
        if (!embedded_ && Window::event(app, e))
            return true;

        return false; 
    }

    void Terminal::handleScrolling(int direction) {
        scrollOffset -= direction;
        // Match the draw-time clamp: the visible output area is
        // maxVisibleLines minus the rows reserved for the (possibly
        // wrapped) input echo, so the user can scroll until the last
        // output line sits at the bottom of that area.
        int outputCount = static_cast<int>(outputLines.size());
        SDL_Rect rc;
        Window::getRect(rc);
        int promptW = 0;
        if (font) TTF_SizeText(font, prompt.c_str(), &promptW, nullptr);
        int maxWidth = rc.w - 10;
        int reqInput = calculateWrappedLinesForText(inputText, maxWidth - promptW, promptW);
        if (reqInput < 1) reqInput = 1;
        int visibleOutputRows = maxVisibleLines - (reqInput + 1);
        if (visibleOutputRows < 1) visibleOutputRows = 1;
        int maxScroll = outputCount - visibleOutputRows;
        if (maxScroll < 0) maxScroll = 0;
        scrollOffset = my_max(0, my_min(scrollOffset, maxScroll));
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
#elif defined(_WIN32)
        stored_commands.push_back(command);
        store_offset = stored_commands.size()-1;
#endif

#if defined(_WIN32)
        print("\n$ " + command + "\n");
#endif
        std::vector<std::string> words;
        words = splitText(command);

        if(words.size()==0)
            return;
        
        if (words[0] == "edit") {
            std::string fname;
            if (words.size() >= 2)
                fname = words[1];
            print(command + "\n");
            launchEditor(fname);
            return;
        }
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
            print(command + "\nMasterX System written by Jared Bruni\n(C) 2026 LostSideDead Software.\nhttps://lostsidedead.biz\n");
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
    if(command.length() > 0 && clear == false) {
        resetWasmLoop();  
        Terminal* term = this;
        setCmdUpdateCallback([term](const std::string& chunk) {
            if (!chunk.empty()) {
                term->print(chunk);
                forceFrameRender();
            }
        });

        setCmdInputCallback([term]() -> std::string {
            return term->getInput();
        });
        
        setCmdFlushCallback([term](const std::string& pending) {
            if (!pending.empty()) {
                term->print(pending);
                term->scroll();
                forceFrameRender();
            }
        });

        executeCmd(command);

        setCmdUpdateCallback(nullptr);
        setCmdInputCallback(nullptr);
        setCmdFlushCallback(nullptr);
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
        outputLineColors.clear();
        std::string total = orig_text;
        std::istringstream stream(total);
        while(std::getline(stream, line)) {
            if (line.length() > 0) {
                std::string currentLine;
                CharStyle defStyle{text_color, {0, 0, 0, 0}, false, false};
                for (size_t i = 0; i < line.length(); ++i) {
                    currentLine += line[i];
                    TTF_SizeText(font, currentLine.c_str(), &w, &h);
                    if (w > maxWidth) {
                        size_t lastSpace = currentLine.find_last_of(' ');
                        if (lastSpace != std::string::npos) {
                            std::string part = currentLine.substr(0, lastSpace);
                            if(!part.empty()) {
                                std::string cleanPart = trimR(part);
                                outputLines.push_back(cleanPart);
                                outputLineColors.emplace_back(cleanPart.size(), defStyle);
                            }
                            currentLine = currentLine.substr(lastSpace + 1);
                        } else {
                            if(!currentLine.empty()) {
                                std::string cleanPart = trimR(currentLine);
                                outputLines.push_back(cleanPart);
                                outputLineColors.emplace_back(cleanPart.size(), defStyle);
                            }
                            currentLine.clear();
                        }
                    }
                }
                if (!currentLine.empty()) {
                    std::string cleanPart = trimR(currentLine);
                    outputLines.push_back(cleanPart);
                    outputLineColors.emplace_back(cleanPart.size(), defStyle);
                }
                scroll();
            }
        }

        if (outputLines.empty()) {
            outputLines.push_back("");
            outputLineColors.emplace_back();
        }

        // Keep ANSI parser state aligned when plain text output is used.
        ansiLines = outputLines;
        ansiLineColors = outputLineColors;
        ansiCursorRow = static_cast<int>(ansiLines.size()) - 1;
        ansiCursorCol = static_cast<int>(ansiLines.back().size());
        ansiCurrentColor = text_color;
        ansiCurrentBg = {0, 0, 0, 0};
        ansiBold = false;
        ansiUnderline = false;
        ansiInitialized = true;
        scroll();
    }

    int Terminal::calculateWrappedLinesForText(const std::string &text, int maxWidth, int promptWidth) {
        if (text.empty()) {
            return 1;
        }

#ifdef FOR_WASM
        if (text.find('\n') != std::string::npos) {
            int contPromptWidth = 0;
            TTF_SizeText(font, continuationPrompt.c_str(), &contPromptWidth, nullptr);

            auto countSegment = [&](const std::string& segmentText, int segmentPromptWidth) {
                if (segmentText.empty()) {
                    return 1;
                }

                int lineCount = 0;
                std::string lineToRender;
                int currentWidth = 0;
                bool isFirstLine = true;

                for (size_t i = 0; i < segmentText.length(); ++i) {
                    lineToRender += segmentText[i];
                    TTF_SizeText(font, lineToRender.c_str(), &currentWidth, nullptr);
                    int currentMaxWidth = isFirstLine ? maxWidth - segmentPromptWidth : maxWidth;
                    if (currentWidth > currentMaxWidth) {
                        lineCount++;
                        lineToRender.clear();
                        lineToRender += segmentText[i];
                        TTF_SizeText(font, lineToRender.c_str(), &currentWidth, nullptr);
                        isFirstLine = false;
                    }
                }

                if (!lineToRender.empty()) {
                    lineCount++;
                }
                return lineCount;
            };

            int total = 0;
            bool firstLogicalLine = true;
            size_t pos = 0;
            while (true) {
                size_t next = text.find('\n', pos);
                std::string segment = (next == std::string::npos) ? text.substr(pos) : text.substr(pos, next - pos);
                total += countSegment(segment, firstLogicalLine ? promptWidth : contPromptWidth);
                if (next == std::string::npos) {
                    break;
                }
                pos = next + 1;
                firstLogicalLine = false;
            }

            return total;
        }
#endif

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
        // Recompute wrap/scroll without rebuilding colors from plain text
        // (print("") would discard ANSI color info captured during parsing).
        scroll();
        Window::dragging = false;
    }

    bool Terminal::sessionAlive() const {
#ifdef _WIN32
        if (procInfo.hProcess == NULL) return false;
        DWORD code = 0;
        if (!GetExitCodeProcess(procInfo.hProcess, &code)) return false;
        return code == STILL_ACTIVE;
#elif !defined(FOR_WASM)
        if (bashPID <= 0) return false;
        int status = 0;
        pid_t r = ::waitpid(bashPID, &status, WNOHANG);
        // r == 0 means still running; r == bashPID means exited; -1 error.
        return r == 0;
#else
        return true;
#endif
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
        if (!terminal->isEmbedded()) {
            SDL_Event quit_event;
            quit_event.type = SDL_QUIT;
            SDL_PushEvent(&quit_event);
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
            if (!terminal->isEmbedded()) {
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
            }
            return 0;
    }
                
#endif    

    void Terminal::launchEditor(const std::string &filename) {
        editor = std::make_unique<TextEditor>(font, text_color);
        if (!filename.empty()) {
            editor->open(filename);
        }
    }

    // -----------------------------------------------------------------------
    // Background shader effects
    // -----------------------------------------------------------------------
    namespace {

        // All shaders are GLES2 / GLSL ES 1.00.
        // Vertex shader inputs: varying vec2 vUV (0..1, top-left origin)
        // Common uniforms: uTime (float), uResolution (vec2), uTex (sampler2D)

        const char *kFS_Plasma =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "void main() {\n"
            "    vec2 uv = vUV * 2.0 - 1.0;\n"
            "    uv.x *= uResolution.x / uResolution.y;\n"
            "    float v = sin(uv.x * 4.0 + uTime * 0.7);\n"
            "    v += sin(uv.y * 4.0 + uTime * 0.5);\n"
            "    v += sin((uv.x + uv.y) * 3.0 + uTime * 0.8);\n"
            "    v += sin(length(uv) * 6.0 - uTime);\n"
            "    v *= 0.25;\n"
            "    float pi = 3.14159;\n"
            "    vec3 col;\n"
            "    col.r = 0.5 + 0.5 * sin(v * pi + uTime * 0.3);\n"
            "    col.g = 0.5 + 0.5 * sin(v * pi + uTime * 0.3 + 2.094);\n"
            "    col.b = 0.5 + 0.5 * sin(v * pi + uTime * 0.3 + 4.189);\n"
            "    gl_FragColor = vec4(col * 0.9, 1.0);\n"
            "}\n";

        const char *kFS_Vortex =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    vec2 uv = vUV - 0.5;\n"
            "    float dist = length(uv);\n"
            "    float angle = atan(uv.y, uv.x);\n"
            "    float twist = uTime * 0.5 + dist * 5.0;\n"
            "    vec2 warped = vec2(cos(angle + twist), sin(angle + twist)) * dist + 0.5;\n"
            "    warped = clamp(warped, 0.0, 1.0);\n"
            "    gl_FragColor = texture2D(uTex, warped);\n"
            "}\n";

        const char *kFS_ChromaticRipple =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    vec2 uv = vUV;\n"
            "    float dist = length(uv - 0.5);\n"
            "    float ripple = sin(dist * 20.0 - uTime * 2.5) * 0.012;\n"
            "    vec2 dir = normalize(uv - 0.5 + vec2(0.001));\n"
            "    vec2 uvR = clamp(uv + dir * ripple * 1.6, 0.0, 1.0);\n"
            "    vec2 uvG = clamp(uv + dir * ripple, 0.0, 1.0);\n"
            "    vec2 uvB = clamp(uv - dir * ripple * 0.6, 0.0, 1.0);\n"
            "    float r = texture2D(uTex, uvR).r;\n"
            "    float g = texture2D(uTex, uvG).g;\n"
            "    float b = texture2D(uTex, uvB).b;\n"
            "    gl_FragColor = vec4(r, g, b, 1.0);\n"
            "}\n";

        const char *kFS_NeonGrid =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "void main() {\n"
            "    vec2 uv = vUV * vec2(uResolution.x / uResolution.y, 1.0) * 20.0;\n"
            "    vec2 gv = fract(uv) - 0.5;\n"
            "    float grid = min(abs(gv.x), abs(gv.y));\n"
            "    float pulse = 0.5 + 0.5 * sin(uTime * 1.5);\n"
            "    float line = smoothstep(0.05, 0.02, grid) * (0.3 + 0.7 * pulse);\n"
            "    vec3 col;\n"
            "    col.r = 0.1 + 0.9 * (0.5 + 0.5 * sin(uv.x * 0.4 + uTime * 0.3));\n"
            "    col.g = 0.1 + 0.9 * (0.5 + 0.5 * sin(uv.y * 0.4 + uTime * 0.4 + 1.0));\n"
            "    col.b = 0.6 + 0.4 * (0.5 + 0.5 * sin(uTime * 0.5 + 2.0));\n"
            "    gl_FragColor = vec4(col * line, 1.0);\n"
            "}\n";

        // Starfield slot: ported from acidcam-gpu/shaders/g_ufo_3d_v2.glsl.
        // 3D rotation + ping-pong scaling distortion of the wallpaper texture,
        // with per-pixel angle modulation for a more chaotic warp.
        const char *kFS_Starfield =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "uniform sampler2D uTex;\n"
            "mat3 rotX(float a){float s=sin(a),c=cos(a);return mat3(1.0,0.0,0.0, 0.0,c,-s, 0.0,s,c);}\n"
            "mat3 rotY(float a){float s=sin(a),c=cos(a);return mat3(c,0.0,s, 0.0,1.0,0.0, -s,0.0,c);}\n"
            "mat3 rotZ(float a){float s=sin(a),c=cos(a);return mat3(c,-s,0.0, s,c,0.0, 0.0,0.0,1.0);}\n"
            "void main(void) {\n"
            "    float aspect = uResolution.x / max(uResolution.y, 1.0);\n"
            "    vec2 ar = vec2(aspect, 1.0);\n"
            "    vec2 m = vec2(0.5);\n"
            "    vec2 p2 = (vUV - m) * ar;\n"
            "    float ax = 0.25 * sin(uTime * 0.7 + vUV.x * 10.0);\n"
            "    float ay = 0.25 * cos(uTime * 0.6 - vUV.y * 10.0);\n"
            "    float az = uTime * 0.5;\n"
            "    vec3 p3 = vec3(p2, 1.0);\n"
            "    mat3 R = rotZ(az) * rotY(ay) * rotX(ax);\n"
            "    vec3 r = R * p3;\n"
            "    float k = 0.6;\n"
            "    float zf = 1.0 / (1.0 + r.z * k);\n"
            "    vec2 q = r.xy * zf;\n"
            "    float dist = length(p2);\n"
            "    float scale_factor = 0.2 * sin(dist * 15.0 - uTime * 2.0 + vUV.x * 5.0 + vUV.y * 5.0);\n"
            "    q *= (1.0 + scale_factor);\n"
            "    vec2 uv = q / ar + m;\n"
            "    uv = clamp(uv, 0.0, 1.0);\n"
            "    gl_FragColor = texture2D(uTex, uv);\n"
            "}\n";

        const char *kFS_LiquidWave =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    vec2 uv = vUV;\n"
            "    float dx = sin(uv.y * 8.0 + uTime * 1.2) * 0.014\n"
            "             + sin(uv.y * 15.0 - uTime * 0.7) * 0.007;\n"
            "    float dy = sin(uv.x * 6.0 + uTime * 0.9) * 0.012\n"
            "             + cos(uv.x * 12.0 + uTime * 1.3) * 0.005;\n"
            "    vec2 warped = clamp(uv + vec2(dx, dy), 0.0, 1.0);\n"
            "    vec4 c = texture2D(uTex, warped);\n"
            "    c.r *= 0.85 + 0.15 * sin(uTime * 0.4);\n"
            "    c.b *= 0.85 + 0.15 * sin(uTime * 0.3 + 1.0);\n"
            "    gl_FragColor = c;\n"
            "}\n";

        const char *kFS_Fractal =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "void main() {\n"
            "    vec2 uv = (vUV - 0.5) * 3.0;\n"
            "    uv.x *= uResolution.x / uResolution.y;\n"
            "    vec2 c = vec2(0.355 + sin(uTime * 0.1) * 0.1,\n"
            "                  0.355 + cos(uTime * 0.13) * 0.1);\n"
            "    vec2 z = uv;\n"
            "    float iter = 0.0;\n"
            "    for (int i = 0; i < 64; i++) {\n"
            "        z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;\n"
            "        if (dot(z, z) > 4.0) { iter = float(i); break; }\n"
            "    }\n"
            "    float t = iter / 64.0;\n"
            "    vec3 col = vec3(0.0);\n"
            "    if (t > 0.0) {\n"
            "        col.r = 0.5 + 0.5 * sin(t * 10.0 + uTime * 0.3);\n"
            "        col.g = 0.5 + 0.5 * sin(t * 10.0 + uTime * 0.3 + 2.094);\n"
            "        col.b = 0.5 + 0.5 * sin(t * 10.0 + uTime * 0.3 + 4.189);\n"
            "    }\n"
            "    gl_FragColor = vec4(col, 1.0);\n"
            "}\n";

        const char *kFS_AcidSpiral =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "uniform sampler2D uTex;\n"
            "void main() {\n"
            "    vec2 uv = vUV - 0.5;\n"
            "    uv.x *= uResolution.x / uResolution.y;\n"
            "    float angle = atan(uv.y, uv.x);\n"
            "    float dist = length(uv);\n"
            "    float segments = 6.0;\n"
            "    float pi = 3.14159;\n"
            "    float segAngle = pi * 2.0 / segments;\n"
            "    angle = mod(angle + pi, segAngle * 2.0);\n"
            "    angle = abs(angle - segAngle);\n"
            "    vec2 folded = vec2(cos(angle), sin(angle)) * dist;\n"
            "    float spiral = dist * 3.0 - uTime * 0.6;\n"
            "    folded += vec2(sin(spiral) * 0.08, cos(spiral) * 0.08);\n"
            "    vec2 tUV = clamp(folded + 0.5, 0.0, 1.0);\n"
            "    gl_FragColor = texture2D(uTex, tUV);\n"
            "}\n";

        // Aurora Borealis: layered sine-wave bands in green/blue/purple
        const char *kFS_Aurora =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "float band(vec2 uv, float offset, float t) {\n"
            "    float w = sin(uv.x * 3.1 + t + offset) * 0.06\n"
            "           + sin(uv.x * 5.7 - t * 1.3 + offset) * 0.04\n"
            "           + sin(uv.x * 8.3 + t * 0.7 + offset) * 0.025;\n"
            "    float cy = 0.32 + offset * 0.11 + w;\n"
            "    float d = uv.y - cy;\n"
            "    return exp(-d * d * 90.0) * (0.5 + 0.5 * sin(uv.x * 4.0 + t * 1.5 + offset));\n"
            "}\n"
            "void main() {\n"
            "    float t = uTime * 0.22;\n"
            "    vec3 col = vec3(0.0, 0.01, 0.04);\n"
            "    col += vec3(0.05, 0.80, 0.30) * band(vUV, 0.0, t);\n"
            "    col += vec3(0.10, 0.50, 0.80) * band(vUV, 1.0, t);\n"
            "    col += vec3(0.50, 0.10, 0.80) * band(vUV, 2.0, t);\n"
            "    col += vec3(0.05, 0.70, 0.40) * band(vUV, 3.0, t);\n"
            "    col += vec3(0.15, 0.20, 0.90) * band(vUV, 4.0, t);\n"
            "    float star = fract(sin(dot(floor(vUV * 160.0), vec2(127.1, 311.7))) * 43758.5);\n"
            "    col += step(0.975, star) * 0.4;\n"
            "    gl_FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);\n"
            "}\n";

        // Tunnel: fly through a rainbow-tiled infinite corridor
        const char *kFS_Tunnel =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "void main() {\n"
            "    vec2 uv = vUV - 0.5;\n"
            "    uv.x *= uResolution.x / uResolution.y;\n"
            "    float dist = length(uv);\n"
            "    float angle = atan(uv.y, uv.x);\n"
            "    float pi2 = 6.28318;\n"
            "    float depth = 0.25 / max(dist, 0.001) + uTime * 0.6;\n"
            "    float a = (angle / pi2) + 0.5;\n"
            "    float tiles = floor(mod(depth * 4.0, 2.0)) + floor(mod(a * 8.0, 2.0));\n"
            "    float check = mod(tiles, 2.0);\n"
            "    float fog = smoothstep(0.0, 0.25, dist);\n"
            "    float hue = a + uTime * 0.12;\n"
            "    vec3 c1 = vec3(0.5 + 0.5 * sin(hue * pi2),\n"
            "                   0.5 + 0.5 * sin(hue * pi2 + 2.094),\n"
            "                   0.5 + 0.5 * sin(hue * pi2 + 4.189));\n"
            "    vec3 c2 = c1 * 0.08;\n"
            "    gl_FragColor = vec4(mix(c2, c1, check) * fog, 1.0);\n"
            "}\n";

        // Crystal: animated Voronoi diagram with jewel colours
        const char *kFS_Crystal =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "vec2 hv2(vec2 p) {\n"
            "    p = vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)));\n"
            "    return fract(sin(p) * 43758.5453);\n"
            "}\n"
            "void main() {\n"
            "    vec2 uv = vUV * vec2(uResolution.x / uResolution.y, 1.0) * 6.0;\n"
            "    vec2 p = floor(uv);\n"
            "    vec2 f = fract(uv);\n"
            "    float md = 9.0, md2 = 9.0;\n"
            "    vec2 cid = vec2(0.0);\n"
            "    for (int j = -1; j <= 1; j++) {\n"
            "        for (int i = -1; i <= 1; i++) {\n"
            "            vec2 b = vec2(float(i), float(j));\n"
            "            vec2 anim = sin(hv2(p + b) * 6.28 + uTime * 0.5) * 0.3;\n"
            "            vec2 r = b - f + 0.5 + anim;\n"
            "            float d = dot(r, r);\n"
            "            if (d < md) { md2 = md; md = d; cid = b; }\n"
            "            else if (d < md2) { md2 = d; }\n"
            "        }\n"
            "    }\n"
            "    float edge = sqrt(md2) - sqrt(md);\n"
            "    float hue = fract(dot(p + cid, vec2(0.317, 0.177)) + uTime * 0.07);\n"
            "    vec3 col = vec3(0.5 + 0.5 * sin(hue * 6.28),\n"
            "                    0.5 + 0.5 * sin(hue * 6.28 + 2.094),\n"
            "                    0.5 + 0.5 * sin(hue * 6.28 + 4.189));\n"
            "    col = col * smoothstep(0.0, 0.08, edge) + 0.9 * smoothstep(0.08, 0.0, edge);\n"
            "    gl_FragColor = vec4(col, 1.0);\n"
            "}\n";

        // Fire slot: ported from acidcam-gpu/shaders/g_ufo_3d.glsl.
        // Smooth 3D rotation + radial pulse warp of the wallpaper texture.
        const char *kFS_Fire =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "uniform sampler2D uTex;\n"
            "mat3 rotX(float a){float s=sin(a),c=cos(a);return mat3(1.0,0.0,0.0, 0.0,c,-s, 0.0,s,c);}\n"
            "mat3 rotY(float a){float s=sin(a),c=cos(a);return mat3(c,0.0,s, 0.0,1.0,0.0, -s,0.0,c);}\n"
            "mat3 rotZ(float a){float s=sin(a),c=cos(a);return mat3(c,-s,0.0, s,c,0.0, 0.0,0.0,1.0);}\n"
            "void main(void) {\n"
            "    float aspect = uResolution.x / max(uResolution.y, 1.0);\n"
            "    vec2 ar = vec2(aspect, 1.0);\n"
            "    vec2 m = vec2(0.5);\n"
            "    vec2 p2 = (vUV - m) * ar;\n"
            "    float ax = 0.25 * sin(uTime * 0.7);\n"
            "    float ay = 0.25 * cos(uTime * 0.6);\n"
            "    float az = uTime * 0.5;\n"
            "    vec3 p3 = vec3(p2, 1.0);\n"
            "    mat3 R = rotZ(az) * rotY(ay) * rotX(ax);\n"
            "    vec3 r = R * p3;\n"
            "    float k = 0.6;\n"
            "    float zf = 1.0 / (1.0 + r.z * k);\n"
            "    vec2 q = r.xy * zf;\n"
            "    float dist = length(p2);\n"
            "    float scale = 1.0 + 0.2 * sin(dist * 15.0 - uTime * 2.0);\n"
            "    q *= scale;\n"
            "    vec2 uv = q / ar + m;\n"
            "    uv = clamp(uv, 0.0, 1.0);\n"
            "    gl_FragColor = texture2D(uTex, uv);\n"
            "}\n";

        // Hyperspace: angular streaks at warp speed.  Uses a sector-based
        // approach: divide circle into N sectors, animate each one's radial
        // position.  No large per-pixel loop required.
        const char *kFS_Hyperspace =
            "#ifdef GL_ES\n"
            "precision mediump float;\n"
            "#endif\n"
            "varying vec2 vUV;\n"
            "uniform float uTime;\n"
            "uniform vec2 uResolution;\n"
            "float h11(float n) { return fract(sin(n * 91.345) * 47453.5453); }\n"
            "void main() {\n"
            "    vec2 uv = vUV - 0.5;\n"
            "    uv.x *= uResolution.x / max(uResolution.y, 1.0);\n"
            "    float dist  = length(uv);\n"
            "    float angle = atan(uv.y, uv.x) / 6.28318 + 0.5;\n"
            "    float sectors = 64.0;\n"
            "    float a  = angle * sectors;\n"
            "    float ai = floor(a);\n"
            "    float af = fract(a) - 0.5;\n"
            "    float seedA = h11(ai);\n"
            "    float seedB = h11(ai + 91.0);\n"
            "    float seedC = h11(ai + 17.0);\n"
            "    float speed = 0.25 + seedA * 0.7;\n"
            "    float t = fract(uTime * speed + seedB);\n"
            "    float head = t * 0.95;\n"
            "    float len  = 0.10 + seedC * 0.30;\n"
            "    float radial = smoothstep(head - len, head - len * 0.2, dist)\n"
            "                 * smoothstep(head + 0.01, head, dist);\n"
            "    float width  = 0.06 + 0.20 * (1.0 - t);\n"
            "    float angular = smoothstep(width, 0.0, abs(af));\n"
            "    float fall = smoothstep(0.0, 0.15, t) * smoothstep(1.0, 0.75, t);\n"
            "    float core = radial * angular * fall;\n"
            "    vec3 tint = mix(vec3(0.7, 0.85, 1.0), vec3(1.0, 0.55, 0.95), seedC);\n"
            "    vec3 col = core * tint * 1.6;\n"
            "    col += vec3(0.0, 0.02, 0.10) * smoothstep(0.6, 0.0, dist);\n"
            "    col += vec3(0.6, 0.8, 1.0) * smoothstep(0.04, 0.0, dist) * 0.6;\n"
            "    gl_FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);\n"
            "}\n";

        static const char *kEffectFS[] = {
            nullptr,             // None (0)
            kFS_Plasma,          // 1
            kFS_Vortex,          // 2
            kFS_ChromaticRipple, // 3
            kFS_NeonGrid,        // 4
            kFS_Starfield,       // 5
            kFS_LiquidWave,      // 6
            kFS_Fractal,         // 7
            kFS_AcidSpiral,      // 8
            kFS_Aurora,          // 9
            kFS_Tunnel,          // 10
            kFS_Crystal,         // 11
            kFS_Fire,            // 12
            kFS_Hyperspace,      // 13
        };

    } // anonymous namespace

    void Terminal::setEffect(TermEffect e, mxApp &app) {
        activeEffect_ = e;
        const int idx = static_cast<int>(e);
        if (idx <= 0 || idx >= kEffectCount) return;
        if (effectProgs_[idx] == 0) {
            // Lazily compile the shader
            effectProgs_[idx] =
                app.ren->buildEffectShader(kEffectFS[idx]);
        }
        // Reset animation time when switching effects
        shaderTime_ = 0.0f;
        lastEffectMs_ = SDL_GetTicks();
    }

    void Terminal::drawEffectBackground(mxApp &app) {
        const int idx = static_cast<int>(activeEffect_);
        if (idx <= 0 || idx >= kEffectCount) return;
        if (effectProgs_[idx] == 0) return;

        // Advance shader time
        Uint32 now = SDL_GetTicks();
        if (lastEffectMs_ == 0) lastEffectMs_ = now;
        shaderTime_ += static_cast<float>(now - lastEffectMs_) * 0.001f;
        lastEffectMs_ = now;

        // Use the wallpaper as the input texture (may be null for procedural effects)
        app.ren->applyEffect(wallpaper, effectProgs_[idx], shaderTime_);
    }
}
 