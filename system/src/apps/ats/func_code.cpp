#include "function.hpp"
#include "code.hpp"

namespace token {
    void token_Code(const std::string &cmd, std::vector<lex::Token> &tokens) {
        if(code.instruct.size()==0) {
            stream << "Error: you must use build command first.\r\n";
            return;
        }
        stream << "Hex: ";
        unsigned int i;
        for(i = 0; i < code.instruct.size(); ++i) {
            std::string hextext = icode::instructionToHex(code.instruct[i]);
            stream << hextext;
        }
        stream << "\r\nBinary: ";
        for(i = 0; i < code.instruct.size(); ++i) {
            std::string bintext = icode::instructionToBinary(code.instruct[i]);
            stream << bintext;
        }
        stream << "\r\n";
    }
}
