#include "function.hpp"
#include "code.hpp"

namespace token {
    
    void token_Insert(const std::string &text, std::vector<lex::Token> &tokens) {
        if(tokens.size()>=3) {
            if(tokens[1].getTokenType() != lex::TOKEN_HEX) {
                stream << "Error: start address must be hex value.\r\n";
                return;
            }
            uint32_t start_address = icode::toHex(tokens[1].getToken());
            unsigned int i = 0;
            for(i = 2; i < tokens.size(); ++i) {
                if(tokens[i].getTokenType() == lex::TOKEN_CHAR) {
                    if(!code.symbols.exisits(tokens[i].getToken())) {
                        stream << "Error: Symbol: " << tokens[i].getToken() << " not found in table..\r\n";
                        return;
                    }
                }
                else if(tokens[i].getTokenType() != lex::TOKEN_HEX) {
                    stream << "Error: list of values must be either symbol or hex value.\r\n";
                    return;
                }
            }
            for(i = 2; i < tokens.size(); ++i) {
                uint32_t address = start_address+(i-2);
                if(address > 0xFFFF) {
                    stream << "Error: memory out of bounds..\r\n";
                    return;
                }
                uint8_t hexValue = 0;
                if(tokens[i].getTokenType() == lex::TOKEN_CHAR)
                    hexValue = static_cast<uint8_t>(code.symbols[tokens[i].getToken()].get_double());
                else
                    hexValue = static_cast<uint8_t>(icode::toHex(tokens[i].getToken()));
                
                stream << "[ $" << std::setfill('0') << std::setw(4) << std::hex << std::uppercase << address << " ] = $" << std::setfill('0') << std::setw(2) << std::hex << std::uppercase << static_cast<uint32_t>(hexValue) << "\r\n";
                
                code.poke(static_cast<uint16_t>(address),hexValue);
            }
        } else {
            stream << "Error: requires starting address followed by values\r\n";
            return;
        }
    }
}
