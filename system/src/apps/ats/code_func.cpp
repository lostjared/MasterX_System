#include "code.hpp"

namespace interp {
    
    bool saveLineSource(const std::string &text) {
        std::fstream file;
        file.open(text, std::ios::out);
        if(!file.is_open()) return false;
        for(unsigned int i = 0; i < lines.size(); ++i) {
            file << lines[i].index << " " << lines[i].text << "\n";
        }
        file.close();
        return true;
    }
    
    std::string eatComment(std::string src) {
        std::string text;
        for(unsigned int i = 0; i < src.length(); ++i) {
            if(src[i] == ';') return text;
            else
                text += src[i];
        }
        return text;
    }
    
    bool openLineSource(const std::string &text) {
        std::fstream file;
        file.open(text, std::ios::in);
        if(!file.is_open()) return false;
        if(!lines.empty()) {
            lines.erase(lines.begin(), lines.end());
        }
        while(!file.eof() && file) {
            std::string in,orig;
            std::getline(file,orig);
            in = eatComment(orig);
            if(in.length()==0) continue;
            
            std::vector<lex::Token> tokens;
            std::istringstream stream(in);
            lex::Scanner scan(stream);
            while(scan.valid()) {
                lex::Token token;
                scan >> token;
                if(token.getTokenType() != lex::TOKEN_EOF)
                	tokens.push_back(token);
            }
            
            
            inputText(tokens, in);
        }
        file.close();
        return true;
    }
    
    void insertText(std::vector<lex::Token> &tokens, const TextLine &in) {
        
        for(unsigned int i = 0; i < lines.size(); ++i) {
            if(lines[i].index == in.index) {
                if(checkInstruction(tokens, in)) {
                	lines[i].text = in.text;
                	return;
                } else {
                    return;
                }
            }
        }
        if(checkInstruction(tokens, in)) {
        	lines.push_back(in);
        	std::sort(lines.begin(), lines.end());
        }
    }
    
    void inputText(std::vector<lex::Token> &tokens, std::string input_line) {
        if(tokens.size()>=2 && input_line.length()>0) {
            lex::Token_type num = tokens[0].getTokenType();
            if(num != lex::TOKEN_DIGIT) {
                stream << "Requires line number before code.\n";
                return;
            }
            int value = atoi(tokens[0].getToken().c_str());
            std::string codetext;
            codetext = input_line.substr(input_line.find(tokens[0].getToken())+tokens[0].getToken().length()+1, input_line.length());
            TextLine in(value, codetext);
            insertText(tokens, in);
        } else if(tokens.size()==1 && input_line.length()>0) {
            int index_n = atoi(tokens[0].getToken().c_str());
            for(int i = 0; i < lines.size(); ++i) {
                if(lines[i].index == index_n) {
                    lines.erase(lines.begin()+i);
                    return;
                }
            }
        } 
        else {
            stream << "Error invalid input.\n";
            return;
        }
    }
    
    // should have used recursive functions
    // will do for translating
    bool checkInstruction(std::vector<lex::Token> &tokens, const TextLine &text) {
        if(tokens.size()<= 1) {
            stream << "Error: Statement requires instruction.\n";
            return false;
        }
        static unsigned int inc_offset = 0;
        if(tokens.size()>=2) {
            icode::opc op = icode::strtoInc(tokens[1].getToken());
            if(op == icode::opc::NOTINC) {
                if(tokens[1].getTokenType() != lex::TOKEN_CHAR) {
                    stream << "Syntax Error: After line number requires either Label or instruction.\n";
                    return false;
                }
                inc_offset = 1;
                op = icode::strtoInc(tokens[1+inc_offset].getToken());
                if(op == icode::opc::NOTINC) {
                    stream << "Syntax Error: After Label requires Instruction.\n";
                    return false;
                }
            } else {
                // check operands
                if(tokens.size()>=3+inc_offset) {
                    if(tokens[2+inc_offset].getToken()=="#") {
                        if(tokens.size()>=4+inc_offset) {
                            if(tokens[3+inc_offset].getTokenType() == lex::TOKEN_DIGIT) {}
                            else if(tokens[3+inc_offset].getTokenType() != lex::TOKEN_DIGIT && tokens[3+inc_offset].getTokenType() != lex::TOKEN_HEX) {
                                stream << "Syntax Error: requires either digit or $.\n";
                                return false;
                                
                            } else if(tokens[3+inc_offset].getTokenType() != lex::TOKEN_HEX) {
                                stream << "Syntax Error: $ is followed by a Hex value\n";
                                return false;
                            }
                            
                            if(tokens.size()>=5+inc_offset && tokens[4+inc_offset].getToken() == ",") {
                                if(tokens.size()>=6+inc_offset && tokens[5+inc_offset].getTokenType() == lex::TOKEN_CHAR) {
                                    // check if register
                                }
                            } else if(tokens.size()>=5+inc_offset && tokens[4+inc_offset].getToken() != ",") {
                                stream << "Syntax Error: Expecting comma instead I found: " << tokens[4].getToken() << "\n";
                                return false;
                            }
                            
                        } else {
                            stream << "Syntax Error: Missing value after #\n";
                            return false;
                        }
                    }
                } else {
                    return true; // check if inc requires no operand
                }
            
            }
        }
        return true;
    }
    
    bool procLine(const TextLine &text, Code &code) {
        std::istringstream input(text.text);
        lex::Scanner scan(input);
        std::vector<lex::Token> tokens;
        while(scan.valid()) {
            lex::Token token;
            scan >> token;
            if(token.getTokenType() != lex::TOKEN_EOF)
            	tokens.push_back(token);
            
        }
        // for now output each token
        std::cout << "Code Line: ["<<text.index <<"] - [" << text.text << "] = {\n";
        for(unsigned int i = 0; i < tokens.size(); ++i) {
            std::cout << tokens[i] << "\n";
        }
        std::cout << "}\n\n";
        return true;
    }
}
