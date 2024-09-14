#include "function.hpp"

namespace token {
    void token_Display(const std::string &command, std::vector<lex::Token> &v) {
    	if(v[1].getTokenType() != lex::TOKEN_DIGIT) {
            stream << "display requires line number.\r\n";
            return;
        }
        std::string index;
        index = v[1].getToken();
        bool found = false;
        int in = atoi(index.c_str());
        for(unsigned int i = 0; i < interp::lines.size(); ++i) {
            if(interp::lines[i].index == in) {
                stream << std::dec << interp::lines[i].index << " " << interp::lines[i].text << "\r\n";
                found = true;
                break;
            }
        }
        if(found == false) {
            stream << "Index: " << in << " not found!\r\n";        }
        
    }  
}
