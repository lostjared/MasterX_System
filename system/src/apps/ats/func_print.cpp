#include "function.hpp"
#include "code.hpp"

namespace token {
    
    void token_Print(const std::string &cmd, std::vector<lex::Token> &tokens) {
        if(code.instruct.size()==0) {
            stream << "Error: execute build command first.\r\n";
            return;
        }
        for(unsigned int i =0; i < code.instruct.size(); ++i) {
            stream << code.instruct[i] << "\r\n";
        }
    }
}
