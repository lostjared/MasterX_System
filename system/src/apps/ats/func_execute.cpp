#include "function.hpp"

namespace token {
    
    void token_Execute(const std::string &command, std::vector<lex::Token> &tokens) {
        if(code.instruct.size()==0) {
            stream << "Error you must build first.\r\n";
            return;
        }
        code.start();
    }
    
}
