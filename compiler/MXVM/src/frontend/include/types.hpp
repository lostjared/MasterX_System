/**
 * @file types.hpp
 * @brief Pascal keyword enumeration and string table
 * @author Jared Bruni
 */
#ifndef __TYPES__H_HPP
#define __TYPES__H_HPP

namespace mxx {

    /** @brief Standard Pascal reserved keyword enumeration */
    enum class Keywords {
        AND,
        ARRAY,
        BEGIN,
        CASE,
        CONST,
        DIV,
        DO,
        DOWNTO,
        ELSE,
        END,
        FILE,
        FOR,
        FUNCTION,
        GOTO,
        IF,
        IN,
        LABEL,
        MOD,
        NIL,
        NOT,
        OF,
        OR,
        PACKED,
        PROCEDURE,
        PROGRAM,
        RECORD,
        REPEAT,
        SET,
        THEN,
        TO,
        TYPE,
        UNTIL,
        VAR,
        WHILE,
        WITH
    };

    /** @brief String representations of Keywords values, indexed by enum ordinal */
    static const char *key_str[] = {
        "and",
        "array",
        "begin",
        "case",
        "const",
        "div",
        "do",
        "downto",
        "else",
        "end",
        "file",
        "for",
        "function",
        "goto",
        "if",
        "in",
        "label",
        "mod",
        "nil",
        "not",
        "of",
        "or",
        "packed",
        "procedure",
        "program",
        "record",
        "repeat",
        "set",
        "then",
        "to",
        "type",
        "until",
        "var",
        "while",
        "with"};

} // namespace mxx

#endif