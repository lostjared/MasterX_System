/**
 * @file icode_gen.cpp
 * @brief x86-64 System V ABI native code generator for Linux/macOS
 * @author Jared Bruni
 */
#include "mxvm/icode.hpp"

#include <algorithm>

namespace mxvm {

    static int error_label_count = 0;

    std::string Program::getPlatformSymbolName(const std::string &name) {
        if (platform == Platform::DARWIN) {
            if (!name.empty() && name[0] != '_')
                return "_" + name;
            else
                return name;
        }
        return name;
    }

    void Program::sysv_analyzeRegAlloc(bool uses_std_module) {
        sysv_reg_vars.clear();
        sysv_reg_save_order.clear();

        // %rbx is reserved for stack alignment scratch
        // %rsi, %rdi are caller-saved on System V (not available)
        std::vector<std::string> available_regs = {"%r14", "%r15"};
        if (!uses_std_module) {
            available_regs.push_back("%r12");
            available_regs.push_back("%r13");
        }

        std::unordered_map<std::string, int> usage_count;
        auto countOp = [&](const Operand &op) {
            if (!op.op.empty() && isVariable(op.op)) {
                Variable &v = getVariable(op.op);
                if (v.type == VarType::VAR_INTEGER && !v.is_global) {
                    usage_count[op.op]++;
                }
            }
        };

        for (const auto &instr : inc) {
            countOp(instr.op1);
            countOp(instr.op2);
            countOp(instr.op3);
            for (const auto &vop : instr.vop)
                countOp(vop);
        }

        std::vector<std::pair<std::string, int>> sorted_vars(usage_count.begin(), usage_count.end());
        std::sort(sorted_vars.begin(), sorted_vars.end(),
                  [](const auto &a, const auto &b) { return a.second > b.second; });

        size_t n = std::min(available_regs.size(), sorted_vars.size());
        for (size_t i = 0; i < n; ++i) {
            if (sorted_vars[i].second >= 2) {
                sysv_reg_vars[sorted_vars[i].first] = available_regs[i];
                sysv_reg_save_order.push_back(available_regs[i]);
            }
        }
    }

    void Program::sysv_emitFlushRegs(std::ostream &out) {
        for (const auto &[var, reg] : sysv_reg_vars) {
            out << "\tmovq " << reg << ", " << getMangledName(var) << "(%rip)\n";
        }
    }

    void Program::sysv_emitReloadRegs(std::ostream &out) {
        for (const auto &[var, reg] : sysv_reg_vars) {
            out << "\tmovq " << getMangledName(var) << "(%rip), " << reg << "\n";
        }
    }

    void Program::sysv_emitSaveRegs(std::ostream &out) {
        for (const auto &reg : sysv_reg_save_order) {
            out << "\tpushq " << reg << "\n";
        }
    }

    void Program::sysv_emitRestoreRegs(std::ostream &out) {
        for (auto it = sysv_reg_save_order.rbegin(); it != sysv_reg_save_order.rend(); ++it) {
            out << "\tpopq " << *it << "\n";
        }
    }

    void Program::sysv_emitStoreVar(std::ostream &out, const std::string &srcReg, const Operand &dest) {
        auto it = sysv_reg_vars.find(dest.op);
        if (it != sysv_reg_vars.end()) {
            if (srcReg != it->second)
                out << "\tmovq " << srcReg << ", " << it->second << "\n";
        } else {
            out << "\tmovq " << srcReg << ", " << getMangledName(dest) << "(%rip)\n";
        }
    }

    void Program::sysv_emitStoreVarImm(std::ostream &out, const std::string &imm, const Operand &dest) {
        auto it = sysv_reg_vars.find(dest.op);
        if (it != sysv_reg_vars.end()) {
            out << "\tmovq $" << imm << ", " << it->second << "\n";
        } else {
            out << "\tmovq $" << imm << ", " << getMangledName(dest) << "(%rip)\n";
        }
    }

    void Program::sysv_emitLoadVar(std::ostream &out, const std::string &dstReg, const Operand &src) {
        auto it = sysv_reg_vars.find(src.op);
        if (it != sysv_reg_vars.end()) {
            if (dstReg != it->second)
                out << "\tmovq " << it->second << ", " << dstReg << "\n";
        } else {
            out << "\tmovq " << getMangledName(src) << "(%rip), " << dstReg << "\n";
        }
    }

    void Program::generateCode(const Platform &platform, bool obj, std::ostream &out) {

        if (platform == Platform::WINX64) {
            x64_generateCode(platform, obj, out);
            return;
        }

        this->add_standard();
        std::unordered_map<int, std::string> labels_;
        for (auto &l : labels) {
            labels_[l.second.first] = l.first;
        }

        bool uses_std_module = false;
        for (const auto &e : external) {
            if (e.mod == "std") {
                uses_std_module = true;
                break;
            }
        }

        if (platform == Platform::DARWIN)
            out << ".section __DATA, __data\n";
        else
            out << ".section .data\n";

        std::vector<std::string> var_names;
        for (auto &v : vars) {
            var_names.push_back(v.first);
        }
        std::sort(var_names.begin(), var_names.end());

        for (auto &v : var_names) {
            const std::string var_out_name = getPlatformSymbolName(getMangledName(v));
            auto varx = getVariable(v);
            if (varx.type == VarType::VAR_INTEGER) {
                out << "\t" << var_out_name << ": .quad " << vars[v].var_value.int_value << "\n";
                continue;
            }
            if (varx.type == VarType::VAR_FLOAT) {
                out << "\t" << var_out_name << ": .double " << vars[v].var_value.float_value << "\n";
                continue;
            }
            if (varx.type == VarType::VAR_BYTE) {
                out << "\t" << var_out_name << ": .byte " << vars[v].var_value.int_value << "\n";
            }
        }
        for (auto &v : var_names) {
            auto varx = getVariable(v);
        }

        if (platform == Platform::LINUX) {
            out << "\t.extern " << getPlatformSymbolName("stderr") << "\n";
            out << "\t.extern " << getPlatformSymbolName("stdin") << "\n";
            out << "\t.extern " << getPlatformSymbolName("stdout") << "\n";
        }

        for (auto &v : var_names) {
            auto varx = getVariable(v);
            if (varx.type == VarType::VAR_STRING && varx.var_value.buffer_size == 0) {
                const std::string var_out_name = getPlatformSymbolName(getMangledName(v));
                out << "\t" << var_out_name << ": .asciz " << "\"" << escapeNewLines(vars[v].var_value.str_value) << "\"\n";
                continue;
            }
        }

        if (base != nullptr) {

            for (auto &v : var_names) {
                auto varx = getVariable(v);
                auto t = varx.type;
                if (varx.is_global || object) {
                    switch (t) {
                    case VarType::VAR_BYTE:
                    case VarType::VAR_FLOAT:
                    case VarType::VAR_INTEGER:
                        out << "\t.global " << getMangledName(v) << "\n";
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if (platform == Platform::DARWIN)
            out << ".section __DATA,__bss\n";
        else
            out << ".section .bss\n";

        for (auto &v : var_names) {
            const std::string var_out_name = getPlatformSymbolName(getMangledName(v));
            auto varx = getVariable(v);
            if (varx.type == VarType::VAR_POINTER) {
                out << "\t.comm " << var_out_name << ", 8\n";
            } else if (varx.type == VarType::VAR_STRING && varx.var_value.buffer_size > 0) {
                out << "\t.comm " << var_out_name << ", " << varx.var_value.buffer_size << "\n";
            }
        }
        if (object) {
            for (auto &v : var_names) {
                auto varx = getVariable(v);
                if (varx.type == VarType::VAR_STRING || varx.type == VarType::VAR_POINTER) {
                    out << "\t.global " << getMangledName(v) << "\n";
                }
            }
        }
        if (platform == Platform::DARWIN)
            out << ".section __TEXT, __text\n";
        else
            out << ".section .text\n";

        for (auto &lbl : labels) {
            if (lbl.second.second == true) {
                out << "\t.global " << name + "_" + lbl.first << "\n";
            }
        }
        std::sort(external.begin(), external.end(), [](const ExternalFunction &a, const ExternalFunction &b) {
            if (a.mod == b.mod)
                return a.name < b.name;
            return a.mod < b.mod;
        });

        for (auto &e : external) {
            if (e.module == true)
                out << "\t.extern " << getPlatformSymbolName(e.name) << "\n";
            else
                out << "\t.extern " << getPlatformSymbolName(e.mod + "_" + e.name) << "\n";
        }

        sysv_analyzeRegAlloc(uses_std_module);

        if (!this->object) {
            out << "\t.p2align 4, 0x90\n";
            out << ".global " << "main" << "\n";
            out << "main" << ":\n";
            out << "\tpush %rbp\n";
            out << "\tmov %rsp, %rbp\n";
            out << "\tpush %rbx\n";
            if (uses_std_module) {
                out << "\tpush %r12\n";
                out << "\tpush %r13\n";
                out << "\tsub $8, %rsp\n";
                out << "\tmov %rdi, %r12\n";
                out << "\tmov %rsi, %r13\n";
                out << "\tmovq %rsp, %rbx\n";
                out << "\tandq $-16, %rsp\n";
                out << "\tcall " << getPlatformSymbolName("set_program_args") << "\n";
                out << "\tmovq %rbx, %rsp\n";
            } else {
                out << "\tsub $8, %rsp\n";
            }
            sysv_emitSaveRegs(out);
            sysv_emitReloadRegs(out);
        }

        bool done_found = false;

        for (size_t i = 0; i < inc.size(); ++i) {
            const Instruction &instr = inc[i];
            for (auto l : labels) {
                if (l.second.first == i && l.second.second) {
                    out << "\t.p2align 4, 0x90\n";
                    out << getPlatformSymbolName(name + "_" + l.first) << ":\n";
                    out << "\tpush %rbp\n";
                    out << "\tmov %rsp, %rbp\n";
                    out << "\tpush %rbx\n";
                    out << "\tsub $8, %rsp\n";
                    sysv_emitSaveRegs(out);
                    sysv_emitReloadRegs(out);
                    if (isVariable("rax") && getVariable("rax").type == VarType::VAR_INTEGER) {
                        Operand rax_op;
                        rax_op.op = "rax";
                        sysv_emitStoreVar(out, "%rax", rax_op);
                    }
                    break;
                }
            }
            for (auto l : labels) {
                if (l.second.first == i && !l.second.second) {
                    out << "." << l.first << ":\n";
                }
            }
            if (instr.instruction == DONE)
                done_found = true;
            generateInstruction(out, instr);
        }

#ifndef __EMSCRIPTEN__
        if (this->object == false && done_found == false)
            throw mx::Exception("Program missing done to signal completion.\n");
#endif
        if (platform == Platform::LINUX)
            out << "\n\n\n.section .note.GNU-stack,\"\",@progbits\n\n";

        std::string mainFunc = " Object";
        if (root_name == name)
            mainFunc = " Program";
        std::cout << Col("MXVM: Compiled: ", mx::Color::BRIGHT_BLUE) << name << ".s" << mainFunc << Col(" platform: ", mx::Color::BRIGHT_CYAN) << ((platform == Platform::LINUX) ? "Linux" : "macOS") << "\n";
    }

    void Program::generateInstruction(std::ostream &out, const Instruction &i) {
        if (i.instruction < JMP || i.instruction > JNS) {
            last_cmp_type = CMP_NONE;
        }
        switch (i.instruction) {
        case ADD:
            gen_arth(out, "add", i);
            break;
        case SUB:
            gen_arth(out, "sub", i);
            break;
        case MUL:
            gen_arth(out, "mul", i);
            break;
        case PRINT:
            gen_print(out, i);
            break;
        case EXIT:
            gen_exit(out, i);
            break;
        case MOV:
            gen_mov(out, i);
            break;
        case JMP:
        case JE:
        case JNE:
        case JL:
        case JLE:
        case JG:
        case JGE:
        case JZ:
        case JNZ:
        case JA:
        case JB:
            gen_jmp(out, i);
            break;
        case CMP:
            gen_cmp(out, i);
            break;
        case ALLOC:
            gen_alloc(out, i);
            break;
        case FREE:
            gen_free(out, i);
            break;
        case REALLOC:
            gen_realloc(out, i);
            break;
        case LOAD:
            gen_load(out, i);
            break;
        case STORE:
            gen_store(out, i);
            break;
        case RET:
            gen_ret(out, i);
            break;
        case CALL:
            gen_call(out, i);
            break;
        case DONE:
            gen_done(out, i);
            break;
        case AND:
            gen_bitop(out, "and", i);
            break;
        case OR:
            gen_bitop(out, "or", i);
            break;
        case XOR:
            gen_bitop(out, "xor", i);
            break;
        case NOT:
            gen_not(out, i);
            break;
        case DIV:
            gen_div(out, i);
            break;
        case MOD:
            gen_mod(out, i);
            break;
        case PUSH:
            gen_push(out, i);
            break;
        case POP:
            gen_pop(out, i);
            break;
        case STACK_LOAD:
            gen_stack_load(out, i);
            break;
        case STACK_STORE:
            gen_stack_store(out, i);
            break;
        case STACK_SUB:
            gen_stack_sub(out, i);
            break;
        case GETLINE:
            gen_getline(out, i);
            break;
        case TO_INT:
            gen_to_int(out, i);
            break;
        case TO_FLOAT:
            gen_to_float(out, i);
            break;
        case INVOKE:
            gen_invoke(out, i);
            break;
        case RETURN:
            gen_return(out, i);
            break;
        case NEG:
            gen_neg(out, i);
            break;
        case FCMP:
            gen_fcmp(out, i);
            break;
        case JAE:
            gen_jae(out, i);
            break;
        case JBE:
            gen_jbe(out, i);
            break;
        case JC:
            gen_jc(out, i);
            break;
        case JNC:
            gen_jnc(out, i);
            break;
        case JP:
            gen_jp(out, i);
            break;
        case JNP:
            gen_jnp(out, i);
            break;
        case JO:
            gen_jo(out, i);
            break;
        case JNO:
            gen_jno(out, i);
            break;
        case JS:
            gen_js(out, i);
            break;
        case JNS:
            gen_jns(out, i);
            break;
        case LEA:
            gen_lea(out, i);
            break;
        default:
            throw mx::Exception("Invalid or unsupported instruction: " + std::to_string(static_cast<unsigned int>(i.instruction)));
        }
    }
    void Program::gen_done(std::ostream &out, const Instruction &i) {
        bool uses_std_module = false;
        for (const auto &e : external) {
            if (e.mod == "std") {
                uses_std_module = true;
                break;
            }
        }

        if (uses_std_module && !this->object) {
            sysv_emitFlushRegs(out);
            out << "\tmovq %rsp, %rbx\n";
            out << "\tandq $-16, %rsp\n";
            out << "\tcall " << getPlatformSymbolName("free_program_args") << "\n";
            out << "\tmovq %rbx, %rsp\n";
        } else {
            sysv_emitFlushRegs(out);
        }

        out << "\txor %eax, %eax\n";
        sysv_emitRestoreRegs(out);
        if (!this->object && uses_std_module) {
            out << "\tmovq -24(%rbp), %r13\n";
            out << "\tmovq -16(%rbp), %r12\n";
        }
        out << "\tmovq -8(%rbp), %rbx\n";
        out << "\tleave\n";
        out << "\tret\n";
    }

    void Program::gen_ret(std::ostream &out, const Instruction &i) {
        sysv_emitFlushRegs(out);
        if (isVariable("rax") && getVariable("rax").type == VarType::VAR_INTEGER) {
            Operand rax_op;
            rax_op.op = "rax";
            sysv_emitLoadVar(out, "%rax", rax_op);
        }
        sysv_emitRestoreRegs(out);
        out << "\tmovq -8(%rbp), %rbx\n";
        out << "\tleave\n";
        out << "\tret\n";
    }

    void Program::gen_return(std::ostream &out, const Instruction &i) {
        if (isVariable(i.op1.op)) {
            Variable &v = getVariable(i.op1.op);
            if (this->last_call_returns_owned_ptr) {
                v.var_value.owns = true;
            }
            switch (v.type) {
            case VarType::VAR_FLOAT:
                out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                break;
            default:
                sysv_emitStoreVar(out, "%rax", i.op1);
            }

        } else {
            throw mx::Exception("return: " + i.op1.op + " requires variable\n");
        }
    }

    void Program::gen_neg(std::ostream &out, const Instruction &i) {
        if (isVariable(i.op1.op)) {
            Variable &v = getVariable(i.op1.op);
            if (v.type == VarType::VAR_INTEGER) {
                generateLoadVar(out, v.type, "%rcx", i.op1);
                out << "\tnegq %rcx\n";
                sysv_emitStoreVar(out, "%rcx", i.op1);
            } else if (v.type == VarType::VAR_FLOAT) {
                generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op1);
                out << "\txorpd %xmm1, %xmm1\n";
                out << "\tsubsd %xmm0, %xmm1\n";
                out << "\tmovsd %xmm1, " << getMangledName(i.op1) << "(%rip)\n";
            } else {
                throw mx::Exception("neg requires float or integer.");
            }
        } else {
            throw mx::Exception("neg requires variable");
        }
    }

    void Program::gen_lea(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op))
            throw mx::Exception("LEA destination must be a variable");
        if (!isVariable(i.op2.op))
            throw mx::Exception("LEA source must be a variable");

        // Flush source register to memory before taking its address
        auto it = sysv_reg_vars.find(i.op2.op);
        if (it != sysv_reg_vars.end()) {
            out << "\tmovq " << it->second << ", " << getMangledName(i.op2) << "(%rip)\n";
        }

        out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rax\n";
        out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
        getVariable(i.op1.op).var_value.owns = false;
    }

    void Program::gen_invoke(std::ostream &out, const Instruction &i) {
        if (i.op1.op.empty()) {
            throw mx::Exception("invoke instruction requires operand of instruction name");
        }
        std::vector<Operand> op;
        op.push_back(i.op1);
        if (!i.op2.op.empty())
            op.push_back(i.op2);
        if (!i.op3.op.empty())
            op.push_back(i.op3);
        for (const auto &vx : i.vop) {
            if (!vx.op.empty())
                op.push_back(vx);
        }
        generateInvokeCall(out, op);
    }

    void Program::generateInvokeCall(std::ostream &out, std::vector<Operand> &op) {
        if (op.empty() || op[0].op.empty()) {
            throw mx::Exception("invoke instruction requires operand of instruction name");
        }
        std::string name = op[0].op;
        std::vector<Operand> args;
        for (size_t i = 1; i < op.size(); ++i) {
            if (!op[i].op.empty())
                args.push_back(op[i]);
        }

        if (isFunctionReturningOwnedPtr(name)) {
            this->last_call_returns_owned_ptr = true;
        } else {
            this->last_call_returns_owned_ptr = false;
        }
        generateFunctionCall(out, name, args);
    }

    void Program::generateFunctionCall(std::ostream &out, const std::string &name, std::vector<Operand> &op) {
        const std::vector<Operand> &args = op;
        xmm_offset = 0;
        size_t stack_args = (args.size() > 6) ? (args.size() - 6) : 0;

        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";

        if (stack_args) {
            size_t stack_bytes = stack_args * 8;
            size_t aligned = (stack_bytes + 15) & ~(size_t)15;
            out << "\tsubq $" << aligned << ", %rsp\n";
            for (size_t idx = 6, slot = 0; idx < args.size(); ++idx, ++slot) {
                generateLoadVar(out, VarType::VAR_INTEGER, "%rax", args[idx]);
                out << "\tmovq %rax, " << (slot * 8) << "(%rsp)\n";
            }
        }

        int reg_count = 0;
        for (size_t z = 0; z < args.size() && reg_count < 6; ++z, ++reg_count) {
            generateLoadVar(out, reg_count, args[z]);
        }

        out << "\txor %eax, %eax\n";
        out << "\tcall " << getPlatformSymbolName(name) << "\n";
        out << "\tmovq %rbx, %rsp\n";
    }

    void Program::gen_call(std::ostream &out, const Instruction &i) {
        if (isFunctionValid(i.op1.op)) {
            sysv_emitFlushRegs(out);
            if (isVariable("rax") && getVariable("rax").type == VarType::VAR_INTEGER) {
                Operand rax_op;
                rax_op.op = "rax";
                sysv_emitLoadVar(out, "%rax", rax_op);
            }
            out << "\tmovq %rsp, %rbx\n";
            out << "\tandq $-16, %rsp\n";
            out << "\tcall " << getPlatformSymbolName(getMangledName(i.op1)) << "\n";
            out << "\tmovq %rbx, %rsp\n";
            if (isVariable("rax") && getVariable("rax").type == VarType::VAR_INTEGER) {
                Operand rax_op;
                rax_op.op = "rax";
                out << "\tmovq %rax, " << getMangledName(rax_op) << "(%rip)\n";
            }
            sysv_emitReloadRegs(out);
        } else {
            throw mx::Exception("Function " + i.op1.op + " not found!");
        }
    }

    void Program::gen_alloc(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("ALLOC destination must be a variable");
        }

        Variable &v = getVariable(i.op1.op);
        if (v.type != VarType::VAR_POINTER) {
            throw mx::Exception("ALLOC destination must be a pointer\n");
        }

        if (!i.op2.op.empty()) {
            if (isVariable(i.op2.op)) {
                sysv_emitLoadVar(out, "%rsi", i.op2);
            } else {
                out << "\tmovq $" << i.op2.op << ", %rsi\n";
            }
        } else {
            out << "\tmovq $8, %rsi\n";
        }

        if (!i.op3.op.empty()) {
            if (isVariable(i.op3.op)) {
                sysv_emitLoadVar(out, "%rdi", i.op3);
            } else if (i.op3.type == OperandType::OP_CONSTANT) {
                out << "\tmovq $" << i.op3.op << ", %rdi\n";
            } else {
                throw mx::Exception("ALLOC: invalid count operand " + i.op3.op);
            }
        } else {
            out << "\tmovq $1, %rdi\n";
        }
        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";
        out << "\tcall " << getPlatformSymbolName("calloc") << "\n";
        out << "\tmovq %rbx, %rsp\n";
        out << "\ttest %rax, %rax\n";
        out << "\tjz .alloc_failed_" << error_label_count << "\n";
        out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
        out << "\tjmp .alloc_done_" << error_label_count << "\n";
        out << ".alloc_failed_" << error_label_count << ":\n";
        out << "\tmovq $0, " << getMangledName(i.op1) << "(%rip)\n";
        out << ".alloc_done_" << error_label_count << ":\n";
        error_label_count++;
        v.var_value.owns = true;
    }

    /**
     * @brief Generate System V x86-64 native code for REALLOC
     *
     * Computes total = count * elemSize, calls C realloc() with the
     * existing pointer and new size, and stores the result back.
     * Stack is 16-byte aligned around the call.  Registers are
     * flushed/reloaded via sysv_emitFlushRegs / sysv_emitReloadRegs.
     */
    void Program::gen_realloc(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("REALLOC destination must be a variable");
        }
        Variable &v = getVariable(i.op1.op);
        if (v.type != VarType::VAR_POINTER) {
            throw mx::Exception("REALLOC destination must be a pointer");
        }

        sysv_emitFlushRegs(out);

        // Compute new total size = count * elemSize into %rsi
        // %rdi = existing ptr (for realloc)
        out << "\tmovq " << getMangledName(i.op1) << "(%rip), %rdi\n";

        if (!i.op3.op.empty()) {
            if (isVariable(i.op3.op)) {
                sysv_emitLoadVar(out, "%rax", i.op3);
            } else {
                out << "\tmovq $" << i.op3.op << ", %rax\n";
            }
        } else {
            out << "\tmovq $1, %rax\n";
        }

        if (!i.op2.op.empty()) {
            if (isVariable(i.op2.op)) {
                sysv_emitLoadVar(out, "%rcx", i.op2);
            } else {
                out << "\tmovq $" << i.op2.op << ", %rcx\n";
            }
        } else {
            out << "\tmovq $8, %rcx\n";
        }

        out << "\timulq %rcx, %rax\n";
        out << "\tmovq %rax, %rsi\n";
        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";
        out << "\tcall " << getPlatformSymbolName("realloc") << "\n";
        out << "\tmovq %rbx, %rsp\n";
        out << "\ttest %rax, %rax\n";
        out << "\tjz .realloc_failed_" << error_label_count << "\n";
        out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
        out << "\tjmp .realloc_done_" << error_label_count << "\n";
        out << ".realloc_failed_" << error_label_count << ":\n";
        out << "\t# realloc failed, keep old pointer\n";
        out << ".realloc_done_" << error_label_count << ":\n";
        error_label_count++;
        v.var_value.owns = true;
        sysv_emitReloadRegs(out);
    }

    void Program::gen_load(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("LOAD destination: " + i.op1.op + " must be a variable");
        }
        Variable &dest = getVariable(i.op1.op);

        if (!isVariable(i.op2.op)) {
            throw mx::Exception("LOAD source: " + i.op2.op + " must be a pointer variable");
        }
        Variable &ptrVar = getVariable(i.op2.op);

        if (ptrVar.type == VarType::VAR_POINTER) {
            // Flush all cached registers so pointer reads see current values
            sysv_emitFlushRegs(out);
            out << "\tmovq " << getMangledName(i.op2) << "(%rip), %rax\n";
        } else if (ptrVar.type == VarType::VAR_STRING) {
            out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rax\n";
        } else {
            throw mx::Exception("LOAD source must be a pointer or string buffer: " + i.op2.op);
        }

        if (!i.op3.op.empty()) {
            if (isVariable(i.op3.op)) {
                sysv_emitLoadVar(out, "%rcx", i.op3);
            } else {
                out << "\tmovq $" << i.op3.op << ", %rcx\n";
            }
        } else {
            out << "\txorq %rcx, %rcx\n";
        }

        if (!i.vop.empty() && !i.vop[0].op.empty()) {
            if (isVariable(i.vop[0].op)) {
                sysv_emitLoadVar(out, "%rdx", i.vop[0]);
                out << "\timulq %rdx, %rcx\n";
                out << "\taddq %rcx, %rax\n";

                switch (dest.type) {
                case VarType::VAR_INTEGER:
                case VarType::VAR_POINTER:
                case VarType::VAR_EXTERN:
                    out << "\tmovq (%rax), %rdx\n";
                    sysv_emitStoreVar(out, "%rdx", i.op1);
                    break;
                case VarType::VAR_FLOAT:
                    out << "\tmovsd (%rax), %xmm0\n";
                    out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                    break;
                case VarType::VAR_BYTE:
                    out << "\tmovzbq (%rax), %rdx\n";
                    out << "\tmovb %dl, " << getMangledName(i.op1) << "(%rip)\n";
                    break;
                default:
                    throw mx::Exception("LOAD: unsupported destination type");
                }
            } else {
                size_t stride = static_cast<size_t>(std::stoll(i.vop[0].op, nullptr, 0));

                if (stride == 1 || stride == 2 || stride == 4 || stride == 8) {
                    switch (dest.type) {
                    case VarType::VAR_INTEGER:
                    case VarType::VAR_POINTER:
                    case VarType::VAR_EXTERN:
                        out << "\tmovq (%rax,%rcx," << stride << "), %rdx\n";
                        sysv_emitStoreVar(out, "%rdx", i.op1);
                        break;
                    case VarType::VAR_FLOAT:
                        out << "\tmovsd (%rax,%rcx," << stride << "), %xmm0\n";
                        out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                        break;
                    case VarType::VAR_BYTE:
                        out << "\tmovzbq (%rax,%rcx," << stride << "), %rdx\n";
                        out << "\tmovb %dl, " << getMangledName(i.op1) << "(%rip)\n";
                        break;
                    default:
                        throw mx::Exception("LOAD: unsupported destination type");
                    }
                } else {
                    out << "\timulq $" << stride << ", %rcx\n";
                    out << "\taddq %rcx, %rax\n";

                    switch (dest.type) {
                    case VarType::VAR_INTEGER:
                    case VarType::VAR_POINTER:
                    case VarType::VAR_EXTERN:
                        out << "\tmovq (%rax), %rdx\n";
                        sysv_emitStoreVar(out, "%rdx", i.op1);
                        break;
                    case VarType::VAR_FLOAT:
                        out << "\tmovsd (%rax), %xmm0\n";
                        out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                        break;
                    case VarType::VAR_BYTE:
                        out << "\tmovzbq (%rax), %rdx\n";
                        out << "\tmovb %dl, " << getMangledName(i.op1) << "(%rip)\n";
                        break;
                    default:
                        throw mx::Exception("LOAD: unsupported destination type");
                    }
                }
            }
        } else {
            switch (dest.type) {
            case VarType::VAR_INTEGER:
            case VarType::VAR_POINTER:
            case VarType::VAR_EXTERN:
                out << "\tmovq (%rax,%rcx,8), %rdx\n";
                sysv_emitStoreVar(out, "%rdx", i.op1);
                break;
            case VarType::VAR_FLOAT:
                out << "\tmovsd (%rax,%rcx,8), %xmm0\n";
                out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                break;
            case VarType::VAR_BYTE:
                out << "\tmovzbq (%rax,%rcx,8), %rdx\n";
                out << "\tmovb %dl, " << getMangledName(i.op1) << "(%rip)\n";
                break;
            default:
                throw mx::Exception("LOAD: unsupported destination type");
            }
        }
    }

    void Program::gen_store(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op2.op)) {
            throw mx::Exception("STORE destination must be a pointer variable: " + i.op2.op);
        }
        Variable &ptrVar = getVariable(i.op2.op);

        if (ptrVar.type == VarType::VAR_POINTER) {
            // Flush all cached registers before pointer store
            sysv_emitFlushRegs(out);
            out << "\tmovq " << getMangledName(i.op2) << "(%rip), %rax\n";
        } else if (ptrVar.type == VarType::VAR_STRING && ptrVar.var_value.buffer_size > 0) {
            out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rax\n";
        } else {
            throw mx::Exception("STORE must target pointer or string buffer");
        }

        if (!i.op3.op.empty()) {
            if (isVariable(i.op3.op)) {
                sysv_emitLoadVar(out, "%rcx", i.op3);
            } else {
                out << "\tmovq $" << i.op3.op << ", %rcx\n";
            }
        } else {
            out << "\txorq %rcx, %rcx\n";
        }

        if (isVariable(i.op1.op)) {
            Variable &src = getVariable(i.op1.op);
            switch (src.type) {
            case VarType::VAR_INTEGER:
            case VarType::VAR_POINTER:
            case VarType::VAR_EXTERN:
                sysv_emitLoadVar(out, "%rdx", i.op1);
                break;
            case VarType::VAR_STRING:
                out << "\tleaq " << getMangledName(i.op1) << "(%rip), %rdx\n";
                break;
            case VarType::VAR_BYTE:
                out << "\tmovzbq " << getMangledName(i.op1) << "(%rip), %rdx\n";
                break;
            case VarType::VAR_FLOAT:
                out << "\tmovsd " << getMangledName(i.op1) << "(%rip), %xmm0\n";
                break;
            default:
                throw mx::Exception("STORE: unsupported source type");
            }
        } else {
            out << "\tmovq $" << i.op1.op << ", %rdx\n";
        }

        if (!i.vop.empty() && !i.vop[0].op.empty()) {
            if (isVariable(i.vop[0].op)) {
                sysv_emitLoadVar(out, "%r8", i.vop[0]);
                out << "\timulq %r8, %rcx\n";
                out << "\taddq %rcx, %rax\n";

                if (isVariable(i.op1.op)) {
                    Variable &src = getVariable(i.op1.op);
                    if (src.type == VarType::VAR_FLOAT) {
                        out << "\tmovsd %xmm0, (%rax)\n";
                    } else if (src.type == VarType::VAR_BYTE) {
                        out << "\tmovb %dl, (%rax)\n";
                    } else {
                        out << "\tmovq %rdx, (%rax)\n";
                    }
                } else {
                    out << "\tmovq %rdx, (%rax)\n";
                }
            } else {
                size_t stride = static_cast<size_t>(std::stoll(i.vop[0].op, nullptr, 0));

                if (stride == 1 || stride == 2 || stride == 4 || stride == 8) {
                    if (isVariable(i.op1.op)) {
                        Variable &src = getVariable(i.op1.op);
                        if (src.type == VarType::VAR_FLOAT) {
                            out << "\tmovsd %xmm0, (%rax,%rcx," << stride << ")\n";
                        } else if (src.type == VarType::VAR_BYTE) {
                            out << "\tmovb %dl, (%rax,%rcx," << stride << ")\n";
                        } else {
                            out << "\tmovq %rdx, (%rax,%rcx," << stride << ")\n";
                        }
                    } else {
                        out << "\tmovq %rdx, (%rax,%rcx," << stride << ")\n";
                    }
                } else {
                    out << "\timulq $" << stride << ", %rcx\n";
                    out << "\taddq %rcx, %rax\n";

                    if (isVariable(i.op1.op)) {
                        Variable &src = getVariable(i.op1.op);
                        if (src.type == VarType::VAR_FLOAT) {
                            out << "\tmovsd %xmm0, (%rax)\n";
                        } else if (src.type == VarType::VAR_BYTE) {
                            out << "\tmovb %dl, (%rax)\n";
                        } else {
                            out << "\tmovq %rdx, (%rax)\n";
                        }
                    } else {
                        out << "\tmovq %rdx, (%rax)\n";
                    }
                }
            }
        } else {
            if (isVariable(i.op1.op)) {
                Variable &src = getVariable(i.op1.op);
                if (src.type == VarType::VAR_FLOAT) {
                    out << "\tmovsd %xmm0, (%rax,%rcx,8)\n";
                } else if (src.type == VarType::VAR_BYTE) {
                    out << "\tmovb %dl, (%rax,%rcx,8)\n";
                } else {
                    out << "\tmovq %rdx, (%rax,%rcx,8)\n";
                }
            } else {
                out << "\tmovq %rdx, (%rax,%rcx,8)\n";
            }
        }
        // Reload cached registers after pointer store (memory may have changed)
        if (ptrVar.type == VarType::VAR_POINTER) {
            sysv_emitReloadRegs(out);
        }
    }

    void Program::gen_free(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("FREE argument must be a variable");
        }
        Variable &v = getVariable(i.op1.op);
        if (v.type != VarType::VAR_POINTER) {
            throw mx::Exception("FREE argument must be a pointer");
        }

        out << "\tmovq " << getMangledName(i.op1) << "(%rip), %rdi\n";
        out << "\ttest %rdi, %rdi\n";
        out << "\tjz 1f\n";
        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";
        out << "\tcall " << getPlatformSymbolName("free") << "\n";
        out << "\tmovq %rbx, %rsp\n";
        out << "\tmovq $0, " << getMangledName(i.op1) << "(%rip)\n";
        out << "1:\n";
        v.var_value.owns = false;
    }

    void Program::gen_to_int(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("to_int requires destination variable (dest int)");
        }
        if (i.op2.op.empty()) {
            throw mx::Exception("to_int requires source operand");
        }

        Variable &dest = getVariable(i.op1.op);
        if (dest.type != VarType::VAR_INTEGER) {
            throw mx::Exception("to_int: first argument must be an integer variable");
        }

        if (isVariable(i.op2.op)) {
            Variable &src = getVariable(i.op2.op);
            switch (src.type) {
            case VarType::VAR_INTEGER:
            case VarType::VAR_POINTER:
            case VarType::VAR_EXTERN: {
                auto ra_src = sysv_reg_vars.find(i.op2.op);
                if (ra_src != sysv_reg_vars.end()) {
                    out << "\tmovq " << ra_src->second << ", %rax\n";
                } else {
                    out << "\tmovq " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %rax\n";
                }
                sysv_emitStoreVar(out, "%rax", i.op1);
                break;
            }

            case VarType::VAR_BYTE:
                out << "\tmovzbq " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %rax\n";
                sysv_emitStoreVar(out, "%rax", i.op1);
                break;

            case VarType::VAR_FLOAT:
                out << "\tmovsd " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %xmm0\n";
                out << "\tcvttsd2si %xmm0, %rax\n";
                sysv_emitStoreVar(out, "%rax", i.op1);
                break;

            case VarType::VAR_STRING:
                out << "\tleaq " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %rdi\n";
                out << "\tmovq %rsp, %rbx\n";
                out << "\tandq $-16, %rsp\n";
                out << "\tcall " << getPlatformSymbolName("atol") << "\n";
                out << "\tmovq %rbx, %rsp\n";
                sysv_emitStoreVar(out, "%rax", i.op1);
                break;

            default:
                throw mx::Exception("to_int: unsupported source variable type");
            }
        } else if (i.op2.type == OperandType::OP_CONSTANT) {
            out << "\tmovq $" << i.op2.op << ", %rax\n";
            sysv_emitStoreVar(out, "%rax", i.op1);
        } else {
            throw mx::Exception("to_int: unsupported source operand");
        }
    }
    void Program::gen_to_float(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("to_float requires destination variable (dest float)");
        }
        if (i.op2.op.empty()) {
            throw mx::Exception("to_float requires source operand");
        }

        Variable &dest = getVariable(i.op1.op);
        if (dest.type != VarType::VAR_FLOAT) {
            throw mx::Exception("to_float: first argument must be a float variable");
        }

        if (isVariable(i.op2.op)) {
            Variable &src = getVariable(i.op2.op);
            switch (src.type) {
            case VarType::VAR_FLOAT:
                out << "\tmovsd " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %xmm0\n";
                out << "\tmovsd %xmm0, " << getPlatformSymbolName(getMangledName(i.op1)) << "(%rip)\n";
                break;

            case VarType::VAR_INTEGER:
            case VarType::VAR_POINTER:
            case VarType::VAR_EXTERN: {
                auto ra_src = sysv_reg_vars.find(i.op2.op);
                if (ra_src != sysv_reg_vars.end()) {
                    out << "\tmovq " << ra_src->second << ", %rax\n";
                } else {
                    out << "\tmovq " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %rax\n";
                }
                out << "\tcvtsi2sd %rax, %xmm0\n";
                out << "\tmovsd %xmm0, " << getPlatformSymbolName(getMangledName(i.op1)) << "(%rip)\n";
                break;
            }

            case VarType::VAR_BYTE:
                out << "\tmovzbq " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %rax\n";
                out << "\tcvtsi2sd %rax, %xmm0\n";
                out << "\tmovsd %xmm0, " << getPlatformSymbolName(getMangledName(i.op1)) << "(%rip)\n";
                break;

            case VarType::VAR_STRING:
                out << "\tleaq " << getPlatformSymbolName(getMangledName(i.op2)) << "(%rip), %rdi\n";
                out << "\tmovq %rsp, %rbx\n";
                out << "\tandq $-16, %rsp\n";
                out << "\tcall " << getPlatformSymbolName("atof") << "\n";
                out << "\tmovq %rbx, %rsp\n";
                out << "\tmovsd %xmm0, " << getPlatformSymbolName(getMangledName(i.op1)) << "(%rip)\n";
                break;

            default:
                throw mx::Exception("to_float: unsupported source variable type");
            }
        } else if (i.op2.type == OperandType::OP_CONSTANT) {
            out << "\tmovq $" << i.op2.op << ", %rax\n";
            out << "\tcvtsi2sd %rax, %xmm0\n";
            out << "\tmovsd %xmm0, " << getPlatformSymbolName(getMangledName(i.op1)) << "(%rip)\n";
        } else {
            throw mx::Exception("to_float: unsupported source operand");
        }
    }

    void Program::gen_div(std::ostream &out, const Instruction &i) {
        if (i.op3.op.empty()) {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type == VarType::VAR_INTEGER) {
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op1);
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rcx", i.op2);
                    out << "\tcmpq $0, %rcx\n";
                    out << "\tje 1f\n";
                    out << "\tcqto\n";
                    out << "\tidivq %rcx\n";
                    out << "\tjmp 2f\n";
                    out << "1:\n";
                    out << "\txor %eax, %eax\n";
                    out << "2:\n";
                    sysv_emitStoreVar(out, "%rax", i.op1);
                } else if (v.type == VarType::VAR_FLOAT) {
                    generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op1);
                    generateLoadVar(out, VarType::VAR_FLOAT, "%xmm1", i.op2);
                    out << "\txorpd %xmm2, %xmm2\n";
                    out << "\tucomisd %xmm2, %xmm1\n";
                    out << "\tje 1f\n";
                    out << "\tdivsd %xmm1, %xmm0\n";
                    out << "\tjmp 2f\n";
                    out << "1:\n";
                    out << "\txorpd %xmm0, %xmm0\n";
                    out << "2:\n";
                    out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                } else {
                    throw mx::Exception("DIV: unsupported variable type");
                }
            } else {
                throw mx::Exception("DIV: first argument must be a variable");
            }
        } else {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type == VarType::VAR_INTEGER) {
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op2);
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rcx", i.op3);
                    out << "\tcmpq $0, %rcx\n";
                    out << "\tje 1f\n";
                    out << "\tcqto\n";
                    out << "\tidivq %rcx\n";
                    out << "\tjmp 2f\n";
                    out << "1:\n";
                    out << "\txor %eax, %eax\n";
                    out << "2:\n";
                    sysv_emitStoreVar(out, "%rax", i.op1);
                } else if (v.type == VarType::VAR_FLOAT) {
                    generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op2);
                    generateLoadVar(out, VarType::VAR_FLOAT, "%xmm1", i.op3);
                    out << "\txorpd %xmm2, %xmm2\n";
                    out << "\tucomisd %xmm2, %xmm1\n";
                    out << "\tje 1f\n";
                    out << "\tdivsd %xmm1, %xmm0\n";
                    out << "\tjmp 2f\n";
                    out << "1:\n";
                    out << "\txorpd %xmm0, %xmm0\n";
                    out << "2:\n";
                    out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                } else {
                    throw mx::Exception("DIV: unsupported variable type");
                }
            } else {
                throw mx::Exception("DIV: first argument must be a variable");
            }
        }
    }

    void Program::gen_mod(std::ostream &out, const Instruction &i) {
        if (i.op3.op.empty()) {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type == VarType::VAR_INTEGER) {
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op1);
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rcx", i.op2);
                    out << "\tcmpq $0, %rcx\n";
                    out << "\tje 1f\n";
                    out << "\tcqto\n";
                    out << "\tidivq %rcx\n";
                    out << "\tjmp 2f\n";
                    out << "1:\n";
                    out << "\txor %edx, %edx\n";
                    out << "2:\n";
                    sysv_emitStoreVar(out, "%rdx", i.op1);
                } else {
                    throw mx::Exception("MOD only supports integer variables");
                }
            } else {
                throw mx::Exception("MOD: first argument must be a variable");
            }
        } else {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type == VarType::VAR_INTEGER) {
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op2);
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rcx", i.op3);
                    out << "\tcmpq $0, %rcx\n";
                    out << "\tje 1f\n";
                    out << "\tcqto\n";
                    out << "\tidivq %rcx\n";
                    out << "\tjmp 2f\n";
                    out << "1:\n";
                    out << "\txor %edx, %edx\n";
                    out << "2:\n";
                    sysv_emitStoreVar(out, "%rdx", i.op1);
                } else {
                    throw mx::Exception("MOD only supports integer variables");
                }
            } else {
                throw mx::Exception("MOD: first argument must be a variable");
            }
        }
    }

    void Program::gen_getline(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("GETLINE destination must be a variable");
        }
        Variable &dest = getVariable(i.op1.op);
        if (dest.type != VarType::VAR_STRING || dest.var_value.buffer_size == 0) {
            throw mx::Exception("GETLINE destination must be a string buffer variable");
        }
        static int over_count = 0;
        out << "\tleaq " << getMangledName(i.op1) << "(%rip), %rdi\n";
        out << "\tmovq $" << dest.var_value.buffer_size << ", %rsi\n";
        out << "\tmovq " << getPlatformSymbolName("stdin") << "(%rip), %rdx\n";
        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";
        out << "\tcall " << getPlatformSymbolName("fgets") << "\n";
        out << "\tmovq %rbx, %rsp\n";
        out << "\tleaq " << getMangledName(i.op1) << "(%rip), %rdi\n";
        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";
        out << "\tcall " << getPlatformSymbolName("strlen") << "\n";
        out << "\tmovq %rbx, %rsp\n";
        out << "\tmovq %rax, %rcx\n";
        out << "\tcmp $0, %rax\n";
        out << "\tje .over" << over_count << "\n";
        out << "\tsub $1, %rcx\n";
        out << "\tleaq " << getMangledName(i.op1) << "(%rip), %rdi\n";
        out << "\tmovb $0, (%rdi, %rcx, 1)\n";
        out << ".over" << over_count << ": \n";
        over_count++;
    }

    void Program::gen_bitop(std::ostream &out, const std::string &opc, const Instruction &i) {
        if (i.op3.op.empty()) {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type == VarType::VAR_INTEGER) {
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op1);
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rcx", i.op2);
                    out << "\t" << opc << "q %rcx, %rax\n";
                    sysv_emitStoreVar(out, "%rax", i.op1);
                } else {
                    throw mx::Exception("Bitwise operations only supported for integer variables");
                }
            } else {
                throw mx::Exception("First argument of bitop instruction must be a variable.");
            }
        } else {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type == VarType::VAR_INTEGER) {
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op2);
                    generateLoadVar(out, VarType::VAR_INTEGER, "%rcx", i.op3);
                    out << "\t" << opc << "q %rcx, %rax\n";
                    sysv_emitStoreVar(out, "%rax", i.op1);
                } else {
                    throw mx::Exception("Bitwise operations only supported for integer variables");
                }
            } else {
                throw mx::Exception("First argument of bitop instruction must be a variable.");
            }
        }
    }

    void Program::gen_not(std::ostream &out, const Instruction &i) {
        if (!i.op1.op.empty()) {
            if (isVariable(i.op1.op)) {
                Variable &v = getVariable(i.op1.op);
                if (v.type != VarType::VAR_INTEGER) {
                    throw mx::Exception("NOT instruction expect integer variable");
                }
                generateLoadVar(out, VarType::VAR_INTEGER, "%rax", i.op1);
                out << "\ttestq %rax, %rax\n";
                out << "\tsete %al\n";
                out << "\tmovzbq %al, %rax\n";
                sysv_emitStoreVar(out, "%rax", i.op1);
            } else {
                throw mx::Exception("NOT instruction requires variable");
            }
        } else {
            throw mx::Exception("NOT instruction requires operand");
        }
    }

    /**
     * @brief Emit x86-64 PUSH for a variable or constant.
     *
     * Handles integer/pointer (pushq), string (lea+pushq), and float
     * variables.  Floating-point values are pushed via
     * `movsd var(%rip), %xmm0; subq $8, %rsp; movsd %xmm0, (%rsp)`
     * since x86 PUSH cannot operate directly on XMM registers.
     *
     * @param out  Assembly output stream.
     * @param i    Instruction whose op1 is the value to push.
     */
    void Program::gen_push(std::ostream &out, const Instruction &i) {
        if (isVariable(i.op1.op)) {
            Variable &v = getVariable(i.op1.op);
            if (v.type == VarType::VAR_INTEGER || v.type == VarType::VAR_POINTER || v.type == VarType::VAR_EXTERN) {
                sysv_emitLoadVar(out, "%rax", i.op1);
                out << "\tpushq %rax\n";
            } else if (v.type == VarType::VAR_STRING) {
                out << "\tleaq " << getMangledName(i.op1) << "(%rip), %rax\n";
                out << "\tpushq %rax\n";
            } else if (v.type == VarType::VAR_FLOAT) {
                out << "\tmovsd " << getMangledName(i.op1) << "(%rip), %xmm0\n";
                out << "\tsubq $8, %rsp\n";
                out << "\tmovsd %xmm0, (%rsp)\n";
            } else {
                throw mx::Exception("PUSH only supports integer, pointer, or float variables");
            }
        } else if (i.op1.type == OperandType::OP_CONSTANT) {
            out << "\tmovq $" << i.op1.op << ", %rax\n";
            out << "\tpushq %rax\n";
        } else {
            throw mx::Exception("PUSH argument must be a variable or integer constant");
        }
    }

    /**
     * @brief Emit x86-64 POP into a variable.
     *
     * Handles integer (popq + store), pointer/extern (popq + movq), and
     * float variables.  Floating-point values are popped via
     * `movsd (%rsp), %xmm0; addq $8, %rsp; movsd %xmm0, var(%rip)`
     * mirroring the SSE push sequence in gen_push().
     *
     * @param out  Assembly output stream.
     * @param i    Instruction whose op1 is the destination variable.
     */
    void Program::gen_pop(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("POP destination must be a variable");
        }
        Variable &v = getVariable(i.op1.op);
        if (v.type == VarType::VAR_INTEGER) {
            out << "\tpopq %rax\n";
            sysv_emitStoreVar(out, "%rax", i.op1);
        } else if (v.type == VarType::VAR_POINTER || v.type == VarType::VAR_EXTERN) {
            out << "\tpopq %rax\n";
            out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
        } else if (v.type == VarType::VAR_FLOAT) {
            out << "\tmovsd (%rsp), %xmm0\n";
            out << "\taddq $8, %rsp\n";
            out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
        } else {
            throw mx::Exception("POP only supports integer, pointer, or float variables");
        }
    }

    void Program::gen_stack_load(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("stack_load, requires first argument to be a variable.");
        }
        Variable &dest = getVariable(i.op1.op);
        if (i.op2.op.empty()) {
            throw mx::Exception("stack_load, requires second argument");
        }
        generateLoadVar(out, dest.type, "%rax", i.op2);
        out << "\tmovq (%rsp, %rax, 8), " << "%rcx\n";
        sysv_emitStoreVar(out, "%rcx", i.op1);
    }

    void Program::gen_stack_store(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("stack_store, requires first argument to be a variable.");
        }
        Variable &dest = getVariable(i.op1.op);
        if (i.op2.op.empty()) {
            throw mx::Exception("stack_store, requires second argument");
        }
        generateLoadVar(out, dest.type, "%rax", i.op2);
        sysv_emitLoadVar(out, "%rcx", i.op1);
        out << "\tmovq %rcx, (%rsp, %rax, 8)\n";
    }

    void Program::gen_stack_sub(std::ostream &out, const Instruction &i) {
        if (!i.op1.op.empty()) {
            if (isVariable(i.op1.op)) {
                sysv_emitLoadVar(out, "%rcx", i.op1);
            } else {
                out << "\tmovq $" << i.op1.op << ", %rcx\n";
            }
        } else {
            out << "\tmovq $1, %rcx\n";
        }
        out << "\tshl $3, %rcx\n";
        out << "\taddq %rcx, %rsp\n";
    }

    int Program::generateLoadVar(std::ostream &out, int r, const Operand &op) {
        int count = 0;

        if (isVariable(op.op)) {
            if (op.type != OperandType::OP_VARIABLE) {
                throw mx::Exception("Operand expected variable instead I found: " + op.op);
            }
            Variable &v = getVariable(op.op);

            std::string reg = getRegisterByIndex(r, v.type);
            switch (v.type) {
            case VarType::VAR_INTEGER:
            case VarType::VAR_POINTER:
            case VarType::VAR_EXTERN: {
                auto ra = sysv_reg_vars.find(op.op);
                if (ra != sysv_reg_vars.end()) {
                    if (ra->second != reg)
                        out << "\tmovq " << ra->second << ", " << reg << "\n";
                } else {
                    out << "\tmovq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                }
                count = 0;
                break;
            }
            case VarType::VAR_FLOAT:
                out << "\tmovsd " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                count = 1;
                break;
            case VarType::VAR_STRING:
                count = 0;
                out << "\tleaq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                break;
            case VarType::VAR_BYTE:
                count = 0;
                out << "\tmovzbq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                break;
            default:
                break;
            }
        } else {
            if (op.type == OperandType::OP_CONSTANT) {
                out << "\tmovq $" << op.op << ", " << getRegisterByIndex(r, VarType::VAR_INTEGER) << "\n";
            }
        }
        return count;
    }

    int Program::generateLoadVar(std::ostream &out, VarType reqType, std::string reg, const Operand &op) {
        int count = 0;
        if (isVariable(op.op)) {
            if (op.type != OperandType::OP_VARIABLE) {
                throw mx::Exception("Operand expected variable instead I found: " + op.op);
            }
            Variable &v = getVariable(op.op);
            if (reqType == VarType::VAR_FLOAT) {
                if (v.type == VarType::VAR_FLOAT) {
                    out << "\tmovsd " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                    count = 1;
                } else if (v.type == VarType::VAR_INTEGER || v.type == VarType::VAR_POINTER || v.type == VarType::VAR_EXTERN) {
                    auto ra = sysv_reg_vars.find(op.op);
                    if (ra != sysv_reg_vars.end()) {
                        out << "\tmovq " << ra->second << ", %rax\n";
                    } else {
                        out << "\tmovq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), %rax\n";
                    }
                    out << "\tcvtsi2sd %rax, " << reg << "\n";
                    count = 1;
                } else if (v.type == VarType::VAR_BYTE) {
                    out << "\tmovzbq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), %rax\n";
                    out << "\tcvtsi2sd %rax, " << reg << "\n";
                    count = 1;
                } else if (v.type == VarType::VAR_STRING) {
                    out << "\tleaq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), %rdi\n";
                    out << "\tmovq %rsp, %rbx\n";
                    out << "\tandq $-16, %rsp\n";
                    out << "\tcall " << getPlatformSymbolName("atof") << "\n";
                    out << "\tmovq %rbx, %rsp\n";
                    out << "\tmovsd %xmm0, " << reg << "\n";
                    count = 1;
                } else {
                    throw mx::Exception("LoadVar: unsupported source type for float");
                }
                return count;
            }

            if (reqType == VarType::VAR_INTEGER || reqType == VarType::VAR_POINTER || reqType == VarType::VAR_EXTERN) {
                if (v.type == VarType::VAR_INTEGER || v.type == VarType::VAR_POINTER || v.type == VarType::VAR_EXTERN) {
                    auto ra = sysv_reg_vars.find(op.op);
                    if (ra != sysv_reg_vars.end()) {
                        if (ra->second != reg)
                            out << "\tmovq " << ra->second << ", " << reg << "\n";
                    } else {
                        out << "\tmovq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                    }
                } else if (v.type == VarType::VAR_BYTE) {
                    out << "\tmovzbq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                } else if (v.type == VarType::VAR_FLOAT) {
                    out << "\tmovsd " << getPlatformSymbolName(getMangledName(op)) << "(%rip), %xmm0\n";
                    out << "\tcvttsd2si %xmm0, " << reg << "\n";
                } else if (v.type == VarType::VAR_STRING) {
                    out << "\tleaq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), %rdi\n";
                    out << "\tmovq %rsp, %rbx\n";
                    out << "\tandq $-16, %rsp\n";
                    out << "\tcall " << getPlatformSymbolName("atol") << "\n";
                    out << "\tmovq %rbx, %rsp\n";
                    out << "\tmovq %rax, " << reg << "\n";
                } else {
                    throw mx::Exception("LoadVar: unsupported source type for integer");
                }
                return count;
            }

            if (reqType == VarType::VAR_STRING) {
                if (v.type == VarType::VAR_STRING) {
                    out << "\tleaq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                } else if (v.type == VarType::VAR_INTEGER || v.type == VarType::VAR_POINTER || v.type == VarType::VAR_EXTERN) {
                    auto ra = sysv_reg_vars.find(op.op);
                    if (ra != sysv_reg_vars.end()) {
                        if (ra->second != reg)
                            out << "\tmovq " << ra->second << ", " << reg << "\n";
                    } else {
                        out << "\tmovq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                    }
                } else {
                    throw mx::Exception("LoadVar: unsupported source type for string");
                }
                return count;
            }

            switch (v.type) {
            case VarType::VAR_INTEGER:
            case VarType::VAR_POINTER:
            case VarType::VAR_EXTERN: {
                auto ra = sysv_reg_vars.find(op.op);
                if (ra != sysv_reg_vars.end()) {
                    if (ra->second != reg)
                        out << "\tmovq " << ra->second << ", " << reg << "\n";
                } else {
                    out << "\tmovq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                }
                count = 0;
                break;
            }
            case VarType::VAR_BYTE:
                out << "\tmovzbq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                break;
            case VarType::VAR_FLOAT:
                out << "\tmovsd " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                count = 1;
                break;
            case VarType::VAR_STRING:
                out << "\tleaq " << getPlatformSymbolName(getMangledName(op)) << "(%rip), " << reg << "\n";
                break;
            default:
                break;
            }
        } else {
            if (op.type == OperandType::OP_CONSTANT) {
                if (reqType == VarType::VAR_INTEGER || reqType == VarType::VAR_POINTER || reqType == VarType::VAR_EXTERN) {
                    out << "\tmovq $" << op.op << ", " << reg << "\n";
                } else if (reqType == VarType::VAR_FLOAT) {
                    out << "\tmovq $" << op.op << ", %rax\n";
                    out << "\tcvtsi2sd %rax, " << reg << "\n";
                    count = 1;
                } else if (reqType == VarType::VAR_STRING) {
                    out << "\tmovq $" << op.op << ", " << reg << "\n";
                } else {
                    out << "\tmovq $" << op.op << ", " << reg << "\n";
                }
            }
        }
        return count;
    }

    std::string Program::getRegisterByIndex(int index, VarType type) {
        if (index < 6 && (type == VarType::VAR_INTEGER || type == VarType::VAR_STRING || type == VarType::VAR_POINTER || type == VarType::VAR_EXTERN || type == VarType::VAR_BYTE)) {
            static const char *reg[] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
            return reg[index];
        } else if (index < 9 && type == VarType::VAR_FLOAT) {
            return "%xmm" + std::to_string(xmm_offset++);
        }
        return "[stack]";
    }

    void Program::gen_print(std::ostream &out, const Instruction &i) {
        xmm_offset = 0;
        out << "\tleaq " << getMangledName(i.op1) << "(%rip), %rdi\n";
        std::vector<Operand> args;
        if (!i.op2.op.empty())
            args.push_back(i.op2);
        if (!i.op3.op.empty())
            args.push_back(i.op3);
        for (const auto &v : i.vop) {
            if (!v.op.empty())
                args.push_back(v);
        }

        int total = 0;
        const size_t stack_args = (args.size() > 5) ? (args.size() - 5) : 0;

        out << "\tmovq %rsp, %rbx\n";
        out << "\tandq $-16, %rsp\n";

        if (stack_args) {
            size_t stack_bytes = stack_args * 8;
            size_t aligned = (stack_bytes + 15) & ~(size_t)15;
            out << "\tsubq $" << aligned << ", %rsp\n";
            size_t slot = 0;
            for (size_t idx = 6; idx <= args.size(); ++idx, ++slot) {
                size_t aidx = idx - 1;
                if (isVariable(args[aidx].op)) {
                    Variable &v = getVariable(args[aidx].op);
                    if (v.type == VarType::VAR_FLOAT) {
                        generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", args[aidx]);
                        out << "\tmovsd %xmm0, " << (slot * 8) << "(%rsp)\n";
                    } else {
                        generateLoadVar(out, v.type, "%rax", args[aidx]);
                        out << "\tmovq %rax, " << (slot * 8) << "(%rsp)\n";
                    }
                } else {
                    out << "\tmovq $" << args[aidx].op << ", " << (slot * 8) << "(%rsp)\n";
                }
            }
        }

        int reg_count = 1;
        for (size_t z = 0; z < args.size() && reg_count < 6; ++z, ++reg_count) {
            total += generateLoadVar(out, reg_count, args[z]);
        }
        if (total == 0) {
            out << "\txor %eax, %eax\n";
        } else {
            out << "\tmovb $" << total << ", %al\n";
        }
        out << "\tcall " << getPlatformSymbolName("printf") << "\n";
        out << "\tmovq %rbx, %rsp\n";
    }

    void Program::gen_jmp(std::ostream &out, const Instruction &i) {
        if (!i.op1.op.empty()) {
            auto pos = labels.find(i.op1.op);
            if (pos == labels.end()) {
                throw mx::Exception("Jump instruction must have valid label: " + i.op1.op);
            }
            const char *mnem = nullptr;
            switch (i.instruction) {
            case JMP:
                mnem = "jmp";
                break;
            case JE:
                mnem = "je";
                break;
            case JNE:
                mnem = "jne";
                break;
            case JZ:
                mnem = "jz";
                break;
            case JNZ:
                mnem = "jnz";
                break;

            case JL:
                mnem = (last_cmp_type == CMP_FLOAT) ? "jb" : "jl";
                break;
            case JLE:
                mnem = (last_cmp_type == CMP_FLOAT) ? "jbe" : "jle";
                break;
            case JG:
                mnem = (last_cmp_type == CMP_FLOAT) ? "ja" : "jg";
                break;
            case JGE:
                mnem = (last_cmp_type == CMP_FLOAT) ? "jae" : "jge";
                break;
            case JA:
                mnem = "ja";
                break;
            case JB:
                mnem = "jb";
                break;
            default:
                throw mx::Exception("Invalid jump opcode for gen_jmp");
            }
            out << "\t" << mnem << " ." << i.op1.op << "\n";
        } else {
            throw mx::Exception("Jump instruction must have label: " + i.op1.op);
        }
    }

    void Program::gen_cmp(std::ostream &out, const Instruction &i) {
        if (i.op2.op.empty()) {
            throw mx::Exception("CMP requires two operands");
        }

        VarType type1 = VarType::VAR_INTEGER;
        VarType type2 = VarType::VAR_INTEGER;
        if (isVariable(i.op1.op)) {
            type1 = getVariable(i.op1.op).type;
        }
        if (isVariable(i.op2.op)) {
            type2 = getVariable(i.op2.op).type;
        }

        if (type1 == VarType::VAR_FLOAT || type2 == VarType::VAR_FLOAT) {
            generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op1);
            generateLoadVar(out, VarType::VAR_FLOAT, "%xmm1", i.op2);
            out << "\tcomisd %xmm1, %xmm0\n";
            last_cmp_type = CMP_FLOAT;

        } else if (type1 == VarType::VAR_POINTER && (type2 == VarType::VAR_INTEGER || type2 == VarType::VAR_BYTE)) {
            generateLoadVar(out, type1, "%rax", i.op1);
            generateLoadVar(out, type2, "%rcx", i.op2);
            out << "\tcmpq %rcx, %rax\n";
            last_cmp_type = CMP_INTEGER;
        } else if (!isVariable(i.op2.op) && i.op2.type == OperandType::OP_CONSTANT && isVariable(i.op1.op)) {
            auto ra = sysv_reg_vars.find(i.op1.op);
            if (ra != sysv_reg_vars.end()) {
                out << "\tcmpq $" << i.op2.op << ", " << ra->second << "\n";
            } else if (type1 == VarType::VAR_BYTE) {
                out << "\tmovzbq " << getMangledName(i.op1) << "(%rip), %rax\n";
                out << "\tcmpq $" << i.op2.op << ", %rax\n";
            } else {
                out << "\tcmpq $" << i.op2.op << ", " << getMangledName(i.op1) << "(%rip)\n";
            }
            last_cmp_type = CMP_INTEGER;
        } else {
            generateLoadVar(out, type1, "%rax", i.op1);
            generateLoadVar(out, type2, "%rcx", i.op2);
            out << "\tcmpq %rcx, %rax\n";
            last_cmp_type = CMP_INTEGER;
        }
    }

    void Program::gen_fcmp(std::ostream &out, const Instruction &i) {
        if (i.op2.op.empty()) {
            throw mx::Exception("FCMP requires two operands");
        }

        generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op1);
        generateLoadVar(out, VarType::VAR_FLOAT, "%xmm1", i.op2);
        out << "\tcomisd %xmm1, %xmm0\n";
        last_cmp_type = CMP_FLOAT;
    }

    void Program::gen_jae(std::ostream &out, const Instruction &i) {
        out << "\tjae ." << i.op1.op << "\n";
    }

    void Program::gen_jbe(std::ostream &out, const Instruction &i) {
        out << "\tjbe ." << i.op1.op << "\n";
    }

    void Program::gen_jc(std::ostream &out, const Instruction &i) {
        out << "\tjc ." << i.op1.op << "\n";
    }

    void Program::gen_jnc(std::ostream &out, const Instruction &i) {
        out << "\tjnc ." << i.op1.op << "\n";
    }

    void Program::gen_jp(std::ostream &out, const Instruction &i) {
        out << "\tjp ." << i.op1.op << "\n";
    }

    void Program::gen_jnp(std::ostream &out, const Instruction &i) {
        out << "\tjnp ." << i.op1.op << "\n";
    }

    void Program::gen_jo(std::ostream &out, const Instruction &i) {
        out << "\tjo ." << i.op1.op << "\n";
    }

    void Program::gen_jno(std::ostream &out, const Instruction &i) {
        out << "\tjno ." << i.op1.op << "\n";
    }

    void Program::gen_js(std::ostream &out, const Instruction &i) {
        out << "\tjs ." << i.op1.op << "\n";
    }

    void Program::gen_jns(std::ostream &out, const Instruction &i) {
        out << "\tjns ." << i.op1.op << "\n";
    }

    void Program::gen_mov(std::ostream &out, const Instruction &i) {
        if (!isVariable(i.op1.op)) {
            throw mx::Exception("MOV destination must be a variable: " + i.op1.op);
        }
        Variable &dest = getVariable(i.op1.op);

        if (isVariable(i.op2.op)) {
            Variable &src = getVariable(i.op2.op);
            if (dest.type == VarType::VAR_FLOAT && src.type == VarType::VAR_FLOAT) {
                out << "\tmovsd " << getMangledName(i.op2) << "(%rip), %xmm0\n";
                out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
            } else if (dest.type == VarType::VAR_FLOAT && src.type != VarType::VAR_FLOAT) {
                if (src.type == VarType::VAR_BYTE) {
                    out << "\tmovzbq " << getMangledName(i.op2) << "(%rip), %rax\n";
                    out << "\tcvtsi2sd %rax, %xmm0\n";
                } else if (src.type == VarType::VAR_STRING) {
                    out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rdi\n";
                    out << "\tmovq %rsp, %rbx\n";
                    out << "\tandq $-16, %rsp\n";
                    out << "\tcall " << getPlatformSymbolName("atof") << "\n";
                    out << "\tmovq %rbx, %rsp\n";
                } else {
                    sysv_emitLoadVar(out, "%rax", i.op2);
                    out << "\tcvtsi2sd %rax, %xmm0\n";
                }
                out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
            } else if (dest.type != VarType::VAR_FLOAT && src.type == VarType::VAR_FLOAT) {
                out << "\tmovsd " << getMangledName(i.op2) << "(%rip), %xmm0\n";
                out << "\tcvttsd2si %xmm0, %rax\n";
                if (dest.type == VarType::VAR_BYTE) {
                    out << "\tmovb %al, " << getMangledName(i.op1) << "(%rip)\n";
                } else {
                    sysv_emitStoreVar(out, "%rax", i.op1);
                }
            } else if (dest.type == VarType::VAR_POINTER && src.type == VarType::VAR_STRING) {
                out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rax\n";
                out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
            } else if (dest.type == VarType::VAR_INTEGER && src.type == VarType::VAR_STRING) {
                out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rdi\n";
                out << "\tmovq %rsp, %rbx\n";
                out << "\tandq $-16, %rsp\n";
                out << "\tcall " << getPlatformSymbolName("atol") << "\n";
                out << "\tmovq %rbx, %rsp\n";
                sysv_emitStoreVar(out, "%rax", i.op1);
            } else if (dest.type == VarType::VAR_BYTE && src.type != VarType::VAR_BYTE) {
                if (src.type == VarType::VAR_FLOAT) {
                    out << "\tmovsd " << getMangledName(i.op2) << "(%rip), %xmm0\n";
                    out << "\tcvttsd2si %xmm0, %rax\n";
                    out << "\tmovb %al, " << getMangledName(i.op1) << "(%rip)\n";
                } else {
                    sysv_emitLoadVar(out, "%rax", i.op2);
                    out << "\tmovb %al, " << getMangledName(i.op1) << "(%rip)\n";
                }
            } else if (dest.type != VarType::VAR_BYTE && src.type == VarType::VAR_BYTE) {
                out << "\tmovzbq " << getMangledName(i.op2) << "(%rip), %rax\n";
                if (dest.type == VarType::VAR_FLOAT) {
                    out << "\tcvtsi2sd %rax, %xmm0\n";
                    out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
                } else {
                    sysv_emitStoreVar(out, "%rax", i.op1);
                }
            } else {
                switch (dest.type) {
                case VarType::VAR_INTEGER:
                case VarType::VAR_POINTER:
                case VarType::VAR_EXTERN:
                    sysv_emitLoadVar(out, "%rax", i.op2);
                    sysv_emitStoreVar(out, "%rax", i.op1);
                    break;
                case VarType::VAR_BYTE:
                    out << "\tmovb " << getMangledName(i.op2) << "(%rip), %al\n";
                    out << "\tmovb %al, " << getMangledName(i.op1) << "(%rip)\n";
                    break;
                case VarType::VAR_STRING:
                    out << "\tleaq " << getMangledName(i.op2) << "(%rip), %rax\n";
                    out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
                    break;
                default:
                    throw mx::Exception("MOV: unsupported destination type");
                }
            }
        } else {
            if (i.op2.type == OperandType::OP_CONSTANT) {
                if (dest.type == VarType::VAR_FLOAT) {
                    double val = std::stod(i.op2.op);
                    uint64_t bits;
                    std::memcpy(&bits, &val, sizeof(bits));
                    out << "\tmovq $" << bits << ", %rax\n";
                    out << "\tmovq %rax, " << getMangledName(i.op1) << "(%rip)\n";
                } else if (dest.type == VarType::VAR_BYTE) {
                    out << "\tmovb $" << i.op2.op << ", " << getMangledName(i.op1) << "(%rip)\n";
                } else {
                    sysv_emitStoreVarImm(out, i.op2.op, i.op1);
                }
            } else {
                throw mx::Exception("MOV: unsupported source operand type");
            }
        }
    }

    void Program::gen_arth(std::ostream &out, std::string arth, const Instruction &i) {
        auto emitIntegerOp = [&](const Operand &lhs, const Operand &rhs, const Operand &dest) {
            if (arth == "mul")
                arth = "imul";
            if (!isVariable(rhs.op) && rhs.type == OperandType::OP_CONSTANT && arth != "imul" && lhs.op == dest.op && isVariable(lhs.op)) {
                auto ra = sysv_reg_vars.find(lhs.op);
                if (ra != sysv_reg_vars.end()) {
                    out << "\t" << arth << "q $" << rhs.op << ", " << ra->second << "\n";
                } else {
                    out << "\t" << arth << "q $" << rhs.op << ", " << getMangledName(lhs) << "(%rip)\n";
                }
                return;
            }
            generateLoadVar(out, VarType::VAR_INTEGER, "%r10", lhs);
            if (!isVariable(rhs.op) && rhs.type == OperandType::OP_CONSTANT) {
                out << "\t" << arth << "q $" << rhs.op << ", %r10\n";
            } else {
                generateLoadVar(out, VarType::VAR_INTEGER, "%r11", rhs);
                out << "\t" << arth << "q %r11, %r10\n";
            }
            sysv_emitStoreVar(out, "%r10", dest);
        };

        if (i.op3.op.empty()) {
            if (!isVariable(i.op1.op)) {
                throw mx::Exception("First argument of arithmetic must be a variable: " + i.op1.op);
            }
            Variable &v = getVariable(i.op1.op);

            if (v.type == VarType::VAR_INTEGER || v.type == VarType::VAR_POINTER) {
                emitIntegerOp(i.op1, i.op2, i.op1);
            } else if (v.type == VarType::VAR_FLOAT) {
                generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op1);
                generateLoadVar(out, VarType::VAR_FLOAT, "%xmm1", i.op2);
                out << "\t" << arth << "sd %xmm1, %xmm0\n";
                out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
            } else {
                throw mx::Exception("Unsupported variable type for arithmetic");
            }
        } else {
            if (!isVariable(i.op1.op)) {
                throw mx::Exception("First argument of arithmetic must be a variable: " + i.op1.op);
            }
            Variable &v = getVariable(i.op1.op);

            if (v.type == VarType::VAR_INTEGER || v.type == VarType::VAR_POINTER) {
                emitIntegerOp(i.op2, i.op3, i.op1);
            } else if (v.type == VarType::VAR_FLOAT) {
                generateLoadVar(out, VarType::VAR_FLOAT, "%xmm0", i.op2);
                generateLoadVar(out, VarType::VAR_FLOAT, "%xmm1", i.op3);
                out << "\t" << arth << "sd %xmm1, %xmm0\n";
                out << "\tmovsd %xmm0, " << getMangledName(i.op1) << "(%rip)\n";
            } else {
                throw mx::Exception("Unsupported variable type for arithmetic");
            }
        }
    }

    void Program::gen_exit(std::ostream &out, const Instruction &i) {
        bool uses_std_module = false;
        for (const auto &e : external) {
            if (e.mod == "std") {
                uses_std_module = true;
                break;
            }
        }
        if (uses_std_module) {
            out << "\tmovq %rsp, %rbx\n";
            out << "\tandq $-16, %rsp\n";
            out << "\tcall " << getPlatformSymbolName("free_program_args") << "\n";
            out << "\tmovq %rbx, %rsp\n";
        }

        if (!i.op1.op.empty()) {
            std::vector<Operand> opz;
            opz.push_back(i.op1);
            generateFunctionCall(out, "exit", opz);
        } else {
            throw mx::Exception("exit requires argument.\n");
        }
    }
} // namespace mxvm