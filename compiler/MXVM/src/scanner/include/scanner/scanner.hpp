/**
 * @file scanner.hpp
 * @brief Lexical scanner that tokenizes source text into typed tokens
 * @author Jared Bruni
 */
#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "string_buffer.hpp"
#include <iostream>
#include <optional>
#include <string>
#include <vector>

namespace scan {

    using TString = buffer::StringBuffer<char>;       ///< Default character string buffer
    using StringType = buffer::StringBuffer<char>::string_type; ///< Underlying string type
    using TToken = token::Token<char>;                ///< Default token type
    using TMap = token::TokenMap<char>;                ///< Default character classification map

    /**
     * @brief Lexical scanner that tokenizes source text
     *
     * Scans a StringBuffer character-by-character and produces a list of
     * typed tokens (identifiers, numbers, strings, symbols, hex literals).
     */
    class Scanner {
      public:
        /**
         * @brief Construct a scanner from a string buffer
         * @param b Source text buffer to scan
         */
        Scanner(const TString &b);
        /**
         * @brief Tokenize the entire source buffer
         * @return Number of tokens produced
         */
        uint64_t scan();
        /** @brief Try to grab an identifier or keyword token */
        std::optional<TToken> grabId();
        /** @brief Try to grab a numeric literal token */
        std::optional<TToken> grabDigits();
        /** @brief Try to grab one or more symbol characters as a token */
        std::optional<TToken> grabSymbols();
        /** @brief Try to grab a double-quoted string literal token */
        std::optional<TToken> grabString();
        /** @brief Try to grab a single-quoted character literal token */
        std::optional<TToken> grabSingle();

        /**
         * @brief Access a token by index
         * @param index Zero-based token index
         * @return Reference to the token
         */
        TToken &operator[](size_t index);
        /** @brief Return the total number of scanned tokens */
        size_t size() const;
        /** @brief Remove end-of-line tokens from the token list */
        void removeEOL();
        /** @brief Get the underlying token vector */
        std::vector<TToken> &getTokens() { return tokens; }

      private:
        TString string_buffer;          ///< source text buffer
        TMap token_map;                  ///< character classification map
        std::vector<TToken> tokens;      ///< scanned token list

        /** @brief Check if a string is a recognized C-style symbol */
        bool is_c_sym(const StringType &str);
        /** @brief Parse a #line directive line number */
        uint64_t parseLineNumber();
        /** @brief Parse a #line directive filename */
        std::string parseFileName();
    };

    /**
     * @brief Exception thrown during scanning errors
     */
    class ScanExcept {
      public:
        ScanExcept() = default;
        /**
         * @brief Construct a scan exception with a reason string
         * @param why Description of the scanning error
         */
        ScanExcept(const StringType &why) : msg{why} {}
        /** @brief Get the error description */
        StringType why() const { return msg; }

      private:
        StringType msg;
    };
} // namespace scan
/** @brief Stream output operator for CharType */
std::ostream &operator<<(std::ostream &out, const types::CharType &c);
/** @brief Stream output operator for TokenType */
std::ostream &operator<<(std::ostream &out, const types::TokenType &t);

#endif