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
    
    class Exception {
    public:
        Exception(const std::string &text) : data{text} {}
        std::string why () const { return data; }
    private:
        std::string data;
    };

    class Interpreter {
    public:
        Interpreter(symbol::SymbolTable &table);
        int execute(ir::IRCode &code);
        void outputDebugInfo(std::ostream &out);

    private:
        symbol::SymbolTable &sym_tab;
        long ip = 0;
        std::string curFunction;
        std::unordered_map<std::string, std::unordered_map<std::string, long>> numeric_variables;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> string_variables;
        std::unordered_map<std::string, long> label_pos;

        void collectLabels(const ir::IRCode &code);

        void executeAdd(const ir::IRInstruction &instr);
        void executeSub(const ir::IRInstruction &instr);
        void executeMul(const ir::IRInstruction &instr);
        void executeDiv(const ir::IRInstruction &instr);
        void executeJump(const ir::IRInstruction &instr);
        void executeLoadConst(const ir::IRInstruction &instr);
        void executeSetConst(const ir::IRInstruction &instr);
        void executeLabel(const ir::IRInstruction &instr);
        long getIntegerValue(const std::string &operand);



    };
}


#endif