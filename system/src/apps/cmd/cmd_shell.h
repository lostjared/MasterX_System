#ifndef CMD_SHELL_H
#define CMD_SHELL_H

#include <string>
#include <functional>

std::string executeCmd(const std::string& command);
void initCmdShell();


void setCmdUpdateCallback(std::function<void(const std::string&)> callback);
void setCmdInputCallback(std::function<std::string()> callback);

struct MultiLineState {
    bool needsMoreInput;
    int blockDepth;
    bool lineContinuation;
};

MultiLineState checkMultiLineState(const std::string& input);

#endif
