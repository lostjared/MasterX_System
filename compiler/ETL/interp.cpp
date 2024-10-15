#include"interp.hpp"


namespace interp {

    Interpreter::Interpreter(symbol::SymbolTable &table) : sym_tab{table}, ip{0} {

    }

    int Interpreter::execute(ir::IRCode &code) {
        collectLabels(code);
        if(label_pos.find("init") == label_pos.end()) {
            throw Exception("Error could not find init entry point.\n");
        }
        ip = label_pos["init"];
        while(ip < static_cast<long>(code.size())) {
            const auto instr = code[ip];
            std::cout << instr.toString() << "\n";

            if(instr.type == ir::InstructionType::LABEL) {
                    executeLabel(instr);
                    ip++;
                    continue;
            }

            switch (instr.type) {
                case ir::InstructionType::ADD:
                    executeAdd(instr);
                    break;
                case ir::InstructionType::SUB:
                    executeSub(instr);
                    break;
                case ir::InstructionType::JUMP:
                    executeJump(instr);
                    break;
                case ir::InstructionType::MUL:
                    executeMul(instr);
                    break;
                case ir::InstructionType::DIV:
                    executeDiv(instr);
                    break;
                case ir::InstructionType::MOD:
                    executeMod(instr);
                    break;
                case ir::InstructionType::AND:
                    executeAnd(instr);
                    break;
                case ir::InstructionType::OR:
                    executeOr(instr);
                    break;
                case ir::InstructionType::XOR:
                    executeXor(instr);
                    break;
                case ir::InstructionType::LSHIFT:
                    executeLShift(instr);
                    break;
                case ir::InstructionType::RSHIFT:
                    executeRShift(instr);
                    break;
                case ir::InstructionType::LOAD_CONST:
                    executeLoadConst(instr);
                    break;
                case ir::InstructionType::LOAD_VAR:
                    executeLoadVar(instr);
                    break;
                case ir::InstructionType::SET:
                    executeSet(instr);
                    break;
                case ir::InstructionType::SET_CONST:
                    executeSetConst(instr);
                    break;
                case ir::InstructionType::ASSIGN:
                    executeAssignment(instr);
                    break;
                case ir::InstructionType::NEG:
                    executeNeg(instr);
                    break;
                case ir::InstructionType::NOT:
                    executeNot(instr);
                    break;
                case ir::InstructionType::LOGICAL_NOT:
                    executeLogicalNot(instr);
                    break;
                case ir::InstructionType::LOGICAL_AND:
                    executeLogicalAnd(instr);
                    break;
                case ir::InstructionType::LOGICAL_OR:
                    executeLogicalOr(instr);
                    break;
                case ir::InstructionType::CONCAT:
                    executeConcat(instr);
                    break;
                case ir::InstructionType::PARAM:
                case ir::InstructionType::PARAM_STRING:
                case ir::InstructionType::PARAM_POINTER:
                    break;
                
                case ir::InstructionType::RETURN: {
                    executeReturn(instr);   
                }
                break;
                case ir::InstructionType::CALL: {
                    executeCall(instr);
                }
                break;
                default:
                    std::cerr << "Unsupported instruction: " << instr.toString() << std::endl;
                    break;
            }

            ip ++;
        }
        return EXIT_SUCCESS;
    }

    void Interpreter::collectLabels(const ir::IRCode &code) {
        int ip_id = 0;
        std::string curFunc;
        while(ip_id < code.size()) {
            const auto instr = code[ip_id];
            if(instr.type == ir::InstructionType::LABEL) {
                    label_pos[instr.dest] = ip_id;
                    curFunc = instr.dest;
                    ftable.enterFunction(curFunc);
                    ip_id++;
                    continue;
            } else if(instr.type == ir::InstructionType::PARAM) {
                ftable.addParam(instr.dest, ast::VarType::NUMBER);
            } else if(instr.type == ir::InstructionType::PARAM_STRING) {
                ftable.addParam(instr.dest, ast::VarType::STRING);
            } else if(instr.type == ir::InstructionType::PARAM_POINTER) {
                ftable.addParam(instr.dest, ast::VarType::POINTER);
            }
            ip_id ++;
        }
    }

    void Interpreter::outputDebugInfo(std::ostream &out) {
        out << "Variales [ strings ]\n";
        for(auto &i : string_variables) {
            for(auto &x : i.second) {
                out << i.first << " [ " << x.first << ", " << x.second << "]\n";
            }
        }
        out << "Variales [ numbers ]\n";
        for(auto &i : numeric_variables) {
            for(auto &x : i.second) {
                out << i.first << " [ " << x.first << ", " << x.second << "]\n";
            }

        }
        for(auto &i : label_pos) {
            out << i.second << " = " << i.first << "\n";
        }
    }

    void Interpreter::executeAdd(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 + val2;
    }

    void Interpreter::executeSub(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 - val2;
    }

    void Interpreter::executeMul(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 * val2;
    }

    void Interpreter::executeDiv(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        if(val2 == 0) {
            throw Exception("Divison By Zero");
        }
        numeric_variables[curFunction][instr.dest] = val1 / val2;
    }

    void Interpreter::executeMod(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 % val2;
    }

    void Interpreter::executeAnd(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 & val2;
    
    }

    void Interpreter::executeOr(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 | val2;
    }

    void Interpreter::executeXor(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 ^ val2;
    }

    void Interpreter::executeLShift(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 << val2;
    }

    void Interpreter::executeRShift(const ir::IRInstruction &instr) {
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 >> val2;
    }
    
    void Interpreter::executeLabel(const ir::IRInstruction &instr) {
        curFunction = instr.dest;
    }

    void Interpreter::executeLoadConst(const ir::IRInstruction &instr) {
        if(instr.op1[0] == '\"') {
            sym_tab.enter(instr.dest);
            auto it = sym_tab.lookup(instr.dest);
            if(it.has_value()) {
                symbol::Symbol *s = it.value();
                s->name = instr.dest;
                s->value = instr.op1;
                s->vtype = ast::VarType::STRING;
                string_variables[curFunction][s->name] = s->value;
            }
        } else {
            sym_tab.enter(instr.dest);
            auto it = sym_tab.lookup(instr.dest);
            if(it.has_value()) {
               symbol::Symbol *s = it.value();
               s->name = instr.dest;
               s->value = instr.op1;
               s->vtype = ast::VarType::NUMBER;
               numeric_variables[curFunction][s->name] = std::stol(instr.op1);
            }
        }
    }

    void Interpreter::executeSetConst(const ir::IRInstruction &instr) {
        auto loc = sym_tab.lookup(instr.dest);
        if(instr.op1[0] == '\"') {
            string_variables[curFunction][instr.dest] = instr.op1;
        } else {
            numeric_variables[curFunction][instr.dest] = std::stol(instr.op1);
        }
    }

    void Interpreter::executeAssignment(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc_src = sym_tab.lookup(instr.op1);
        auto loc_dst = sym_tab.lookup(instr.dest);
        if(loc_src.has_value()) {
            if(loc_src.value()->vtype == ast::VarType::STRING) {
                string_variables[curFunction][instr.dest] = string_variables[curFunction][instr.op1];
            } else if(loc_src.value()->vtype == ast::VarType::NUMBER) {
                numeric_variables[curFunction][instr.dest] = numeric_variables[curFunction][instr.op1];
            } else if(loc_src.value()->vtype == ast::VarType::POINTER) {
                pointer_variables[curFunction][instr.dest] =  pointer_variables[curFunction][instr.op1];   
            }
        }
    }
    
    void Interpreter::executeLoadVar(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.op1);
        sym_tab.enter(instr.dest);
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_op1 = sym_tab.lookup(instr.op1);
        if(loc_dest.has_value()) {
            if(loc_op1.value()->vtype == ast::VarType::STRING) {
                string_variables[curFunction][instr.dest] = string_variables[curFunction][instr.op1];
            } else if(loc_op1.value()->vtype == ast::VarType::NUMBER) {
                numeric_variables[curFunction][instr.dest] = numeric_variables[curFunction][instr.op1];
            } else if(loc_op1.value()->vtype == ast::VarType::POINTER) {
                pointer_variables[curFunction][instr.dest] = pointer_variables[curFunction][instr.op1];
            }
        }
    }
    void Interpreter::executeSet(const ir::IRInstruction &instr) {
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value()) {
            if(loc.value()->vtype == ast::VarType::STRING) {
                string_variables[curFunction][instr.dest] = string_variables[curFunction][instr.op1];
            } else if(loc.value()->vtype == ast::VarType::NUMBER) {
                numeric_variables[curFunction][instr.dest] = numeric_variables[curFunction][instr.op1];
            } else if(loc.value()->vtype == ast::VarType::POINTER) {
                pointer_variables[curFunction][instr.dest] = pointer_variables[curFunction][instr.op1];
            }
        }
    }
    
    void Interpreter::executeNeg(const ir::IRInstruction  &instr) {
        sym_tab.enter(instr.dest);      
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_src = sym_tab.lookup(instr.op1);
        if(loc_dest.has_value() && loc_src.has_value() && loc_src.value()->vtype == ast::VarType::NUMBER) {
            numeric_variables[curFunction][instr.dest] = -numeric_variables[curFunction][instr.op1];
            loc_dest.value()->vtype = ast::VarType::NUMBER;
        } else {
            std::ostringstream stream;
            stream << "Neg Requires NUMBER variable: " << instr.op1 << "\n";
            throw Exception(stream.str());
        }
    }

    void Interpreter::executeNot(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);      
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_src = sym_tab.lookup(instr.op1);
        if(loc_dest.has_value() && loc_src.has_value() && loc_src.value()->vtype == ast::VarType::NUMBER) {
            numeric_variables[curFunction][instr.dest] = ~numeric_variables[curFunction][instr.op1];
            loc_dest.value()->vtype = ast::VarType::NUMBER;
        } else {
            std::ostringstream stream;
            stream << "Not Requires NUMBER variable: " << instr.op1 << "\n";
            throw Exception(stream.str());
        }
    }

    void Interpreter::executeLogicalNot(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);      
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_src = sym_tab.lookup(instr.op1);
        if(loc_dest.has_value() && loc_src.has_value()) {
            numeric_variables[curFunction][instr.dest] = !numeric_variables[curFunction][instr.op1];
        }
    }

    void Interpreter::executeLogicalAnd(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_op1 = sym_tab.lookup(instr.op1);
        auto loc_op2 = sym_tab.lookup(instr.op2);

        if(loc_op1.has_value() && loc_op2.has_value()) {
            if(loc_op1.value()->vtype == ast::VarType::NUMBER && loc_op2.value()->vtype == ast::VarType::NUMBER)
                numeric_variables[curFunction][instr.dest] = numeric_variables[curFunction][instr.op1] && numeric_variables[curFunction][instr.op2];
            else {
                std::ostringstream stream;
                stream << "Incorrect Variable type for Logical And &&: " << instr.op1 << " && " << instr.op2 << "\n";
                throw Exception(stream.str());    
            }

        } else {
            std::ostringstream stream;
            stream << "Undefined variable in " << instr.op1 << " && " << instr.op2;
            throw Exception(stream.str());
        }
    }
    void Interpreter::executeLogicalOr(const ir::IRInstruction &instr) {
       sym_tab.enter(instr.dest);
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_op1 = sym_tab.lookup(instr.op1);
        auto loc_op2 = sym_tab.lookup(instr.op2);

        if(loc_op1.has_value() && loc_op2.has_value()) {

            if(loc_op1.value()->vtype == ast::VarType::NUMBER && loc_op2.value()->vtype == ast::VarType::NUMBER)
                numeric_variables[curFunction][instr.dest] = numeric_variables[curFunction][instr.op1] || numeric_variables[curFunction][instr.op2];
            else {
                std::ostringstream stream;
                stream << "Incorrect Variable type for Logical Or ||: " << instr.op1 << " || " << instr.op2 << "\n";
                throw Exception(stream.str());
            }

        } else {
            std::ostringstream stream;
            stream << "Undefined variable in " << instr.op1 << " || " << instr.op2;
            throw Exception(stream.str());
        }
    }
        

    void Interpreter::executeCall(const ir::IRInstruction &instr) {
        Function &func = ftable.getFunction(instr.functionName);
        sym_tab.enter(instr.dest);
         for(size_t i = 0; i < func.arg_names.size(); ++i) {
            sym_tab.enterScope(instr.functionName);
            switch(func.arg_types[i]) {
                case ast::VarType::NUMBER: {
                    sym_tab.enter(func.arg_names[i]);
                    auto loc = sym_tab.lookup(func.arg_names[i]);
                    if(loc.has_value()) {
                        loc.value()->vtype = ast::VarType::NUMBER;
                        numeric_variables[func.functionName][func.arg_names[i]] = numeric_variables[curFunction][instr.args[i]];
                    }
                }
                break;
            }
        }
        long pos = label_pos[instr.functionName]-1;
        call_stack.push_back(std::make_pair(curFunction, ip));
        ip = pos;      
    }
    
    void Interpreter::executeReturn(const ir::IRInstruction &instr) {
        if(call_stack.empty()) {
                auto loc = sym_tab.lookup(instr.dest);
                if(loc.has_value()) {
                    if(loc.value()->vtype == ast::VarType::NUMBER) {
                        throw Exit_Exception(numeric_variables[curFunction][instr.dest]);
                    }
                }
            } else {
                auto pos = call_stack.back();
                call_stack.pop_back();
                sym_tab.enterScope(pos.first);
                sym_tab.enter(instr.dest);
                ip = pos.second;
           }
    }
    void Interpreter::executeJump(const ir::IRInstruction &instr) {
        if (numeric_variables[curFunction][instr.op1] != 0) {
            
        }
    }

    long Interpreter::getIntegerValue(const std::string &operand) {
        if(isdigit(operand[0])) {
            return std::stol(operand);
        }
        return numeric_variables[curFunction][operand];
    }

    void Interpreter::executeConcat(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc_dest = sym_tab.lookup(instr.dest);
        auto loc_op1 = sym_tab.lookup(instr.op1);
        auto loc_op2 = sym_tab.lookup(instr.op2);
        if(loc_op1.has_value() && loc_op2.has_value() && loc_op1.value()->vtype == ast::VarType::STRING && loc_op2.value()->vtype == ast::VarType::STRING) {
            std::ostringstream stream;
            stream << '\"' << stripQuotes(string_variables[curFunction][instr.op1]) << stripQuotes(string_variables[curFunction][instr.op2]) << '\"';
            string_variables[curFunction][instr.dest] = stream.str();
            loc_dest.value()->vtype = ast::VarType::STRING;
        }
    }

    std::string Interpreter::stripQuotes(const std::string &value) {
        if(value[0] == '\"' && value.back() == '\"') {
            return value.substr(1, value.length()-2);
        }
        throw Exception("String without quotes: " + value);
    }


    void FunctionTable::enterFunction(const std::string &name) {
        curFunction = name;
        func[curFunction].functionName = name;
    }
    
    void FunctionTable::addParam(const std::string &name, ast::VarType type) {
        func[curFunction].arg_names.push_back(name);
        func[curFunction].arg_types.push_back(type);
    }

    Function &FunctionTable::getFunction(const std::string &name) {
        return func[name];
    }

}
