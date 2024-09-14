#include "function.hpp"

namespace token {
    void token_Remove(const std::string &command, std::vector<lex::Token> &v) {
        if(v[1].getTokenType() != lex::TOKEN_DIGIT) {
            stream << "remove requires line number.\r\n";
            return;
        }
        std::string index;
        index = v[1].getToken();
        int in = atoi(index.c_str());
        bool found = false;
        for(unsigned int i = 0; i < interp::lines.size(); ++i) {
            if(interp::lines[i].index == in) {
                interp::lines.erase(interp::lines.begin()+i);
                found = true;
                break;
            }
        }
        if(found == true)
            stream << "Line: " << in << " removed..\r\n";
        else
            stream << "Line: " << in << " not found..\r\n";
        
    }
}
