#include"function.hpp"


namespace token {
    
    void token_Integer(const std::string &cmd, std::vector<lex::Token> &tokens) {
        if(tokens.size() == 1) {
            stream << "Error: Requires values...\r\n";
            return;
        }
        
        for(unsigned int i = 1; i < tokens.size(); ++i) {
            switch(tokens[i].getTokenType()) {
                case lex::TOKEN_DIGIT: {
                    int value = atoi(tokens[i].getToken().c_str());
                    stream << std::dec << value <<  " = [ $" << std::hex << std::uppercase << value << " ]\r\n";
                }
                    break;
                case lex::TOKEN_HEX: {
                    int value = icode::toHex(tokens[i].getToken());
                    stream << "$" << std::hex << std::uppercase << value <<  " = [ " << std::dec << value << " ]\r\n";
                }
                    break;
                default: {
                    stream << "Error requires hex or decimal integers.\r\n";
                    return;
                }
                    break;
            }
            
        }
    }
}
