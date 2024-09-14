#include "function.hpp"


namespace token {
    
    void token_Stack(const std::string &cmd, std::vector<lex::Token> &tokens) {
        for(unsigned int i = 0; i < code.stack.size(); ++i) {
            stream << "[$" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << i << "] = $" << std::setw(2) << std::setfill('0')<< std::hex << std::uppercase << static_cast<unsigned int>(code.stack[i]) <<  "\r\n";
            
        }
    }
    
}
