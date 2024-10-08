#include"interp.hpp"


namespace interp {

    Interpreter::Interpreter(symbol::SymbolTable &table) : sym_tab{table}, ip{0} {

    }

    int Interpreter::execute(ir::IRCode &code) {
        ip = 0;
        while(ip < static_cast<long>(code.size())) {
            const auto instr = code[ip];
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
                default:
                    std::cerr << "Unsupported instruction: " << instr.toString() << std::endl;
                    break;
            }

            ip ++;
        }


        return EXIT_SUCCESS;
    }
    
    void Interpreter::executeAdd(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        numeric_variables[instr.dest] = val1 + val2;
    }

    void Interpreter::executeSub(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        numeric_variables[instr.dest] = val1 - val2;
    }

    void Interpreter::executeMul(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        numeric_variables[instr.dest] = val1 * val2;
    }

    void Interpreter::executeDiv(const ir::IRInstruction &instr) {
        int val1 = getIntegerValue(instr.op1);
        int val2 = getIntegerValue(instr.op2);
        if(val2 == 0) {
            throw Exception("Divison By Zero");
        }
        numeric_variables[instr.dest] = val1 / val2;
    }
        
    void Interpreter::executeJump(const ir::IRInstruction &instr) {
        if (numeric_variables[instr.op1] != 0) {
            
        }
    }

    long Interpreter::getIntegerValue(const std::string &operand) {
        if(isdigit(operand[0])) {
            return std::stol(operand);
        }
        return numeric_variables[operand];
    }

}