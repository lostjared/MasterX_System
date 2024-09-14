#include "function.hpp"

namespace token {
    void token_Clear(const std::string &command, std::vector<lex::Token> &v) {
        if(!interp::lines.empty()) {
            interp::lines.erase(interp::lines.begin(), interp::lines.end());
        }
        
        if(!interp::label_table.empty())
            interp::label_table.erase(interp::label_table.begin(), interp::label_table.end());
        if(!interp::label_line_table.empty())
            interp::label_line_table.erase(interp::label_line_table.begin(), interp::label_line_table.end());
        
        code.reset();
    }
}
