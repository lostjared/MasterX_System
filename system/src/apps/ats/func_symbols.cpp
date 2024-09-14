#include "function.hpp"

namespace token {
    
    void token_DisplaySymbols(const std::string &command, std::vector<lex::Token> &tokens) {
        stream << "Symbol Table: \r\n" << code.symbols << "\r\n";
    }
    
}
