#include "function.hpp"

namespace token {
    
    void token_Peek(const std::string &command, std::vector<lex::Token> &tokens) {
        if(tokens[1].getTokenType() != lex::TOKEN_HEX) {
            stream << "Error: requires hex constant argumnet.\r\n";
            return;
        }
        uint32_t val;
        val = icode::toHex(tokens[1].getToken());
        if(val > 0xFFFF) {
            stream << "Error: constant must be 16 bit value.\r\n";
            return;
        }
        stream << "[ $" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << val << " ] = $" << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<unsigned int>(code.peek(val)) << " | #" << std::dec << static_cast<unsigned int>(code.peek(val)) << "\r\n";
    }
}
