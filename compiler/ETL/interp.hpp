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

    class Exit_Exception {
    public:
        Exit_Exception(int status) : code{status} {}
        int status() const { return code; }
    private:
        int code = 0;
    };

    struct Function {
        std::string functionName;
        std::vector<std::string> arg_names;
        std::vector<ast::VarType> arg_types;
        ast::VarType return_type;
    };

    class FunctionTable {
    public:
        void enterFunction(const std::string &name);
        void addParam(const std::string &name, ast::VarType type);
        Function &getFunction(const std::string &name);
    private:
        std::string curFunction;
        std::unordered_map<std::string, Function> func;
    };

    class Interpreter {
    public:
        Interpreter(symbol::SymbolTable &table);
        int execute(ir::IRCode &code);
        void outputDebugInfo(std::ostream &out);
    private:
        symbol::SymbolTable &sym_tab;
        FunctionTable ftable;
        long ip = 0;

        std::string curFunction;
        std::unordered_map<std::string, std::unordered_map<std::string, long>> numeric_variables;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> string_variables;
        std::unordered_map<std::string, std::unordered_map<std::string, void *>> pointer_variables;
        std::unordered_map<std::string, long> label_pos;
        std::unordered_map<std::string, long> sub_labels;
        struct ReturnValue {
            std::string fname, rt_name;
            long pos;
        };

        std::vector<ReturnValue> call_stack;

        void collectLabels(const ir::IRCode &code);

        void executeAdd(const ir::IRInstruction &instr);
        void executeSub(const ir::IRInstruction &instr);
        void executeMul(const ir::IRInstruction &instr);
        void executeDiv(const ir::IRInstruction &instr);
        void executeMod(const ir::IRInstruction &instr);
        void executeAnd(const ir::IRInstruction &instr);
        void executeOr(const ir::IRInstruction &instr);
        void executeXor(const ir::IRInstruction &instr);
        void executeLShift(const ir::IRInstruction &instr);
        void executeRShift(const ir::IRInstruction &instr);
        void executeJump(const ir::IRInstruction &instr);
        void executeLoadConst(const ir::IRInstruction &instr);
        void executeSetConst(const ir::IRInstruction &instr);
        void executeAssignment(const ir::IRInstruction &instr);
        void executeLoadVar(const ir::IRInstruction &instr);
        void executeSet(const ir::IRInstruction &instr);
        void executeLabel(const ir::IRInstruction &instr);
        void executeNeg(const ir::IRInstruction &instr);
        void executeNot(const ir::IRInstruction &instr);
        void executeLt(const ir::IRInstruction &instr);
        void executeGt(const ir::IRInstruction &instr);
        void executeLte(const ir::IRInstruction &instr);
        void executeGte(const ir::IRInstruction &instr);
        void executeEq(const ir::IRInstruction &instr);
        void executeNeq(const ir::IRInstruction &instr);
        void executeLogicalNot(const ir::IRInstruction &instr);
        void executeLogicalAnd(const ir::IRInstruction &instr);
        void executeLogicalOr(const ir::IRInstruction &instr);
        void executeCall(const ir::IRInstruction &instr);
        void executeReturn(const ir::IRInstruction &instr);
        long getIntegerValue(const std::string &operand);
        void executeConcat(const ir::IRInstruction &instr);

        std::string stripQuotes(const std::string &value);
    };
}


#endif