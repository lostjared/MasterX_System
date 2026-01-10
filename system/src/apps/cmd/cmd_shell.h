#ifndef CMD_SHELL_H
#define CMD_SHELL_H

#include <string>
#include <functional>

std::string executeCmd(const std::string& command);
void initCmdShell();

// Set a callback that will be called during loop iterations to update the display
void setCmdUpdateCallback(std::function<void(const std::string&)> callback);

// Multi-line input support
struct MultiLineState {
    bool needsMoreInput;
    int blockDepth;
    bool lineContinuation;
};

// Check if input needs more lines (incomplete block or line continuation)
MultiLineState checkMultiLineState(const std::string& input);

#endif
