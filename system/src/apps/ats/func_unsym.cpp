#include "function.hpp"

namespace token {
    void token_Unsym(const std::string &command, std::vector<lex::Token> &tokens) {
        if(tokens[1].getTokenType() == lex::TOKEN_CHAR) {
            if(code.symbols.exisits(tokens[1].getToken())) {
                code.symbols.remove(tokens[1].getToken());
            } else {
                stream << "Symbol: " << tokens[1].getToken() << " does not exisit\r\n";
            }
        } else {
            stream << "unsym requires one argument, symbol to remove\r\n";
        }
    }
}
