#include "symbol.hpp"
#include<sstream>

extern std::ostringstream stream;

namespace symbol {
    
    Symbol::Symbol() {
        var["hex"].create("hex", symbol::Value("0", 0));
    }
    
    Symbol::Symbol(const Symbol &s) : var(s.var) {
        
    }
    Symbol::Symbol(const Symbol &&s) : var(std::move(s.var)) {
        
        
    }
    Symbol &Symbol::operator=(const Symbol &s) {
        var = s.var;
        return *this;
    }
    
    Symbol &Symbol::operator=(const Symbol &&s) {
        var = std::move(s.var);
        return *this;
    }
    
    
    bool Symbol::exisits(const std::string &name) {
        auto it = var.find(name);
        if(it == var.end()) return false;
        return true;
    }
    
    bool Symbol::get(const std::string &name, Variable &v) {
        auto it = var.find(name);
        if(it == var.end()) return false;
        v = it->second;
        return true;
    }
    
    Variable &Symbol::operator[](const std::string &index) {
        return var[index];
    }
    
    void Symbol::list_sorted(std::ostream &out) {
        std::vector<Variable *> var_;
        for(auto i = var.begin(); i != var.end(); ++i) {
            var_.push_back(&i->second);
        }
        std::sort(var_.begin(), var_.end());
        for(int z = 0; z < var_.size(); ++z) {
            stream << *var_[z] << "\r\n";
        }
    }
    
    void Symbol::clear() {
        if(!var.empty()) {
            var.erase(var.begin(), var.end());
        }
    }
    
    bool Symbol::remove(const std::string &n) {
        auto it = var.find(n);
        if(it == var.end()) return false;
        var.erase(it);
        return true;
    }
    
    Variable &Symbol::variable(const std::string &index) {
        return var[index];
    }
    
    std::ostream &operator<<(std::ostream &out, Symbol &s) {
        s.list_sorted(out);
        return out;
    }
}
