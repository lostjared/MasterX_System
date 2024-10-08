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


    private:
        symbol::SymbolTable &sym_tab;
        int ip = 0;
    };
}


#endif