#include"interp.hpp"


namespace interp {

    Interpreter::Interpreter(symbol::SymbolTable &table) : sym_tab{table}, ip{0} {

    }


}