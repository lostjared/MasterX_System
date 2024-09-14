#include "function.hpp"
#include<bitset>

namespace token {
    
    bool output_bin(const std::string &text, unsigned int value) {
        if(text == "8") {
            stream << std::bitset<8>(value);
            return true;
        }
        else if(text == "16") {
            stream << std::bitset<16>(value);
            return true;
    	}
        else if(text == "32") {
            stream << std::bitset<32>(value);
            return true;
        }
        return false;
    }
    
    void token_Bin(const std::string &cmd, std::vector<lex::Token> &tokens) {
        if(tokens.size() <= 1) return;
        if(tokens[1].getTokenType() != lex::TOKEN_DIGIT) {
            stream << "Error: command requires bit depth first!\r\n";
            return;
        }
        unsigned int numeric = atoi(tokens[1].getToken().c_str());
        if( (numeric != 8) && (numeric != 16) && (numeric != 32)) {
            stream << "Error: Supported bit depths are 8, 16, and 32.\r\n";
            return;
        }
        for(unsigned int i = 2; i < tokens.size(); ++i) {
            switch(tokens[i].getTokenType()) {
                case lex::TOKEN_DIGIT: {
                    unsigned int num = atoi(tokens[i].getToken().c_str());
                    stream << std::dec << std::setfill('0') << std::setw(8) << num << " = " << std::setfill('0') << std::setw(numeric);
                    output_bin(tokens[1].getToken(), num);
                    stream << "\r\n";
                }
                    break;
                case lex::TOKEN_HEX: {
                    unsigned int num = icode::toHex(tokens[i].getToken());
                    stream << std::dec << std::setfill('0') << std::setw(8) << num << " = " << std::setfill('0') << std::setw(numeric);
                    output_bin(tokens[1].getToken(), num);
                    stream << "\r\n";
                }
                    break;
                case lex::TOKEN_CHAR: {
                    std::string tok = tokens[i].getToken();
                    if(code.symbols.exisits(tok)) {
                        unsigned int num;
                        num = static_cast<unsigned int>(code.symbols[tok].get_double());
                        stream << std::dec << std::setfill('0') << std::setw(8) << num << " = " << std::setfill('0') << std::setw(numeric);
                        output_bin(tokens[1].getToken(), num);
                        stream << "\r\n";
                    } else {
                        stream << "Error: variable: " << tok << " does not exisit!\r\n";
                        return;
                    }
                }
                    break;
                default:
                    break;
                    
            }
            
        }
        
    }
    
}
