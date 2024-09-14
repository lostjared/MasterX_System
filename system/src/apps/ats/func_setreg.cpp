#include"function.hpp"


namespace token {
    
    void token_SetReg(const std::string &command, std::vector<lex::Token> &tokens) {
        std::string reg = icode::lcase(tokens[1].getToken());
        uint32_t val;
        if(tokens[2].getTokenType() == lex::TOKEN_HEX)
            val = icode::toHex(tokens[2].getToken());
        else if(tokens[2].getTokenType() == lex::TOKEN_DIGIT)
            val = atoi(tokens[2].getToken().c_str());
        else {
            stream << "Error: setreg requires decimal or hex argument.\r\n";
            return;
        }
        if(val > 255) {
            stream << "Error: x,y,a,p registers are 8 bits.\r\n";
            return;
        }
        
        if(reg == "x")
            code.proc.reg_x = val;
		 else if(reg == "y")
            code.proc.reg_y = val;
        else if(reg == "a")
            code.proc.reg_a = val;
        else if(reg == "p")
            code.proc.setFlags(val);
        
    }
}
