#include "function.hpp"

namespace token {
    void token_DisplaySymbol(const std::string &command, std::vector<lex::Token> &tokens) {
        if(tokens[1].getTokenType() == lex::TOKEN_CHAR) {
            if(code.symbols.exisits(tokens[1].getToken())) {
                stream << code.symbols[tokens[1].getToken()] << "\r\n";
            } else {
                stream << "Error symbol: " << tokens[1].getToken() << " not found\r\n";
            }
        } else {
            stream << "symbol: requires an argument of a valid symbol\r\n";
        }
    }
}
