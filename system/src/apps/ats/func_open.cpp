#include "function.hpp"

namespace token {
    void token_Open(const std::string &command, std::vector<lex::Token> &v) {
        if(v[1].getTokenType() != lex::TOKEN_STRING) {
            stream << "open requires string operand in quotes.\r\n";
            return;
        }
        std::string filename;
        filename = v[1].getToken();
        if(interp::openLineSource(filename)) {
            stream << "Loaded: " << filename << "\r\n";
        } else {
            stream << "Could not load file: " << filename << "\r\n";
        }
    }
}
