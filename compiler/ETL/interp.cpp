#include"interp.hpp"
#include"embed_func.hpp"

namespace interp {

    Interpreter::Interpreter(symbol::SymbolTable &table) : sym_tab{table}, ip{0} {

    }

    int Interpreter::execute(ir::IRCode &code, bool debug) {
        collectLabels(code);
        if(label_pos.find("init") == label_pos.end()) {
            throw Exception("Error could not find init entry point.\n");
        }
        ip = label_pos["init"];
        while(ip < static_cast<long>(code.size())) {
            const auto instr = code[ip];
            if(debug)
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
                    continue;
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
                case ir::InstructionType::LT:
                    executeLt(instr);
                    break;
                case ir::InstructionType::GT:
                    executeGt(instr);
                    break;
                case ir::InstructionType::LE:
                    executeLte(instr);
                    break;
                case ir::InstructionType::GE:
                    executeGte(instr);
                    break;
                case ir::InstructionType::EQ:
                    executeEq(instr);
                    break;
                case ir::InstructionType::NEQ:
                    executeNeq(instr);
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
                case ir::InstructionType::SUB_LABEL:
                    break;
                
                case ir::InstructionType::RETURN: {
                    executeReturn(instr);
                    continue;   
                }
                break;
                case ir::InstructionType::CALL: {
                    executeCall(instr);
                    continue;
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
        std::string curDefine;

        lf_table.addFunction("printf", lib::func_table["printf"]);

        while(ip_id < code.size()) {
            const auto instr = code[ip_id];
            if(instr.type == ir::InstructionType::SUB_LABEL) {
                sub_labels[instr.dest] = ip_id;
                ip_id++;
                continue;
            } else if(instr.type == ir::InstructionType::LABEL) {
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
            } else if(instr.type == ir::InstructionType::DEFINE) {
                curDefine = instr.dest;
                if(lib::func_table.find(instr.dest) != lib::func_table.end()) {
                    lf_table.addFunction(instr.dest, lib::func_table[instr.dest]);
                }
            } else if(instr.type == ir::InstructionType::DEF_PARAM) {
                lf_table.defineInteger(curDefine, instr.dest);
            } else if(instr.type == ir::InstructionType::DEF_PARAM_STRING) {
                lf_table.defineString(curDefine, instr.dest);
            } else if(instr.type == ir::InstructionType::DEF_PARAM_POINTER) {
                lf_table.definePointer(curDefine, instr.dest);
            }
            ip_id ++;
        }
    }

    void Interpreter::outputDebugInfo(std::ostream &out) {
        out << "Variales [ strings ]\n";
        for(auto &i : string_variables) {
            for(auto &x : i.second) {
                out << i.first << " [ " << x.first << ", " << x.second << " ]\n";
            }
        }
        out << "Variables [ numbers ]\n";
        for(auto &i : numeric_variables) {
            for(auto &x : i.second) {
                out << i.first << " [ " << x.first << ", " << x.second << " ]\n";
            }
        }
        out << "Variables [ pointers ]\n";
        for(auto &i : pointer_variables) {
            for(auto &x : i.second) {
                out << i.first << " [ " << x.first << ", " << (long)x.second << " ]\n";
            }
        }
        std::cout << "Labels: {\n";
        for(auto &i : label_pos) {
            out << i.second << " = " << i.first << "\n";
        }
        std::cout << "}\n";
        std::cout << "Sub Labels: {\n";
        for(auto &s : sub_labels) {
            out << s.second << " = " << s.first << "\n";
        }
        std::cout << "}\n";
        lf_table.print(std::cout);
    }

    void Interpreter::executeAdd(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 + val2;
    }

    void Interpreter::executeSub(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 - val2;
    }

    void Interpreter::executeMul(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 * val2;
    }

    void Interpreter::executeDiv(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        if(val2 == 0) {
            throw Exception("Divison By Zero");
        }
        numeric_variables[curFunction][instr.dest] = val1 / val2;
    }

    void Interpreter::executeMod(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 % val2;
    }

    void Interpreter::executeAnd(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 & val2;
    
    }

    void Interpreter::executeOr(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 | val2;
    }

    void Interpreter::executeXor(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 ^ val2;
    }

    void Interpreter::executeLShift(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 << val2;
    }

    void Interpreter::executeRShift(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
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
            loc_dst.value()->vtype = loc_src.value()->vtype;
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
            loc_dest.value()->vtype = loc_op1.value()->vtype;
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
        
        if(ftable.isDefined(instr.functionName)) {        
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
                    case ast::VarType::STRING: {
                        sym_tab.enter(func.arg_names[i]);
                        auto loc = sym_tab.lookup(func.arg_names[i]);
                        if(loc.has_value()) {
                            loc.value()->vtype = ast::VarType::STRING;
                            string_variables[func.functionName][func.arg_names[i]] = string_variables[curFunction][instr.args[i]];
                        }
                    }
                    break;
                    case ast::VarType::POINTER: {
                        sym_tab.enter(func.arg_names[i]);
                        auto loc = sym_tab.lookup(func.arg_names[i]);
                        if(loc.has_value()) {
                            loc.value()->vtype = ast::VarType::POINTER;
                            pointer_variables[func.functionName][func.arg_names[i]] = pointer_variables[curFunction][instr.args[i]];
                        }
                    }
                    break;
                }
            }
            long pos = label_pos[instr.functionName];
            call_stack.push_back({curFunction, instr.dest, ip});
            ip = pos;      
        } else {
            Functor *f = lf_table.getFunction(instr.functionName);
            if(f == nullptr && instr.functionName != "printf") {
                std::ostringstream stream;
                stream << "Function: " << instr.functionName << " not defined!\n";
                throw Exception(stream.str());
            }
            if(instr.functionName != "printf" && f->int_vars.size() != instr.args.size()) {
                std::ostringstream stream;
                stream << "Function: " << instr.functionName << " requires: " << f->int_vars.size() << " arguments, found: " << instr.args.size();
                throw Exception(stream.str());
            }
            std::vector<Var> v;

            if(instr.functionName != "printf") {
                for(size_t i = 0; i < f->int_vars.size(); ++i) {
                    auto loc = sym_tab.lookup(instr.args[i]);
                    if(!loc.has_value()) {
                        std::ostringstream stream;
                        stream << instr.args[i] << " not found in symbol table!\n";
                        throw Exception(stream.str());
                    }
                    v.push_back(Var(instr.args[i], loc.value()->vtype));
                    size_t off = v.size()-1;
                    switch(v[off].type) {
                        case ast::VarType::NUMBER:
                        v[off].numeric_value = numeric_variables[curFunction][instr.args[i]];
                        break;
                        case ast::VarType::STRING:
                        v[off].string_value = stripQuotes(string_variables[curFunction][instr.args[i]]);
                        break;
                        case ast::VarType::POINTER:
                        v[off].ptr_value = pointer_variables[curFunction][instr.args[i]];
                        break;
                    }
                }
            } else {
                for(size_t i = 0; i < instr.args.size(); ++i) {
                    auto loc = sym_tab.lookup(instr.args[i]);
                    if(!loc.has_value()) {
                        std::ostringstream stream;
                        stream << instr.args[i] << " not found in symbol table!\n";
                        throw Exception(stream.str());
                    }
                    v.push_back(Var(instr.args[i], loc.value()->vtype));
                    size_t off = v.size()-1;
                    switch(v[off].type) {
                        case ast::VarType::NUMBER:
                        v[off].numeric_value = numeric_variables[curFunction][instr.args[i]];
                        break;
                        case ast::VarType::STRING:
                        v[off].string_value = stripQuotes(string_variables[curFunction][instr.args[i]]);
                        break;
                        case ast::VarType::POINTER:
                        v[off].ptr_value = pointer_variables[curFunction][instr.args[i]];
                        break;
                    }
                }
            }
            sym_tab.enter(instr.dest);
            Var v_ = std::move(lf_table.callFunction(instr.functionName, v));
            auto loc = sym_tab.lookup(instr.dest);
            switch(v_.type) {
                case ast::VarType::NUMBER:
                numeric_variables[curFunction][instr.dest] = v_.numeric_value;
                break;
                case ast::VarType::STRING:
                string_variables[curFunction][instr.dest] = v_.string_value;
                break;
                case ast::VarType::POINTER:
                pointer_variables[curFunction][instr.dest] = v_.ptr_value;
                break;
            }
            loc.value()->vtype = v_.type;
            ip++;
        }
    }
    
    void Interpreter::executeReturn(const ir::IRInstruction &instr) {
        if(call_stack.empty()) {
                auto loc = sym_tab.lookup(instr.dest);
                if(loc.has_value()) {
                    if(loc.value()->vtype == ast::VarType::NUMBER) {
                        throw Exit_Exception(numeric_variables[curFunction][instr.dest]);
                    } else {
                        std::ostringstream stream;
                        stream << "Final return should return a number for status " << instr.dest << "...\n";
                        throw Exception(stream.str());
                    }
                }
            } else {
                auto pos = call_stack.back();
                call_stack.pop_back();
                std::string curScope = sym_tab.curScope();
                auto rt_var = sym_tab.lookup(instr.dest);
                if(rt_var.has_value()) {
                    sym_tab.enterScope(pos.fname);
                    sym_tab.enter(pos.rt_name);
                    auto rt_dest = sym_tab.lookup(pos.rt_name);
                    if(rt_dest.has_value()) {
                        switch(rt_var.value()->vtype) {
                            case ast::VarType::NUMBER:
                                numeric_variables[pos.fname][pos.rt_name] = numeric_variables[curScope][instr.dest];
                            break;
                            case ast::VarType::STRING:
                                string_variables[pos.fname][pos.rt_name] = string_variables[curScope][instr.dest];
                            break;
                            case ast::VarType::POINTER:
                                pointer_variables[pos.fname][pos.rt_name] = pointer_variables[curScope][instr.dest];
                            break;
                        }
                    }
                }
                ip = pos.pos;
                curFunction = pos.fname;
           }
    }
    void Interpreter::executeJump(const ir::IRInstruction &instr) {
        if(instr.op1.empty() && instr.op2.empty()) {
            ip = sub_labels[instr.dest];
            return;
        }
        long op1 = getIntegerValue(instr.op1);
        if(op1 == 0) {
            ip = sub_labels[instr.dest];
        } else {
            ip++;
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

    
    void Interpreter::executeLt(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 < val2;
    }
    
    void Interpreter::executeGt(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 > val2;
    }
    
    void Interpreter::executeLte(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 <= val2;
    }
    
    void Interpreter::executeGte(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 >= val2;
    }
    
    void Interpreter::executeEq(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 == val2;
    }
    
    void Interpreter::executeNeq(const ir::IRInstruction &instr) {
        sym_tab.enter(instr.dest);
        auto loc = sym_tab.lookup(instr.dest);
        if(loc.has_value())
            loc.value()->vtype = ast::VarType::NUMBER;
        long val1 = getIntegerValue(instr.op1);
        long val2 = getIntegerValue(instr.op2);
        numeric_variables[curFunction][instr.dest] = val1 != val2;
    }

    std::string Interpreter::stripQuotes(const std::string &value) {
        if(value[0] == '\"' && value.back() == '\"') {
            return value.substr(1, value.length()-2);
        } 
        return value; // nothing to do
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

    bool FunctionTable::isDefined(const std::string &f) {
        return func.find(f) != func.end();
    }

}
