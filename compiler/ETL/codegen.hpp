#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <regex>
#include <set>
#include "ir.hpp"
#include "symbol.hpp"
#include "clib.hpp"

namespace codegen {
    
    enum class ReturnType {
        INTEGER,
        POINTER,
        VOID, 
        FLOAT,
        DOUBLE,
    };

    enum class VariableType {
        STRING_CONST,
        NUMERIC_CONST,
        VAR,
        VAR_STRING,
        POINTER,
    };

    struct VariableInfo {
        VariableType type;
        bool isAllocated;
        std::string vname, text;
        VariableInfo(VariableType t, bool alloc = false, const std::string& name = "", const std::string &txt = "")
            : type(t), isAllocated(alloc), vname(name), text(txt) {}
        VariableInfo() : type(VariableType::VAR), isAllocated(false), vname(""), text("") {} 
    };

    class CodeEmitter {
    public:
        CodeEmitter(symbol::SymbolTable &symbolTable, std::unordered_map<std::string, int> &functionVarCount)
            : table(symbolTable), functionLocalVarCount(functionVarCount), currentStackOffset{}, maxStackUsage{} {}

        std::string emit(const ir::IRCode &code) {
            std::ostringstream output;
            collectLiteralsAndConstants(code);
            analyzeTempVarUsage(code);
            emitDataSection(output);
            bool has_init = false;
            for(auto &it : code) {
                if(it.type == ir::InstructionType::LABEL && it.dest == "init") {
                    has_init = true;
                    break;
                }
            }
            curFunction = "main";
            if(has_init == true) {
                emitPreamble(output);
                emitCallInit(output);
            } else {
#ifdef __APPLE__
                output << ".section __TEXT,__text\n";
#else
                output << ".section .text\n";
#endif
            }
            emitCode(code, output);
#ifdef __linux__
            output << ".section .note.GNU-stack,\"\",@progbits\n";
#endif
            return applyPeephole(output);
        }

    private:
        symbol::SymbolTable &table;
        symbol::SymbolTable local;
        std::unordered_map<std::string, int> &functionLocalVarCount;
        std::unordered_map<std::string, int> currentStackOffset;
        std::unordered_map<std::string, int> maxStackUsage;
        std::unordered_map<std::string, std::unordered_map<std::string, int>> variableOffsets;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> stringLiterals;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> numericConstants;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> valueLocations;
        std::unordered_map<std::string, std::unordered_map<std::string, int>> valueToStackOffset;
        std::unordered_map<std::string, int> tempVarCountPerFunction;
        std::unordered_map<std::string, std::unordered_set<std::string>> allocatedMemory;  
        std::unordered_map<std::string, std::unordered_map<std::string, VariableInfo>> variableInfo;
        std::unordered_map<std::string, std::set<std::string>> ownedMemory;
        std::string curFunction;

        void collectLiteralsAndConstants(const ir::IRCode &code) {
        
           static int counter = 1;

           for (const auto &instr : code) {

                if(instr.type == ir::InstructionType::LABEL) {
                    curFunction = instr.dest;
                    continue;
                }

                if (instr.type == ir::InstructionType::LOAD_CONST || instr.type == ir::InstructionType::SET_CONST) {
                    if (instr.op1[0] == '\"') {
                        std::string label =  instr.dest;
                        stringLiterals[curFunction][instr.op1] = label;
                        variableInfo[curFunction][instr.dest] = VariableInfo(VariableType::STRING_CONST, false, label, instr.op1);
                        local.enter(label);
                        auto it = local.lookup(label);
                        if(it.has_value()) {
                            symbol::Symbol *s = it.value();
                            s->name = label;
                            s->value = instr.op1;
                        }
                    } else {
                        std::string label = instr.dest;
                        if(numericConstants[curFunction].find(instr.op1) == numericConstants[curFunction].end()) {
                            numericConstants[curFunction][instr.op1] = label;
                            variableInfo[curFunction][instr.dest] = VariableInfo(VariableType::NUMERIC_CONST, false, label, instr.op1);
                            local.enter(label);
                            auto it = local.lookup(label);
                            if(it.has_value()) {
                                symbol::Symbol *s = it.value();
                                 s->name = label;
                                s->value = instr.op1;
                            }
                        }
                    }
                }
            }
        }

        void analyzeTempVarUsage(const ir::IRCode &code) {
            std::unordered_map<std::string, int> tempVarIndices;
            std::string currentFunction;

            for (const auto &instr : code) {
                if (instr.type == ir::InstructionType::LABEL) {
                    currentFunction = instr.dest;
                    tempVarCountPerFunction[currentFunction] = 0;
                    maxStackUsage[currentFunction] = 0;
                    tempVarIndices.clear();
                }

                if (instr.type == ir::InstructionType::CALL || instr.type == ir::InstructionType::CONCAT || instr.type == ir::InstructionType::ASSIGN || instr.dest[0] == 't' || instr.type == ir::InstructionType::LOAD_CONST || instr.type == ir::InstructionType::LOAD_VAR) {
                    if (tempVarIndices.find(instr.dest) == tempVarIndices.end()) {
                        int index = tempVarCountPerFunction[currentFunction]++;
                        tempVarIndices[instr.dest] = index;
                        maxStackUsage[currentFunction] += 8;
                    }
                } 
            }

            for (auto &entry : maxStackUsage) {
                entry.second = ((entry.second + 15) / 16) * 16;
            }
        }

        void emitDataSection(std::ostringstream &output) {
#ifdef __APPLE__
output << ".section __TEXT,__cstring\n";
#else
output << ".section .data\n";
#endif
            for(auto &func : variableInfo) {
                for(const auto &v : func.second) {
                    if(v.second.type == VariableType::NUMERIC_CONST) {
                        //output << v.second.vname << ": .quad " << v.second.text << "\n";
                    } else if(v.second.type == VariableType::STRING_CONST) {
                        output << v.second.vname << ": .asciz " << ir::escapeString(v.second.text) << "\n";
                    }
                }
            }
        }

        void emitPreamble(std::ostringstream &output) {
#ifdef __APPLE__
            output << ".section __TEXT,__text\n";
            output << ".globl _start\n";
            output << "_start:\n";
            output << "    xor %rbp, %rbp\n";
            output << "    movq %rsp, %rdi\n";
            output << "    andq $-16, %rsp\n";
            output << "    subq $8, %rsp \n";
            output << "    call _main\n";
            output << "    movq %rax, %rdi\n";
            output << "    movl $0, %edi\n";
            output << "    call _exit\n\n";
            output << ".globl _main\n";
            output << "_main:\n";
#else
            output << ".section .text\n";
            output << ".globl main\n";
            output << "main:\n";

#endif
            output << "    pushq %rbp\n";
            output << "    movq %rsp, %rbp\n";
            output << "    subq $16, %rsp\n";
            curFunction = "main";
        }

        void emitCallInit(std::ostringstream &output) {
#ifdef __APPLE__
            output << "    call _init\n";
#else
            output << "    call init\n";
#endif
            output << "    movq $0, %rax\n";
            output << "    leave\n";
            output << "    ret\n";
            
        }

        void emitFunctionPrologue(std::ostringstream &output, const std::string &functionName) {
            output << "    pushq %rbp\n";
            output << "    movq %rsp, %rbp\n";

            int stackSpace = maxStackUsage[functionName] + 64; 
            if ((stackSpace % 16) != 0) {
                stackSpace += 8; 
            }

           if (stackSpace > 0) {
                output << "    subq $" << stackSpace << ", %rsp\n";
            }

            output << "    movq $0, %rcx\n";
        }

        void emitFunctionEpilogue(std::ostringstream &output) {
            output << "    leave\n";
            output << "    ret\n";
            
        }

        void emitCode(const ir::IRCode &code, std::ostringstream &output) {
            std::string currentFunction;

            for (const auto &instr : code) {
                if (instr.type == ir::InstructionType::LABEL) {
                    currentFunction = instr.dest;
                }

                switch (instr.type) {
                    case ir::InstructionType::ADD:
                        emitBinaryOp(output, instr, "addq");
                        break;
                    case ir::InstructionType::SUB:
                        emitBinaryOp(output, instr, "subq");
                        break;
                    case ir::InstructionType::MUL:
                        emitBinaryOp(output, instr, "imulq");
                        break;
                    case ir::InstructionType::DIV:
                        emitDiv(output, instr);
                        break;
                    case ir::InstructionType::ASSIGN:
                        emitAssign(output, instr);
                        break;
                    case ir::InstructionType::LOAD_CONST:
                        emitLoadConst(output, instr);
                        break;
                    case ir::InstructionType::LOAD_VAR:
                        emitLoadVar(output, instr);
                        break;
                    case ir::InstructionType::NEG:
                        emitNeg(output, instr);
                        break;
                    case ir::InstructionType::NOT:
                        emitNot(output, instr);
                        break;
                    case ir::InstructionType::CALL:
                        emitCall(output, instr);
                        break;
                    case ir::InstructionType::LABEL:
                        emitLabel(output, instr);
                        break;
                    case ir::InstructionType::RETURN:
                        emitReturn(output, instr);
                        break;
                    case ir::InstructionType::CONCAT:
                        emitConcat(output, instr);
                        break;
                    case ir::InstructionType::PARAM:
                        emitParam(output, instr);
                        break;
                    case ir::InstructionType::PARAM_STRING:
                        emitParamString(output, instr);
                        break;
                    case ir::InstructionType::DEF_PARAM:
                        emitDefParam(output, instr);
                        break;
                    case ir::InstructionType::DEF_PARAM_STRING:
                        emitDefParamString(output, instr);
                        break;
                    case ir::InstructionType::DEFINE:
                        break;
                    case ir::InstructionType::MOD:
                        emitMod(output, instr);
                        break;
                    case ir::InstructionType::AND:
                        emitAnd(output, instr);
                        break;
                    case ir::InstructionType::XOR:
                        emitXor(output, instr);
                        break;
                    case ir::InstructionType::OR:
                        emitOr(output, instr);
                        break;
                    case ir::InstructionType::LSHIFT:
                        emitLShift(output, instr); 
                        break;
                    case ir::InstructionType::RSHIFT:
                        emitRShift(output, instr);
                        break; 
                    case ir::InstructionType::EQ:  
                        emitEq(output, instr);
                        break;
                    case ir::InstructionType::NEQ: 
                        emitNeq(output, instr);
                        break;
                    case ir::InstructionType::LT:  
                        emitLt(output, instr);
                        break;
                    case ir::InstructionType::LE:  
                        emitLe(output, instr);
                        break;
                    case ir::InstructionType::GT:  
                        emitGt(output, instr);
                        break;
                    case ir::InstructionType::GE:  
                        emitGe(output, instr);
                        break;
                    case ir::InstructionType::LOGICAL_AND:  
                        emitLogicalAnd(output, instr);
                        break;
                    case ir::InstructionType::LOGICAL_OR:  
                        emitLogicalOr(output, instr);
                        break;
                    case ir::InstructionType::SUB_LABEL:
                        emitSubLabel(output, instr);
                        break;
                    case ir::InstructionType::JUMP:
                        emitJump(output, instr);
                        break;
                    case ir::InstructionType::SET:
                        emitSet(output, instr);
                        break;  
                    case ir::InstructionType::SET_CONST:
                        emitSetConst(output, instr);
                        break;
                    case ir::InstructionType::LOGICAL_NOT:
                        emitLogicalNot(output, instr);
                        break;
                    case ir::InstructionType::PARAM_POINTER:
                        emitParamPointer(output, instr);
                        break;
                    case ir::InstructionType::DEF_PARAM_POINTER:
                        emitDefParamPointer(output, instr);
                        break;
                    default:
                        std::cerr << "Unsupported IR Instruction: " << instr.toString() << std::endl;
                        break;
                }
            }
        }


        void emitLogicalNot(std::ostringstream &output, const ir::IRInstruction &instr) {
            loadToRegister(output, instr.op1, "%rax");
            output << "    cmpq $0, %rax\n";
            output << "    sete %al\n";
            output << "    movzbq %al, %rbx\n";
            storeToTemp(output, instr.dest, "%rbx");
        }

        void emitSet(std::ostringstream &output, const ir::IRInstruction &instr) {
            variableInfo[curFunction][instr.dest].type = variableInfo[curFunction][instr.op1].type;
            auto val = table.lookup(instr.dest);
            if(val.has_value()) {
                auto loc = table.lookup(instr.op1);
                if(loc.has_value()) {
                    val.value()->vtype = loc.value()->vtype;
                }
                auto val = ownedMemory[curFunction].find(instr.op1);
                if(val != ownedMemory[curFunction].end()) {
                    ownedMemory[curFunction].erase(val);
                    ownedMemory[curFunction].insert(instr.dest);
                }
                output << "# load " << instr.op1 << " to " << instr.dest << "\n";
                output << "    movq "<< getOperand(instr.op1) << ", %rcx\n";
                output << "    movq " << "%rcx, " << getOperand(instr.dest)<< "\n";
            }
        }

        void emitSetConst(std::ostringstream &output, const ir::IRInstruction &instr) {
            auto loc = table.lookup(instr.dest);
            if (instr.op1[0] == '\"') {
                std::string label = stringLiterals[curFunction][instr.op1];
                variableInfo[curFunction][instr.dest].type = VariableType::STRING_CONST;
                if(loc.has_value()) {
                    loc.value()->vtype = ast::VarType::STRING;
                    loc.value()->value = instr.op1;
                }
                output << "    movq " << getOperand(label) << ", %rcx\n";
                output << "    movq %rcx, " << getOperand(instr.dest) << "\n";
            } else {
                std::string label = numericConstants[curFunction][instr.op1];
                variableInfo[curFunction][instr.dest].type = VariableType::NUMERIC_CONST;
                if(loc.has_value()) {
                    loc.value()->vtype = ast::VarType::NUMBER;
                    loc.value()->value = instr.op1;
                }
                output << "    movq $" << instr.op1 << ", %rcx # here\n";
                output << "    movq %rcx, " << getOperand(instr.dest) << "\n";
            } 
           
        }

        void emitSubLabel(std::ostringstream &output, const ir::IRInstruction &instr) {
            output << instr.dest << ": \n";
        }

        void emitJump(std::ostringstream &output, const ir::IRInstruction &instr) {
            if (instr.op1.empty() && instr.op2.empty()) {
                output << "    jmp " << instr.dest << "\n";
            } else {
                loadToRegister(output, instr.op1, "%rax");
                output << "    cmpq $0, %rax\n";
                output << "    je " << instr.dest << "\n";
            }
        }

        void emitLogicalAnd(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if (it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rsi");
            output << "    cmpq $0, %rsi\n";
            output << "    setne %al\n";
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq $0, %rdi\n";
            output << "    setne %cl\n";
            output << "    andb %al, %cl\n";
            output << "    movzbq %cl, %rax\n"; 
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitLogicalOr(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if (it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rdi");
            output << "    cmpq $0, %rdi\n";
            output << "    setne %al\n";
            loadToRegister(output, instr.op2, "%rdx");
            output << "    cmpq $0, %rdx\n";
            output << "    setne %cl\n";
            output << "    orb %al, %cl\n";
            output << "    movzbq %cl, %rdx\n"; 
            storeToTemp(output, instr.dest, "%rdx");
        }
        
        void emitEq(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if (it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rsi");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq %rdi, %rsi\n";
            output << "    sete %cl\n";
            output << "    movzbq %cl, %rdx\n";
            storeToTemp(output, instr.dest, "%rdx");
        }

        void emitNeq(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq %rdi, %rax\n";  
            output << "    setne %cl\n";         
            output << "    movzbq %cl, %rdx\n"; 
            storeToTemp(output, instr.dest, "%rdx");
        }

        void emitLt(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq %rdi, %rax\n";  
            output << "    setl %cl\n";         
            output << "    movzbq %cl, %rdx\n"; 
            storeToTemp(output, instr.dest, "%rdx");
        }

        void emitLe(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq %rdi, %rax\n";  
            output << "    setle %cl\n";         
            output << "    movzbq %cl, %rdx\n"; 
            storeToTemp(output, instr.dest, "%rdx");
        }

        void emitGt(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq %rdi, %rax\n";  
            output << "    setg %cl\n";         
            output << "    movzbq %cl, %rdx\n"; 
            storeToTemp(output, instr.dest, "%rdx");

        }

        void emitGe(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    cmpq %rdi, %rax\n";  
            output << "    setge %cl\n";         
            output << "    movzbq %cl, %rdx\n"; 
            storeToTemp(output, instr.dest, "%rdx");
        }

        void emitAnd(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    andq %rdi, %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitOr(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    orq %rdi, %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitXor(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rdi");
            output << "    xorq %rdi, %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitLShift(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rcx");
            output << "    salq %cl, %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitRShift(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            loadToRegister(output, instr.op2, "%rcx");
            output << "    sarq %cl, %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitMod(std::ostringstream &output, const ir::IRInstruction &instr) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            output << "    cqto\n";          
            loadToRegister(output, instr.op2, "%rdi");
            output << "    idivq %rdi\n";
            storeToTemp(output, instr.dest, "%rdx");
        }

        std::vector<std::string> cargs;
        int paramIndex = 0; 

        void emitDefParam(std::ostringstream &stream, const ir::IRInstruction &instr) {
        
                static std::vector<std::pair<std::string, int>> paramLocations = {
                {"%rdi", -8},  
                {"%rsi", -16}, 
                {"%rdx", -24}, 
                {"%rcx", -32}, 
                {"%r8", -40},  
                {"%r9", -48}   
            };

            if(curFunction == "main") return;

            if (paramIndex < paramLocations.size()) {
                std::string reg = paramLocations[paramIndex].first;
                int offset = paramLocations[paramIndex].second;
                paramIndex++;
                table.enter(instr.dest);
                auto it = table.lookup(instr.dest);
                variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            } else {
                std::cerr << "ETL Error: More parameters than registers available.\n";
                exit(EXIT_FAILURE);
            }
        }

        void emitDefParamString(std::ostringstream &stream, const ir::IRInstruction &instr) {
            static std::vector<std::pair<std::string, int>> paramLocations = {
                {"%rdi", -8},  
                {"%rsi", -16}, 
                {"%rdx", -24}, 
                {"%rcx", -32}, 
                {"%r8", -40},  
                {"%r9", -48}   
            };
            if(curFunction == "main") return;

            if (paramIndex < paramLocations.size()) {
                std::string reg = paramLocations[paramIndex].first;
                int offset = paramLocations[paramIndex].second;
                paramIndex++;
                table.enter(instr.dest);
                auto it = table.lookup(instr.dest);
                if(it.has_value()) {
                    it.value()->vtype = ast::VarType::STRING;
                }
                variableInfo[curFunction][instr.dest].type = VariableType::VAR_STRING;
            } else {
                std::cerr << "ETL Error: More parameters than registers available.\n";
                exit(EXIT_FAILURE);
            }
        }

        
        void emitParamPointer(std::ostringstream &output, const ir::IRInstruction  &instr) {
            static std::vector<std::pair<std::string, int>> paramLocations = {
                {"%rdi", -8},  
                {"%rsi", -16}, 
                {"%rdx", -24}, 
                {"%rcx", -32}, 
                {"%r8", -40},  
                {"%r9", -48}   
            };

            if(curFunction == "main") return;

            if (paramIndex < paramLocations.size()) {
                std::string reg = paramLocations[paramIndex].first;
                int offset = paramLocations[paramIndex].second;
                storeToTemp(output, instr.dest, reg);
                paramIndex++;
                table.enter(instr.dest);
                auto it = table.lookup(instr.dest);
                if(it.has_value()) {
                    it.value()->vtype = ast::VarType::POINTER;
                }
                variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            } else {
                std::cerr << "ETL Error: More parameters than registers available.\n";
                exit(EXIT_FAILURE);
            }
        }

        void emitDefParamPointer(std::ostringstream &output, const ir::IRInstruction &instr) {
            static std::vector<std::pair<std::string, int>> paramLocations = {
                {"%rdi", -8},  
                {"%rsi", -16}, 
                {"%rdx", -24}, 
                {"%rcx", -32}, 
                {"%r8", -40},  
                {"%r9", -48}   
            };

            if(curFunction == "main") return;

            if (paramIndex < paramLocations.size()) {
                std::string reg = paramLocations[paramIndex].first;
                int offset = paramLocations[paramIndex].second;
                paramIndex++;
                table.enter(instr.dest);
                auto it = table.lookup(instr.dest);
                if(it.has_value()) {
                    it.value()->vtype = ast::VarType::POINTER;
                }
                variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            } else {
                std::cerr << "ETL Error: More parameters than registers available.\n";
                exit(EXIT_FAILURE);
            }
        }

        void emitParam(std::ostringstream &stream, const ir::IRInstruction &instr) {
            static std::vector<std::pair<std::string, int>> paramLocations = {
                {"%rdi", -8},  
                {"%rsi", -16}, 
                {"%rdx", -24}, 
                {"%rcx", -32}, 
                {"%r8", -40},  
                {"%r9", -48}   
            };

            if(curFunction == "main") return;

            if (paramIndex < paramLocations.size()) {
                std::string reg = paramLocations[paramIndex].first;
                int offset = paramLocations[paramIndex].second;
                storeToTemp(stream, instr.dest, reg);
                paramIndex++;
                table.enter(instr.dest);
                auto it = table.lookup(instr.dest);
                variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            } else {
                std::cerr << "ETL Error: More parameters than registers available.\n";
                exit(EXIT_FAILURE);
            }
        }

        void emitParamString(std::ostringstream &output, const ir::IRInstruction &instr) {
            static std::vector<std::pair<std::string, int>> paramLocations = {
                {"%rdi", -8},  
                {"%rsi", -16}, 
                {"%rdx", -24}, 
                {"%rcx", -32}, 
                {"%r8", -40},  
                {"%r9", -48}   
            };
            if(curFunction == "main") return;

            if (paramIndex < paramLocations.size()) {
                std::string reg = paramLocations[paramIndex].first;
                int offset = paramLocations[paramIndex].second;
                storeToTemp(output, instr.dest, reg);
                paramIndex++;
                table.enter(instr.dest);
                auto it = table.lookup(instr.dest);
                if(it.has_value()) {
                    it.value()->vtype = ast::VarType::STRING;
                }
                variableInfo[curFunction][instr.dest].type = VariableType::VAR_STRING;
            } else {
                std::cerr << "ETL Error: More parameters than registers available.\n";
                exit(EXIT_FAILURE);
            }
        }
        
        void emitLoadConst(std::ostringstream &output, const ir::IRInstruction &instr) {

            table.enter(instr.dest);
            auto loc = table.lookup(instr.dest);


            if (instr.op1[0] == '\"') {
                std::string label = stringLiterals[curFunction][instr.op1];
                output << "    leaq " << label << "(%rip), %rax\n";
                variableInfo[curFunction][instr.dest].type = VariableType::STRING_CONST;
                if(loc.has_value()) {
                    loc.value()->vtype = ast::VarType::STRING;
                }
            } else {
                std::string label = numericConstants[curFunction][instr.op1];
                output << "    movq " << "$" << instr.op1 << ", %rax\n";
                variableInfo[curFunction][instr.dest].type = VariableType::NUMERIC_CONST;
                if(loc.has_value()) {
                    loc.value()->vtype = ast::VarType::NUMBER;
                }
            } 
            storeToTemp(output, instr.dest, "%rax");
        }
        
        void emitConcat(std::ostringstream &output, const ir::IRInstruction &instr) {
            auto op1_it = table.lookup(instr.op1);
            auto op2_it = table.lookup(instr.op2);
            std::string prefix;
        #ifdef __APPLE__
            prefix = "_";
        #endif
            output << "    movq $0, %rcx\n";
            storeToTemp(output, "counter", "%rcx");
            auto addStringLength = [&](const std::string &op, auto &op_it) {
                if (variableInfo[curFunction][op].type == VariableType::STRING_CONST) {
                    if (!variableInfo[curFunction][op].text.empty() && variableInfo[curFunction][op].text[0] == '\"') {
                        auto len = variableInfo[curFunction][op].text.length() + 1;
                        output << "    addq $" << len << ", " << getOperand("counter") << "\n";
                    } else {
                        loadToRegister(output, op, "%rdi");
                        output << "    call " << prefix << "strlen #" << op << "\n";
                        output << "    addq %rax, " << getOperand("counter") << "\n";
                    }
                } else if (variableInfo[curFunction][op].type == VariableType::VAR_STRING ||
                        (op_it.has_value() && op_it.value()->vtype == ast::VarType::STRING)) {
                    loadToRegister(output, op, "%rdi");
                    output << "    call " << prefix << "strlen # " << op << "\n";
                    output << "    addq %rax, " << getOperand("counter") << "\n";
                }
            };
            addStringLength(instr.op1, op1_it);
            addStringLength(instr.op2, op2_it);
            output << "    addq $1, " << getOperand("counter") << "\n";
            output << "    movq $" << sizeof(char) << ", %rsi\n";
            output << "    xorq %rax, %rax\n";
            loadToRegister(output, "counter", "%rdi");
            output << "    call " << prefix << "calloc\n";
            output << "    movq %rax, %rdi\n";         
            storeToTemp(output, instr.dest, "%rdi");
            loadToRegister(output, instr.op1, "%rsi");
            output << "    call " << prefix << "strcpy\n";
            loadToRegister(output, instr.op2, "%rsi");
            output << "    call " << prefix << "strcat\n";
            allocatedMemory[curFunction].insert(instr.dest);
            
            variableInfo[curFunction][instr.dest].type = VariableType::VAR_STRING;
            table.enter(instr.dest);
            if (auto it = table.lookup(instr.dest); it.has_value()) {
                it.value()->vtype = ast::VarType::STRING;
            }
        }


        void emitBinaryOp(std::ostringstream &output, const ir::IRInstruction &instr, const std::string &op) {
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            loadToRegister(output, instr.op1, "%rax");
            output << "    " << op << " " << getOperand(instr.op2) << ", %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitDiv(std::ostringstream &output, const ir::IRInstruction &instr) {
            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
            table.enter(instr.dest);
            auto it = table.lookup(instr.dest);
            if(it.has_value()) {
                it.value()->vtype = ast::VarType::NUMBER;
            }
            loadToRegister(output, instr.op1, "%rax");
            output << "    cqto\n";
            output << "    idivq " << getOperand(instr.op2) << "\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitAssign(std::ostringstream &output, const ir::IRInstruction &instr) {
            variableInfo[curFunction][instr.dest].type = variableInfo[curFunction][instr.op1].type;
            table.enter(instr.dest);
            auto val = table.lookup(instr.dest);
            if(val.has_value()) {
                auto loc = table.lookup(instr.op1);
                if(loc.has_value()) {
                    val.value()->vtype = loc.value()->vtype;
                }
                auto val = ownedMemory[curFunction].find(instr.op1);
                if(val != ownedMemory[curFunction].end()) {
                    ownedMemory[curFunction].erase(val);
                    ownedMemory[curFunction].insert(instr.dest);
                }
            }
            loadToRegister(output, instr.op1, "%rax");
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitLoadVar(std::ostringstream &output, const ir::IRInstruction &instr) {
            loadToRegister(output, instr.op1, "%rax");
            storeToTemp(output, instr.dest, "%rax");
            table.enter(instr.op1);
            table.enter(instr.dest);
            auto it = table.lookup(instr.op1);
            if(it.has_value()) {
                symbol::Symbol *s = it.value();
                auto src = table.lookup(instr.dest);
                if(src.has_value()) {
                    symbol::Symbol *v = src.value();
                    v->vtype = s->vtype;
                    variableInfo[curFunction][instr.dest].type = VariableType::VAR_STRING;
                   
                }
            }
        }

        void emitNeg(std::ostringstream &output, const ir::IRInstruction &instr) {
            loadToRegister(output, instr.op1, "%rax");
            output << "    negq %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        void emitNot(std::ostringstream &output, const ir::IRInstruction &instr) {
            loadToRegister(output, instr.op1, "%rax");
            output << "    notq %rax\n";
            storeToTemp(output, instr.dest, "%rax");
        }

        std::string lastFunctionCall;
        std::string lastFunctionCallDest;


        bool checkArgumentTypes(const std::string& functionName, const std::vector<ast::VarType>& providedArgs, const std::vector<ast::VarType>& expectedArgs) {
            if (providedArgs.size() != expectedArgs.size()) {
                std::cerr << "Error: Function '" << functionName << "' expected " << expectedArgs.size() << " arguments, but got " << providedArgs.size() << ".\n";
                return false;
            }

            for (size_t i = 0; i < providedArgs.size(); ++i) {
                ast::VarType actual = providedArgs[i];
                ast::VarType expected = expectedArgs[i];

                //a pointer is a number
               
               if(actual == ast::VarType::NUMBER && expected == ast::VarType::POINTER) 
                    continue;
                if(actual == ast::VarType::POINTER && expected == ast::VarType::NUMBER)
                    continue;
                
                if (actual != expected) {
                    std::cerr << "Error: Type mismatch for argument " << i << " in function '" << functionName
                            << "'. Expected type: " << ast::VarString[static_cast<int>(expected)]
                            << ", Actual type: " << ast::VarString[static_cast<int>(actual)] << ".\n";
                    return false;
                }
            }

            return true;
        }
        void checkFunctionArguments(const std::string& functionName, const std::vector<std::string>& args) {
                std::vector<ast::VarType> providedArgs;
                for (const auto& arg : args) {
                    auto argIt = table.lookup(arg);
                    if (argIt.has_value()) {
                        providedArgs.push_back(argIt.value()->vtype);
                    } else {
                        std::cerr << "Error: Argument '" << arg << "' not found in symbol table.\n";
                        exit(EXIT_FAILURE);
                    }
                }

                auto localFunc = table.lookupFunc(functionName);
                if (localFunc.has_value()) {
                    const auto& expectedArgs = localFunc.value()->argTypes;
                    if (!checkArgumentTypes(functionName, providedArgs, expectedArgs)) {
                        std::cerr << "Error: Argument type check failed for function '" << functionName << "'.\n";
                        exit(EXIT_FAILURE);
                    }
                } else {
                    if(functionName != "printf" && functionName != "sprintf") {
                        std::cerr << "Error: Function '" << functionName << "' is not defined.\n";
                        exit(EXIT_FAILURE);
                    }
                }
        }


        void emitCall(std::ostringstream &output, const ir::IRInstruction &instr) {
            static const std::vector<std::string> argumentRegisters = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            size_t numArgs = instr.args.size();

            for (size_t i = 0; i < numArgs && i < argumentRegisters.size(); ++i) {
                loadToRegister(output, instr.args[i], argumentRegisters[i]);
            }

            checkFunctionArguments(instr.functionName, instr.args);

            lastFunctionCall = instr.functionName;
            lastFunctionCallDest = instr.dest;
            output << "    movq $0, %rax\n"; 
#ifdef __APPLE__
            output << "    call " << "_" << instr.functionName << "\n";
#else
            output << "    call " << instr.functionName << "\n";
#endif
            storeToTemp(output, instr.dest, "%rax");
            table.enter(instr.dest);
            if(variableInfo[curFunction][instr.dest].type == VariableType::VAR_STRING) {
                ownedMemory[curFunction].insert(instr.dest);
            }

            

            if(instr.functionName == "str") {
                output << "    addq $22, " << getOperand("counter") << "\n";
            }
            auto fn = clib::clibrary.find(instr.functionName);
            
            if (fn != clib::clibrary.end()) {

                auto dest_rt = table.lookup(instr.dest);

                switch (fn->second.return_type) {
                    case clib::VarType::POINTER:
                        variableInfo[curFunction][instr.dest].type = VariableType::VAR_STRING;
                        if(dest_rt.has_value()) {
                            dest_rt.value()->vtype = ast::VarType::STRING;
                        }
                        break;
                    case clib::VarType::INTEGER:
                        variableInfo[curFunction][instr.dest].type = VariableType::VAR;
                        if(dest_rt.has_value()) {
                            dest_rt.value()->vtype = ast::VarType::NUMBER;
                        }
                        break;
                    default:
                        std::cerr << "ETL: Return type not supported yet.\n";
                        exit(EXIT_FAILURE);
                        break;
                }

                if (instr.args.size() != fn->second.args.size() && instr.functionName != "printf" && instr.functionName != "sprintf") {
                    std::cerr << "ETL: Fatal, incorrect number of arguments for: " << instr.functionName << "\n";
                    exit(EXIT_FAILURE);
                }

                if (instr.functionName != "printf" && instr.functionName != "sprintf") {
                    for (size_t i = 0; i < fn->second.args.size(); ++i) {
                        ast::VarType actualVarType , expectedType;

                        auto one = instr.args.at(i);
                        auto t1 = table.lookup(one);
                        if(t1.has_value()) {
                            actualVarType = t1.value()->vtype;
                          
                        }

                        auto type = fn->second.args.at(i);
                        switch(type) {
                            case clib::VarType::POINTER:
                            expectedType = ast::VarType::STRING;
                            break;
                            case clib::VarType::INTEGER:
                            expectedType = ast::VarType::NUMBER;
                            break;
                            default:
                                break;
                        }
                       
                        if (actualVarType != expectedType) {
                            std::cerr << "ETL: Type mismatch for argument " << i << " " << static_cast<int>(actualVarType) << ":" << static_cast<int>(expectedType) << " " << instr.args.at(i) << " in function " << instr.functionName << "\n";
                            exit(EXIT_FAILURE);
                        }
                    }
                }

                if (fn->second.allocated || fn->first == "str") {
                    if (allocatedMemory[curFunction].find(instr.dest) == allocatedMemory[curFunction].end()) {
                        allocatedMemory[curFunction].insert(instr.dest);
                    }
                }
                if(fn->first == "str") {
                    table.enter(instr.dest);
                    auto it = table.lookup(instr.dest);
                    if(it.has_value()) {
                        it.value()->vtype = ast::VarType::STRING;
                    }
                }
            } else {
                auto f = table.lookupFunc(instr.functionName);
                if(f.has_value()) {
                    if(f.value()->num_args != instr.args.size()) {
                        std::cerr << "ETL: Error function argument count doesn't match call to: " << instr.functionName << "\n";
                        exit(EXIT_FAILURE);
                    }
                    ast::VarType returnType = f.value()->vtype;
                    table.enter(instr.dest);
                    auto loc = table.lookup(instr.dest);
                    if(loc.has_value()) {
                        loc.value()->name = instr.dest;
                        loc.value()->vtype = returnType;
                    }

                       switch (returnType) {
                        case ast::VarType::STRING:
                                if(variableInfo[instr.functionName][instr.transfer_var].type == VariableType::VAR_STRING) {
                                    variableInfo[curFunction][instr.dest].type = VariableType::VAR_STRING;
                                    allocatedMemory[curFunction].insert(instr.dest);
                                }
                            break;
                        case ast::VarType::NUMBER:
                            variableInfo[curFunction][instr.dest].type = VariableType::VAR;
                            break;
                        case ast::VarType::POINTER:
                             variableInfo[curFunction][instr.dest].type = VariableType::POINTER;
                             break;
                        default:
                            std::cerr << "ETL: Unsupported return type for local function " << instr.functionName << "\n";
                            exit(EXIT_FAILURE);
                    }
                }
            }

        

            cargs.clear();
        }

        void emitLabel(std::ostringstream &output, const ir::IRInstruction &instr) {
#ifdef __APPLE__
            output << ".globl _" << instr.dest << "\n";
            output << "_" << instr.dest << ":\n";
#else
            output << ".globl " << instr.dest << "\n";
            output << instr.dest << ":\n";
#endif
            curFunction = instr.dest;
            local.enterScope(curFunction);
            paramIndex = 0;
            if (instr.dest != "main") {
                emitFunctionPrologue(output, instr.dest);
            }
            
        }

        void emitReturn(std::ostringstream &output, const ir::IRInstruction &instr) {

            if(variableInfo[curFunction][instr.dest].type == VariableType::VAR_STRING) {
                auto val = allocatedMemory[curFunction].find(instr.dest);
                if(val != allocatedMemory[curFunction].end()) {
                    allocatedMemory[curFunction].erase(val);
                }
            } 

            std::string prefix;
#ifdef __APPLE__
            prefix = "_";
#endif

            
            /*
            for (const auto &var : allocatedMemory[curFunction]) {
                if(variableInfo[curFunction][var].type == VariableType::VAR_STRING) {
                    loadToRegister(output, var, "%rdi");
                    output << "    call " << prefix << "free # local variable: "<<var<<"\n";
                }
            } 

            for(const auto &var : ownedMemory[curFunction]) {
                    loadToRegister(output, var, "%rdi");
                    output << "    call " << prefix << "free # ownership transfer: "<<var<<"\n";
            }*/


            if (!instr.dest.empty()) {
                loadToRegister(output, instr.dest, "%rax");
            } else {
                output << "    movq $0, %rax\n";
            }
            
            emitFunctionEpilogue(output);
            local.exitScope();
        }

        void loadToRegister(std::ostringstream &output, const std::string &operand, const std::string &reg) {
            if (operand[0] == '$' || operand[0] == '%') {
                output << "    movq " << operand << ", " << reg << " # " << operand << "," << reg << "\n";
            } 
            else if(isdigit(operand[0])) {
                output << "    movq " << "$" << operand << ", " << reg << " # operand -> reg \n";
            } 
            else {
                int offset = getVariableOffset(operand);
                output << "    movq " << offset << "(%rbp), " << reg << " # " << operand << " # " << operand << "\n";
            }
        }

        void storeToTemp(std::ostringstream &output, const std::string &temp, const std::string &reg) {
            if (valueLocations[curFunction][temp] == reg) {
                return; 
            }
            int offset = getVariableOffset(temp);
            valueToStackOffset[curFunction][temp] = offset;
            output << "    movq " << reg << ", " << offset << "(%rbp)\n";
            valueLocations[curFunction][temp] = reg;
        }

        int getVariableOffset(const std::string &varName) {
            if (variableOffsets[curFunction].find(varName) == variableOffsets[curFunction].end()) {
                currentStackOffset[curFunction] -= 8;
                variableOffsets[curFunction][varName] = currentStackOffset[curFunction];
                //maxStackUsage[curFunction] = std::min(maxStackUsage[curFunction], currentStackOffset[curFunction]);
            }
            return variableOffsets[curFunction][varName];
        }

        std::string getOperand(const std::string &operand) {
            if (operand.empty() || operand[0] == '$' || operand[0] == '%') {
                return operand;
            }
            int offset = getVariableOffset(operand);
            return std::to_string(offset) + "(%rbp)";
        }

        std::string optimizeRedundantLoadStore(const std::string &assemblyCode) {
            std::istringstream input(assemblyCode);
            std::ostringstream output;
            std::string line;
            std::string lastStoreLine;
            std::string lastStoreLocation;
            std::string lastStoreRegister;
            bool lastWasStore = false;

            std::regex movqRegex(R"(movq\s+([^\s,]+)\s*,\s*([^\s]+))");

            while (std::getline(input, line)) {
                std::smatch match;
                if (std::regex_search(line, match, movqRegex)) {
                    std::string src = match[1];
                    std::string dest = match[2];

                    if (lastWasStore) {
                        if (src == lastStoreLocation && dest == lastStoreRegister) {
                            lastWasStore = false; 
                            continue;
                        }
                    }

                    if (dest.find("(%rbp)") != std::string::npos) {
                        lastStoreLine = line;
                        lastStoreLocation = dest;
                        lastStoreRegister = src;
                        lastWasStore = true;
                    } else {
                        lastWasStore = false;
                    }
                } else {
                    lastWasStore = false;
                }

                output << line << "\n";
            }
            return output.str();
        }

        std::string applyPeephole(std::ostringstream &output) {
            return optimizeRedundantLoadStore(output.str());
        }
    };

}

#endif