/**
 * @file ast.hpp
 * @brief AST node hierarchy for the MXVM parser using the Visitor pattern
 * @author Jared Bruni
 */
#ifndef __AST_HPP_X_
#define __AST_HPP_X_

#include "mxvm/instruct.hpp"
#include <memory>
#include <string>
#include <vector>

namespace mxvm {

    class ASTVisitor;

    /** @brief Abstract base class for all AST nodes */
    class ASTNode {
      public:
        virtual ~ASTNode() = default;
        virtual void accept(ASTVisitor &visitor) = 0;
        virtual std::string toString() const = 0;
    };

    /** @brief Represents a program section (.data, .code, .module, .object) */
    class SectionNode : public ASTNode {
      public:
        /** @brief Section type discriminator */
        enum SectionType {
            DATA,
            CODE,
            MODULE,
            OBJECT
        };
        SectionType type;
        std::vector<std::unique_ptr<ASTNode>> statements;

        SectionNode(SectionType sectionType) : type(sectionType) {}

        void addStatement(std::unique_ptr<ASTNode> stmt) {
            statements.push_back(std::move(stmt));
        }

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief Root AST node representing an entire program or object */
    class ProgramNode : public ASTNode {
      public:
        std::string name;
        bool object = false;            ///< true if this node represents an object rather than a program
        std::string root_name;
        std::vector<std::unique_ptr<SectionNode>> sections;
        std::vector<std::unique_ptr<ProgramNode>> inlineObjects;

        void addSection(std::unique_ptr<SectionNode> section) {
            sections.push_back(std::move(section));
        }

        void addInlineObject(std::unique_ptr<ProgramNode> obj) {
            inlineObjects.push_back(std::move(obj));
        }

        virtual void accept(ASTVisitor &visitor) override;
        virtual std::string toString() const override;
    };

    /** @brief AST node for a variable declaration in a .data section */
    class VariableNode : public ASTNode {
      public:
        std::string name;
        VarType type;
        std::string initialValue;
        bool hasInitializer;
        size_t buffer_size = 0;         ///< buffer allocation size (for pointer/array types)
        bool is_global = false;
        std::string object;             ///< owning object name, if any

        VariableNode(VarType varType, const std::string &varName)
            : name(varName), type(varType), hasInitializer(false), buffer_size(0) {}

        VariableNode(VarType varType, const std::string &varName, size_t buf_size)
            : name(varName), type(varType), hasInitializer(false), buffer_size(buf_size) {}

        VariableNode(VarType varType, const std::string &varName, const std::string &value)
            : name(varName), type(varType), initialValue(value), hasInitializer(true), buffer_size(0) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a module import declaration */
    class ModuleNode : public ASTNode {
      public:
        std::string name;
        ModuleNode(const std::string &n) : name(n) {}
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for an object import declaration */
    class ObjectNode : public ASTNode {
      public:
        std::string name;
        ObjectNode(const std::string &o) : name(o) {}
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node representing a single MXVM instruction with its operands */
    class InstructionNode : public ASTNode {
      public:
        Inc instruction;                ///< instruction opcode
        std::vector<Operand> operands;

        InstructionNode(Inc inst) : instruction(inst) {}

        InstructionNode(Inc inst, const std::vector<Operand> &ops)
            : instruction(inst), operands(ops) {}

        void addOperand(const Operand &operand) {
            operands.push_back(operand);
        }

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a label (or function entry point) in the code section */
    class LabelNode : public ASTNode {
      public:
        std::string name;
        bool function = false;          ///< true if this label marks a callable function
        LabelNode(const std::string &labelName) : name(labelName) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a literal expression value */
    class ExpressionNode : public ASTNode {
      public:
        std::string value;
        bool isNumeric;

        ExpressionNode(const std::string &val, bool numeric = false)
            : value(val), isNumeric(numeric) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a source-level comment */
    class CommentNode : public ASTNode {
      public:
        std::string text;
        CommentNode(const std::string &commentText) : text(commentText) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief Visitor interface for traversing the AST node hierarchy */
    class ASTVisitor {
      public:
        virtual ~ASTVisitor() = default;
        virtual void visit(ProgramNode &node) = 0;
        virtual void visit(SectionNode &node) = 0;
        virtual void visit(InstructionNode &node) = 0;
        virtual void visit(LabelNode &node) = 0;
        virtual void visit(VariableNode &node) = 0;
        virtual void visit(ExpressionNode &node) = 0;
        virtual void visit(CommentNode &node) = 0;
        virtual void visit(ModuleNode &node) = 0;
        virtual void visit(ObjectNode &node) = 0;
    };
} // namespace mxvm

#endif