#include "function.hpp"

namespace token {
    void token_HTML(const std::string &command, std::vector<lex::Token> &tokens) {
        if(tokens[1].getTokenType() != lex::TOKEN_STRING) {
            stream << "Error command requires string with filename.\r\n";
            return;
        }
        if(code.instruct.size()==0) {
            stream << "Error: Must run build command first.\r\n";
            return;
        }
        std::string fname = tokens[1].getToken();
        std::fstream file;
        file.open(fname, std::ios::out);
        if(!file.is_open()) {
            stream << "Error: file " << fname << " could not be opened for writing.\r\n";
            return;
        }
        file << "<!DOCTYPE html>\r\n<html>\r\n<head><title>ats (Atari Script) - Debug Information</title></head>\r\n<body>\r\n";
        file << "<table border=\"1\" cellpadding=\"3\" cellspacing=\"3\">\r\n";
        file << "<tr><th>Line</th><th>Address Mode</th><th>Instruction</th><th>Machine Code</th></tr>\r\n";
        for(unsigned int i = 0; i < code.instruct.size(); ++i) {
            file << "<tr>";
            file << "<th>" << code.instruct[i].line_num << "</th>";
            file << "<th>" << interp::add_mode[code.instruct[i].mode] << "</th>";
            file << "<th>" << code.instruct[i].text << "</th>";
            file << "<th>" << icode::instructionToHex(code.instruct[i]) << "</th>";
            file << "</tr>\r\n";
        }
        file << "\r\n</table>\r\n";
        file << "\r\n</body>\r\n</html>\r\n";
        file.close();
        stream << "Wrote HTML debug info to file: " << fname << "\r\n";
        
    }
}
