#include "cmd_shell.h"
#include "scanner.hpp"
#include "parser.hpp"
#include "ast.hpp"
#include "command.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>

static bool cmd_initialized = false;
static bool last_command_succeeded = true;

void initCmdShell() {
    if (cmd_initialized) return;
    cmd::AstExecutor::getExecutor();
    cmd_initialized = true;
}

void setCmdUpdateCallback(std::function<void(const std::string&)> callback) {
    cmd::AstExecutor &executor = cmd::AstExecutor::getExecutor();
    executor.setUpdateCallback(callback);
#if defined(__EMSCRIPTEN__)
    setWasmRenderCallback(callback);
#endif
}

void setCmdInputCallback(std::function<std::string()> callback) {
    cmd::AstExecutor &executor = cmd::AstExecutor::getExecutor();
    executor.setInputCallback(callback);
}

static int countWord(const std::string& input, const std::string& word) {
    int count = 0;
    size_t pos = 0;
    while ((pos = input.find(word, pos)) != std::string::npos) {
        // Check word boundary before
        bool validStart = (pos == 0 || !std::isalnum(input[pos - 1]));
        // Check word boundary after
        bool validEnd = (pos + word.length() >= input.length() || 
                        !std::isalnum(input[pos + word.length()]));
        if (validStart && validEnd) {
            count++;
        }
        pos += word.length();
    }
    return count;
}

static bool endsWithBackslash(const std::string& input) {
    if (input.empty()) return false;
    // Find last non-whitespace character
    size_t lastNonSpace = input.find_last_not_of(" \t\n\r");
    if (lastNonSpace != std::string::npos && input[lastNonSpace] == '\\') {
        return true;
    }
    return false;
}

MultiLineState checkMultiLineState(const std::string& input) {
    MultiLineState state;
    state.needsMoreInput = false;
    state.blockDepth = 0;
    state.lineContinuation = false;
    
    if (input.empty()) {
        return state;
    }
    
    if (endsWithBackslash(input)) {
        state.lineContinuation = true;
        state.needsMoreInput = true;
        return state;
    }
    int forCount = countWord(input, "for");
    int whileCount = countWord(input, "while");
    int ifCount = countWord(input, "if");
    int defineCount = countWord(input, "define");
    
    int doneCount = countWord(input, "done");
    int fiCount = countWord(input, "fi");
    int endCount = countWord(input, "end");
    int loopDepth = (forCount + whileCount) - doneCount;
    int ifDepth = ifCount - fiCount;
    int defineDepth = defineCount - endCount;
    
    state.blockDepth = loopDepth + ifDepth + defineDepth;
    
    if (state.blockDepth > 0) {
        state.needsMoreInput = true;
    }
    
    return state;
}

std::string getCommandName(const std::string& command) {
    std::string trimmed = command;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    trimmed = trimmed.substr(start);
    size_t end = trimmed.find_first_of(" \t\n\r");
    if (end == std::string::npos) {
        return trimmed;
    }
    return trimmed.substr(0, end);
}

bool isCommandRegistered(const std::string& cmdName) {
    cmd::AstExecutor &executor = cmd::AstExecutor::getExecutor();
    return executor.getCommandRegistry().commandExists(cmdName);
}

bool looksLikeStatement(const std::string& command) {
    size_t eq_pos = command.find('=');
    if (eq_pos != std::string::npos) {
        return true;
    }
    
    std::string trimmed = command;
    size_t start = trimmed.find_first_not_of(" \t\n\r");
    if (start != std::string::npos) {
        trimmed = trimmed.substr(start);
        if (trimmed.find("for ") == 0 || trimmed.find("while ") == 0 || 
            trimmed.find("if ") == 0 || trimmed.find("function ") == 0 ||
            trimmed.find("define ") == 0 || trimmed.find("return ") == 0 || 
            trimmed.find("break") == 0 || trimmed.find("continue") == 0) {
            return true;
        }
    }
    
    return false;
}

std::string executeCmd(const std::string& command) {
    if (!cmd_initialized) {
        initCmdShell();
    }    
    if (command.empty()) {
        return "";
    }
    
    std::ostringstream out_stream;
    std::ostringstream err_stream;
    std::streambuf* original_cerr = std::cerr.rdbuf();
    std::cerr.rdbuf(err_stream.rdbuf());
    
    std::string result;
    last_command_succeeded = true;
    
    try {
        std::stringstream input_stream(command);
        scan::TString string_buffer(command);
        scan::Scanner scanner(string_buffer);
        cmd::Parser parser(scanner);
        auto ast = parser.parse();
        cmd::AstExecutor &executor = cmd::AstExecutor::getExecutor();
        executor.execute(input_stream, out_stream, ast);
        std::cerr.flush();
        result = out_stream.str() + err_stream.str();
    } catch(const scan::ScanExcept &e) {
        std::cerr.flush();
        last_command_succeeded = false;
        result = out_stream.str() + err_stream.str() + "Scanner Error: " + e.why() + "\n";
    } catch(const cmd::Exit_Exception &e) {
        std::cerr.flush();
        if (e.getCode() == 0) {
            result = out_stream.str() + err_stream.str();
        } else {
            result = out_stream.str() + err_stream.str() + "Exit with code: " + std::to_string(e.getCode()) + "\n";
        }
    } catch(const std::runtime_error &e) {
        std::cerr.flush();
        last_command_succeeded = false;
        result = out_stream.str() + err_stream.str() + "Runtime Error: " + std::string(e.what()) + "\n";
    } catch(const std::exception &e) {
        std::cerr.flush();
        last_command_succeeded = false;
        result = out_stream.str() + err_stream.str() + "Error: " + std::string(e.what()) + "\n";
    } catch(const cmd::AstFailure &e) {
        std::cerr.flush();
        last_command_succeeded = false;
        result = out_stream.str() + err_stream.str() + "Failure: " + std::string(e.what()) + "\n";
    } catch(...) {
        std::cerr.flush();
        last_command_succeeded = false;
        result = out_stream.str() + err_stream.str() + "Unknown error occurred\n";
    }
    
    std::cerr.rdbuf(original_cerr);
    if (result.empty() && last_command_succeeded && !looksLikeStatement(command)) {
        std::string cmdName = getCommandName(command);
        if (!cmdName.empty() && !isCommandRegistered(cmdName)) {
            return "Command not found: " + cmdName + "\n";
        }
    }
    
    return result;
}
