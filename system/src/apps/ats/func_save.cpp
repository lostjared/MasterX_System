#include "function.hpp"

namespace token {
    void token_Save(const std::string &command, std::vector<lex::Token> &v) {
        if(v[1].getTokenType() != lex::TOKEN_STRING) {
            stream << "Save requires string operand in quotes.\r\n";
            return;
        }
        std::string filename;
        filename = v[1].getToken();
        if(interp::saveLineSource(filename)) {
            stream << "Saved: " << filename << "\r\n";
        }
    }

}
