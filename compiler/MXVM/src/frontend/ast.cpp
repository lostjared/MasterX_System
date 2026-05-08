/**
 * @file ast.cpp
 * @brief Pascal AST node accept(), toString(), and print() implementations
 * @author Jared Bruni
 */
#include "ast.hpp"
#include <sstream>

namespace pascal {

    BlockNode::BlockNode(std::vector<std::unique_ptr<ASTNode>> decls, std::unique_ptr<CompoundStmtNode> stmt)
        : declarations(std::move(decls)), compoundStatement(std::move(stmt)) {}

    ProgramNode::ProgramNode(const std::string &p_name, std::unique_ptr<BlockNode> blk)
        : name(p_name), block(std::move(blk)) {}

    void ASTNode::print(std::ostream &out, int indent) const {
        for (int i = 0; i < indent; ++i)
            out << "  ";
        out << toString() << std::endl;
    }

    void ProgramNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ProgramNode::toString() const {
        return "Program: " + name;
    }

    void UnitNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string UnitNode::toString() const {
        return "Unit: " + name;
    }

    void BlockNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string BlockNode::toString() const {
        return "Block with " + std::to_string(declarations.size()) + " declarations";
    }

    void VarDeclNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string VarDeclNode::toString() const {
        std::ostringstream oss;
        oss << "VarDecl: ";
        for (size_t i = 0; i < identifiers.size(); ++i) {
            if (i > 0)
                oss << ", ";
            oss << identifiers[i];
        }
        oss << " : ";
        std::visit([&oss](const auto &t) {
            using T = std::decay_t<decltype(t)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << t;
            } else if constexpr (std::is_same_v<T, std::unique_ptr<ASTNode>>) {
                if (t)
                    oss << t->toString();
                else
                    oss << "<null>";
            }
        },
                   type);
        return oss.str();
    }

    void ProcDeclNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ProcDeclNode::toString() const {
        return "ProcDecl: " + name + " (" + std::to_string(parameters.size()) + " params)";
    }

    void FuncDeclNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string FuncDeclNode::toString() const {
        return "FuncDecl: " + name + " -> " + returnType;
    }

    void ParameterNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ParameterNode::toString() const {
        std::ostringstream oss;
        oss << "Param: ";
        if (isVar)
            oss << "var ";
        for (size_t i = 0; i < identifiers.size(); ++i) {
            if (i > 0)
                oss << ", ";
            oss << identifiers[i];
        }
        oss << " : " << type;
        return oss.str();
    }

    void CompoundStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string CompoundStmtNode::toString() const {
        return "CompoundStmt: " + std::to_string(statements.size()) + " statements";
    }

    void AssignmentNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string AssignmentNode::toString() const {
        return "Assignment";
    }

    void IfStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string IfStmtNode::toString() const {
        std::string s;
        if (elseStatement != nullptr) {
            s = "with else";
        }
        return "IfStmt " + s;
    }

    void WhileStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string WhileStmtNode::toString() const {
        return "WhileStmt";
    }

    void ForStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ForStmtNode::toString() const {
        return "ForStmt: " + variable + (isDownto ? " downto" : " to");
    }

    void RepeatStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void ProcCallNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ProcCallNode::toString() const {
        return "ProcCall: " + name + " (" + std::to_string(arguments.size()) + " args)";
    }

    void BinaryOpNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string BinaryOpNode::toString() const {
        return "BinaryOp: " + opToString(operator_);
    }

    std::string BinaryOpNode::opToString(OpType op) {
        switch (op) {
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case MULTIPLY:
            return "*";
        case DIVIDE:
            return "/";
        case DIV:
            return "div";
        case MOD:
            return "mod";
        case EQUAL:
            return "=";
        case NOT_EQUAL:
            return "<>";
        case LESS:
            return "<";
        case LESS_EQUAL:
            return "<=";
        case GREATER:
            return ">";
        case GREATER_EQUAL:
            return ">=";
        case AND:
            return "and";
        case OR:
            return "or";
        case IN:
            return "in";
        default:
            return "unknown";
        }
    }

    void UnaryOpNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string UnaryOpNode::toString() const {
        return "UnaryOp: " + opToString(operator_);
    }

    std::string UnaryOpNode::opToString(Operator op) {
        switch (op) {
        case PLUS:
            return "+";
        case MINUS:
            return "-";
        case NOT:
            return "not";
        default:
            return "unknown";
        }
    }

    void FuncCallNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string FuncCallNode::toString() const {
        return "FuncCall: " + name + " (" + std::to_string(arguments.size()) + " args)";
    }

    void VariableNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string VariableNode::toString() const {
        return "Variable: " + name;
    }

    void NumberNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string NumberNode::toString() const {
        return "Number: " + value + (isInteger ? " (int)" : " (real)");
    }

    void StringNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string StringNode::toString() const {
        return "String: \"" + value + "\"";
    }

    void BooleanNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string BooleanNode::toString() const {
        std::string s;
        if (value) {
            s = "true";
        } else {
            s = "false";
        }
        return "Boolean: " + s;
    }

    void EmptyStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string EmptyStmtNode::toString() const {
        return "EmptyStmt";
    }

    void ConstDeclNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ConstDeclNode::toString() const {
        return "ConstDecl";
    }

    void CaseStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string CaseStmtNode::toString() const {
        return "CaseStmt: " + std::to_string(branches.size()) + " branches";
    }

    void ArrayTypeNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ArrayTypeNode::toString() const {
        std::string elemTypeStr = "unknown";
        if (elementType) {
            elemTypeStr = elementType->toString();
        }

        std::string lowerStr = "?";
        if (lowerBound) {
            lowerStr = lowerBound->toString();
        }

        std::string upperStr = "?";
        if (upperBound) {
            upperStr = upperBound->toString();
        }

        return "ArrayType: " + elemTypeStr + "[" + lowerStr + ".." + upperStr + "]";
    }

    std::string ExitNode::toString() const { return "exit"; }

    void ArrayDeclarationNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ArrayDeclarationNode::toString() const {
        return "ArrayDecl: " + name + " of " + arrayType->toString();
    }

    void ArrayAccessNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ArrayAccessNode::toString() const {
        return "ArrayAccess: ";
    }

    void ArrayAssignmentNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ArrayAssignmentNode::toString() const {
        return "ArrayAssignment: " + arrayName + "[" + index->toString() + "] := " + value->toString();
    }

    RecordTypeNode::RecordTypeNode(std::vector<std::unique_ptr<ASTNode>> fields)
        : fields(std::move(fields)) {}

    void RecordTypeNode::accept(ASTVisitor &visitor) { visitor.visit(*this); }

    std::string RecordTypeNode::toString() const { return "RecordType"; }

    RecordDeclarationNode::RecordDeclarationNode(const std::string &name, std::unique_ptr<RecordTypeNode> recordType)
        : name(name), recordType(std::move(recordType)) {}

    void RecordDeclarationNode::accept(ASTVisitor &visitor) { visitor.visit(*this); }

    std::string RecordDeclarationNode::toString() const { return "RecordDeclaration: " + name; }

    FieldAccessNode::FieldAccessNode(std::unique_ptr<ASTNode> recordExpr, const std::string &fieldName)
        : recordExpr(std::move(recordExpr)), fieldName(fieldName) {}

    void FieldAccessNode::accept(ASTVisitor &visitor) { visitor.visit(*this); }

    std::string FieldAccessNode::toString() const { return "FieldAccess: " + fieldName; }

    void TypeDeclNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void TypeAliasNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string TypeDeclNode::toString() const {
        std::string result = "TypeDeclNode(";
        for (const auto &decl : typeDeclarations) {
            result += decl->toString() + ", ";
        }
        if (!typeDeclarations.empty())
            result.pop_back(), result.pop_back();
        result += ")";
        return result;
    }

    std::string TypeAliasNode::toString() const {
        return "TypeAliasNode(" + typeName + " = " + baseType + ")";
    }

    void ArrayTypeDeclarationNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ArrayTypeDeclarationNode::toString() const {
        return name;
    }

    void ExitNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void SimpleTypeNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void ContinueNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ContinueNode::toString() const {
        return "Continue Node";
    }

    void BreakNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string BreakNode::toString() const {
        return "Continue Node";
    }

    void NilNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void PointerTypeNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void PointerDerefNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void AddressOfNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void WithStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void GotoStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void LabelStmtNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void SetLiteralNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void SetTypeNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    void EnumTypeDeclNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

} // namespace pascal
