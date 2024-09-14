#include "function.hpp"

interp::Code code;

std::unordered_map<std::string, Command> function_map = {
    { "convert",Command(token::token_Convert, 0, "Convert stored code to tokens \r\nUse:\r\nconvert") },
    { "list",Command(token::token_List, 0, "List source code\r\nUse:\r\nlist") },
    { "save",Command(token::token_Save, 1, "Save stored code\r\nUse:\r\nsave \"filename\"") },
    { "open",Command(token::token_Open, 1, "Open code from file\r\nUse:\r\nopen \"filename\"") },
    { "clear",Command(token::token_Clear, 0, "Clear all lines\r\nUse:\r\nclear")},
    { "remove",Command(token::token_Remove, 1, "Remove individual line argument 1 is line number\r\nUse:\r\nremove index") },
    { "display",Command(token::token_Display, 1, "Display indivdual line\r\nUse:\r\ndisplay line") },
    { "symbols",Command(token::token_DisplaySymbols, 0, "Print stored shell variable symbols\r\nUse:\r\nsymbols") },
    { "symbol", Command(token::token_DisplaySymbol, 1, "Print indivdual symbol argument is the name\r\nUse:\r\nsymbol variable") },
    { "unsym", Command(token::token_Unsym, -1, "Remove symbol from table\r\nUse:\r\nunsym variable") },
    { "execute", Command(token::token_Execute, 0, "Execute compiled code\r\nUse:\r\nexecute") },
    { "help", Command(token::token_Help, -1, "Print help message\r\nUse:\r\nhelp") },
    { "sh", Command(token::token_sh, 1, "Execute shell command stored in argument one as a string\r\nUse:\r\nsh \"command\"") },
    { "html", Command(token::token_HTML, 1, "Print code info to HTML\r\nUse:\r\nhtml \"filename\"") },
    { "procinfo", Command(token::token_ProcInfo, 0, "Print registers to console\r\nUse:\r\nprocinfo") },
    { "setreg", Command(token::token_SetReg, 2, "Set register to value\r\nUse:\r\nsetreg register value") },
    { "peek", Command(token::token_Peek, 1, "Show memory location argument one is memory address\r\nUse:\r\npeek address") },
    { "poke", Command(token::token_Poke, 2, "Store value in memory location argument one is memory argument two is value (symbol or constant)\r\nUse:\r\npoke address value") },
    { "setsym", Command(token::token_SetSym, 2, true, "Set symbol value in Symbol Table\r\nUse:\r\nsetsym variable value") },
    { "tokenize", Command(token::token_Tokenize, -1, "Tokenize string\r\nUse:\r\ntokenize string") },
    { "expr", Command(token::token_Expression, -1, "Parse Expression ofor symbols\r\nUse:\r\nexpr expression") },
    { "build", Command(token::token_Build, 0, "Build code\r\nUse:\r\nbuild") },
    { "print", Command(token::token_Print, 0, "Print out information about built code\r\nUse:\r\nprint") },
    { "code", Command(token::token_Code, 0, "Print out Hex values for Machine lanauge\r\nUse:\r\ncode") },
    { "step", Command(token::token_Step, 0, "Make Step in debugger\r\nUse:\r\nstep") },
    { "memory", Command(token::token_Memory, 2, "Print memory range argument one is hex value starting value range argument two is hex value stop value range\r\nUse:\r\nmemory start stop") },
    { "peek16", Command(token::token_Peek16, 1, "Peek 16 bit value from memory.\r\nUse:peek16 address value") },
    { "insert", Command(token::token_Insert, -1, "Insert a stream of bytes at a starting address\r\nUse: insert startaddress val1 val2 val3 ....") },
    { "continue", Command(token::token_Continue, 0, "Continue execution\r\nUse: continue") },
    { "debug", Command(token::token_Debug, 0, "Run program with debug information\r\nUse: debug") },
    { "stack", Command(token::token_Stack, 0, "Display contents of stack\r\nUse: stack") },
    { "bin", Command(token::token_Bin, -1, "Convert to Binary\r\nUse: bin numbers") },
    { "int", Command(token::token_Integer, -1, "Output bin/dec/hex for Integer value\r\nUse: int base values") }
};

namespace token {
    
}
