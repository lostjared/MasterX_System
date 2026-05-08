/**
 * @file valid.hpp
 * @brief Validator for MXVM programs — checks variable/label usage and instruction operand specifications
 * @author Jared Bruni
 */
#ifndef _VALID_H_X
#define _VALID_H_X

#include "mxvm/instruct.hpp"
#include "scanner/scanner.hpp"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace mxvm {

    /** @brief Classification of operand kinds for validation */
    enum class OpKind {
        Num,
        Hex,
        Str,
        Id,
        Member,
        Label,
        Any
    };

    /** @brief A parsed operand with its kind, text, and source location */
    struct ParsedOp {
        OpKind kind;
        std::string text;
        const scan::TToken *at;  ///< token at the operand's source position
    };

    /** @brief Records a variable usage site for validation */
    struct UseVar {
        std::string name;
        const scan::TToken *at;
    };
    /** @brief Records a label usage site for validation */
    struct UseLabel {
        std::string name;
        const scan::TToken *at;
    };

    /** @brief Variadic arity policy for instruction operand specs */
    enum class VArity {
        None,      ///< exact fixed operand count required
        AnyTail,   ///< fixed operands followed by any number of trailing operands
        ArgsTail   ///< fixed operands followed by argument-typed trailing operands
    };

    /** @brief Specification of valid operand patterns for an instruction */
    struct OpSpec {
        std::string name;              ///< instruction mnemonic
        std::vector<OpKind> fixed;     ///< required fixed operand kinds
        VArity varPolicy = VArity::None;
        int minArgs = -1;
        int maxArgs = -1;
    };

    /**
     * @brief Validates MXVM source for correct variable/label usage and instruction operands
     *
     * Scans the source token stream and checks that all instructions use valid operand
     * patterns, all referenced variables are declared, and all jump targets exist.
     */
    class Validator {
      private:
        std::string filename;
        scan::Scanner scanner;
        std::string source;
        size_t index = 0;
        const scan::TToken *token = nullptr;
        std::string current_function;
        std::unordered_map<std::string, std::unordered_set<std::string>> function_vars;

      public:
        /**
         * @brief Construct a validator from MXVM source code
         * @param source The MXVM source text to validate
         */
        Validator(const std::string &source);
        /** @brief A variable name paired with its declaration token */
        struct VarNamePair {
            std::string first;              ///< variable name
            const scan::TToken *second;     ///< token at the declaration
        };
        std::vector<VarNamePair> var_names;
        /**
         * @brief Validate an instruction's operands against the spec table
         * @param op Instruction mnemonic
         * @param ops Parsed operand list
         * @param vars Declared variables map
         * @param labels Declared labels map
         * @param objects Declared object names
         * @param usedVarsRef Output: variable usage records
         * @param usedLabelsRef Output: label usage records
         */
        void validateAgainstSpec(
            const std::string &op,
            const std::vector<ParsedOp> &ops,
            const std::unordered_map<std::string, Variable> &vars,
            const std::unordered_map<std::string, std::string> &labels,
            const std::unordered_set<std::string> &objects,
            std::vector<UseVar> &usedVarsRef,
            std::vector<UseLabel> &usedLabelsRef);

        /**
         * @brief Run full validation on the source program
         * @param name Program name (for error messages)
         * @return true if the program passes all checks
         */
        bool validate(const std::string &name);
        /** @brief Check if the current token value matches and advance */
        bool match(const std::string &m);
        /** @brief Require the current token value to match, or report an error */
        void require(const std::string &r);
        /** @brief Check if the current token type matches and advance */
        bool match(const types::TokenType &t);
        /** @brief Require the current token type to match, or report an error */
        void require(const types::TokenType &t);
        /** @brief Advance to the next token; returns false at end */
        bool next();
        /** @brief Check if the current token value matches without consuming */
        bool peekIs(const std::string &s);
        /** @brief Check if the current token type matches without consuming */
        bool peekIs(const types::TokenType &t);
        /** @brief Convert a TokenType to its display string */
        std::string tokenTypeToString(types::TokenType t);
        /** @brief Pre-collect all label declarations and map them to function names */
        void collect_labels(std::unordered_map<std::string, std::string> &labels);
        /** @brief Collect all object names declared between token indices */
        void collect_objects(std::unordered_set<std::string> &objects, size_t start_index, size_t end_index);
        /** @brief Parse a single instruction operand from the token stream */
        ParsedOp parseOperand();
        /** @brief Parse a comma-separated list of operands */
        std::vector<ParsedOp> parseOperandList();
    };
} // namespace mxvm

#endif