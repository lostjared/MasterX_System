#include "function.hpp"


namespace token {
    
    void token_Poke(const std::string &command, std::vector<lex::Token> &tokens) {
        
        if(tokens[1].getTokenType() != lex::TOKEN_HEX) {
            stream << "Error: command requires hex value as first argument\r\n";
            return;
        }
        
        std::string var_value = tokens[2].getToken();
        uint32_t val1, val2;
        bool var_set = false;
        
        if(tokens[2].getTokenType() == lex::TOKEN_CHAR) {
            if(!code.symbols.exisits(var_value)) {
                stream << "Error: variable doesn't exisit.\r\n";
                return;
            }
            
            val2 = static_cast<uint32_t>(code.symbols[var_value].get_double());
            var_set = true;
        }
        else if(tokens[2].getTokenType() != lex::TOKEN_HEX) {
            stream << "Error: command requires hex value as second argument\r\n";
            return;
        }
        
        val1 = icode::toHex(tokens[1].getToken());
        
        if(val1 > 0xFFFF) {
            stream << "Error: First argument must be 16 bit value\r\n";
            return;
        }
        
        if(var_set == false) val2 = icode::toHex(tokens[2].getToken());
        
        if(val2 > 0xFF) {
            stream << "Error: second argument must be 8 bit value\r\n";
            return;
        }
        
        code.poke(static_cast<uint16_t>(val1),static_cast<uint8_t>(val2));
    }
}
