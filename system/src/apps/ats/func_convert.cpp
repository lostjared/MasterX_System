#include "function.hpp"

namespace token {
    void token_Convert(const std::string &command, std::vector<lex::Token> &v) {
        code.clear();
        for(unsigned int i = 0; i < interp::lines.size(); ++i) {
            if(procLine(interp::lines[i], code) == false)
                stream << "Line: " << std::dec << interp::lines[i].index << " contains errors.\r\n";
        }
    }
}
