#include "mx_editor.hpp"
#include "tee_stream.hpp"
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sstream>

namespace mx {

    TextEditor::TextEditor(TTF_Font *f, SDL_Color c) : font(f), color(c) {
        lines.emplace_back("");
    }

    bool TextEditor::open(const std::string &fname) {
        filename = fname;
        lines.clear();
        cursorRow = cursorCol = 0;
        topRow = leftCol = 0;
        modified = false;
        std::ifstream in(filename);
        if (!in) {
            // New file.
            lines.emplace_back("");
            setStatus("New file");
            return true;
        }
        std::string line;
        while (std::getline(in, line)) {
            // Strip a possible trailing CR for CRLF files.
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            lines.push_back(std::move(line));
        }
        if (lines.empty())
            lines.emplace_back("");
        setStatus("Read " + std::to_string(lines.size()) + " lines");
        return true;
    }

    bool TextEditor::save() { return saveAs(filename); }

    bool TextEditor::saveAs(const std::string &name) {
        if (name.empty()) {
            setStatus("Cancelled");
            return false;
        }
        std::ofstream out(name, std::ios::binary | std::ios::trunc);
        if (!out) {
            setStatus("Error writing " + name);
            return false;
        }
        for (size_t i = 0; i < lines.size(); ++i) {
            out << lines[i];
            if (i + 1 < lines.size())
                out << '\n';
        }
        out.close();
        filename = name;
        modified = false;
        setStatus("Wrote " + std::to_string(lines.size()) + " lines");
        return true;
    }

    void TextEditor::setStatus(const std::string &msg) {
        statusMsg = msg;
        statusUntil = SDL_GetTicks() + 3000;
    }

    void TextEditor::clampCursor() {
        if (lines.empty())
            lines.emplace_back("");
        if (cursorRow < 0)
            cursorRow = 0;
        if (cursorRow >= static_cast<int>(lines.size()))
            cursorRow = static_cast<int>(lines.size()) - 1;
        int lineLen = static_cast<int>(lines[cursorRow].size());
        if (cursorCol < 0)
            cursorCol = 0;
        if (cursorCol > lineLen)
            cursorCol = lineLen;
    }

    void TextEditor::ensureVisible() {
        if (cursorRow < topRow)
            topRow = cursorRow;
        if (viewRows > 0 && cursorRow >= topRow + viewRows)
            topRow = cursorRow - viewRows + 1;
        if (cursorCol < leftCol)
            leftCol = cursorCol;
        if (viewCols > 0 && cursorCol >= leftCol + viewCols)
            leftCol = cursorCol - viewCols + 1;
        if (topRow < 0)
            topRow = 0;
        if (leftCol < 0)
            leftCol = 0;
    }

    void TextEditor::insertChar(char c) {
        clampCursor();
        lines[cursorRow].insert(lines[cursorRow].begin() + cursorCol, c);
        cursorCol++;
        modified = true;
    }

    void TextEditor::insertString(const std::string &s) {
        for (char c : s) {
            if (c == '\n')
                newline();
            else if (c == '\r')
                continue;
            else if (c == '\t')
                insertString("    ");
            else if (static_cast<unsigned char>(c) >= 32)
                insertChar(c);
        }
    }

    void TextEditor::backspace() {
        clampCursor();
        if (cursorCol > 0) {
            lines[cursorRow].erase(cursorCol - 1, 1);
            cursorCol--;
            modified = true;
        } else if (cursorRow > 0) {
            int prevLen = static_cast<int>(lines[cursorRow - 1].size());
            lines[cursorRow - 1] += lines[cursorRow];
            lines.erase(lines.begin() + cursorRow);
            cursorRow--;
            cursorCol = prevLen;
            modified = true;
        }
    }

    void TextEditor::deleteChar() {
        clampCursor();
        if (cursorCol < static_cast<int>(lines[cursorRow].size())) {
            lines[cursorRow].erase(cursorCol, 1);
            modified = true;
        } else if (cursorRow + 1 < static_cast<int>(lines.size())) {
            lines[cursorRow] += lines[cursorRow + 1];
            lines.erase(lines.begin() + cursorRow + 1);
            modified = true;
        }
    }

    void TextEditor::newline() {
        clampCursor();
        std::string tail = lines[cursorRow].substr(cursorCol);
        lines[cursorRow].erase(cursorCol);
        lines.insert(lines.begin() + cursorRow + 1, tail);
        cursorRow++;
        cursorCol = 0;
        modified = true;
    }

    void TextEditor::moveLeft() {
        if (cursorCol > 0) {
            cursorCol--;
        } else if (cursorRow > 0) {
            cursorRow--;
            cursorCol = static_cast<int>(lines[cursorRow].size());
        }
    }
    void TextEditor::moveRight() {
        clampCursor();
        if (cursorCol < static_cast<int>(lines[cursorRow].size())) {
            cursorCol++;
        } else if (cursorRow + 1 < static_cast<int>(lines.size())) {
            cursorRow++;
            cursorCol = 0;
        }
    }
    void TextEditor::moveUp() {
        if (cursorRow > 0)
            cursorRow--;
    }
    void TextEditor::moveDown() {
        if (cursorRow + 1 < static_cast<int>(lines.size()))
            cursorRow++;
    }
    void TextEditor::pageUp() {
        int step = std::max(1, viewRows - 1);
        cursorRow = std::max(0, cursorRow - step);
        topRow = std::max(0, topRow - step);
    }
    void TextEditor::pageDown() {
        int step = std::max(1, viewRows - 1);
        cursorRow = std::min(static_cast<int>(lines.size()) - 1,
                             cursorRow + step);
        topRow = std::min(std::max(0, static_cast<int>(lines.size()) - 1),
                          topRow + step);
    }

    void TextEditor::fillRect(mxApp &app, const SDL_Rect &r, Uint8 r8,
                              Uint8 g8, Uint8 b8, Uint8 a8) {
        SDL_SetRenderDrawBlendMode(app.ren, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(app.ren, r8, g8, b8, a8);
        SDL_RenderFillRect(app.ren, &r);
    }

    void TextEditor::renderLine(mxApp &app, const std::string &text, int x,
                                int y) {
        if (text.empty())
            return;
        SDL_Surface *surf = TTF_RenderText_Blended(font, text.c_str(), color);
        if (!surf)
            return;
        SDL_Texture *tex = SDL_CreateTextureFromSurface(app.ren, surf);
        if (tex) {
            SDL_Rect dst = {x, y, surf->w, surf->h};
            SDL_RenderCopy(app.ren, tex, nullptr, &dst);
            SDL_DestroyTexture(tex);
        }
        SDL_FreeSurface(surf);
    }

    void TextEditor::draw(mxApp &app, const SDL_Rect &area) {
        // Background.
        fillRect(app, area, 0, 0, 0, 220);

        lineH = TTF_FontHeight(font);
        if (lineH <= 0)
            lineH = 16;
        TTF_SizeText(font, "M", &charW, nullptr);
        if (charW <= 0)
            charW = 8;

        const int margin = 4;
        const int titleH = lineH + 4;
        const int statusH = lineH + 4;
        const int helpH = lineH + 4;

        // Title bar.
        SDL_Rect title = {area.x, area.y, area.w, titleH};
        fillRect(app, title, 40, 40, 80, 255);
        std::string tname = filename.empty() ? std::string("[New Buffer]")
                                             : filename;
        std::string titleText = " MX Editor   File: " + tname +
                                (modified ? "  [Modified]" : "");
        renderLine(app, titleText, area.x + margin, area.y + 2);

        // Help bar at bottom: nano-style hints.
        SDL_Rect help = {area.x, area.y + area.h - helpH, area.w, helpH};
        fillRect(app, help, 40, 40, 40, 255);
        renderLine(app,
                   "^X Exit   ^O Save As   ^S Save   ^K Cut Line   "
                   "Arrows Move",
                   area.x + margin, help.y + 2);

        // Status bar (above help bar).
        SDL_Rect status = {area.x, help.y - statusH, area.w, statusH};
        fillRect(app, status, 20, 20, 20, 255);
        if (SDL_GetTicks() < statusUntil && !statusMsg.empty()) {
            renderLine(app, "[ " + statusMsg + " ]", area.x + margin,
                       status.y + 2);
        } else if (prompt != PromptMode::None) {
            std::string label;
            switch (prompt) {
            case PromptMode::ExitConfirm:
                label = "Save modified buffer? (Y/N) ";
                break;
            case PromptMode::ExitFilename:
            case PromptMode::SaveFilename:
                label = "File name to write: ";
                break;
            default:
                break;
            }
            renderLine(app, label + promptInput, area.x + margin,
                       status.y + 2);
        } else {
            std::ostringstream oss;
            oss << "Line " << (cursorRow + 1) << "/" << lines.size()
                << "   Col " << (cursorCol + 1);
            renderLine(app, oss.str(), area.x + margin, status.y + 2);
        }

        // Editing area.
        SDL_Rect edit = {area.x, area.y + titleH, area.w,
                         area.h - titleH - statusH - helpH};
        viewRows = std::max(1, edit.h / lineH);
        viewCols = std::max(1, (edit.w - margin * 2) / charW);

        clampCursor();
        ensureVisible();

        for (int r = 0; r < viewRows; ++r) {
            int rowIndex = topRow + r;
            if (rowIndex >= static_cast<int>(lines.size()))
                break;
            const std::string &full = lines[rowIndex];
            std::string visible;
            if (leftCol < static_cast<int>(full.size()))
                visible = full.substr(leftCol, viewCols);
            renderLine(app, visible, edit.x + margin, edit.y + r * lineH);
        }

        // Cursor (only when no prompt is active).
        if (prompt == PromptMode::None) {
            int cx = edit.x + margin + (cursorCol - leftCol) * charW;
            int cy = edit.y + (cursorRow - topRow) * lineH;
            Uint32 t = SDL_GetTicks();
            if ((t / 500) % 2 == 0) {
                SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b,
                                       255);
                SDL_RenderDrawLine(app.ren, cx, cy, cx, cy + lineH);
            }
        } else {
            // Prompt cursor at end of input.
            int promptWidth = 0;
            std::string label;
            switch (prompt) {
            case PromptMode::ExitConfirm:
                label = "Save modified buffer? (Y/N) ";
                break;
            case PromptMode::ExitFilename:
            case PromptMode::SaveFilename:
                label = "File name to write: ";
                break;
            default:
                break;
            }
            std::string before = label + promptInput.substr(0, promptCursor);
            TTF_SizeText(font, before.c_str(), &promptWidth, nullptr);
            int cx = area.x + margin + promptWidth;
            int cy = status.y + 2;
            Uint32 t = SDL_GetTicks();
            if ((t / 500) % 2 == 0) {
                SDL_SetRenderDrawColor(app.ren, color.r, color.g, color.b,
                                       255);
                SDL_RenderDrawLine(app.ren, cx, cy, cx, cy + lineH);
            }
        }
    }

    bool TextEditor::event(mxApp &app, SDL_Event &e) {
        (void)app;
        if (!active)
            return false;

        // ---- Prompt handling: takes precedence over normal editing. ----
        if (prompt != PromptMode::None) {
            if (e.type == SDL_TEXTINPUT) {
                if (prompt == PromptMode::ExitConfirm) {
                    char c = e.text.text[0];
                    if (c == 'y' || c == 'Y') {
                        // Need a filename to save to.
                        if (filename.empty()) {
                            prompt = PromptMode::ExitFilename;
                            promptInput.clear();
                            promptCursor = 0;
                        } else {
                            if (save())
                                active = false;
                            else
                                prompt = PromptMode::None;
                        }
                    } else if (c == 'n' || c == 'N') {
                        active = false;
                        prompt = PromptMode::None;
                    }
                    return true;
                }
                // Filename prompts: collect text input.
                promptInput.insert(promptCursor, e.text.text);
                promptCursor += static_cast<int>(strlen(e.text.text));
                return true;
            }
            if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE ||
                    (k == SDLK_c && (e.key.keysym.mod & KMOD_CTRL))) {
                    prompt = PromptMode::None;
                    promptInput.clear();
                    promptCursor = 0;
                    setStatus("Cancelled");
                    return true;
                }
                if (prompt == PromptMode::ExitConfirm) {
                    // Y/N also captured via SDLK_y / SDLK_n if no TEXTINPUT.
                    if (k == SDLK_y) {
                        if (filename.empty()) {
                            prompt = PromptMode::ExitFilename;
                            promptInput.clear();
                            promptCursor = 0;
                        } else if (save())
                            active = false;
                        return true;
                    }
                    if (k == SDLK_n) {
                        active = false;
                        prompt = PromptMode::None;
                        return true;
                    }
                    return true;
                }
                // Filename prompts.
                if (k == SDLK_BACKSPACE) {
                    if (promptCursor > 0) {
                        promptInput.erase(promptCursor - 1, 1);
                        promptCursor--;
                    }
                    return true;
                }
                if (k == SDLK_LEFT) {
                    if (promptCursor > 0)
                        promptCursor--;
                    return true;
                }
                if (k == SDLK_RIGHT) {
                    if (promptCursor < static_cast<int>(promptInput.size()))
                        promptCursor++;
                    return true;
                }
                if (k == SDLK_HOME) {
                    promptCursor = 0;
                    return true;
                }
                if (k == SDLK_END) {
                    promptCursor = static_cast<int>(promptInput.size());
                    return true;
                }
                if (k == SDLK_RETURN || k == SDLK_KP_ENTER) {
                    bool exitFlag = (prompt == PromptMode::ExitFilename);
                    bool ok = saveAs(promptInput);
                    prompt = PromptMode::None;
                    promptInput.clear();
                    promptCursor = 0;
                    if (ok && exitFlag)
                        active = false;
                    return true;
                }
            }
            return true;
        }

        // ---- Normal editing. ----
        if (e.type == SDL_TEXTINPUT) {
            insertString(e.text.text);
            return true;
        }
        if (e.type == SDL_KEYDOWN) {
            SDL_Keycode k = e.key.keysym.sym;
            bool ctrl = (e.key.keysym.mod & KMOD_CTRL) != 0;

            if (ctrl) {
                switch (k) {
                case SDLK_x: // Exit
                    if (modified) {
                        prompt = PromptMode::ExitConfirm;
                        promptInput.clear();
                        promptCursor = 0;
                    } else {
                        active = false;
                    }
                    return true;
                case SDLK_s: // Save
                    if (filename.empty()) {
                        prompt = PromptMode::SaveFilename;
                        promptInput.clear();
                        promptCursor = 0;
                    } else {
                        save();
                    }
                    return true;
                case SDLK_o: // Save as
                    prompt = PromptMode::SaveFilename;
                    promptInput = filename;
                    promptCursor = static_cast<int>(promptInput.size());
                    return true;
                case SDLK_k: // Cut current line
                    clampCursor();
                    if (lines.size() == 1) {
                        lines[0].clear();
                        cursorCol = 0;
                    } else {
                        lines.erase(lines.begin() + cursorRow);
                        if (cursorRow >= static_cast<int>(lines.size()))
                            cursorRow = static_cast<int>(lines.size()) - 1;
                        cursorCol = 0;
                    }
                    modified = true;
                    return true;
                case SDLK_a:
                    cursorCol = 0;
                    return true;
                case SDLK_e:
                    cursorCol = static_cast<int>(lines[cursorRow].size());
                    return true;
                default:
                    break;
                }
            }

            switch (k) {
            case SDLK_LEFT:
                moveLeft();
                return true;
            case SDLK_RIGHT:
                moveRight();
                return true;
            case SDLK_UP:
                moveUp();
                return true;
            case SDLK_DOWN:
                moveDown();
                return true;
            case SDLK_HOME:
                cursorCol = 0;
                return true;
            case SDLK_END:
                cursorCol = static_cast<int>(lines[cursorRow].size());
                return true;
            case SDLK_PAGEUP:
                pageUp();
                return true;
            case SDLK_PAGEDOWN:
                pageDown();
                return true;
            case SDLK_BACKSPACE:
                backspace();
                return true;
            case SDLK_DELETE:
                deleteChar();
                return true;
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                newline();
                return true;
            case SDLK_TAB:
                insertString("    ");
                return true;
            default:
                break;
            }
        }
        return true;
    }

}
