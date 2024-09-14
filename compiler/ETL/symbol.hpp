#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <unordered_map>
#include <optional>
#include <string>
#include"ast.hpp"
namespace symbol {


     struct Symbol {
        std::string name;
        std::string value;
        int ivalue;
        double dvalue;
        ast::VarType vtype;
        bool allocated;
        Symbol() : name{}, value{}, ivalue{}, dvalue{}, vtype{ast::VarType::NUMBER}, allocated{false} {}
        Symbol(const Symbol &s) : name{s.name}, value{s.value}, ivalue{s.ivalue}, dvalue{s.dvalue}, vtype{s.vtype}, allocated{s.allocated} {}
        Symbol &operator=(const Symbol &s) {
            name = s.name;
            value = s.value;
            ivalue = s.ivalue;
            dvalue = s.dvalue;
            return *this;
        }
    };

      struct Function {
        std::string functionName;
        std::vector<ast::VarType> argTypes;  // Store argument types
        ast::VarType vtype;
        size_t num_args;
        Function() = default;
        Function(const std::string& name, const std::vector<ast::VarType>& args, ast::VarType retType, size_t num)
            : functionName(name), argTypes(args), vtype(retType), num_args{num} {}
    };


    class SymbolTable {
    public:
        SymbolTable() {
            symbols["global"] = std::unordered_map<std::string, Symbol>();
            cur_scope = &symbols["global"];  // Initialize cur_scope to point to the global scope
        }

        bool is_there(const std::string &sym) {
            auto it = cur_scope->find(sym);
            return (it != cur_scope->end());
        }

        void enter(const std::string &sym) {
            if(cur_scope->find(sym) == cur_scope->end())
                (*cur_scope)[sym] = Symbol();
        }

        void enterScope(const std::string &fname) {
            if (symbols.find(fname) == symbols.end()) {
                symbols[fname] = std::unordered_map<std::string, Symbol>();
            }
            cur_scope = &symbols[fname];
        }
    
        void exitScope() {
            cur_scope = &symbols["global"];
        }

        void enterFunction(const std::string &s, const std::vector<ast::VarType>& args, ast::VarType vtype) {
            if(func.find(s) == func.end()) {
                func[s] = Function(s, args, vtype, args.size());
            }
        }

        std::optional<Function *> lookupFunc(const std::string &f) {
            auto it = func.find(f);
            if(it != func.end()) {
                return &it->second;
            }
            return std::nullopt;
        }

        std::optional<Symbol*> lookup(const std::string &sym) {
            auto it = cur_scope->find(sym);
            if (it != cur_scope->end())
                return &it->second;
            return std::nullopt;
        }
        
        int getCurrentScopeSize() const {
            return cur_scope->size();
        }
        
        auto getSymbols() {
            return symbols;
        }

        // New method to check if a symbol is in memory
        bool isInMemory(const std::string &sym) const {
            return cur_scope->find(sym) != cur_scope->end();
        }

        void print() {
            for(auto &i : symbols) {
                for(auto &z : i.second) {
                    std::cout << z.second.name << " : " << z.second.value << " -> " << static_cast<int>(z.second.vtype) << "\n";
                }
            }
        }

        auto getTable() {
            return symbols;
        }
    private:
        std::unordered_map<std::string, std::unordered_map<std::string, Symbol>> symbols;
        std::unordered_map<std::string, Function> func;
        std::unordered_map<std::string, Symbol> *cur_scope;  // Pointer to the current scope
    };

}

#endif