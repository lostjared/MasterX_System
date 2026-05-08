/**
 * @file types.hpp
 * @brief Token, character, and operator type enumerations for the scanner
 * @author Jared Bruni
 */
#ifndef _TYPES_X__
#define _TYPES_X__

#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace types {
    /** @brief Classification of scanned tokens */
    enum class TokenType {
        TT_ID,    ///< identifier or keyword
        TT_ARG,   ///< argument token (e.g. $1)
        TT_SYM,   ///< symbol / operator token
        TT_STR,   ///< string literal
        TT_NUM,   ///< decimal numeric literal
        TT_HEX,   ///< hexadecimal numeric literal
        TT_NULL   ///< sentinel / invalid token
    };
    /** @brief Classification of individual characters during lexing */
    enum class CharType {
        TT_CHAR,    ///< alphabetic character
        TT_DIGIT,   ///< digit character
        TT_SYMBOL,  ///< multi-char symbol start
        TT_STRING,  ///< string delimiter (quote)
        TT_SINGLE,  ///< single-char operator
        TT_SPACE,   ///< whitespace
        TT_NULL     ///< unmapped / null
    };
    /** @brief Enumeration of all recognised operator symbols */
    enum class OperatorType {
        OP_INC,           // '++'
        OP_DEC,           // '--'
        OP_LSHIFT_ASSIGN, // '<<='
        OP_RSHIFT_ASSIGN, // '>>='
        OP_PLUS_ASSIGN,   // '+='
        OP_MINUS_ASSIGN,  // '-='
        OP_MUL_ASSIGN,    // '*='
        OP_DIV_ASSIGN,    // '/='
        OP_MOD_ASSIGN,    // '%='
        OP_AND_ASSIGN,    // '&='
        OP_OR_ASSIGN,     // '|='
        OP_XOR_ASSIGN,    // '^='
        OP_LSHIFT,        // '<<'
        OP_RSHIFT,        // '>>'
        OP_EQ,            // '=='
        OP_NEQ,           // '!='
        OP_LE,            // '<='
        OP_GE,            // '>='
        OP_AND_AND,       // '&&'
        OP_OR_OR,         // '||'
        OP_ARROW,         // '->'
        OP_PLUS,          // '+'
        OP_MINUS,         // '-'
        OP_MUL,           // '*'
        OP_DIV,           // '/'
        OP_MOD,           // '%'
        OP_ASSIGN,        // '='
        OP_AND,           // '&'
        OP_OR,            // '|'
        OP_XOR,           // '^'
        OP_NOT,           // '!'
        OP_LT,            // '<'
        OP_GT,            // '>'
        OP_LPAREN,        // '('
        OP_RPAREN,        // ')'
        OP_LBRACKET,      // '['
        OP_RBRACKET,      // ']'
        OP_LBRACE,        // '{'
        OP_RBRACE,        // '}'
        OP_COMMA,         // ','
        OP_SEMICOLON,     // ';'
        OP_COLON,         // ':'
        OP_DOT,           // '.'
        OP_QUESTION,      // '?'
        OP_HASH,          // '#'
        OP_SCOPE,         // '::'
        OP_TILDE,         // '~'q
        OP_DOLLAR,        // '$'
        OP_AT,            // '@'
    };

    /** @brief Human-readable display names indexed by OperatorType */
    inline std::vector<std::string> opName = {
        "Increment",             // OP_INC             -> '++'
        "Decrement",             // OP_DEC             -> '--'
        "Left Shift Assign",     // OP_LSHIFT_ASSIGN   -> '<<='
        "Right Shift Assign",    // OP_RSHIFT_ASSIGN   -> '>>='
        "Plus Assign",           // OP_PLUS_ASSIGN     -> '+='
        "Minus Assign",          // OP_MINUS_ASSIGN    -> '-='
        "Multiply Assign",       // OP_MUL_ASSIGN      -> '*='
        "Divide Assign",         // OP_DIV_ASSIGN      -> '/='
        "Modulo Assign",         // OP_MOD_ASSIGN      -> '%='
        "Bitwise And Assign",    // OP_AND_ASSIGN      -> '&='
        "Bitwise Or Assign",     // OP_OR_ASSIGN       -> '|='
        "Bitwise Xor Assign",    // OP_XOR_ASSIGN      -> '^='
        "Left Shift",            // OP_LSHIFT          -> '<<'
        "Right Shift",           // OP_RSHIFT          -> '>>'
        "Equal",                 // OP_EQ              -> '=='
        "Not Equal",             // OP_NEQ             -> '!='
        "Less Than Or Equal",    // OP_LE              -> '<='
        "Greater Than Or Equal", // OP_GE            -> '>='
        "Logical And",           // OP_AND_AND         -> '&&'
        "Logical Or",            // OP_OR_OR           -> '||'
        "Arrow",                 // OP_ARROW           -> '->'
        "Plus",                  // OP_PLUS            -> '+'
        "Minus",                 // OP_MINUS           -> '-'
        "Multiply",              // OP_MUL             -> '*'
        "Divide",                // OP_DIV             -> '/'
        "Modulo",                // OP_MOD             -> '%'
        "Assign",                // OP_ASSIGN          -> '='
        "Bitwise And",           // OP_AND             -> '&'
        "Bitwise Or",            // OP_OR              -> '|'
        "Bitwise Xor",           // OP_XOR             -> '^'
        "Not",                   // OP_NOT             -> '!'
        "Less Than",             // OP_LT              -> '<'
        "Greater Than",          // OP_GT              -> '>'
        "Left Parenthesis",      // OP_LPAREN          -> '('
        "Right Parenthesis",     // OP_RPAREN          -> ')'
        "Left Bracket",          // OP_LBRACKET        -> '['
        "Right Bracket",         // OP_RBRACKET        -> ']'
        "Left Brace",            // OP_LBRACE          -> '{'
        "Right Brace",           // OP_RBRACE          -> '}'
        "Comma",                 // OP_COMMA           -> ','
        "Semicolon",             // OP_SEMICOLON       -> ';'
        "Colon",                 // OP_COLON           -> ':'
        "Dot",                   // OP_DOT             -> '.'
        "Question Mark",         // OP_QUESTION        -> '?'
        "Hash",                  // OP_HASH            -> '#'
        "Scope",                 // OP_SCOPE           -> '::'
        "Tilde",                 // OP_TILDE           -> '~'
        "Dollar",                // OP_DOLLAR          -> '$'
        "AT",                    // OP_AT              -> '@'
    };

    /** @brief Operator string representations indexed by OperatorType */
    inline std::vector<std::string> opStrings = {
        "++",  // OP_INC
        "--",  // OP_DEC
        "<<=", // OP_LSHIFT_ASSIGN
        ">>=", // OP_RSHIFT_ASSIGN
        "+=",  // OP_PLUS_ASSIGN
        "-=",  // OP_MINUS_ASSIGN
        "*=",  // OP_MUL_ASSIGN
        "/=",  // OP_DIV_ASSIGN
        "%=",  // OP_MOD_ASSIGN
        "&=",  // OP_AND_ASSIGN
        "|=",  // OP_OR_ASSIGN
        "^=",  // OP_XOR_ASSIGN
        "<<",  // OP_LSHIFT
        ">>",  // OP_RSHIFT
        "==",  // OP_EQ
        "!=",  // OP_NEQ
        "<=",  // OP_LE
        ">=",  // OP_GE
        "&&",  // OP_AND_AND
        "||",  // OP_OR_OR
        "->",  // OP_ARROW
        "+",   // OP_PLUS
        "-",   // OP_MINUS
        "*",   // OP_MUL
        "/",   // OP_DIV
        "%",   // OP_MOD
        "=",   // OP_ASSIGN
        "&",   // OP_AND
        "|",   // OP_OR
        "^",   // OP_XOR
        "!",   // OP_NOT
        "<",   // OP_LT
        ">",   // OP_GT
        "(",   // OP_LPAREN
        ")",   // OP_RPAREN
        "[",   // OP_LBRACKET
        "]",   // OP_RBRACKET
        "{",   // OP_LBRACE
        "}",   // OP_RBRACE
        ",",   // OP_COMMA
        ";",   // OP_SEMICOLON
        ":",   // OP_COLON
        ".",   // OP_DOT
        "?",   // OP_QUESTION
        "#",   // OP_HASH
        "::",
        "~", // OP_TILDE
        "$",
        "@",

    };

    /** @brief Keyword type enumeration */
    enum class KeywordType {
        KW_LET,       ///< variable declaration
        KW_PROC,      ///< procedure/function
        KW_DEFINE,    ///< constant definition
        KW_IF,        ///< conditional
        KW_ELSE,      ///< else branch
        KW_SWITCH,    ///< switch/case
        KW_WHILE,     ///< while loop
        KW_FOR,       ///< for loop
        KW_RETURN,    ///< return statement
        KW_BREAK,     ///< break statement
        KW_CONTINUE   ///< continue statement
    };
    /** @brief Keyword string representations indexed by KeywordType */
    inline std::vector<std::string> kwStr{"let", "proc", "define", "if", "else", "switch", "while", "for", "return", "break", "continue"};

    extern std::vector<std::string> strTokenType;   ///< string names of TokenType values
    extern std::vector<std::string> sstrCharType;    ///< string names of CharType values
    /**
     * @brief Print a TokenType name to a stream
     * @param out Output stream
     * @param tt Token type to print
     */
    void print_type_TokenType(std::ostream &out, const TokenType &tt);
    /**
     * @brief Print a CharType name to a stream
     * @param out Output stream
     * @param c Character type to print
     */
    void print_type_CharType(std::ostream &out, const CharType &c);
    /**
     * @brief Look up an operator string and return its OperatorType
     * @param op Operator string (e.g. "++", "<=")
     * @return The matching OperatorType, or nullopt if not found
     */
    std::optional<OperatorType> lookUp(const std::string &op);
    /**
     * @brief Look up a keyword string and return its KeywordType
     * @param op Keyword string (e.g. "if", "while")
     * @return The matching KeywordType, or nullopt if not found
     */
    std::optional<KeywordType> lookUp_Keyword(const std::string &op);
} // namespace types

#endif