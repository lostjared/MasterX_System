/**
 * @file ast.cpp
 * @brief AST node accept() and toString() implementations for the MXVM Visitor pattern
 * @author Jared Bruni
 */

#include "mxvm/ast.hpp"
#include <sstream>

namespace mxvm {

    void ProgramNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ProgramNode::toString() const {
        std::ostringstream oss;
        std::string ptype = (object == true) ? "Object" : "Program";
        oss << ptype << " {\n";
        for (const auto &section : sections) {
            oss << "  " << section->toString() << "\n";
        }
        oss << "}";
        return oss.str();
    }

    void SectionNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string SectionNode::toString() const {
        std::ostringstream oss;
        oss << "Section " << (type == DATA ? "data" : "code") << " {\n";
        for (const auto &stmt : statements) {
            oss << "    " << stmt->toString() << "\n";
        }
        oss << "  }";
        return oss.str();
    }

    void InstructionNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string InstructionNode::toString() const {
        std::ostringstream oss;
        oss << "Instruction: " << instruction;
        for (const auto &operand : operands) {
            oss << " " << operand.op;
        }
        return oss.str();
    }

    void VariableNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string VariableNode::toString() const {
        std::ostringstream oss;
        oss << "Variable: " << name << " (type: " << type << ")";
        if (hasInitializer) {
            oss << " = " << initialValue;
        }
        return oss.str();
    }

    void ModuleNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ModuleNode::toString() const {
        std::ostringstream stream;
        stream << "Module: (" << name << ")";
        return stream.str();
    }

    void ObjectNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ObjectNode::toString() const {
        std::ostringstream stream;
        stream << "Object: (" << name << ")";
        return stream.str();
    }

    void LabelNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string LabelNode::toString() const {
        return "Label: " + name;
    }

    void ExpressionNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string ExpressionNode::toString() const {
        return "Expression: " + value + (isNumeric ? " (numeric)" : " (symbolic)");
    }

    void CommentNode::accept(ASTVisitor &visitor) {
        visitor.visit(*this);
    }

    std::string CommentNode::toString() const {
        return "Comment: " + text;
    }
} // namespace mxvm
