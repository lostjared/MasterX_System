/**
 * @file ast.hpp
 * @brief AST node hierarchy for the Pascal-to-MXVM frontend parser
 * @author Jared Bruni
 */
#ifndef _PASCAL_AST_H_
#define _PASCAL_AST_H_

#include <iostream>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace pascal {

    class ASTVisitor;

    /**
     * @brief Abstract base class for all AST nodes
     *
     * Every concrete AST node inherits from this class, implementing
     * the visitor pattern via accept() and providing a debug string
     * via toString().
     */
    class ASTNode {
        int line_number = 0; ///< source line number where this node originated

      public:
        virtual ~ASTNode() = default;
        /** @brief Accept an ASTVisitor (double-dispatch) */
        virtual void accept(ASTVisitor &visitor) = 0;
        /** @brief Return a human-readable description of this node */
        virtual std::string toString() const = 0;
        /** @brief Pretty-print this node to a stream with indentation */
        virtual void print(std::ostream &out, int indent = 0) const;
        /** @brief Set the originating source line number */
        void setLineNumber(int line) { line_number = line; }
        /** @brief Get the originating source line number */
        int getLineNumber() const { return line_number; }
    };

    /** @brief AST node for an exit / halt statement */
    class ExitNode : public ASTNode {
      public:
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
        std::unique_ptr<ASTNode> expr; ///< optional exit-code expression
        ExitNode() = default;
        /** @brief Construct with an optional exit-code expression */
        explicit ExitNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    };

    /** @brief AST node for a continue statement */
    class ContinueNode : public ASTNode {
      public:
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
        std::unique_ptr<ASTNode> expr; ///< reserved expression (unused)
        ContinueNode() = default;
        explicit ContinueNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    };

    /** @brief AST node for a break statement */
    class BreakNode : public ASTNode {
      public:
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
        std::unique_ptr<ASTNode> expr; ///< reserved expression (unused)
        BreakNode() = default;
        explicit BreakNode(std::unique_ptr<ASTNode> e) : expr(std::move(e)) {}
    };

    class CompoundStmtNode;

    /**
     * @brief AST node representing a block (declarations + compound statement)
     */
    class BlockNode : public ASTNode {
      public:
        std::vector<std::unique_ptr<ASTNode>> declarations;       ///< variable / type / const / procedure / function declarations
        std::unique_ptr<CompoundStmtNode> compoundStatement;       ///< the begin..end compound statement

        /**
         * @brief Construct a block node
         * @param decls List of declaration nodes
         * @param stmt The compound statement body
         */
        BlockNode(std::vector<std::unique_ptr<ASTNode>> decls, std::unique_ptr<CompoundStmtNode> stmt);

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /**
     * @brief AST node for a complete program (program name + block)
     */
    class ProgramNode : public ASTNode {
      public:
        std::string name;                  ///< program identifier
        std::unique_ptr<BlockNode> block;  ///< the program body
        std::vector<std::string> uses;     ///< modules imported via 'uses' clause

        ProgramNode(const std::string &name, std::unique_ptr<BlockNode> blk);

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /**
     * @brief AST node for a Pascal unit (separately compiled module)
     *
     * A unit has an interface section (exported forward declarations) and
     * an implementation section (actual code).  Compiles to an MXVM object.
     */
    class UnitNode : public ASTNode {
      public:
        std::string name;                                        ///< unit identifier
        std::vector<std::string> uses;                           ///< modules/units imported via 'uses' clause
        std::vector<std::unique_ptr<ASTNode>> interfaceDecls;    ///< exported procedure/function forward declarations
        std::vector<std::unique_ptr<ASTNode>> implDecls;         ///< implementation declarations (proc/func/var/const/type)

        UnitNode(const std::string &name) : name(name) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /**
     * @brief AST node for variable declarations (var id1, id2 : type)
     */
    class VarDeclNode : public ASTNode {
      public:
        std::vector<std::string> identifiers;                            ///< declared variable names
        std::variant<std::string, std::unique_ptr<ASTNode>> type;        ///< type (simple name or complex type node)
        std::vector<std::unique_ptr<ASTNode>> initializers;              ///< optional initialisers

        VarDeclNode(std::vector<std::string> identifiers,
                    std::variant<std::string, std::unique_ptr<ASTNode>> type,
                    std::vector<std::unique_ptr<ASTNode>> initializers)
            : identifiers(std::move(identifiers)), type(std::move(type)),
              initializers(std::move(initializers)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a type declaration section */
    class TypeDeclNode : public ASTNode {
      public:
        std::vector<std::unique_ptr<ASTNode>> typeDeclarations; ///< list of individual type declarations

        TypeDeclNode(std::vector<std::unique_ptr<ASTNode>> declarations)
            : typeDeclarations(std::move(declarations)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a type alias (type NewName = ExistingType) */
    class TypeAliasNode : public ASTNode {
      public:
        std::string typeName;  ///< the new type name
        std::string baseType;  ///< the existing type being aliased

        TypeAliasNode(const std::string &name, const std::string &base)
            : typeName(name), baseType(base) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for constant declarations (const id = value) */
    class ConstDeclNode : public ASTNode {
      public:
        /** @brief A single constant name = value pair */
        struct ConstAssignment {
            std::string identifier;              ///< constant name
            std::unique_ptr<ASTNode> value;       ///< constant value expression

            ConstAssignment(const std::string &id, std::unique_ptr<ASTNode> val)
                : identifier(id), value(std::move(val)) {}
        };

        std::vector<std::unique_ptr<ConstAssignment>> assignments; ///< list of constant assignments
        ConstDeclNode(std::vector<std::unique_ptr<ConstAssignment>> assigns)
            : assignments(std::move(assigns)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a function declaration */
    class FuncDeclNode : public ASTNode {
      public:
        std::string name;                                  ///< function name
        std::vector<std::unique_ptr<ASTNode>> parameters;  ///< formal parameters
        std::string returnType;                            ///< return type name
        std::shared_ptr<ASTNode> block;                    ///< function body block

        FuncDeclNode(const std::string &n, std::vector<std::unique_ptr<ASTNode>> p, const std::string &rt, std::unique_ptr<ASTNode> b)
            : name(n), parameters(std::move(p)), returnType(rt), block(std::move(b)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a procedure declaration */
    class ProcDeclNode : public ASTNode {
      public:
        std::string name;                                  ///< procedure name
        std::vector<std::unique_ptr<ASTNode>> parameters;  ///< formal parameters
        std::shared_ptr<ASTNode> block;                    ///< procedure body block

        ProcDeclNode(const std::string &n, std::vector<std::unique_ptr<ASTNode>> p, std::unique_ptr<ASTNode> b)
            : name(n), parameters(std::move(p)), block(std::move(b)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a formal parameter list entry */
    class ParameterNode : public ASTNode {
      public:
        std::vector<std::string> identifiers;  ///< parameter names
        std::string type;                      ///< parameter type name
        bool isVar;                            ///< true if passed by reference (var parameter)

        ParameterNode(std::vector<std::string> ids, const std::string &paramType, bool var = false)
            : identifiers(std::move(ids)), type(paramType), isVar(var) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a compound statement (begin ... end) */
    class CompoundStmtNode : public ASTNode {
      public:
        std::vector<std::unique_ptr<ASTNode>> statements; ///< ordered list of statements

        CompoundStmtNode(std::vector<std::unique_ptr<ASTNode>> stmts)
            : statements(std::move(stmts)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for an assignment statement (variable := expression) */
    class AssignmentNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> variable;    ///< target of the assignment
        std::unique_ptr<ASTNode> expression;  ///< value expression

        AssignmentNode(std::unique_ptr<ASTNode> var, std::unique_ptr<ASTNode> expr)
            : variable(std::move(var)), expression(std::move(expr)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    enum class VarType;

    /** @brief AST node for a simple (named) type reference */
    class SimpleTypeNode : public ASTNode {
      public:
        std::string typeName; ///< the referenced type name

        SimpleTypeNode(const std::string &typeName) : typeName(typeName) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "SimpleType: " + typeName; }
    };

    /** @brief AST node for an array type (array[lower..upper] of elementType) */
    class ArrayTypeNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> elementType;  ///< element type node
        std::unique_ptr<ASTNode> lowerBound;   ///< lower index bound expression
        std::unique_ptr<ASTNode> upperBound;   ///< upper index bound expression

        ArrayTypeNode(std::unique_ptr<ASTNode> elementType,
                      std::unique_ptr<ASTNode> lowerBound,
                      std::unique_ptr<ASTNode> upperBound)
            : elementType(std::move(elementType)),
              lowerBound(std::move(lowerBound)),
              upperBound(std::move(upperBound)) {}

        std::string toString() const override;
        void accept(ASTVisitor &visitor) override;
    };
    /** @brief AST node for a named array variable declaration */
    class ArrayDeclarationNode : public ASTNode {
      public:
        std::string name;                                  ///< array variable name
        std::unique_ptr<ArrayTypeNode> arrayType;           ///< array type (bounds + element)
        std::vector<std::unique_ptr<ASTNode>> initializers; ///< optional initialisers

        ArrayDeclarationNode(std::string name, std::unique_ptr<ArrayTypeNode> arrayType,
                             std::vector<std::unique_ptr<ASTNode>> initializers = {})
            : name(std::move(name)), arrayType(std::move(arrayType)),
              initializers(std::move(initializers)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a named array type declaration (type Name = array[...]) */
    class ArrayTypeDeclarationNode : public ASTNode {
      public:
        std::string name;                        ///< type name
        std::unique_ptr<ArrayTypeNode> arrayType; ///< the underlying array type

        ArrayTypeDeclarationNode(const std::string &name, std::unique_ptr<ArrayTypeNode> arrayType)
            : name(name), arrayType(std::move(arrayType)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for an array element access (arr[index]) */
    class ArrayAccessNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> base;   ///< array expression
        std::unique_ptr<ASTNode> index;  ///< index expression

        ArrayAccessNode(std::unique_ptr<ASTNode> base, std::unique_ptr<ASTNode> index)
            : base(std::move(base)), index(std::move(index)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for assigning to an array element (arr[index] := value) */
    class ArrayAssignmentNode : public ASTNode {
      public:
        std::string arrayName;             ///< array variable name
        std::unique_ptr<ASTNode> index;    ///< index expression
        std::unique_ptr<ASTNode> value;    ///< value expression

        ArrayAssignmentNode(std::string arrayName, std::unique_ptr<ASTNode> index,
                            std::unique_ptr<ASTNode> value)
            : arrayName(std::move(arrayName)), index(std::move(index)),
              value(std::move(value)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for an if-then-else statement */
    class IfStmtNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> condition;       ///< boolean condition
        std::unique_ptr<ASTNode> thenStatement;    ///< then branch
        std::unique_ptr<ASTNode> elseStatement;    ///< optional else branch

        IfStmtNode(std::unique_ptr<ASTNode> cond,
                   std::unique_ptr<ASTNode> thenStmt,
                   std::unique_ptr<ASTNode> elseStmt = nullptr)
            : condition(std::move(cond)), thenStatement(std::move(thenStmt)), elseStatement(std::move(elseStmt)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a while loop (while cond do stmt) */
    class WhileStmtNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> condition;  ///< loop condition
        std::unique_ptr<ASTNode> statement;  ///< loop body

        WhileStmtNode(std::unique_ptr<ASTNode> cond, std::unique_ptr<ASTNode> stmt)
            : condition(std::move(cond)), statement(std::move(stmt)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a for loop (for var := start to/downto end do stmt) */
    class ForStmtNode : public ASTNode {
      public:
        std::string variable;                  ///< loop variable name
        std::unique_ptr<ASTNode> startValue;   ///< initial value expression
        std::unique_ptr<ASTNode> endValue;     ///< final value expression
        bool isDownto;                         ///< true if downto, false if to
        std::unique_ptr<ASTNode> statement;    ///< loop body

        ForStmtNode(const std::string &var,
                    std::unique_ptr<ASTNode> start,
                    std::unique_ptr<ASTNode> end,
                    bool downto,
                    std::unique_ptr<ASTNode> stmt)
            : variable(var), startValue(std::move(start)), endValue(std::move(end)),
              isDownto(downto), statement(std::move(stmt)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a case statement */
    class CaseStmtNode : public ASTNode {
      public:
        /** @brief A single case branch (one or more values mapping to a statement) */
        struct CaseBranch {
            std::vector<std::unique_ptr<ASTNode>> values;  ///< match values
            std::unique_ptr<ASTNode> statement;             ///< branch body

            CaseBranch(std::vector<std::unique_ptr<ASTNode>> vals, std::unique_ptr<ASTNode> stmt)
                : values(std::move(vals)), statement(std::move(stmt)) {}
        };

        std::unique_ptr<ASTNode> expression;                      ///< selector expression
        std::vector<std::unique_ptr<CaseBranch>> branches;         ///< case branches
        std::unique_ptr<ASTNode> elseStatement;                    ///< optional else/otherwise branch

        CaseStmtNode(std::unique_ptr<ASTNode> expr,
                     std::vector<std::unique_ptr<CaseBranch>> branches,
                     std::unique_ptr<ASTNode> elseStmt = nullptr)
            : expression(std::move(expr)), branches(std::move(branches)), elseStatement(std::move(elseStmt)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a procedure call statement */
    class ProcCallNode : public ASTNode {
      public:
        std::string name;                                  ///< procedure name
        std::vector<std::unique_ptr<ASTNode>> arguments;   ///< actual arguments

        ProcCallNode(const std::string &procName, std::vector<std::unique_ptr<ASTNode>> args)
            : name(procName), arguments(std::move(args)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a binary operator expression */
    class BinaryOpNode : public ASTNode {
      public:
        /** @brief Binary operator kinds */
        enum OpType {
            PLUS,
            MINUS,
            MULTIPLY,
            DIVIDE,
            DIV,
            MOD,
            EQUAL,
            NOT_EQUAL,
            LESS,
            LESS_EQUAL,
            GREATER,
            GREATER_EQUAL,
            AND,
            OR,
            IN
        };

        std::unique_ptr<ASTNode> left;   ///< left operand
        OpType operator_;                 ///< operator kind
        std::unique_ptr<ASTNode> right;   ///< right operand

        BinaryOpNode(std::unique_ptr<ASTNode> l, OpType op, std::unique_ptr<ASTNode> r)
            : left(std::move(l)), operator_(op), right(std::move(r)) {}

        BinaryOpNode(OpType op, std::unique_ptr<ASTNode> l, std::unique_ptr<ASTNode> r)
            : left(std::move(l)), operator_(op), right(std::move(r)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;

        /** @brief Convert an OpType to its string representation */
        static std::string opToString(OpType op);
    };

    /** @brief AST node for a unary operator expression */
    class UnaryOpNode : public ASTNode {
      public:
        /** @brief Unary operator kinds */
        enum Operator {
            PLUS,   ///< unary +
            MINUS,  ///< unary -
            NOT     ///< logical not
        };

        Operator operator_;              ///< the unary operator
        std::unique_ptr<ASTNode> operand; ///< the operand expression

        UnaryOpNode(Operator op, std::unique_ptr<ASTNode> operandNode)
            : operator_(op), operand(std::move(operandNode)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;

        /** @brief Convert a unary Operator to its string representation */
        static std::string opToString(Operator op);
    };

    /** @brief AST node for a function call expression */
    class FuncCallNode : public ASTNode {
      public:
        std::string name;                                  ///< function name
        std::vector<std::unique_ptr<ASTNode>> arguments;   ///< actual arguments

        FuncCallNode(const std::string &funcName, std::vector<std::unique_ptr<ASTNode>> args)
            : name(funcName), arguments(std::move(args)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a variable reference */
    class VariableNode : public ASTNode {
      public:
        std::string name; ///< variable name

        VariableNode(const std::string &varName) : name(varName) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for an integer or real numeric literal */
    class NumberNode : public ASTNode {
      public:
        std::string value;  ///< textual representation of the number
        bool isInteger;     ///< true if integer literal
        bool isReal;        ///< true if real (floating-point) literal

        NumberNode(const std::string &val, bool integer = true, bool real = false)
            : value(val), isInteger(integer), isReal(real) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a string literal */
    class StringNode : public ASTNode {
      public:
        std::string value; ///< the string contents (without quotes)

        StringNode(const std::string &val) : value(val) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a boolean literal (true / false) */
    class BooleanNode : public ASTNode {
      public:
        bool value; ///< the boolean value

        BooleanNode(bool val) : value(val) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for an empty statement (no-op) */
    class EmptyStmtNode : public ASTNode {
      public:
        EmptyStmtNode() {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for the nil literal */
    class NilNode : public ASTNode {
      public:
        NilNode() = default;

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "nil"; }
    };

    /** @brief AST node for a pointer type declaration (^BaseType) */
    class PointerTypeNode : public ASTNode {
      public:
        std::string baseTypeName; ///< the type being pointed to

        PointerTypeNode(const std::string &base) : baseTypeName(base) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "PointerType: ^" + baseTypeName; }
    };

    /** @brief AST node for a pointer dereference (ptr^) */
    class PointerDerefNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> pointer; ///< expression yielding a pointer

        PointerDerefNode(std::unique_ptr<ASTNode> ptr) : pointer(std::move(ptr)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "PointerDeref"; }
    };

    /** @brief AST node for an address-of expression (@operand) */
    class AddressOfNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> operand; ///< operand whose address is taken

        AddressOfNode(std::unique_ptr<ASTNode> op) : operand(std::move(op)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "AddressOf"; }
    };

    /** @brief AST node for a repeat..until loop */
    class RepeatStmtNode : public ASTNode {
      public:
        std::vector<std::unique_ptr<ASTNode>> statements; ///< loop body statements
        std::unique_ptr<ASTNode> condition;                ///< termination condition

        RepeatStmtNode(std::vector<std::unique_ptr<ASTNode>> stmts, std::unique_ptr<ASTNode> cond)
            : statements(std::move(stmts)), condition(std::move(cond)) {}

        void accept(ASTVisitor &visitor) override;

        std::string toString() const override {
            return "RepeatStmt";
        }
    };

    /** @brief AST node for an enumerated type declaration (type Color = (Red, Green, Blue)) */
    class EnumTypeDeclNode : public ASTNode {
      public:
        std::string typeName;                    ///< the enum type name
        std::vector<std::string> values;         ///< ordered list of enum value identifiers

        EnumTypeDeclNode(const std::string &name, std::vector<std::string> vals)
            : typeName(name), values(std::move(vals)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "EnumTypeDecl: " + typeName; }
    };

    /** @brief A single variant arm inside a variant record */
    struct VariantArm {
        std::vector<std::unique_ptr<ASTNode>> caseLabels; ///< case constant(s) for this arm
        std::vector<std::unique_ptr<ASTNode>> fields;      ///< field declarations in this arm
    };

    /** @brief AST node for a record type (record ... end) */
    class RecordTypeNode : public ASTNode {
      public:
        std::vector<std::unique_ptr<ASTNode>> fields; ///< fixed field declarations
        std::string variantTagName;                    ///< tag field name (empty if no variant part)
        std::string variantTagType;                    ///< tag field type name
        std::vector<VariantArm> variantArms;           ///< variant arms (empty if no variant part)
        RecordTypeNode(std::vector<std::unique_ptr<ASTNode>> fields);
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a named record declaration */
    class RecordDeclarationNode : public ASTNode {
      public:
        std::string name;                              ///< record type name
        std::unique_ptr<RecordTypeNode> recordType;     ///< the record body
        RecordDeclarationNode(const std::string &name, std::unique_ptr<RecordTypeNode> recordType);
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /** @brief AST node for a with statement (with record do statement) */
    class WithStmtNode : public ASTNode {
      public:
        std::string recordVar;                ///< record variable name
        std::unique_ptr<ASTNode> statement;   ///< body statement

        WithStmtNode(const std::string &recordVar, std::unique_ptr<ASTNode> stmt)
            : recordVar(recordVar), statement(std::move(stmt)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "WithStmt: " + recordVar; }
    };

    /** @brief AST node for a goto statement */
    class GotoStmtNode : public ASTNode {
      public:
        std::string label; ///< target label (numeric string)

        GotoStmtNode(const std::string &lbl) : label(lbl) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "Goto: " + label; }
    };

    /** @brief AST node for a label definition (label: statement) */
    class LabelStmtNode : public ASTNode {
      public:
        std::string label;                    ///< the label (numeric string)
        std::unique_ptr<ASTNode> statement;   ///< labeled statement

        LabelStmtNode(const std::string &lbl, std::unique_ptr<ASTNode> stmt)
            : label(lbl), statement(std::move(stmt)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "Label: " + label; }
    };

    /** @brief AST node for a set literal [val1, val2, ...] */
    class SetLiteralNode : public ASTNode {
      public:
        std::vector<std::unique_ptr<ASTNode>> elements; ///< set elements

        SetLiteralNode(std::vector<std::unique_ptr<ASTNode>> elems)
            : elements(std::move(elems)) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "SetLiteral"; }
    };
    /** @brief AST node for a set type declaration (set of <base type>) */
    class SetTypeNode : public ASTNode {
      public:
        std::string baseTypeName; ///< the ordinal base type of the set

        SetTypeNode(const std::string &base) : baseTypeName(base) {}

        void accept(ASTVisitor &visitor) override;
        std::string toString() const override { return "SetType: set of " + baseTypeName; }
    };
    /** @brief AST node for accessing a record field (record.field) */
    class FieldAccessNode : public ASTNode {
      public:
        std::unique_ptr<ASTNode> recordExpr;  ///< expression yielding a record
        std::string fieldName;                ///< name of the accessed field
        FieldAccessNode(std::unique_ptr<ASTNode> recordExpr, const std::string &fieldName);
        void accept(ASTVisitor &visitor) override;
        std::string toString() const override;
    };

    /**
     * @brief Abstract visitor interface for the AST (double-dispatch)
     *
     * Derived visitors implement visit() overloads for every concrete
     * AST node type to perform code generation, validation, or printing.
     */
    class ASTVisitor {
      public:
        virtual ~ASTVisitor() = default;
        virtual void visit(ProgramNode &node) = 0;
        virtual void visit(UnitNode &node) = 0;
        virtual void visit(BlockNode &node) = 0;
        virtual void visit(VarDeclNode &node) = 0;
        virtual void visit(ConstDeclNode &node) = 0;
        virtual void visit(ProcDeclNode &node) = 0;
        virtual void visit(FuncDeclNode &node) = 0;
        virtual void visit(ParameterNode &node) = 0;
        virtual void visit(CompoundStmtNode &node) = 0;
        virtual void visit(AssignmentNode &node) = 0;
        virtual void visit(IfStmtNode &node) = 0;
        virtual void visit(WhileStmtNode &node) = 0;
        virtual void visit(ForStmtNode &node) = 0;
        virtual void visit(ProcCallNode &node) = 0;
        virtual void visit(BinaryOpNode &node) = 0;
        virtual void visit(UnaryOpNode &node) = 0;
        virtual void visit(FuncCallNode &node) = 0;
        virtual void visit(VariableNode &node) = 0;
        virtual void visit(NumberNode &node) = 0;
        virtual void visit(StringNode &node) = 0;
        virtual void visit(BooleanNode &node) = 0;
        virtual void visit(EmptyStmtNode &node) = 0;
        virtual void visit(RepeatStmtNode &node) = 0;
        virtual void visit(CaseStmtNode &node) = 0;
        virtual void visit(ArrayTypeNode &node) = 0;
        virtual void visit(ArrayDeclarationNode &node) = 0;
        virtual void visit(ArrayAccessNode &node) = 0;
        virtual void visit(ArrayAssignmentNode &node) = 0;
        virtual void visit(RecordTypeNode &node) = 0;
        virtual void visit(RecordDeclarationNode &node) = 0;
        virtual void visit(FieldAccessNode &node) = 0;
        virtual void visit(TypeDeclNode &node) = 0;
        virtual void visit(TypeAliasNode &node) = 0;
        virtual void visit(ExitNode &node) = 0;
        virtual void visit(SimpleTypeNode &node) = 0;
        virtual void visit(ArrayTypeDeclarationNode &node) = 0;
        virtual void visit(ContinueNode &node) = 0;
        virtual void visit(BreakNode &node) = 0;
        virtual void visit(NilNode &node) = 0;
        virtual void visit(PointerTypeNode &node) = 0;
        virtual void visit(PointerDerefNode &node) = 0;
        virtual void visit(AddressOfNode &node) = 0;
        virtual void visit(WithStmtNode &node) = 0;
        virtual void visit(GotoStmtNode &node) = 0;
        virtual void visit(LabelStmtNode &node) = 0;
        virtual void visit(SetLiteralNode &node) = 0;
        virtual void visit(SetTypeNode &node) = 0;
        virtual void visit(EnumTypeDeclNode &node) = 0;
    };
} // namespace pascal

#endif