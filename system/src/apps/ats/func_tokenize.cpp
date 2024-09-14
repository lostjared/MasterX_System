#include "function.hpp"

namespace token {
    void token_Tokenize(const std::string &command, std::vector<lex::Token> &tokens) {
        for(unsigned int i = 1; i < tokens.size(); ++i) {
            stream << tokens[i] << "\r\n";
        }
    }
}
