#include "translate.hpp"
#include "function.hpp"


namespace translate {
    
    bool confirm_mode(const icode::opc code, unsigned int mode, unsigned char &op_byte) {
        for(unsigned int i = 0; interp::m_code[i].p_code != icode::opc::NOTINC; ++i) {
            if(interp::m_code[i].p_code == code && interp::m_code[i].address_mode == mode) {
                op_byte = interp::m_code[i].op_code;
                return true;
            }
        }
        op_byte = 0;
        return false;
    }
    
    bool check_labels() {
        for(unsigned int ix = 0; ix < code.instruct.size(); ++ix) {
            icode::Instruction &i = code.instruct[ix];
            if(i.op1.op_t == icode::op_type::OP_LABELTEXT) {
                auto valid = interp::label_table.find(i.op1.label_text);
                if(valid == interp::label_table.end()) {
                    stream << "Error could not find label: " << i.op1.label_text << "\r\n";
                    return false;
                }
                i.op1.label_index = valid->second;
            } if(i.op1.op_t == icode::op_type::OP_LABEL) {
                auto valid = interp::label_line_table.find(i.op1.op);
                if(valid == interp::label_line_table.end()) {
                    stream << "Error line number: " << i.op1.op << " does not exisit.\r\n";
                    return false;
                }
                i.op1.label_index = valid->second;
            }
        }
        return true;
    }
    
    bool build_code() {
        try {
            if(interp::lines.size()==0) {
                stream << "Error: No code to build...\r\n";
                return false;
            }
            
            if(!interp::label_table.empty())
                interp::label_table.erase(interp::label_table.begin(), interp::label_table.end());
            if(!interp::label_line_table.empty())
                interp::label_line_table.erase(interp::label_line_table.begin(), interp::label_line_table.end());
            
            for(unsigned int i = 0; i < interp::lines.size(); ++i) {
                if(build_line(i)==false) {
                    stream << "Error on line: " << interp::lines[i].index << "\r\n";
                    return false;
                }
            }
            
            if(check_labels() == false)
                return false;
            
            return true;
        }
        catch(const cExcep &e) {
            stream << e.err;
        }
        return false;
    }
    
    bool build_line(unsigned int line) {
        std::vector<lex::Token> tokens;
        unsigned int line_value = interp::lines[line].index;
        interp::label_line_table[line_value] = line;
        tokenize_line(interp::lines[line].text, tokens);
        icode::Instruction inst;
        inst.text = interp::lines[line].text;
        inst.line_num = line_value;
        match(tokens[0], lex::TOKEN_CHAR);
        icode::opc op;
        inst.instruction_text = interp::lines[line].text;
        op = icode::strtoInc(tokens[0].getToken());
        if(op == icode::opc::NOTINC) {
            inst.label = true;
            inst.label_text = tokens[0].getToken();
            inst.label_index = line;
            interp::label_table[inst.label_text] = line;
            match(tokens[1], lex::TOKEN_CHAR);
            icode::opc op_code;
            op_code = icode::strtoInc(tokens[1].getToken());
            if(op_code == icode::opc::NOTINC) {
                std::ostringstream stream;
                stream << "Error: Line: " << line_value << "Expected instruction instead found: " << tokens[1].getToken() << "\r\n";
                throw cExcep(stream.str());
                
            } else {
                inst.opcode = op_code;
            }
        } else {
            inst.opcode = op;
        }
        
        
        if(inst.label == true) {
            tokens.erase(tokens.begin());
        }
        
        unsigned int tok_size = tokens.size()-1;
        
        switch(tok_size) {
            case 0: {
                if(confirm_mode(inst.opcode, interp::IMPLIED, inst.op_byte)==false) {
                    std::ostringstream stream;
                    stream << "Error on Line: " << line_value << " instruction " << icode::op_array[static_cast<unsigned int>(inst.opcode)] << " not valid in implied address mode.\r\n";
                    throw cExcep(stream.str());
                }
                inst.mode = interp::IMPLIED;
            }
                break;
            case 1:
                switch(tokens[1].getTokenType()) {
                    case lex::TOKEN_DIGIT: {
                        
                        if(confirm_mode(inst.opcode, interp::RELATIVE, inst.op_byte) == false) {
                            if(confirm_mode(inst.opcode, interp::ABSOULTE, inst.op_byte) == true) {
                                inst.mode = interp::ABSOULTE;
                            } else {
                                std::ostringstream stream;
                                stream << "Error on Line: " << line_value << " instruction " << inst.opcode << " not supported in relative addressing mode.\r\n";
                                throw cExcep(stream.str());
                            }
                        } else
                        inst.mode = interp::RELATIVE;
                        unsigned int label_value = atoi(tokens[1].getToken().c_str());
                        inst.op1 = icode::Operand(label_value, icode::op_type::OP_LABEL);
                    }
                        break;
                    case lex::TOKEN_HEX: {
                        if(confirm_mode(inst.opcode, interp::ABSOULTE, inst.op_byte) == false) {
                            std::ostringstream stream;
                            stream << "Error on Line: " << line_value << " " << inst.opcode << " not supported in absoulte addressing mode.\r\n";
                            throw cExcep(stream.str());
                        }
                        unsigned int hex_address = icode::toHex(tokens[1].getToken());
                        if(hex_address <= 255) {
                            inst.op1 = icode::Operand(hex_address, icode::op_type::OP_MEMORY);
                            inst.mode = interp::ZEROPAGE;
                            
                        } else {
                            inst.op1 = icode::Operand(hex_address, icode::op_type::OP_MEMORY);
                            inst.mode = interp::ABSOULTE;
                        }
                    }
                        break;
                    case lex::TOKEN_CHAR:
                        // check if label exisits
                        if(tokens[1].getToken()=="A" && confirm_mode(inst.opcode, interp::ACCUMULATOR, inst.op_byte)==true) {
                            inst.op1 = icode::Operand(0, icode::op_type::OP_REGISTER_A);
                            inst.mode = interp::ACCUMULATOR;
                            break;
                        }
                        else
                        if(confirm_mode(inst.opcode, interp::ABSOULTE, inst.op_byte) == false) {
                            
                            if(confirm_mode(inst.opcode, interp::RELATIVE, inst.op_byte) == true) {
                                inst.mode = interp::RELATIVE;
                            } else {
                                
                              
                                
                                std::ostringstream stream;
                                stream << "Error on Line: " << line_value << " instruction " << inst.opcode << " does not support addressing mode.\r\n";
                            }
                        } else {
                            inst.mode = interp::ABSOULTE;
                        }
                        inst.op1 = icode::Operand(0, icode::op_type::OP_LABELTEXT);
                        inst.op1.label_text = tokens[1].getToken();
                        break;
                    default:
                        break;
                }
                
                break;
            case 2:
                
                switch(tokens[1].getTokenType()) {
                    case lex::TOKEN_OPERATOR: {
                        if(tokens[1].getToken() == "#") {
                            
                            unsigned int numeric_value = 0;
                            
                            if(tokens[2].getTokenType() == lex::TOKEN_DIGIT) {
                                numeric_value = atoi(tokens[2].getToken().c_str());
                                
                            } else if(tokens[2].getTokenType() == lex::TOKEN_HEX) {
                                numeric_value = icode::toHex(tokens[2].getToken());
                            } else {
                                std::ostringstream stream;
                                stream << "Error on Line: " << line_value << " Deicmal or Hex value expected..\r\n";
                                throw cExcep(stream.str());
                            }
                            
                            if(confirm_mode(inst.opcode, interp::IMMEDIATE, inst.op_byte) == false) {
                                std::ostringstream stream;
                                stream << "Error on Line: " << line_value << " instruction " << inst.opcode << " not supported in immediate addressing mode.\r\n";
                                throw cExcep(stream.str());
                            }
                            
                            if(numeric_value > 255) {
                                std::ostringstream stream;
                                stream << "Error on Line: " << line_value << " operand is a single byte (no greater than 255).\r\n";
                                throw cExcep(stream.str());
                            }
                            
                            inst.op1 = icode::Operand(numeric_value, icode::op_type::OP_DECIMAL);
                            inst.mode = interp::IMMEDIATE;
                            
                        } else {
                            std::ostringstream stream;
                            stream << "Error on Line: " << line_value << " Expected # operator..\r\n";
                            throw cExcep(stream.str());
                        }
                    }
                        break;
                    default:
                        break;
                }
                break;
            case 3: {
                std::string reg = icode::lcase(tokens[3].getToken());
                switch(tokens[1].getTokenType()) {
                    case lex::TOKEN_HEX: {
                        
                        unsigned int hex_value = icode::toHex(tokens[1].getToken());
                        match(tokens[2], lex::TOKEN_OPERATOR);
                        match(tokens[2], ",");
                        
                        if(tokens[2].getToken() == "," && reg == "x") {
                            
                            if(confirm_mode(inst.opcode, interp::ABSOULTE_X, inst.op_byte)==false) {
                                if(confirm_mode(inst.opcode, interp::ZEROPAGE_X, inst.op_byte)) {
                                    inst.mode = interp::ZEROPAGE_X;
                                } else {
                                    std::ostringstream stream;
                                    stream << "Error on Line: " << line_value << " instruction " << inst.opcode << " has X register but not supported in absoulte X address mode.\r\n";
                                    throw cExcep(stream.str());
                                }
                            }
                            
                            if(hex_value > 0xFF)
                                inst.mode = interp::ABSOULTE_X;
                            else
                                inst.mode = interp::ZEROPAGE_X;
                        }
                        if(tokens[2].getToken() == "," && reg == "y") {
                            if(confirm_mode(inst.opcode, interp::ABSOULTE_Y,inst.op_byte)==false) {
                                if(confirm_mode(inst.opcode, interp::ZEROPAGE_Y, inst.op_byte)) {
                                    inst.mode = interp::ZEROPAGE_Y;
                                } else {
                                    std::ostringstream stream;
                                    stream << "Error on Line: " << line_value << " instruction " << inst.opcode << " has Y register but not supported in absoulte y address mode.\r\n";
                                    throw cExcep(stream.str());
                                }
                            }
                            
                            if(hex_value <= 255)
                                inst.mode = interp::ZEROPAGE_Y;
                            else
                                inst.mode = interp::ABSOULTE_Y;
                            
                        }
                        inst.op1 = icode::Operand(hex_value, icode::op_type::OP_MEMORY);
                    }
                        break;
                    default: {
                        
                        std::ostringstream stream;
                        stream << "Error on Line: " << line_value << " instruction " << inst.opcode << " requires address value.\r\n";
                        throw cExcep(stream.str());
                        
                    }
                        break;
                }
                
            }
                break;
            case 4:
                stream << "4: " << tokens[4] << "\r\n";
                break;
            default:
                stream << "Default: " << tok_size << "\r\n";
                break;
        }
        
        code.instruct.push_back(inst);
        return true;
    }
    
    void tokenize_line(const std::string &text, std::vector<lex::Token> &tokens) {
        std::istringstream stream(text);
        lex::Scanner scan(stream);
        while(scan.valid()) {
            lex::Token token;
            scan >> token;
            if(token.getTokenType() != lex::TOKEN_EOF)
                tokens.push_back(token);
        }
    }
    
    void match(const lex::Token &token, const lex::Token_type &type) {
        if(token.getTokenType() != type) {
            std::ostringstream stream;
            stream << "Expected Type: " << type << "\r\n";
            throw cExcep(stream.str());
        }
    }
    
    void match(const lex::Token &token, const std::string &text) {
        if(token.getToken() != text){
            std::ostringstream stream;
            stream << "Expected: " << text << " found: " << token.getToken() << "\r\n";
            throw cExcep (stream.str());
        }
    }
}
