/**
 * @file icode_imp.cpp
 * @brief Pascal code generator helper methods and builtin function handler implementations
 * @author Jared Bruni
 */
#include "icode.hpp"
#include <algorithm>

namespace pascal {

    /**
     * @brief Convert a string to lowercase for case-insensitive comparison
     * @param s Input string
     * @return Lowercased copy of @p s
     */
    static std::string toLower(const std::string &s) {
        std::string r = s;
        std::transform(r.begin(), r.end(), r.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return r;
    }

    std::unordered_set<std::string> sdlFunctions = {
        "sdl_init", "sdl_quit", "sdl_create_window", "sdl_destroy_window", "sdl_set_window_title",
        "sdl_set_window_position", "sdl_get_window_size", "sdl_set_window_fullscreen", "sdl_set_window_icon",
        "sdl_create_renderer", "sdl_destroy_renderer", "sdl_set_draw_color", "sdl_clear", "sdl_present",
        "sdl_get_renderer_output_size", "sdl_draw_point", "sdl_draw_line", "sdl_draw_rect", "sdl_fill_rect",
        "sdl_poll_event", "sdl_get_event_type", "sdl_get_key_code", "sdl_get_mouse_x", "sdl_get_mouse_y",
        "sdl_get_mouse_button", "sdl_get_mouse_buttons", "sdl_get_relative_mouse_x", "sdl_get_relative_mouse_y",
        "sdl_get_relative_mouse_buttons", "sdl_create_rgb_surface", "sdl_free_surface", "sdl_blit_surface",
        "sdl_get_mouse_state", "sdl_get_relative_mouse_state", "sdl_get_keyboard_state", "sdl_is_key_pressed",
        "sdl_get_num_keys", "sdl_set_clipboard_text", "sdl_get_clipboard_text", "sdl_show_cursor",
        "sdl_create_texture", "sdl_destroy_texture", "sdl_load_texture", "sdl_load_texture_color_key", "sdl_load_texture_color_key_rgb", "sdl_render_texture",
        "sdl_update_texture", "sdl_lock_texture", "sdl_unlock_texture", "sdl_get_ticks", "sdl_delay",
        "sdl_open_audio", "sdl_close_audio", "sdl_pause_audio", "sdl_load_wav", "sdl_free_wav",
        "sdl_queue_audio", "sdl_get_queued_audio_size", "sdl_clear_queued_audio", "sdl_init_text",
        "sdl_quit_text", "sdl_load_font", "sdl_draw_text", "sdl_create_render_target", "sdl_set_render_target",
        "sdl_destroy_render_target", "sdl_present_scaled", "sdl_present_stretched"};

    std::unordered_set<std::string> stdFunctions = {
        "argc", "argv", "set_program_args", "free_program_args", "abs", "fabs",
        "sqrt", "pow", "sin", "cos", "tan", "floor", "ceil", "rand", "srand",
        "malloc", "calloc", "free", "toupper", "tolower", "isalpha", "isdigit",
        "isspace", "atoi", "atof", "exit", "system", "memcpy", "memcmp",
        "memmove", "memset", "exp", "exp2", "log", "log10", "log2", "fmod",
        "atan2", "asin", "acos", "atan", "sinh", "cosh", "tanh", "hypot",
        "round", "trunc", "float_to_int", "int_to_float", "halt",
        "ord", "chr", "succ", "pred", "inc", "dec", "include", "exclude"};

    bool IOFunctionHandler::canHandle(const std::string &funcName) const {
        auto f = toLower(funcName);
        return f == "writeln" || f == "write" || f == "readln" ||
               f == "seed_random" || f == "rand_number";
    }

    void IOFunctionHandler::generate(CodeGenVisitor &visitor, const std::string &funcName_,
                                     const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        auto funcName = toLower(funcName_);
        if (funcName == "writeln" || funcName == "write") {
            // Check if first argument is a file variable
            bool fileOutput = false;
            std::string fileSlot;
            size_t startArg = 0;
            if (!arguments.empty()) {
                if (auto *varNode = dynamic_cast<VariableNode *>(arguments[0].get())) {
                    std::string mangledName = visitor.findMangledName(varNode->name);
                    if (visitor.fileVars.count(toLower(mangledName)) ||
                        visitor.fileVars.count(toLower(varNode->name))) {
                        fileOutput = true;
                        fileSlot = visitor.eval(arguments[0].get());
                        startArg = 1;
                        visitor.usedModules.insert("io");
                    }
                }
            }

            for (size_t i = startArg; i < arguments.size(); ++i) {
                const auto &arg = arguments[i];
                std::string val = visitor.eval(arg.get());
                VarType type = visitor.getExpressionType(arg.get());

                if (fileOutput) {
                    // Use fprintf for file output
                    if (type == VarType::STRING || type == VarType::PTR) {
                        visitor.usedStrings.insert("fmt_str");
                        visitor.emit_invoke("fprintf", {fileSlot, "fmt_str", val});
                    } else if (type == VarType::DOUBLE) {
                        visitor.usedStrings.insert("fmt_float");
                        visitor.emit_invoke("fprintf", {fileSlot, "fmt_float", val});
                    } else if (type == VarType::CHAR) {
                        visitor.usedStrings.insert("fmt_chr");
                        visitor.emit_invoke("fprintf", {fileSlot, "fmt_chr", val});
                    } else {
                        visitor.usedStrings.insert("fmt_int");
                        visitor.emit_invoke("fprintf", {fileSlot, "fmt_int", val});
                    }
                } else {
                    if (type == VarType::STRING || type == VarType::PTR) {
                        visitor.usedStrings.insert("fmt_str");
                        visitor.emit2("print", "fmt_str", val);
                    } else if (type == VarType::DOUBLE) {
                        visitor.usedStrings.insert("fmt_float");
                        visitor.emit2("print", "fmt_float", val);
                    } else if (type == VarType::CHAR) {
                        visitor.usedStrings.insert("fmt_chr");
                        visitor.emit2("print", "fmt_chr", val);
                    } else {
                        visitor.usedStrings.insert("fmt_int");
                        visitor.emit2("print", "fmt_int", val);
                    }
                }
                if (visitor.isReg(val) && !visitor.isParmReg(val)) {
                    visitor.freeReg(val);
                }
            }
            if (funcName == "writeln") {
                if (fileOutput) {
                    std::string nlLabel = visitor.internString("\n");
                    visitor.emit_invoke("fputs", {nlLabel, fileSlot});
                } else {
                    visitor.usedStrings.insert("newline");
                    visitor.emit1("print", "newline");
                }
            }
            if (fileOutput && visitor.isReg(fileSlot) && !visitor.isParmReg(fileSlot))
                visitor.freeReg(fileSlot);
        } else if (funcName == "readln") {
            if (arguments.empty()) {
                visitor.emit1("getline", "input_buffer");
                return;
            }

            // Check if first argument is a file variable
            bool fileInput = false;
            std::string fileSlot;
            size_t startArg = 0;
            if (auto *firstVar = dynamic_cast<VariableNode *>(arguments[0].get())) {
                std::string mangledName = visitor.findMangledName(firstVar->name);
                if (visitor.fileVars.count(toLower(mangledName)) ||
                    visitor.fileVars.count(toLower(firstVar->name))) {
                    fileInput = true;
                    fileSlot = visitor.eval(arguments[0].get());
                    startArg = 1;
                    visitor.usedModules.insert("io");
                }
            }

            for (size_t i = startArg; i < arguments.size(); ++i) {
                auto &arg = arguments[i];
                if (auto varNode = dynamic_cast<VariableNode *>(arg.get())) {
                    std::string varName = varNode->name;
                    int slot = visitor.newSlotFor(varName);
                    std::string memLoc = visitor.slotVar(slot);

                    auto varType = visitor.getVarType(varName);

                    if (fileInput) {
                        // Read line from file using mxvm_fgets
                        visitor.emit_invoke("mxvm_fgets", {fileSlot});
                        std::string retReg = visitor.allocReg();
                        visitor.emit("return " + retReg);
                        if (varType == VarType::DOUBLE) {
                            visitor.emit2("to_float", memLoc, retReg);
                        } else if (varType == VarType::INT) {
                            visitor.emit2("to_int", memLoc, retReg);
                        } else {
                            visitor.emit2("mov", memLoc, retReg);
                        }
                        visitor.freeReg(retReg);
                    } else {
                        visitor.emit1("getline", "input_buffer");
                        if (varType == VarType::DOUBLE) {
                            visitor.emit2("to_float", memLoc, "input_buffer");
                        } else if (varType == VarType::INT) {
                            visitor.emit2("to_int", memLoc, "input_buffer");
                        }
                    }

                    visitor.recordLocation(varName, {CodeGenVisitor::ValueLocation::MEMORY, memLoc});
                } else {
                    int lineNum = arg->getLineNumber();
                    throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                             ": readln argument must be a variable");
                }
            }
            if (fileInput && visitor.isReg(fileSlot) && !visitor.isParmReg(fileSlot))
                visitor.freeReg(fileSlot);
        } else if (funcName == "seed_random") {
            visitor.usedModules.insert("io");
            if (arguments.size() != 0)
                throw std::runtime_error("seed_random requires 0 arguments");
            visitor.emit_invoke("seed_random", {});
        }
    }

    bool IOFunctionHandler::generateWithResult(CodeGenVisitor &visitor, const std::string &funcName_,
                                               const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        auto funcName = toLower(funcName_);
        if (funcName == "seed_random") {
            visitor.usedModules.insert("io");
            if (arguments.size() != 0)
                throw std::runtime_error("seed_random requires 0 arguments");
            std::string dst = visitor.allocReg();
            visitor.emit_invoke("seed_random", {});
            visitor.emit("return " + dst);
            visitor.pushValue(dst);
            return true;
        } else if (funcName == "rand_number") {
            visitor.usedModules.insert("io");
            if (arguments.size() != 1)
                throw std::runtime_error("rand_number requires 1 argument (size)");
            std::string arg = visitor.eval(arguments[0].get());
            std::string dst = visitor.allocReg();
            visitor.emit_invoke("rand_number", {arg});
            visitor.emit("return " + dst);
            if (visitor.isReg(arg) && !visitor.isParmReg(arg))
                visitor.freeReg(arg);
            visitor.pushValue(dst);
            return true;
        }
        return false;
    }

    bool StdFunctionHandler::canHandle(const std::string &funcName) const {
        return stdFunctions.find(toLower(funcName)) != stdFunctions.end();
    }

    void StdFunctionHandler::generate(CodeGenVisitor &visitor, const std::string &funcName,
                                      const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        visitor.usedModules.insert("std");

        int lineNum = arguments.empty() ? 1 : arguments[0]->getLineNumber();

        std::vector<std::string> args;
        for (auto &arg : arguments) {
            args.push_back(visitor.eval(arg.get()));
        }

        if (funcName == "srand") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": srand requires 1 argument");
            visitor.emit_invoke("srand", args);
        } else if (funcName == "free") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": free requires 1 argument");
            visitor.emit_invoke("release", args);
        } else if (funcName == "halt") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": halt requires 1 argument");
            visitor.emit("exit " + args[0]);
        } else if (funcName == "inc") {
            if (arguments.size() < 1 || arguments.size() > 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": inc requires 1 or 2 arguments");
            std::string amount = (arguments.size() == 2) ? args[1] : "1";
            visitor.emit2("add", args[0], amount);
        } else if (funcName == "dec") {
            if (arguments.size() < 1 || arguments.size() > 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": dec requires 1 or 2 arguments");
            std::string amount = (arguments.size() == 2) ? args[1] : "1";
            visitor.emit2("sub", args[0], amount);
        } else if (funcName == "include") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": include requires 2 arguments (set, element)");
            std::string setPtr = visitor.ensurePtrBase(args[0]);
            std::string oneReg = visitor.allocReg();
            visitor.emit2("mov", oneReg, "1");
            visitor.emit4("store", oneReg, setPtr, args[1], "8");
            visitor.freeReg(oneReg);
        } else if (funcName == "exclude") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": exclude requires 2 arguments (set, element)");
            std::string setPtr = visitor.ensurePtrBase(args[0]);
            std::string zeroReg = visitor.allocReg();
            visitor.emit2("mov", zeroReg, "0");
            visitor.emit4("store", zeroReg, setPtr, args[1], "8");
            visitor.freeReg(zeroReg);
        }

        for (const std::string &arg : args) {
            if (visitor.isReg(arg))
                visitor.freeReg(arg);
        }
    }

    bool StdFunctionHandler::generateWithResult(CodeGenVisitor &visitor, const std::string &funcName,
                                                const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        visitor.usedModules.insert("std");

        int lineNum = arguments.empty() ? 1 : arguments[0]->getLineNumber();

        std::unordered_set<std::string> floatMathFuncs = {
            "sin", "cos", "tan", "asin", "acos", "atan", "sinh", "cosh", "tanh",
            "exp", "log", "log10", "sqrt", "pow", "fmod", "ceil", "floor", "fabs",
            "round", "trunc", "hypot"};

        if (floatMathFuncs.find(funcName) != floatMathFuncs.end()) {
            int requiredArgs = (funcName == "pow" || funcName == "fmod" || funcName == "hypot") ? 2 : 1;
            if (static_cast<int>(arguments.size()) != requiredArgs)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": " + funcName + " requires " + std::to_string(requiredArgs) + " argument(s)");

            std::vector<std::string> args;
            args.reserve(arguments.size());

            for (const auto &argNode : arguments) {
                std::string argLocation = visitor.eval(argNode.get());
                auto argType = visitor.getExpressionType(argNode.get());

                if (argType != VarType::DOUBLE) {
                    std::string floatReg = visitor.allocFloatReg();
                    visitor.emit2("to_float", floatReg, argLocation);
                    if (visitor.isReg(argLocation))
                        visitor.freeReg(argLocation);
                    args.push_back(floatReg);
                } else {
                    if (!visitor.isFloatReg(argLocation)) {
                        std::string floatReg = visitor.allocFloatReg();
                        visitor.emit2("mov", floatReg, argLocation);
                        args.push_back(floatReg);
                    } else {
                        args.push_back(argLocation);
                    }
                }
            }

            visitor.emit_invoke(funcName, args);
            std::string resultReg = visitor.allocFloatReg();
            visitor.emit("return " + resultReg);
            visitor.pushValue(resultReg);
            for (const std::string &fr : args) {
                if (visitor.isFloatReg(fr)) {
                    visitor.freeFloatReg(fr);
                }
            }
            return true;
        } else if (funcName == "abs") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": abs requires 1 argument");
            std::string a = visitor.eval(arguments[0].get());

            std::string resultReg = visitor.allocReg();
            visitor.emit_invoke("abs", {a});
            visitor.emit("return " + resultReg);
            visitor.pushValue(resultReg);
            if (visitor.isReg(a))
                visitor.freeReg(a);
            return true;
        } else if (funcName == "rand") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": rand requires 0 arguments");
            visitor.emit_invoke("rand", {});
            std::string resultReg = visitor.allocReg();
            visitor.emit("return " + resultReg);
            visitor.pushValue(resultReg);
            return true;
        } else if (funcName == "float" || funcName == "real") {
            if (arguments.size() != 1) {
                throw std::runtime_error("float/real expects one argument");
            }
            std::string argLocation = visitor.eval(arguments[0].get());

            auto argType = visitor.getExpressionType(arguments[0].get());
            if (argType == VarType::DOUBLE) {
                visitor.pushValue(argLocation);
                return true;
            }

            std::string floatReg = visitor.allocFloatReg();
            visitor.emit2("to_float", floatReg, argLocation);
            visitor.pushValue(floatReg);
            if (visitor.isReg(argLocation))
                visitor.freeReg(argLocation);
            return true;
        } else if (funcName == "ord") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": ord requires 1 argument");
            std::string a = visitor.eval(arguments[0].get());
            // ord() is identity for integers/chars/enums — value is already an ordinal
            visitor.pushValue(a);
            return true;
        } else if (funcName == "chr") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": chr requires 1 argument");
            std::string a = visitor.eval(arguments[0].get());
            // chr() is identity — just changes interpretation to char
            visitor.pushValue(a);
            return true;
        } else if (funcName == "succ") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": succ requires 1 argument");
            std::string a = visitor.eval(arguments[0].get());
            std::string r = visitor.allocReg();
            visitor.emit2("mov", r, a);
            visitor.emit2("add", r, "1");
            if (visitor.isReg(a) && !visitor.isParmReg(a))
                visitor.freeReg(a);
            visitor.pushValue(r);
            return true;
        } else if (funcName == "pred") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": pred requires 1 argument");
            std::string a = visitor.eval(arguments[0].get());
            std::string r = visitor.allocReg();
            visitor.emit2("mov", r, a);
            visitor.emit2("sub", r, "1");
            if (visitor.isReg(a) && !visitor.isParmReg(a))
                visitor.freeReg(a);
            visitor.pushValue(r);
            return true;
        }

        return false;
    }

    bool SDLFunctionHandler::canHandle(const std::string &funcName) const {
        return sdlFunctions.count(toLower(funcName)) != 0;
    }

    void SDLFunctionHandler::generate(CodeGenVisitor &visitor, const std::string &funcName, const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        visitor.usedModules.insert("sdl");

        int lineNum = arguments.empty() ? 1 : arguments[0]->getLineNumber();

        std::vector<std::string> args;
        args.reserve(arguments.size());
        for (auto &arg : arguments)
            args.push_back(visitor.eval(arg.get()));

        auto freeArgs = [&]() {
            for (const std::string &a : args)
                if (visitor.isReg(a))
                    visitor.freeReg(a);
        };

        if (funcName == "sdl_init") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_init requires 0 arguments");
            visitor.emit_invoke("init", {});
        } else if (funcName == "sdl_quit") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_quit requires 0 arguments");
            visitor.emit_invoke("quit", {});
        } else if (funcName == "sdl_destroy_window") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_destroy_window requires 1 argument (window_id)");
            visitor.emit_invoke("destroy_window", args);
        } else if (funcName == "sdl_destroy_renderer") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_destroy_renderer requires 1 argument (renderer_id)");
            visitor.emit_invoke("destroy_renderer", args);
        } else if (funcName == "sdl_set_window_title") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_window_title requires 2 arguments (window_id, title)");
            visitor.emit_invoke("set_window_title", args);
        } else if (funcName == "sdl_set_window_position") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_window_position requires 3 arguments (window_id, x, y)");
            visitor.emit_invoke("set_window_position", args);
        } else if (funcName == "sdl_get_window_size") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_window_size requires 3 arguments (window_id, w_var, h_var)");
            visitor.emit_invoke("get_window_size", args);
        } else if (funcName == "sdl_set_window_fullscreen") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_window_fullscreen requires 2 arguments (window_id, fullscreen)");
            visitor.emit_invoke("set_window_fullscreen", args);
        } else if (funcName == "sdl_set_window_icon") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_window_icon requires 2 arguments (window_id, path)");
            visitor.emit_invoke("set_window_icon", args);
        } else if (funcName == "sdl_set_draw_color") {
            if (arguments.size() != 5)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_draw_color requires 5 arguments (renderer_id, r, g, b, a)");
            visitor.emit_invoke("set_draw_color", args);
        } else if (funcName == "sdl_clear") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_clear requires 1 argument (renderer_id)");
            visitor.emit_invoke("clear", args);
        } else if (funcName == "sdl_present") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_present requires 1 argument (renderer_id)");
            visitor.emit_invoke("present", args);
        } else if (funcName == "sdl_draw_point") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_draw_point requires 3 arguments (renderer_id, x, y)");
            visitor.emit_invoke("draw_point", args);
        } else if (funcName == "sdl_draw_line") {
            if (arguments.size() != 5)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_draw_line requires 5 arguments (renderer_id, x1, y1, x2, y2)");
            visitor.emit_invoke("draw_line", args);
        } else if (funcName == "sdl_draw_rect") {
            if (arguments.size() != 5)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_draw_rect requires 5 arguments (renderer_id, x, y, w, h)");
            visitor.emit_invoke("draw_rect", args);
        } else if (funcName == "sdl_fill_rect") {
            if (arguments.size() != 5)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_fill_rect requires 5 arguments (renderer_id, x, y, w, h)");
            visitor.emit_invoke("fill_rect", args);
        } else if (funcName == "sdl_get_mouse_state") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_mouse_state requires 2 arguments (x_var, y_var)");
            visitor.emit_invoke("get_mouse_state", args);
        } else if (funcName == "sdl_get_relative_mouse_state") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_relative_mouse_state requires 2 arguments (x_var, y_var)");
            visitor.emit_invoke("get_relative_mouse_state", args);
        } else if (funcName == "sdl_get_keyboard_state") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_keyboard_state requires 1 argument (numkeys_var)");
            visitor.emit_invoke("get_keyboard_state", args);
        } else if (funcName == "sdl_show_cursor") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_show_cursor requires 1 argument (show)");
            visitor.emit_invoke("show_cursor", args);
        } else if (funcName == "sdl_free_surface") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_free_surface requires 1 argument (surf_ptr)");
            visitor.emit_invoke("free_surface", args);
        } else if (funcName == "sdl_set_clipboard_text") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_clipboard_text requires 1 argument (text)");
            visitor.emit_invoke("set_clipboard_text", args);
        } else if (funcName == "sdl_destroy_texture") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_destroy_texture requires 1 argument (texture_id)");
            visitor.emit_invoke("destroy_texture", args);
        } else if (funcName == "sdl_render_texture") {
            if (arguments.size() != 10)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_render_texture requires 10 arguments (renderer_id, texture_id, src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h)");
            visitor.emit_invoke("render_texture", args);
        } else if (funcName == "sdl_delay") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_delay requires 1 argument (ms)");
            visitor.emit_invoke("delay", args);
        } else if (funcName == "sdl_close_audio") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_close_audio requires 0 arguments");
            visitor.emit_invoke("close_audio", {});
        } else if (funcName == "sdl_pause_audio") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_pause_audio requires 1 argument (pause_on)");
            visitor.emit_invoke("pause_audio", args);
        } else if (funcName == "sdl_free_wav") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_free_wav requires 1 argument (audio_buf)");
            visitor.emit_invoke("free_wav", args);
        } else if (funcName == "sdl_queue_audio") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_queue_audio requires 2 arguments (data, len)");
            visitor.emit_invoke("queue_audio", args);
        } else if (funcName == "sdl_clear_queued_audio") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_clear_queued_audio requires 0 arguments");
            visitor.emit_invoke("clear_queued_audio", {});
        } else if (funcName == "sdl_update_texture") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_update_texture requires 3 arguments (texture_id, pixels, pitch)");
            visitor.emit_invoke("update_texture", args);
        } else if (funcName == "sdl_unlock_texture") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_unlock_texture requires 1 argument (texture_id)");
            visitor.emit_invoke("unlock_texture", args);
        } else if (funcName == "sdl_init_text") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_init_text requires 0 arguments");
            visitor.emit_invoke("init_text", {});
        } else if (funcName == "sdl_quit_text") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_quit_text requires 0 arguments");
            visitor.emit_invoke("quit_text", {});
        } else if (funcName == "sdl_draw_text") {
            if (arguments.size() != 9)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_draw_text requires 9 arguments (renderer_id, font_id, text, x, y, r, g, b, a)");
            visitor.emit_invoke("draw_text", args);
        } else if (funcName == "sdl_set_render_target") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_set_render_target requires 2 arguments (renderer_id, target_id)");
            visitor.emit_invoke("set_render_target", args);
        } else if (funcName == "sdl_destroy_render_target") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_destroy_render_target requires 1 argument (target_id)");
            visitor.emit_invoke("destroy_render_target", args);
        } else if (funcName == "sdl_present_scaled") {
            if (arguments.size() != 4)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_present_scaled requires 4 arguments (renderer_id, target_id, src_width, src_height)");
            visitor.emit_invoke("present_scaled", args);
        } else if (funcName == "sdl_present_stretched") {
            if (arguments.size() != 4)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_present_stretched requires 4 arguments (renderer_id, target_id, dst_width, dst_height)");
            visitor.emit_invoke("present_stretched", args);
        } else if (funcName == "sdl_lock_texture") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_lock_texture requires 3 arguments (texture_id, pixels_var, pitch_var)");
            visitor.emit_invoke("lock_texture", args);
        }

        freeArgs();
    }

    bool SDLFunctionHandler::generateWithResult(CodeGenVisitor &visitor, const std::string &funcName, const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        visitor.usedModules.insert("sdl");

        int lineNum = arguments.empty() ? 1 : arguments[0]->getLineNumber();

        std::vector<std::string> args;
        args.reserve(arguments.size());
        for (auto &arg : arguments)
            args.push_back(visitor.eval(arg.get()));

        auto freeArgs = [&]() {
            for (const std::string &a : args)
                if (visitor.isReg(a))
                    visitor.freeReg(a);
        };

        auto emitCallWithReturn = [&](const std::string &name) -> std::string {
            std::string dst = visitor.allocReg();
            visitor.emit_invoke(name, args);
            visitor.emit("return " + dst);
            return dst;
        };

        if (funcName == "sdl_init") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_init requires 0 arguments");
            std::string r = emitCallWithReturn("init");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_create_window") {
            if (arguments.size() != 6)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_create_window requires 6 arguments (title, x, y, w, h, flags)");
            std::string r = emitCallWithReturn("create_window");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_create_renderer") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_create_renderer requires 3 arguments (window_id, index, flags)");
            std::string r = emitCallWithReturn("create_renderer");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_poll_event") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_poll_event requires 0 arguments");
            std::string r = emitCallWithReturn("poll_event");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_event_type") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_event_type requires 0 arguments");
            std::string r = emitCallWithReturn("get_event_type");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_key_code") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_key_code requires 0 arguments");
            std::string r = emitCallWithReturn("get_key_code");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_mouse_x") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_mouse_x requires 0 arguments");
            std::string r = emitCallWithReturn("get_mouse_x");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_mouse_y") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_mouse_y requires 0 arguments");
            std::string r = emitCallWithReturn("get_mouse_y");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_mouse_button") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_mouse_button requires 0 arguments");
            std::string r = emitCallWithReturn("get_mouse_button");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_mouse_buttons") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_mouse_buttons requires 0 arguments");
            std::string r = emitCallWithReturn("get_mouse_buttons");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_relative_mouse_x") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_relative_mouse_x requires 0 arguments");
            std::string r = emitCallWithReturn("get_relative_mouse_x");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_relative_mouse_y") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_relative_mouse_y requires 0 arguments");
            std::string r = emitCallWithReturn("get_relative_mouse_y");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_relative_mouse_buttons") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_relative_mouse_buttons requires 0 arguments");
            std::string r = emitCallWithReturn("get_relative_mouse_buttons");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_is_key_pressed") {
            if (arguments.size() != 1)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_is_key_pressed requires 1 argument (scancode)");
            std::string r = emitCallWithReturn("is_key_pressed");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_get_num_keys") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_num_keys requires 0 arguments");
            std::string r = emitCallWithReturn("get_num_keys");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_get_clipboard_text") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_clipboard_text requires 0 arguments");
            std::string r = emitCallWithReturn("get_clipboard_text");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_create_rgb_surface") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_create_rgb_surface requires 3 arguments (width, height, depth)");
            std::string r = emitCallWithReturn("create_rgb_surface");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_blit_surface") {
            if (arguments.size() != 4)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_blit_surface requires 4 arguments (src_ptr, dst_ptr, x, y)");
            std::string r = emitCallWithReturn("blit_surface");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_create_texture") {
            if (arguments.size() != 5)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_create_texture requires 5 arguments (renderer_id, format, access, w, h)");
            std::string r = emitCallWithReturn("create_texture");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_load_texture") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_load_texture requires 2 arguments (renderer_id, file_path)");
            std::string r = emitCallWithReturn("load_texture");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_load_texture_color_key") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_load_texture_color_key requires 2 arguments (renderer_id, file_path)");
            std::string r = emitCallWithReturn("load_texture_color_key");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_load_texture_color_key_rgb") {
            if (arguments.size() != 5)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_load_texture_color_key_rgb requires 5 arguments (renderer_id, file_path, r, g, b)");
            std::string r = emitCallWithReturn("load_texture_color_key_rgb");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_get_ticks") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_ticks requires 0 arguments");
            std::string r = emitCallWithReturn("get_ticks");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_open_audio") {
            if (arguments.size() != 4)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_open_audio requires 4 arguments (freq, format, channels, samples)");
            std::string r = emitCallWithReturn("open_audio");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_load_wav") {
            if (arguments.size() != 4)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_load_wav requires 4 arguments (file_path, audio_buf_var, audio_len_var, audio_spec_var)");
            std::string r = emitCallWithReturn("load_wav");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_get_queued_audio_size") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_get_queued_audio_size requires 0 arguments");
            std::string r = emitCallWithReturn("get_queued_audio_size");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_init_text") {
            if (arguments.size() != 0)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_init_text requires 0 arguments");
            std::string r = emitCallWithReturn("init_text");
            visitor.pushValue(r);
            return true;
        } else if (funcName == "sdl_load_font") {
            if (arguments.size() != 2)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_load_font requires 2 arguments (file, ptsize)");
            std::string r = emitCallWithReturn("load_font");
            visitor.pushValue(r);
            freeArgs();
            return true;
        } else if (funcName == "sdl_create_render_target") {
            if (arguments.size() != 3)
                throw std::runtime_error("Error on line " + std::to_string(lineNum) +
                                         ": sdl_create_render_target requires 3 arguments (renderer_id, width, height)");
            std::string r = emitCallWithReturn("create_render_target");
            visitor.pushValue(r);
            freeArgs();
            return true;
        }

        return false;
    }

    bool StringFunctionHandler::canHandle(const std::string &f) const {
        static const std::unordered_set<std::string> funcs = {
            "length", "pos", "copy", "insert", "delete", "inttostr", "strtoint"};
        return funcs.count(toLower(f)) != 0;
    }

    VarType StringFunctionHandler::getReturnType(const std::string &f_) const {
        auto f = toLower(f_);
        if (f == "length" || f == "pos" || f == "strtoint")
            return VarType::INT;
        if (f == "copy" || f == "insert" || f == "delete" || f == "inttostr")
            return VarType::PTR;
        return VarType::UNKNOWN;
    }

    void StringFunctionHandler::generate(CodeGenVisitor &v,
                                         const std::string &f,
                                         const std::vector<std::unique_ptr<ASTNode>> &args) {
        generateWithResult(v, f, args);
    }

    bool StringFunctionHandler::generateWithResult(CodeGenVisitor &v,
                                                   const std::string &f,
                                                   const std::vector<std::unique_ptr<ASTNode>> &arguments) {

        v.usedModules.insert("string");
        std::vector<std::string> a;
        a.reserve(arguments.size());
        for (auto &n : arguments)
            a.push_back(v.eval(n.get()));

        auto freeArgs = [&]() {
            for (auto &x : a)
                if (v.isReg(x) && !v.isParmReg(x))
                    v.freeReg(x);
        };

        auto emitIntRet = [&](const std::string &name) {
            v.emit_invoke(name, a);
            std::string r = v.allocReg();
            v.emit("return " + r);
            v.pushValue(r);
        };
        auto emitPtrRet = [&](const std::string &name) {
            v.emit_invoke(name, a);
            std::string r = v.allocTempPtr();
            v.emit("return " + r);
            v.pushValue(r);
            v.markAllocatedPtr(r);
        };

        if (f == "length") {
            if (a.size() != 1)
                throw std::runtime_error("length expects 1 arg");
            emitIntRet("strlen");
        } else if (f == "pos") {
            if (a.size() != 2)
                throw std::runtime_error("pos expects 2 args");
            emitIntRet("strfind");
        } else if (f == "strtoint") {
            if (a.size() != 1)
                throw std::runtime_error("strtoint expects 1 arg");
            emitIntRet("strtoint");
        } else if (f == "inttostr") {
            if (a.size() != 1)
                throw std::runtime_error("inttostr expects 1 arg");
            emitPtrRet("inttostr");
        } else if (f == "copy") {
            if (a.size() != 3)
                throw std::runtime_error("copy expects 3 args");
            emitPtrRet("copy");
        } else if (f == "insert") {
            if (a.size() != 3)
                throw std::runtime_error("insert expects 3 args");
            emitPtrRet("insert");
        } else if (f == "delete") {
            if (a.size() != 3)
                throw std::runtime_error("delete expects 3 args");
            emitPtrRet("delete");
        } else {
            freeArgs();
            return false;
        }

        freeArgs();
        return true;
    }

    static std::unordered_set<std::string> fileFunctions = {
        "assign", "reset", "rewrite", "append", "close", "eof", "eoln"};

    bool FileFunctionHandler::canHandle(const std::string &funcName) const {
        return fileFunctions.find(toLower(funcName)) != fileFunctions.end();
    }

    void FileFunctionHandler::generate(CodeGenVisitor &visitor, const std::string &funcName_,
                                       const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        auto funcName = toLower(funcName_);
        visitor.usedModules.insert("io");

        if (arguments.empty())
            throw std::runtime_error(funcName + " requires at least 1 argument");

        // Get the file variable name from the first argument
        auto *fileVarNode = dynamic_cast<VariableNode *>(arguments[0].get());
        if (!fileVarNode)
            throw std::runtime_error(funcName + " first argument must be a file variable");

        std::string fileVarName = fileVarNode->name;
        std::string mangledFile = visitor.findMangledName(fileVarName);
        std::string fileSlot = visitor.eval(arguments[0].get());

        if (funcName == "assign") {
            if (arguments.size() != 2)
                throw std::runtime_error("assign requires 2 arguments (file, filename)");
            std::string filenameVal = visitor.eval(arguments[1].get());
            // Store filename in companion variable
            auto fnIt = visitor.fileVarNames.find(mangledFile);
            if (fnIt == visitor.fileVarNames.end())
                fnIt = visitor.fileVarNames.find(fileVarName);
            if (fnIt != visitor.fileVarNames.end()) {
                auto slotIt = visitor.varSlot.find(fnIt->second);
                if (slotIt != visitor.varSlot.end())
                    visitor.emit2("mov", visitor.slotVar(slotIt->second), filenameVal);
                else
                    visitor.emit2("mov", fnIt->second, filenameVal);
            }
            if (visitor.isReg(filenameVal) && !visitor.isParmReg(filenameVal))
                visitor.freeReg(filenameVal);
        } else if (funcName == "reset" || funcName == "rewrite" || funcName == "append") {
            // Open file: reset='r', rewrite='w', append='a'
            std::string mode;
            if (funcName == "reset") mode = "r";
            else if (funcName == "rewrite") mode = "w";
            else mode = "a";

            // Get filename from companion variable
            auto fnIt = visitor.fileVarNames.find(mangledFile);
            if (fnIt == visitor.fileVarNames.end())
                fnIt = visitor.fileVarNames.find(fileVarName);
            std::string filenameSlot;
            if (fnIt != visitor.fileVarNames.end()) {
                auto slotIt = visitor.varSlot.find(fnIt->second);
                if (slotIt != visitor.varSlot.end())
                    filenameSlot = visitor.slotVar(slotIt->second);
                else
                    filenameSlot = fnIt->second;
            } else {
                throw std::runtime_error(funcName + ": file variable not found: " + fileVarName);
            }

            // Create mode string constant
            std::string modeLabel = visitor.internString(mode);

            visitor.emit_invoke("fopen", {filenameSlot, modeLabel});
            std::string retReg = visitor.allocReg();
            visitor.emit("return " + retReg);
            // Store the file pointer back into the file variable
            auto fSlotIt = visitor.varSlot.find(mangledFile);
            if (fSlotIt != visitor.varSlot.end())
                visitor.emit2("mov", visitor.slotVar(fSlotIt->second), retReg);
            else
                visitor.emit2("mov", mangledFile, retReg);
            visitor.freeReg(retReg);
        } else if (funcName == "close") {
            visitor.emit_invoke("fclose", {fileSlot});
        }

        if (visitor.isReg(fileSlot) && !visitor.isParmReg(fileSlot))
            visitor.freeReg(fileSlot);
    }

    bool FileFunctionHandler::generateWithResult(CodeGenVisitor &visitor, const std::string &funcName_,
                                                  const std::vector<std::unique_ptr<ASTNode>> &arguments) {
        auto funcName = toLower(funcName_);
        visitor.usedModules.insert("io");

        if (funcName == "eof") {
            if (arguments.size() != 1)
                throw std::runtime_error("eof requires 1 argument (file variable)");
            std::string fileSlot = visitor.eval(arguments[0].get());
            visitor.emit_invoke("feof", {fileSlot});
            std::string dst = visitor.allocReg();
            visitor.emit("return " + dst);
            if (visitor.isReg(fileSlot) && !visitor.isParmReg(fileSlot))
                visitor.freeReg(fileSlot);
            visitor.pushValue(dst);
            return true;
        } else if (funcName == "eoln") {
            // eoln not fully supported, return false as a stub
            if (arguments.size() != 1)
                throw std::runtime_error("eoln requires 1 argument (file variable)");
            std::string dst = visitor.allocReg();
            visitor.emit2("mov", dst, "0");
            visitor.pushValue(dst);
            return true;
        }
        return false;
    }

    VarType FileFunctionHandler::getReturnType(const std::string &funcName) const {
        auto f = toLower(funcName);
        if (f == "eof" || f == "eoln")
            return VarType::INT;
        return VarType::UNKNOWN;
    }

} // namespace pascal