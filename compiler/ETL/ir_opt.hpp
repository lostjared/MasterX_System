#ifndef _IR_OP_H_
#define _IR_OP_H_

#include "ir.hpp"
#include <unordered_map>
#include <unordered_set>

namespace ir {

    class IROptimizer {
    public:
        IRCode optimize(const IRCode &originalCode) {
            IRCode optimizedCode;
            std::unordered_map<std::string, std::string> lastValueMapping;

            for (const auto &instr : originalCode) {
                switch (instr.type) {
                    case InstructionType::LOAD_CONST:
                        lastValueMapping[instr.dest] = instr.op1;
                        optimizedCode.push_back(instr);
                        break;

                    case InstructionType::ASSIGN:
                        if (lastValueMapping[instr.dest] == instr.op1) {
                            continue;
                        }
                        lastValueMapping[instr.dest] = instr.op1;
                        optimizedCode.push_back(instr);
                        break;

                    case InstructionType::ADD:
                    case InstructionType::SUB:
                    case InstructionType::MUL:
                    case InstructionType::DIV:
                    case InstructionType::NEG:
                    case InstructionType::CONCAT:
                        lastValueMapping[instr.dest] = instr.dest;
                        optimizedCode.push_back(instr);
                        break;

                    case InstructionType::CALL:
                        for (const auto &arg : instr.args) {
                            lastValueMapping[arg].clear();
                        }
                        lastValueMapping[instr.dest] = instr.dest;
                        optimizedCode.push_back(instr);
                        break;

                    case InstructionType::RETURN:
                    case InstructionType::LABEL:
                        lastValueMapping.clear();
                        optimizedCode.push_back(instr);
                        break;

                    default:
                        optimizedCode.push_back(instr);
                        break;
                }
            }

            return optimizedCode;
        }
    };

}

#endif