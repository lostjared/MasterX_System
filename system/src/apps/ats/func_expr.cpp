#include "function.hpp"
#include "parser.hpp"


namespace token {
    
    void token_Expression(const std::string &t, std::vector<lex::Token> &tokens) {
        std::string text = t;
        if(tokens.size() <= 1) {
            stream << "Error: requires an numeric expression.\r\n";
            return;
        }
        if(text.find(";") == std::string::npos) {
            stream << "Error: Expression is terminated with semi-colon\r\n";
            return;
        }
        text = text.substr(5, text.length()-5);
        std::istringstream stream(text);
        lex::Parser p(stream);
        p.eval();
    }
}
