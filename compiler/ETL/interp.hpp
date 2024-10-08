#ifndef _INTERP_H__
#define _INTERP_H__

#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <regex>
#include <set>
#include "types.hpp"
#include "ir.hpp"
#include "symbol.hpp"
#include "clib.hpp"

namespace interp {

    class Interpreter {
    public:
        Interpreter(symbol::SymbolTable &table);
        int execute(ir::IRCode &code);

    private:
        symbol::SymbolTable &sym_tab;
        long ip = 0;
        std::unordered_map<std::string, long> numeric_variables;
        std::unordered_map<std::string, std::string> string_variables;

        void executeAdd(const ir::IRInstruction &instr);
        void executeSub(const ir::IRInstruction &instr);
        void executeJump(const ir::IRInstruction &instr);


        long getIntegerValue(const std::string &operand);

    };
}


#endif