/**
 * @file parser.hpp
 * @brief Pascal recursive-descent parser producing a Pascal AST
 * @author Jared Bruni
 */
#ifndef _PARSER_H__H
#define _PARSER_H__H
#include "ast.hpp"
#include "scanner/scanner.hpp"
#include "validator.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace pascal {
    /** @brief Exception thrown on parse errors */
    class ParseException : public std::runtime_error {
      public:
        /** @brief Construct with an error message */
        explicit ParseException(const std::string &msg) : std::runtime_error(msg) {}
    };
} // namespace pascal

namespace mxx {
    /**
     * @brief Base parser providing token-stream navigation
     *
     * Wraps a Scanner to provide next(), peekIs(), and
     * token-type-to-string conversion for derived parsers.
     */
    class XParser {
      public:
        XParser() = delete;
        /**
         * @brief Construct and scan the source text
         * @param source Source code string
         */
        explicit XParser(const std::string &source)
            : scanner(source), token(nullptr), index(0) {
            scanner.scan();
            scanner.removeEOL();
            if (scanner.size() > 0) {
                token = &scanner[0];
                index = 0;
            }
        }

        /** @brief Advance to the next token; returns false at end of stream */
        bool next() {
            if (index + 1 < scanner.size()) {
                index++;
                token = &scanner[index];
                return true;
            }
            token = nullptr;
            return false;
        }

        /** @brief Check if the current token's value matches a string (case-insensitive) */
        bool peekIs(const std::string &s) {
            if (!token) return false;
            const auto &v = token->getTokenValue();
            if (v.size() != s.size()) return false;
            for (size_t i = 0; i < v.size(); ++i)
                if (std::tolower(static_cast<unsigned char>(v[i])) !=
                    std::tolower(static_cast<unsigned char>(s[i])))
                    return false;
            return true;
        }
        /** @brief Check if the current token's type matches */
        bool peekIs(const types::TokenType &t) {
            return token && token->getTokenType() == t;
        }

        /**
         * @brief Convert a TokenType to a human-readable string
         * @param t Token type
         * @return Display name
         */
        static std::string tokenTypeToString(types::TokenType t) {
            switch (t) {
            case types::TokenType::TT_ID:
                return "IDENTIFIER";
            case types::TokenType::TT_NUM:
                return "NUMBER";
            case types::TokenType::TT_STR:
                return "STRING";
            case types::TokenType::TT_SYM:
                return "SYMBOL";
            default:
                return "UNKNOWN";
            }
        }

        std::string filename;  ///< source filename for error messages

      protected:
        scan::Scanner scanner;        ///< underlying scanner
        scan::TToken *token = nullptr; ///< current token pointer
        size_t index = 0;             ///< current token index
    };
} // namespace mxx

namespace pascal {
    /**
     * @brief Recursive-descent parser for the Pascal language
     *
     * Parses Pascal source code into an AST rooted at a ProgramNode.
     * Includes a TPValidator for semantic checks during parsing.
     */
    class PascalParser : public mxx::XParser {
      public:
        PascalParser() = delete;
        /**
         * @brief Construct and prepare the parser
         * @param source Pascal source code string
         */
        explicit PascalParser(const std::string &source) : mxx::XParser(source), validator(source) {
            removeBraceComments();
            if (scanner.size() > 0) {
                token = &scanner[0];
                index = 0;
            } else {
                token = nullptr;
            }
        }

        /** @brief Parse a complete Pascal program and return the root AST node */
        std::unique_ptr<ProgramNode> parseProgram();
        /** @brief Parse a Pascal unit and return the root AST node */
        std::unique_ptr<UnitNode> parseUnit();
        /** @brief Check if the source starts with a 'unit' keyword */
        bool isUnitSource() const;
        mxx::TPValidator validator; ///< semantic validator

      private:
        /** @brief Remove {brace} comments from the token stream */
        void removeBraceComments();
        /** @brief Report a parse error with the given message */
        void error(const std::string &message);
        /** @brief Consume a token matching the expected string, or throw */
        void expectToken(const std::string &expected);
        /** @brief Consume a token matching the expected type, or throw */
        void expectToken(types::TokenType expected);
        /** @brief Try to match and consume a token by value */
        bool match(const std::string &s);
        /** @brief Try to match and consume a token by type */
        bool match(types::TokenType t);
        /** @brief Parse an lvalue (variable, array access, pointer deref, field access) */
        std::unique_ptr<ASTNode> parseLValue();
        /** @brief Parse a block (declarations + compound statement) */
        std::unique_ptr<BlockNode> parseBlock();
        /** @brief Parse a compound statement (begin..end) */
        std::unique_ptr<CompoundStmtNode> parseCompoundStatement();
        /** @brief Parse the declarations section */
        std::vector<std::unique_ptr<ASTNode>> parseDeclarations();
        /** @brief Parse a var declaration section */
        std::unique_ptr<ASTNode> parseVarDeclaration();
        /** @brief Parse a const declaration section */
        std::unique_ptr<ASTNode> parseConstDeclaration();
        /** @brief Parse a procedure declaration */
        std::unique_ptr<ASTNode> parseProcedureDeclaration();
        /** @brief Parse a function declaration */
        std::unique_ptr<ASTNode> parseFunctionDeclaration();
        /** @brief Parse interface forward declarations (procedure/function signatures only) */
        std::vector<std::unique_ptr<ASTNode>> parseInterfaceDeclarations();
        /** @brief Parse a procedure forward declaration (signature only, no body) */
        std::unique_ptr<ASTNode> parseProcedureForwardDecl();
        /** @brief Parse a function forward declaration (signature only, no body) */
        std::unique_ptr<ASTNode> parseFunctionForwardDecl();
        /** @brief Parse a formal parameter list */
        std::vector<std::unique_ptr<ASTNode>> parseParameterList();
        /** @brief Parse a list of statements (separated by semicolons) */
        std::vector<std::unique_ptr<ASTNode>> parseStatementList();
        /** @brief Parse a single formal parameter */
        std::unique_ptr<ASTNode> parseParameter();
        /** @brief Parse a record type definition */
        std::unique_ptr<ASTNode> parseRecordType();
        /** @brief Parse a type declaration section */
        std::unique_ptr<ASTNode> parseTypeDeclaration();
        /** @brief Parse a single statement */
        std::unique_ptr<ASTNode> parseStatement();
        /** @brief Parse an assignment or procedure call starting with an identifier */
        std::unique_ptr<ASTNode> parseAssignmentOrProcCall();
        /** @brief Parse an if-then-else statement */
        std::unique_ptr<ASTNode> parseIfStatement();
        /** @brief Parse a while-do statement */
        std::unique_ptr<ASTNode> parseWhileStatement();
        /** @brief Parse a for-to/downto-do statement */
        std::unique_ptr<ASTNode> parseForStatement();
        /** @brief Parse a repeat-until statement */
        std::unique_ptr<ASTNode> parseRepeatStatement();
        /** @brief Parse a case statement */
        std::unique_ptr<ASTNode> parseCaseStatement();
        /** @brief Parse a with statement */
        std::unique_ptr<ASTNode> parseWithStatement();
        /** @brief Parse a goto statement */
        std::unique_ptr<ASTNode> parseGotoStatement();
        /** @brief Parse a label declaration section (label 100, 200;) */
        void parseLabelDeclaration();
        /** @brief Set of user-declared goto labels */
        std::unordered_set<std::string> declaredLabels;
        /** @brief Parse a full expression (simple expression with optional relational op) */
        std::unique_ptr<ASTNode> parseExpression();
        /** @brief Parse a simple expression (terms combined by +, -, or) */
        std::unique_ptr<ASTNode> parseSimpleExpression();
        /** @brief Parse a term (factors combined by *, /, div, mod, and) */
        std::unique_ptr<ASTNode> parseTerm();
        /** @brief Parse a factor (literal, variable, function call, sub-expression) */
        std::unique_ptr<ASTNode> parseFactor();
        /** @brief Parse a procedure call given its already-parsed name */
        std::unique_ptr<ASTNode> parseProcedureCall(const std::string &name);
        /** @brief Parse a function call given its already-parsed name */
        std::unique_ptr<ASTNode> parseFunctionCall(const std::string &name);
        /** @brief Parse a comma-separated argument list */
        std::vector<std::unique_ptr<ASTNode>> parseArgumentList();

        /** @brief Check if current token is a relational operator */
        bool isRelationalOperator();
        /** @brief Check if current token is an additive operator */
        bool isAddOperator();
        /** @brief Check if current token is a multiplicative operator */
        bool isMulOperator();

        /** @brief Parse a type specifier */
        std::unique_ptr<ASTNode> parseTypeSpec();

        /** @brief Consume and return a relational operator string */
        std::string getRelationalOp();
        /** @brief Consume and return an additive operator string */
        std::string getAddOp();
        /** @brief Consume and return a multiplicative operator string */
        std::string getMulOp();
        /** @brief Consume and return a unary operator string */
        std::string getUnaryOp();
        /** @brief Map a comparison operator string to BinaryOpNode::OpType */
        BinaryOpNode::OpType getComparisonOperator(const std::string &op);
        /** @brief Map a logical operator string to BinaryOpNode::OpType */
        BinaryOpNode::OpType getLogicalOperator(const std::string &op);
        /** @brief Map an arithmetic operator string to BinaryOpNode::OpType */
        BinaryOpNode::OpType getArithmeticOperator(const std::string &op);
        /** @brief Convert a TokenType to its display string */
        std::string tokenTypeToString(types::TokenType type);
        /** @brief Check if a string is a known type name */
        bool isType(const std::string &token);
        /** @brief Check if a name is a built-in procedure */
        bool isBuiltinProcedure(const std::string &name);
        /** @brief Check if a name is a built-in function */
        bool isBuiltinFunction(const std::string &name);
        /** @brief Check if a string is a reserved keyword */
        bool isKeyword(const std::string &s);
        /** @brief Parse an array type specification */
        std::unique_ptr<ASTNode> parseArrayType();
        /** @brief Parse an array variable declaration */
        std::unique_ptr<ASTNode> parseArrayDeclaration(const std::string &varName);
        /** @brief Check if the current context is an array access */
        bool isArrayAccess();
    };
} // namespace pascal

#endif