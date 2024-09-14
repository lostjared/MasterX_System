#include "function.hpp"

namespace token {
    
    void token_Peek16(const std::string &cmd, std::vector<lex::Token> &tokens) {
        unsigned int address = icode::toHex(tokens[1].getToken());
        if(address > 0xFFFE) {
            stream << "Error: out of memory bounds..\r\n";
            return;
        }
        uint8_t bytes[2] = {code.peek(address),code.peek(address+1)};
        
        stream << "[ $" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << address << ":" << "$" << std::hex << std::uppercase << std::setfill('0') << std::setw(4) << address+1 << " = [ $" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(bytes[0]) << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << static_cast<unsigned int>(bytes[1])  << " ]\r\n";
    }
    
}
