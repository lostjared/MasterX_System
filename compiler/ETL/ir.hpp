#ifndef _IR_X_H_
#define _IR_X_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <stack>
#include "symbol.hpp"
#include "ast.hpp"

namespace ir {

    std::string escapeString(const std::string &input);

    enum class InstructionType {
        ADD,
        SUB,
        MUL,
        DIV,
        ASSIGN,
        LOAD_CONST,
        LOAD_VAR,
        NOT,
        NEG,
        CALL,
        RETURN,
        CONCAT,
        PARAM,
        PARAM_STRING,
        PARAM_POINTER,
        DEF_PARAM,
        DEF_PARAM_STRING,
        DEF_PARAM_POINTER,
        LABEL,
        DEFINE,
        MOD,
        AND,
        XOR,
        OR,
        LSHIFT,
        RSHIFT,
        EQ,   
        NEQ, 
        LT,   
        LE,
        GT,
        GE,
        LOGICAL_AND, 
        LOGICAL_OR,
        LOGICAL_NOT,
        SUB_LABEL,
        JUMP,
        SET,
        SET_CONST,
    };

    inline std::vector<std::string> InstructionStrings{
        "ADD",
        "SUB",
        "MUL",
        "DIV",
        "ASSIGN",
        "LOAD_CONST",
        "LOAD_VAR",
        "NOT",
        "NEG",
        "CALL",
        "RETURN",
        "CONCAT",
        "PARAM",
        "PARAM_STRING",
        "PARAM_POINTER",
        "DEF_PARAM",
        "DEF_PARAM_STRING",
        "DEF_PARAM_POINTER",
        "LABEL",
        "DEFINE",
        "MOD", 
        "AND",
        "XOR",
        "OR",
        "LSHIFT",
        "RSHIFT",
        "EQ", 
        "NEQ",  
        "LT",
        "LE",
        "GT",
        "GE",
        "LOGICAL_AND",
        "LOGICAL_OR",
        "LOGICAL_NOT",
        "SUB_LABEL",
        "JUMP", 
        "SET",
        "SET_CONST"
    };

    struct IRInstruction {
        InstructionType type;
        std::string dest;
        std::string op1;
        std::string op2;
        std::vector<std::string> args;
        std::string functionName;
        std::string transfer_var;
        bool is_allocated = false;

        IRInstruction(const IRInstruction &i) : type{i.type}, dest{i.dest}, op1{i.op1}, op2{i.op2}, args(i.args), functionName(i.functionName), transfer_var(i.transfer_var), is_allocated(i.is_allocated) {}

        IRInstruction(InstructionType t, const std::string &d, const std::string &o1 = "", const std::string &o2 = "")
            : type(t), dest(d), op1(o1), op2(o2) {}

        IRInstruction(InstructionType t, const std::string &d, const std::string &funcName, const std::vector<std::string> &a)
            : type(t), dest(d), functionName(funcName), args(a) {}

        std::string toString() const {
            std::string result = InstructionStrings[static_cast<int>(type)] + " " + dest;
            if (!functionName.empty()) result += " " + functionName;
            if (!op1.empty()) result += " " + escapeString(op1);
            if (!op2.empty()) result += " " + escapeString(op2);
            for (const auto &arg : args) {
                result += " " + arg;
            }
            return result;
        }
    };

    using IRCode = std::vector<IRInstruction>;

    struct IRContext {
        IRCode instructions;
        symbol::SymbolTable table;
        std::unordered_map<std::string, int> functionLocalVarCount;

        IRContext() : instructions(), table(), functionLocalVarCount() {}
    };

    class IRException {
    public:
        IRException(const std::string &what) : why_{what} {}
        std::string why() const { return why_; }
    private:
        std::string why_;
    };

}

namespace parse {

    class IRGenerator {
    public:
        symbol::SymbolTable table;
        std::unordered_map<std::string, int> functionLocalVarCount;

        ir::IRContext generateIR(const std::unique_ptr<ast::ASTNode> &ast) { 
            ir::IRContext context;
            generate(ast.get(), context.instructions);
            context.table = table;
            context.functionLocalVarCount = functionLocalVarCount;
            return context;
        }

    private:
        int tempVarCounter = 0;
        std::string currentFunctionName;  
        std::unordered_map<std::string, std::string> functionReturnValues;  
        std::stack<std::pair<std::string, std::string>> loopLabelStack;

        std::string getNextTempVar() {
            return "t" + std::to_string(tempVarCounter++);
        }

        void generate(const ast::ASTNode *node, ir::IRCode &code) {
            if (auto program = dynamic_cast<const ast::Program*>(node)) {
                generateProgram(program, code);
            } else if (auto assign = dynamic_cast<const ast::Assignment*>(node)) {
                generateAssignment(assign, code);
            } else if (auto binOp = dynamic_cast<const ast::BinaryOp*>(node)) {
                generateBinaryOp(binOp, code);
            } else if (auto unaryOp = dynamic_cast<const ast::UnaryOp*>(node)) {
                generateUnaryOp(unaryOp, code);
            } else if (auto fdef = dynamic_cast<const ast::DefineFunction*>(node)) {
                generateDefFunction(fdef, code);
            } else if (auto func = dynamic_cast<const ast::Function*>(node)) {
                generateFunction(func, code);
            }else if (auto call = dynamic_cast<const ast::Call*>(node)) {
                generateCall(call, code);
            } else if (auto literal = dynamic_cast<const ast::Literal*>(node)) {
                generateLiteral(literal, code);
            } else if (auto identifier = dynamic_cast<const ast::Identifier*>(node)) {
                generateIdentifier(identifier, code);
            } else if (auto rt = dynamic_cast<const ast::Return*>(node)) {
                generateReturn(rt, code);
            } else if(auto if_s = dynamic_cast<const ast::IfStatement *>(node)) {
                generateIf(if_s, code);
            } else if(auto while_s = dynamic_cast<const ast::WhileStatement *>(node)) {
                generateWhile(while_s, code);
            }  if (auto break_s = dynamic_cast<const ast::Break *>(node)) {
                generateBreak(break_s, code);
            } else if (auto cont_s = dynamic_cast<const ast::Continue *>(node)) {
                generateCont(cont_s, code);
            } else if(auto for_s = dynamic_cast<const ast::ForStatement *>(node)) {
                generateFor(for_s, code);
            }
        }


        void generateProgram(const ast::Program *program, ir::IRCode &code) {
            for (const auto &stmt : program->body) {
                generate(stmt.get(), code);
            }
        }

        void generateFor(const ast::ForStatement *for_s, ir::IRCode &code) {
            std::string startLabel = "for_start_" + std::to_string(tempVarCounter);
            std::string endLabel = "for_end_" + std::to_string(tempVarCounter++);
            std::string postLabel = "for_post_" + std::to_string(tempVarCounter++);
            loopLabelStack.push({startLabel, endLabel});
            if (for_s->init_statement) {
                generate(for_s->init_statement.get(), code);
            }
            code.emplace_back(ir::InstructionType::SUB_LABEL, startLabel);
            if (for_s->condition) {
                generate(for_s->condition.get(), code);
                std::string conditionResult = lastComputedValue["result"];
                code.emplace_back(ir::InstructionType::JUMP, endLabel, conditionResult, "0");
            }
            for (const auto &stmt : for_s->body) {
                generate(stmt.get(), code);
            }
            code.emplace_back(ir::InstructionType::SUB_LABEL, postLabel);
            if (for_s->post) {
                generate(for_s->post.get(), code);
            }
            code.emplace_back(ir::InstructionType::JUMP, startLabel);
            code.emplace_back(ir::InstructionType::SUB_LABEL, endLabel);
            loopLabelStack.pop();
        }
        void generateBreak(const ast::Break *break_s, ir::IRCode &code) {
            if (loopLabelStack.empty()) {
                throw ir::IRException("Error: 'break' statement used outside of a loop");
            }
            std::string endLabel = loopLabelStack.top().second;
            code.emplace_back(ir::InstructionType::JUMP, endLabel);
        }

        void generateCont(const ast::Continue *cont_s, ir::IRCode &code) {
            if (loopLabelStack.empty()) {
                throw ir::IRException("Error: 'continue' statement used outside of a loop");
            }
            std::string startLabel = loopLabelStack.top().first;
            code.emplace_back(ir::InstructionType::JUMP, startLabel);
        }

        void generateWhile(const ast::WhileStatement *while_s, ir::IRCode &code) {
            std::string startLabel = "while_start_" + std::to_string(tempVarCounter);
            std::string endLabel = "while_end_" + std::to_string(tempVarCounter++);
            loopLabelStack.push({startLabel, endLabel});
            code.emplace_back(ir::InstructionType::SUB_LABEL, startLabel);
            generate(while_s->condition.get(), code);
            std::string conditionResult = lastComputedValue["result"];
            code.emplace_back(ir::InstructionType::JUMP, endLabel, conditionResult, "0");
            for (const auto &stmt : while_s->body) {
                generate(stmt.get(), code);
            }
            code.emplace_back(ir::InstructionType::JUMP, startLabel);
            code.emplace_back(ir::InstructionType::SUB_LABEL, endLabel);
            loopLabelStack.pop();
        }
        
        void generateIf(const ast::IfStatement *if_s, ir::IRCode &code) {
            generate(if_s->condition.get(), code);
            std::string conditionResult = lastComputedValue["result"];
            std::string ifLabel = "sublabel_if_" + std::to_string(tempVarCounter);
            std::string elseLabel = "sublabel_else_" + std::to_string(tempVarCounter);
            std::string endLabel = "sublabel_end_" + std::to_string(tempVarCounter++);
            code.emplace_back(ir::InstructionType::JUMP, elseLabel, conditionResult, "0");
            code.emplace_back(ir::InstructionType::SUB_LABEL, ifLabel);
            for (const auto &stmt : if_s->if_body) {
                generate(stmt.get(), code);
            }
            code.emplace_back(ir::InstructionType::JUMP, endLabel);
            code.emplace_back(ir::InstructionType::SUB_LABEL, elseLabel);
            if (!if_s->else_body.empty()) {
                for (const auto &stmt : if_s->else_body) {
                    generate(stmt.get(), code);
                }
            }
            code.emplace_back(ir::InstructionType::SUB_LABEL, endLabel);
        }

        std::unordered_map<std::string, std::string> string_const;

        void generateAssignment(const ast::Assignment *assign, ir::IRCode &code) {
            auto lhs = dynamic_cast<const ast::Identifier*>(assign->left.get());
            if (lhs) {
                auto rhsLiteral = dynamic_cast<const ast::Literal*>(assign->right.get());
                if (rhsLiteral) {
                    if(assign->there == false && table.is_there(lhs->name)) {
                        std::ostringstream stream;
                        stream << " Variable: " << lhs->name << " already defined in constant assignment.\n";
                        throw ir::IRException(stream.str());
                    }
                    table.enter(lhs->name);
                    auto entry = table.lookup(lhs->name);
                    if(entry.has_value()) {
                        symbol::Symbol *e = entry.value();
                        e->name = lhs->name;
                        e->value = rhsLiteral->value;      
                        if(rhsLiteral->type == types::TokenType::TT_NUM)
                            e->vtype = ast::VarType::NUMBER;
                        else if(rhsLiteral->type == types::TokenType::TT_STR)
                            e->vtype = ast::VarType::STRING;
                    }

                    std::string vname = getNextTempVar();
                    string_const[lhs->name] = vname;

                    if(assign->there == false)
                        code.emplace_back(ir::InstructionType::LOAD_CONST, vname, rhsLiteral->value);
                    else
                        code.emplace_back(ir::InstructionType::SET_CONST, lhs->name, rhsLiteral->value);


                } else {
                    generate(assign->right.get(), code);
                    std::string rhs = lastComputedValue["result"];
                    if (lastComputedValue[rhs] == lhs->name) {
                        lastComputedValue[lhs->name] = rhs;
                    } else {
                        table.enter(lhs->name);
                        auto it = table.lookup(lhs->name);
                        if(it.has_value()) {
                            symbol::Symbol *s = it.value();
                            table.enter(rhs);
                            auto r = table.lookup(rhs);
                            if(r.has_value()) {
                                s->vtype = r.value()->vtype;
                            }
                        }
                        if(assign->there == false)
                            code.emplace_back(ir::InstructionType::ASSIGN, lhs->name, rhs);
                        else
                            code.emplace_back(ir::InstructionType::SET, lhs->name, rhs);

                        lastComputedValue[lhs->name] = lhs->name;
                    }
                }
            } else {
                throw ir::IRException("IR Fatal Error: LHS of assignment is not an identifier");
            }
        }

        void generateReturn(const ast::Return *return_value, ir::IRCode &code) {
            if (return_value->return_value) {
                generate(return_value->return_value.get(), code);
                std::string result = lastComputedValue["result"];
                ir::IRInstruction t(ir::InstructionType::RETURN, result);
                t.transfer_var = result; 
                code.push_back(t);
                functionReturnValues[currentFunctionName] = result;
                
            } else {
                code.emplace_back(ir::InstructionType::RETURN, "");
            }
        }

        void generateBinaryOp(const ast::BinaryOp *binOp, ir::IRCode &code) {
            generate(binOp->left.get(), code);
            std::string leftResult = lastComputedValue["result"];
            generate(binOp->right.get(), code);
            std::string rightResult = lastComputedValue["result"];

            std::string dest = getNextTempVar();

            bool leftIsString = false;
            bool rightIsString = false;

            if (auto identifier = dynamic_cast<ast::Identifier*>(binOp->left.get())) {
                    leftIsString = identifier->vtype == ast::VarType::STRING;
            }


            if (auto identifier = dynamic_cast<ast::Identifier*>(binOp->right.get())) {
                    rightIsString = identifier->vtype == ast::VarType::STRING;
            }

            if(!leftIsString) {
                auto it = table.lookup(leftResult);
                if(it.has_value()) {
                    symbol::Symbol *s = it.value();
                    if((!s->value.empty() && s->value[0] == '\"') || s->vtype == ast::VarType::STRING)
                        leftIsString = true;
                }
            }

            
            if(!rightIsString) {
                auto it = table.lookup(rightResult);
                if(it.has_value()) {
                    symbol::Symbol *s = it.value();
                    if((!s->value.empty() && s->value[0] == '\"') || s->vtype == ast::VarType::STRING)
                        rightIsString = true;
                }
            }
     
            if (leftIsString && rightIsString) {
                ir::IRInstruction t(ir::InstructionType::CONCAT, dest, leftResult, rightResult);
                t.is_allocated = true;
                code.push_back(t);
                table.enter(dest);
                auto it = table.lookup(dest);
                if (it.has_value()) {
                    it.value()->name = dest;
                    it.value()->value = '\"';
                    it.value()->vtype = ast::VarType::STRING;
                    it.value()->allocated = true;
                }
            } else if (!leftIsString && !rightIsString) {
                    table.enter(dest);
                    auto it = table.lookup(dest);
                    if(it.has_value()) {
                        symbol::Symbol *s = it.value();
                        s->name = dest;
                        s->vtype = ast::VarType::NUMBER;
                    }

                    switch (binOp->op) {
                        case types::OperatorType::OP_PLUS:
                            code.emplace_back(ir::InstructionType::ADD, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_MINUS:
                            code.emplace_back(ir::InstructionType::SUB, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_MUL:
                            code.emplace_back(ir::InstructionType::MUL, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_DIV:
                            code.emplace_back(ir::InstructionType::DIV, dest, leftResult, rightResult);
                            break;                        case types::OperatorType::OP_MOD:
                            code.emplace_back(ir::InstructionType::MOD, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_LSHIFT:
                            code.emplace_back(ir::InstructionType::LSHIFT, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_RSHIFT:
                            code.emplace_back(ir::InstructionType::RSHIFT, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_AND:
                            code.emplace_back(ir::InstructionType::AND, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_OR:
                            code.emplace_back(ir::InstructionType::OR, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_XOR:
                            code.emplace_back(ir::InstructionType::XOR, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_EQ:
                            code.emplace_back(ir::InstructionType::EQ, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_NEQ:
                            code.emplace_back(ir::InstructionType::NEQ, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_LT:
                            code.emplace_back(ir::InstructionType::LT, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_LE:
                            code.emplace_back(ir::InstructionType::LE, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_GT:
                            code.emplace_back(ir::InstructionType::GT, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_GE:
                            code.emplace_back(ir::InstructionType::GE, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_AND_AND: 
                            code.emplace_back(ir::InstructionType::LOGICAL_AND, dest, leftResult, rightResult);
                            break;
                        case types::OperatorType::OP_OR_OR:   
                            code.emplace_back(ir::InstructionType::LOGICAL_OR, dest, leftResult, rightResult);
                            break;                        
                        default:
                            std::ostringstream stream;
                            stream << "Error: Unsupported numeric operation: " << static_cast<int>(binOp->op);
                            throw ir::IRException(stream.str());
                    }
            } else {
                std::ostringstream stream;
                stream << "Error: Binary Operator requires both operands to be of string type or both to be of number type. "
                    << "Found: " << (leftIsString ? "string" : "number") << " + " 
                    << (rightIsString ? "string" : "number") << ". Use str() to convert.";
                throw ir::IRException(stream.str());
            }

            lastComputedValue["result"] = dest;
        }
        void generateUnaryOp(const ast::UnaryOp *unaryOp, ir::IRCode &code) {
            generate(unaryOp->operand.get(), code);
            std::string result = lastComputedValue["result"];

            std::string dest = getNextTempVar();
            if (unaryOp->op == types::OperatorType::OP_TILDE) {
                code.emplace_back(ir::InstructionType::NOT, dest, result); 
            } else if (unaryOp->op == types::OperatorType::OP_NOT) {
                code.emplace_back(ir::InstructionType::LOGICAL_NOT, dest, result); 
            } else {
                code.emplace_back(ir::InstructionType::NEG, dest, result); 
            }   
            lastComputedValue["result"] = dest;
        }

        void generateFunction(const ast::Function *func, ir::IRCode &code) {
            currentFunctionName = func->name;
            code.emplace_back(ir::InstructionType::LABEL, func->name, "");
            table.enterScope(func->name);

            for (const auto &param : func->parameters) {
                table.enter(param.first);  
                auto p = table.lookup(param.first);
                if(p.has_value()) {
                    symbol::Symbol *s = p.value();
                    s->vtype = param.second;
                }
                if(param.second == ast::VarType::STRING) {
                    code.emplace_back(ir::InstructionType::PARAM_STRING, param.first, "");  
                } else if(param.second == ast::VarType::NUMBER) {
                    code.emplace_back(ir::InstructionType::PARAM, param.first, "");  
                } else if(param.second == ast::VarType::POINTER){
                    code.emplace_back(ir::InstructionType::PARAM_POINTER, param.first, "");
                }
            }
            std::vector<ast::VarType> paramTypes;
            for (const auto& param : func->parameters) {
                paramTypes.push_back(param.second); // Assuming parameters are stored as pairs <name, type>     
            }
            table.enterFunction(func->name, paramTypes, func->return_type);
            for (const auto &stmt : func->body) {
                generate(stmt.get(), code);
            }
            functionLocalVarCount[func->name] = table.getCurrentScopeSize();

            table.exitScope();
        }

        void generateDefFunction(const ast::DefineFunction *func, ir::IRCode &code) {
            code.emplace_back(ir::InstructionType::DEFINE, func->name, "");
            table.enterScope(func->name);
            for (const auto &param : func->parameters) {
                table.enter(param.first);  
                auto p = table.lookup(param.first);
                if(p.has_value()) {
                    symbol::Symbol *s = p.value();
                    s->vtype = param.second;
                }
                if(param.second == ast::VarType::STRING) {
                    code.emplace_back(ir::InstructionType::DEF_PARAM_STRING, param.first, "");  
                } else if(param.second == ast::VarType::NUMBER) {
                    code.emplace_back(ir::InstructionType::DEF_PARAM, param.first, "");  
                } else if(param.second == ast::VarType::POINTER) {
                    code.emplace_back(ir::InstructionType::DEF_PARAM_POINTER, param.first, "");
                }
            }

            std::vector<ast::VarType> paramTypes;
            for (const auto& param : func->parameters) {
                paramTypes.push_back(param.second); // Assuming parameters are stored as pairs <name, type>     
            }
            table.enterFunction(func->name, paramTypes, func->return_type);
            //table.enterFunction(func->name, func->parameters.size(), func->return_type);
            table.exitScope();
        }

        void generateLiteral(const ast::Literal *literal, ir::IRCode &code) {
            std::string tempVar = getNextTempVar();
            code.emplace_back(ir::InstructionType::LOAD_CONST, tempVar, literal->value);
            table.enter(tempVar);
            auto value = table.lookup(tempVar);
            if(value.has_value()) {
                symbol::Symbol *v = value.value();
                v->value = literal->value;
                v->name = tempVar;
                if(literal->value.empty() && literal->value[0] == '\"') {
                    v->vtype = ast::VarType::STRING;
                } else {
                    v->vtype = ast::VarType::NUMBER;
                }
            }
            lastComputedValue["result"] = tempVar;
        }

        void generateIdentifier(const ast::Identifier *identifier, ir::IRCode &code) {
            if (lastComputedValue.find(identifier->name) != lastComputedValue.end()) {
                lastComputedValue["result"] = lastComputedValue[identifier->name];
            } else {
                std::string tempVar = getNextTempVar();

                table.enter(tempVar);
                auto v = table.lookup(tempVar);
                if(v.has_value()) {
                    symbol::Symbol *vx = v.value();
                    vx->name = tempVar;
                    vx->vtype = identifier->vtype;
                    auto cpx = table.lookup(identifier->name);
                    if(cpx.has_value()) {
                        vx->value = cpx.value()->value;
                        vx->vtype = cpx.value()->vtype;
                    } else {
                        std::ostringstream stream;
                        stream << "Error variable: " << identifier->name << " not defined but used.\n";
                        throw ir::IRException(stream.str());
                    }
                }

                std::string id = identifier->name;
                if(string_const.find(id) != string_const.end()) {
                    id = string_const[id];
                }

                code.emplace_back(ir::InstructionType::LOAD_VAR, tempVar, id);
                lastComputedValue["result"] = tempVar;
            }
        }

        void generateCall(const ast::Call *call, ir::IRCode &code) {
            std::vector<std::string> argRegisters;
            for (const auto &arg : call->arguments) {
                generate(arg.get(), code);
                argRegisters.push_back(lastComputedValue["result"]);
            }
            std::string callDest = getNextTempVar();
            table.enter(callDest);
            auto e = table.lookup(callDest);
            if(e.has_value()) {
                symbol::Symbol *s = e.value();
                s->name = callDest;
                if(call->functionName == "str") {
                    s->vtype = ast::VarType::STRING;
                }
            }

            if (functionReturnValues.find(call->functionName) != functionReturnValues.end()) {
                std::string transfer_var = functionReturnValues[call->functionName];
                ir::IRInstruction instr(ir::InstructionType::CALL, callDest, call->functionName, argRegisters);
                instr.transfer_var = transfer_var;  
                code.push_back(instr);
            } else {
                code.emplace_back(ir::InstructionType::CALL, callDest, call->functionName, argRegisters);
            }
         lastComputedValue["result"] = callDest;
        }

        std::unordered_map<std::string, std::string> lastComputedValue;
    };

}

#endif