#include"interp.hpp"


namespace interp {

    Interpreter::Interpreter(symbol::SymbolTable &table) : sym_tab{table}, ip{0} {

    }

    int Interpreter::execute(ir::IRCode &code) {
        collectLabels(code);
        ip = 0;
        while(ip < static_cast<long>(code.size())) {
            const auto instr = code[ip];

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
                case ir::InstructionType::LOAD_CONST:
                    executeLoadConst(instr);
                    break;
                default:
                    std::cerr << "Unsupported instruction: " << instr.toString() << std::endl;
                    break;
            }

            ip ++;
        }

        outputDebugInfo(std::cout);
        return EXIT_SUCCESS;
    }

    void Interpreter::collectLabels(const ir::IRCode &code) {
        int ip_id = 0;
        while(ip_id < code.size()) {
            const auto instr = code[ip_id];
            if(instr.type == ir::InstructionType::LABEL) {
                    label_pos[instr.dest] = ip_id;
                    ip_id++;
                    continue;
            } else {
                ip_id ++;
            }
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
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 + val2;
    }

    void Interpreter::executeSub(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 - val2;
    }

    void Interpreter::executeMul(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 * val2;
    }

    void Interpreter::executeDiv(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        if(val2 == 0) {
            throw Exception("Divison By Zero");
        }
        numeric_variables[curFunction][instr.dest] = val1 / val2;
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

}
