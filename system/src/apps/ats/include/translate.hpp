#ifndef __TRANSLATE__H_
#define __TRANSLATE__H_


#include "code.hpp"

namespace translate {
   
    class cExcep {
    public:
        std::string err;
        cExcep(const std::string &text) : err(text) {}
    };
    
    bool build_code();
    bool build_line(unsigned int line);
    void tokenize_line(const std::string &text, std::vector<lex::Token> &tokens);
    void match(const lex::Token &token, const lex::Token_type &type);
    void match(const lex::Token &token, const std::string &text);
    bool confirm_mode(const icode::opc code, unsigned int mode, unsigned char &op_byte);
}

#endif
