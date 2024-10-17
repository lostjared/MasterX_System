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
        bool isDefined(const std::string &f);
    private:
        std::string curFunction;
        std::unordered_map<std::string, Function> func;
    };

    struct Var {
        Var() = default;
        Var(const std::string &n, ast::VarType v) : name{n}, numeric_value{0}, ptr_value{nullptr}, type{v} {}
        Var(const std::string &n, const std::string &value) : name{n}, string_value{value}, type{ast::VarType::STRING} {}
        Var(const std::string &n, long value) : name{n}, numeric_value{value}, type{ast::VarType::NUMBER} {}
        Var(const std::string &n, void *ptr) : name{n}, ptr_value{ptr}, type {ast::VarType::POINTER} {}
        Var(const Var &v) : name{v.name}, numeric_value{v.numeric_value}, string_value{v.string_value}, ptr_value{v.ptr_value}, type{v.type} {}
        Var(Var &&v) : name{std::move(v.name)}, numeric_value{v.numeric_value}, string_value{std::move(v.string_value)}, ptr_value{v.ptr_value}, type{v.type} {}

        Var &operator=(const Var &v) {
            name = v.name;
            numeric_value = v.numeric_value;
            string_value = v.string_value;
            ptr_value = v.ptr_value;
            type = v.type;
            return *this;
        }

        Var &operator=(Var &&v) {
            name = std::move(v.name);
            numeric_value = v.numeric_value;
            string_value = std::move(v.string_value);
            ptr_value = v.ptr_value;
            type = v.type;
            return *this;
        }

        std::string name;
        long numeric_value = 0;
        std::string string_value;
        void *ptr_value = nullptr;
        ast::VarType type;
    };

    using FuncPtr =  Var(*)(const std::vector<Var> &vars);

    class Functor {
    public:
        Functor() = default;
        Functor(const std::string &n, FuncPtr fp) : name{n}, f_ptr{fp} {}
        Functor(const Functor &f) : name{f.name}, f_ptr{f.f_ptr}, int_vars{f.int_vars} {}
        Functor(Functor &&f) noexcept : name{std::move(f.name)}, f_ptr{f.f_ptr}, int_vars{std::move(f.int_vars)} {}

        Functor &operator=(const Functor &f) {
            name = f.name;
            f_ptr = f.f_ptr;
            int_vars = f.int_vars;
            return *this;
        }

        Functor &operator=(Functor &&f) noexcept {
            name = std::move(f.name);
            f_ptr = f.f_ptr;
            int_vars = std::move(f.int_vars);
            return *this;
        }

        std::string name;
        std::vector<Var> int_vars;

        Var operator()(const std::vector<Var> &vars) {
            if(f_ptr != nullptr)
                return f_ptr(vars);

            throw Exception("Function: " + name + " Not found!");
        }   
    private:
        FuncPtr f_ptr = nullptr;
    };

    class LibraryFunctions {
    public:
        LibraryFunctions() = default;
        template<typename F>
        void addFunction(std::string name, F value) {
            functions[name] = Functor(name, value);
        }
        void defineInteger(std::string fname, std::string iname) {
            functions[fname].int_vars.push_back(Var(iname, ast::VarType::NUMBER));
        }
        void defineString(std::string fname, std::string sname) {
            functions[fname].int_vars.push_back(Var(sname, ast::VarType::STRING));
        }
        void definePointer(std::string fname, std::string pname) {
            functions[fname].int_vars.push_back(Var(pname, ast::VarType::POINTER));
        }
        Var callFunction(std::string name, const std::vector<Var> &v) {
            return functions[name](v);
        }
        Functor *getFunction(const std::string &n) {
            if(functions.find(n) != functions.end())
                return &functions[n];
            return nullptr;
        }
        void print(std::ostream &out) {
            for(auto &f : functions) {
                out << f.first << " [ ";
                for(size_t i = 0; i < f.second.int_vars.size(); ++i) {
                    out << f.second.int_vars[i].name << ":" << static_cast<int>(f.second.int_vars[i].type) << " ";
                }
                out << " ]\n";
            }
        }

    private:
        std::unordered_map<std::string, Functor> functions;
    };

    class Interpreter {
    public:
        Interpreter(symbol::SymbolTable &table);
        int execute(ir::IRCode &code, bool debug);
        void release();
        void outputDebugInfo(std::ostream &out);
    private:
        symbol::SymbolTable &sym_tab;
        FunctionTable ftable;
        LibraryFunctions lf_table;
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
        void executeConcat(const ir::IRInstruction &instr);

        long getIntegerValue(const std::string &operand);
        std::string stripQuotes(const std::string &value);
        
    };
}


#endif