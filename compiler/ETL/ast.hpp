#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>
#include <vector>

namespace ast {

    enum class VarType { NUMBER, STRING, POINTER };
    inline std::vector<std::string> VarString { "NUMBER", "STRING", "POINTER" };

    struct ASTNode {
        virtual ~ASTNode() = default;
        virtual std::string text() const = 0;  // Pure virtual method to be implemented by subclasses
    };

    struct Expression : ASTNode {};

    struct Statement : ASTNode {
        ~Statement() override = default;
    };

    struct Literal : Expression {
        std::string value;
        types::TokenType type;

        Literal(const std::string &v) : value(v) {}
        Literal(const std::string &v, const types::TokenType &t) : value(v), type{t} {
            if(type == types::TokenType::TT_STR) {
                value = "\"" + value + "\"";
            }
        }

        std::string text() const override {
            return value;  
        }
    };

    struct Identifier : Expression {
        std::string name;
        VarType vtype;
        Identifier(const std::string &n) : name(n), vtype{} {}

        std::string text() const override {
            return name;  
        }
    };

    struct BinaryOp : Expression {
        types::OperatorType op;
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        bool allocated;

        BinaryOp(types::OperatorType o, std::unique_ptr<Expression> l, std::unique_ptr<Expression> r, bool a = false)
            : op(o), left(std::move(l)), right(std::move(r)), allocated(a) {}

        std::string text() const override {
            return "(" + left->text() + " " + types::opName[static_cast<unsigned int>(op)] + " " + right->text() + ")";
        }
    };

    struct UnaryOp : Expression {
        types::OperatorType op;
        std::unique_ptr<Expression> operand;

        UnaryOp(types::OperatorType o, std::unique_ptr<Expression> e)
            : op(o), operand(std::move(e)) {}

        std::string text() const override {
            return types::opName[static_cast<unsigned int>(op)] + operand->text();
        }
    };

    struct Call : Statement, Expression {
        std::string functionName;
        std::vector<std::unique_ptr<Expression>> arguments;
        VarType vtype;
        Call(const std::string &name, std::vector<std::unique_ptr<Expression>> args)
            : functionName(name), arguments(std::move(args)), vtype{} {}

        std::string text() const override {
            std::string result = functionName + "(";
            for (size_t i = 0; i < arguments.size(); ++i) {
                result += arguments[i]->text();
                if (i < arguments.size() - 1) {
                    result += ", ";
                }
            }
            result += ")";
            return result;
        }
    };

    struct Assignment : Statement {
        std::unique_ptr<Expression> left;
        std::unique_ptr<Expression> right;
        VarType vtype;
        bool there = false;

        Assignment(std::unique_ptr<Expression> l, std::unique_ptr<Expression> r, VarType vtype = VarType::NUMBER, bool there = false)
            : left(std::move(l)), right(std::move(r)), vtype(vtype), there(there) {}

        std::string text() const override {
            return left->text() + " = " + right->text() + ";";
        }
    };

    struct Return : Statement {
        std::unique_ptr<Expression> return_value;
        bool allocated;

        Return(std::unique_ptr<Expression> rt, bool a = false) : return_value(std::move(rt)), allocated(false) {}

        std::string text() const override {
            return "return " + return_value->text() + ";";
        }
    };

    struct IfStatement : Statement {
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<ASTNode>> if_body;
        std::vector<std::unique_ptr<ASTNode>> else_body;

        IfStatement(std::unique_ptr<Expression> cond, 
                    std::vector<std::unique_ptr<ASTNode>> if_b, 
                    std::vector<std::unique_ptr<ASTNode>> else_b = {})
            : condition(std::move(cond)), if_body(std::move(if_b)), else_body(std::move(else_b)) {}

        std::string text() const override {
            std::string result = "if (" + condition->text() + ") {\n";
            
            for (const auto& stmt : if_body) {
                result += "  " + stmt->text() + "\n";
            }
            result += "}";

            if (!else_body.empty()) {
                result += " else {\n";
                for (const auto& stmt : else_body) {
                    result += "  " + stmt->text() + "\n";
                }
                result += "}";
            }
            return result;
        }
    };

    struct Break : Statement {
        std::string text() const override {
            return "break;";
        }
    };

    struct Continue : Statement {
        std::string text() const override {
            return "continue;";
        }
    };

    struct WhileStatement : Statement {
        std::unique_ptr<Expression> condition;
        std::vector<std::unique_ptr<ASTNode>> body;

         WhileStatement(std::unique_ptr<Expression> cond, 
                    std::vector<std::unique_ptr<ASTNode>> b)
            : condition(std::move(cond)), body(std::move(b)) {}

        std::string text() const override {
            std::ostringstream oss;
            oss << "while (" << condition->text() << ") {\n";
            
            for (const auto& statement : body) {
                oss << "    " << statement->text() << "\n";
            }
            oss << "}";
            return oss.str();
        }
    };

    struct ForStatement : Statement {
        std::unique_ptr<Expression> condition;
        std::unique_ptr<Assignment> init_statement, post;
        std::vector<std::unique_ptr<ASTNode>> body;

         ForStatement(std::unique_ptr<Assignment> init_x,std::unique_ptr<Expression> cond, std::unique_ptr<Assignment> p,
                    std::vector<std::unique_ptr<ASTNode>> b)
            : init_statement(std::move(init_x)), condition(std::move(cond)), post(std::move(p)), body(std::move(b)) {}

        std::string text() const override {
            std::ostringstream oss;
            oss << "for (" << init_statement->text() << ";" << condition->text() << ";" << post->text() << ") {\n";
            
            for (const auto& statement : body) {
                oss << "    " << statement->text() << "\n";
            }
            oss << "}";
            return oss.str();
        }
    };

    struct Function : ASTNode {
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> body;
        std::vector<std::pair<std::string,VarType>> parameters;
        ast::VarType return_type;
        Function(const std::string &n, const std::vector<std::pair<std::string, VarType>> &p, VarType vt) : name(n), parameters(std::move(p)), return_type{vt} {}
        Function(const std::string &n) : name{n} {}
        std::string text() const override {
            std::string result = "function " + name + "() {\n";
            for (const auto& stmt : body) {
                result += "  " + stmt->text() + "\n";
            }
            result += "}";
            return result;
        }
    };

    struct DefineFunction : ASTNode {
        std::string name;
        std::vector<std::unique_ptr<ASTNode>> body;
        std::vector<std::pair<std::string,VarType>> parameters;
        ast::VarType return_type;
        DefineFunction(const std::string &n, const std::vector<std::pair<std::string, VarType>> &p, VarType vt) : name(n), parameters(std::move(p)), return_type{vt} {}
        DefineFunction(const std::string &n) : name{n} {}
        std::string text() const override {
            std::string result = "define " + name + "(";
            for(auto &i : parameters) {
                result += i.first + " ";
            }
            result += ")";
            return result;
        }
    };

    struct Program : ASTNode {
        std::vector<std::unique_ptr<ASTNode>> body;

        Program() = default;

        std::string text() const override {
            std::string result;
            for (const auto& stmt : body) {
                result += stmt->text() + "\n";
            }
            return result;
        }
    };

}

#endif