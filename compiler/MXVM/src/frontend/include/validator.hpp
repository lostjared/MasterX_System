/**
 * @file validator.hpp
 * @brief Semantic validator for Pascal programs — scope, type, and declaration checking
 * @author Jared Bruni
 */
#ifndef __VALIDATOR_H_
#define __VALIDATOR_H_

#include "scanner/scanner.hpp"
#include "scanner/types.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace mxx {

    /** @brief A single scope level, tracking declared identifiers by category */
    struct Scope {
        std::unordered_set<std::string> vars;    ///< variable names
        std::unordered_set<std::string> consts;  ///< constant names
        std::unordered_set<std::string> types;   ///< type names
        std::unordered_set<std::string> funcs;   ///< function names
        std::unordered_set<std::string> procs;   ///< procedure names
        std::unordered_set<std::string> params;  ///< parameter names
    };

    /**
     * @brief Semantic validator for Pascal programs
     *
     * Performs a second pass over the token stream to check that all
     * variables, constants, types, functions, and procedures are
     * properly declared before use, enforcing scope rules.
     */
    class TPValidator {
      public:
        /**
         * @brief Construct a validator for the given source
         * @param source_ Pascal source code string
         */
        explicit TPValidator(const std::string &source_);

        /**
         * @brief Run semantic validation
         * @param name Program name (used in error messages)
         * @return true if the program is semantically valid
         */
        bool validate(const std::string &name);

      private:
        std::unordered_map<std::string, std::unordered_set<std::string>> recordFieldScopesByType; ///< field names per record type
        std::string currentRecordTypeName; ///< name of record type currently being parsed
        std::vector<Scope> scopeStack;     ///< stack of lexical scopes
        scan::Scanner scanner;             ///< underlying scanner
        const scan::TToken *token = nullptr; ///< current token pointer
        std::string source;                ///< original source text
        std::string filename;              ///< source filename for error messages
        size_t index = 0;                  ///< current token index
        int inFunctionDepth = 0;           ///< > 0 when inside a function body

        /** @brief Check if a string is a Pascal reserved keyword */
        bool isPascalKeyword(const std::string &s) const;

        std::unordered_set<std::string> declaredVars;    ///< globally declared variables
        std::unordered_set<std::string> declaredConsts;  ///< globally declared constants
        std::unordered_set<std::string> declaredFuncs;   ///< globally declared functions
        std::unordered_set<std::string> declaredProcs;   ///< globally declared procedures
        std::unordered_set<std::string> importedUnits;   ///< unit names from uses clause
        /** @brief Push a new scope onto the scope stack */
        void pushScope();
        /** @brief Pop the current scope from the stack */
        void popScope();
        /** @brief Get the current (top) scope */
        Scope *currentScope();
        /** @brief Get the current scope (const) */
        const Scope *currentScope() const;
        /** @brief Check if a variable is declared in the current scope */
        bool isVariableInCurrentScope(const std::string &name) const;
        /** @brief Return a display name for the current scope */
        std::string currentScopeName() const;

        /** @brief Check if a variable is declared in the innermost scope */
        bool isVarDeclaredHere(const std::string &name) const;
        /** @brief Check if a type is declared in the innermost scope */
        bool isTypeDeclaredHere(const std::string &name) const;
        /** @brief Check if a function is declared in the innermost scope */
        bool isFuncDeclaredHere(const std::string &name) const;
        /** @brief Check if a procedure is declared in the innermost scope */
        bool isProcDeclaredHere(const std::string &name) const;
        /** @brief Check if a parameter is declared in the innermost scope */
        bool isParamDeclaredHere(const std::string &name) const;
        /** @brief Declare a variable in the current scope */
        void declareVar(const std::string &name, const scan::TToken *at);
        /** @brief Declare a constant in the current scope */
        void declareConst(const std::string &name, const scan::TToken *at);
        /** @brief Declare a function in the current scope */
        void declareFunc(const std::string &name, const scan::TToken *at);
        /** @brief Declare a procedure in the current scope */
        void declareProc(const std::string &name, const scan::TToken *at);
        /** @brief Declare a parameter in the current scope */
        void declareParam(const std::string &name, const scan::TToken *at);

        /** @brief Assert a variable is declared and accessible in scope */
        void checkVar(const std::string &name, const scan::TToken *at);
        /** @brief Assert a name is a variable or constant in scope */
        void checkVarOrConst(const std::string &name, const scan::TToken *at);
        /** @brief Assert a name is a constant (used in const expressions) */
        void checkConstOnly(const std::string &name, const scan::TToken *at);

        /**
         * @brief Parse and validate a constant expression
         * @param stops Set of token values that terminate the expression
         * @param constOnly If true, reject non-constant identifiers
         */
        void parseConstExpr(const std::unordered_set<std::string> &stops,
                            bool constOnly);

        std::unordered_set<std::string> declaredTypes; ///< globally declared type names

        /** @brief Check if the current token is a built-in type name */
        bool isBuiltinType() const;
        /** @brief Declare a user-defined type in the current scope */
        void declareType(const std::string &name, const scan::TToken *at);
        /** @brief Assert a type name is declared and accessible */
        void checkType(const std::string &name, const scan::TToken *at);
        /** @brief Check if a name is a built-in constant (true, false, maxint) */
        bool isBuiltinConst(const std::string &name) const;
        /** @brief Advance to the next token */
        bool next();
        /** @brief Check if the current token value matches a string */
        bool match(const std::string &s) const;
        /** @brief Check if the current token type matches */
        bool match(types::TokenType t) const;
        /** @brief Check if the current token value matches (and optionally advance) */
        bool peekIs(const std::string &s);

        /** @brief Require the current token to match a string, or fail */
        void require(const std::string &s);
        /** @brief Require the current token to match a type, or fail */
        void require(types::TokenType t);
        /** @brief Require the current token to be a keyword, or fail */
        void requireKW(const std::string &k);

        /** @brief Report a validation error with a generic message */
        void fail(const std::string &msg);
        /** @brief Report a validation error at the current token */
        void failHere(const std::string &msg);
        /** @brief Report a validation error at a specific token */
        void failAt(const scan::TToken *at, const std::string &msg);

        /** @brief Convert a TokenType to a display string */
        static std::string tokenTypeToString(types::TokenType t);
        /** @brief Convert a string to lowercase */
        static std::string lower(std::string s);
        /** @brief Return a display string for the current token value */
        std::string found() const;
        /** @brief Check if current token value (lowercased) matches a keyword */
        bool isKW(const std::string &k) const;

        /** @brief Validate a complete Pascal program */
        void parseProgram();
        /** @brief Validate a Pascal unit */
        void parseUnit();
        /** @brief Validate interface section declarations */
        void parseInterfaceSection();
        /** @brief Validate a uses clause */
        void parseUses();
        /** @brief Validate a block (declarations + compound statement) */
        void parseBlock();
        /** @brief Validate a label section */
        void parseLabelSection();
        /** @brief Validate a const section */
        void parseConstSection();
        /** @brief Validate a type section */
        void parseTypeSection();
        /** @brief Validate a var section */
        void parseVarSection();
        /** @brief Validate a procedure or function declaration */
        void parseSubprogram();
        /** @brief Validate formal parameter declarations */
        void parseFormalParams();
        /** @brief Validate a comma-separated identifier list */
        void parseIdentList();
        /** @brief Validate a type specification */
        void parseType(const std::string &);
        /** @brief Validate a type name reference */
        void parseTypeName();
        /** @brief Validate a subrange type */
        void parseSubrange();
        /** @brief Validate a constant value */
        void parseConstant();
        /** @brief Validate a simple constant (number, string, or name) */
        void parseConstSimple();
        /** @brief Validate a compound statement (begin..end) */
        void parseCompoundStatement();
        /** @brief Validate a single statement */
        void parseStatement();
        /** @brief Validate an if statement */
        void parseIf();
        /** @brief Validate a while statement */
        void parseWhile();
        /** @brief Validate a repeat..until statement */
        void parseRepeat();
        /** @brief Validate a for statement */
        void parseFor();
        /** @brief Validate a case statement */
        void parseCase();
        /** @brief Validate a case label list */
        void parseCaseLabelList();
        /** @brief Validate a single case label */
        void parseCaseLabel();
        /** @brief Validate a with statement */
        void parseWith();
        /** @brief Validate a goto statement */
        void parseGoto();
        /** @brief Validate a simple statement, procedure call, or assignment */
        void parseSimpleOrCallOrAssign();
        /** @brief Validate a designator (variable with selectors) */
        void parseDesignator();
        /** @brief Validate actual parameter list */
        void parseActualParams();
        /** @brief Validate an expression up to a set of stop tokens */
        void parseExprStop(const std::unordered_set<std::string> &stops);

        /** @brief Check if current token is a relational operator */
        bool isRelOp() const;
        /** @brief Check if current token is an additive operator */
        bool isAddOp() const;
        /** @brief Check if current token is a multiplicative operator */
        bool isMulOp() const;
        /** @brief Check if current token is a set operator */
        bool isSetOp() const;

      private:
        /** @brief Convert a string to lowercase (static helper) */
        static std::string toLower(const std::string &s);
        /** @brief Pre-declare a built-in or forward-declared type */
        void predeclareType(const std::string &name);
        /** @brief Check if a lowercase key is a built-in type name */
        bool isBuiltinTypeName(const std::string &key) const;
        /** @brief Push a record field scope for the given type */
        void pushRecordFieldScope(const std::string &recordTypeName);
        /** @brief Pop the current record field scope */
        void popRecordFieldScope();
        /** @brief Check if currently inside a record field scope */
        bool inRecordFieldScope() const;

        /** @brief Declare a record field in the current record scope */
        void declareRecordField(const std::string &name, const scan::TToken *at);
        /** @brief Parse a field identifier list within a record definition */
        void parseFieldIdentList();

        int withDepth = 0; ///< nesting depth of with statements (suppress undeclared-id errors)
    };

} // namespace mxx
#endif