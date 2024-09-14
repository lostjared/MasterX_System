#define VERSION_INFO "1.0"
#include"code.hpp"
#include"function.hpp"
#include<unordered_map>

class ATS_Export {
public:
    ATS_Export() {
        code.symbols["version"].create("version", symbol::Value(VERSION_INFO, 1.0));
        code.symbols["author"].create("author", symbol::Value("lostjared", 0));
    }

    std::string scanATS(std::string input_line) {
            std::istringstream stream_value(input_line);
            lex::Scanner scan(stream_value);
            std::vector<lex::Token> v;
            stream.str("");
            while(scan.valid()) {
                lex::Token s;
                scan >> s;
                if(s.getTokenType() != lex::TOKEN_EOF)
                	v.push_back(s);
            }
            if(v.size()==0) return "No instructions.\n";

            std::string first_token;
            first_token = icode::lcase(v[0].getToken());
            if(first_token == "quit") return "";
            auto rt = function_map.find(first_token);
            if(rt == function_map.end()) {
                interp::inputText(v, input_line);
                if(stream.str().length() == 0) {
                    return "OK ...";
                }
                return stream.str();
            }

            int token_count = v.size()-1;
            if((rt->second.greater == false && token_count == rt->second.args) || (rt->second.args == -1) || (rt->second.greater == true && token_count >= rt->second.args)) rt->second.func(input_line, v);
            else {
                stream << "Error: " << first_token << " requires: " << rt->second.args << " arguments. You gave " << token_count << "\n";
            }
            return stream.str();
    }

};

ATS_Export ats;

std::string scanATS(std::string input) {
    return ats.scanATS(input);
}