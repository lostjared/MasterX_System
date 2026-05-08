/**
 * @file string_buffer.hpp
 * @brief Character-by-character string buffer with line/column tracking and Token/TokenMap types
 * @author Jared Bruni
 */
#ifndef _STR_BUF_X
#define _STR_BUF_X

#include "types.hpp"
#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>

namespace scan {
    namespace buffer {
        /**
         * @brief Character-by-character string buffer with line/column tracking
         * @tparam Ch Character type (default: char)
         * @tparam String String type (default: std::basic_string<Ch>)
         *
         * Wraps a source string and provides sequential character access
         * with automatic line and column tracking for error reporting.
         * Supports #line directive processing.
         */
        template <typename Ch = char, typename String = std::basic_string<Ch, std::char_traits<Ch>>>
        class StringBuffer {
          public:
            using ch_type = Ch;           ///< character type alias
            using string_type = String;    ///< string type alias
            /**
             * @brief Construct a string buffer
             * @param buf Source text
             * @param filename Source filename for error messages
             */
            StringBuffer(const string_type &buf, const std::string &filename = "unknown");
            /** @brief Consume and return the next character, advancing the position */
            std::optional<ch_type> getch();
            /** @brief Return the current character without advancing */
            std::optional<ch_type> curch();
            /**
             * @brief Peek ahead by num positions without advancing
             * @param num Number of positions to look ahead (default: 1)
             */
            std::optional<ch_type> peekch(int num = 1);
            /** @brief Return the previously consumed character */
            std::optional<ch_type> prevch();
            /**
             * @brief Advance the position by num characters
             * @param num Number of characters to skip (default: 1)
             */
            std::optional<ch_type> forward_step(int num = 1);
            /**
             * @brief Move the position backward by num characters
             * @param num Number of characters to step back (default: 1)
             */
            std::optional<ch_type> backward_step(int num = 1);
            /** @brief Get the current (line, column) position */
            std::pair<uint64_t, uint64_t> cur_line();
            /** @brief Get the current source filename */
            std::string cur_file() const;
            /**
             * @brief Process a #line directive to update position tracking
             * @param newLine New line number
             * @param newFile New filename
             */
            void process_line_directive(uint64_t newLine, const std::string &newFile);
            /**
             * @brief Check if end-of-buffer has been reached
             * @param pos Additional offset from current position
             */
            bool eof(uint64_t pos = 0);
            /**
             * @brief Reset the buffer position
             * @param pos New position (default: 0)
             */
            void reset(uint64_t pos = 0);

          private:
            string_type buffer_;       ///< the source text with appended sentinel
            uint64_t index;            ///< current read position
            uint64_t currentLine;      ///< current line number (1-based)
            uint64_t currentColumn;    ///< current column number (1-based)
            std::string currentFile;   ///< current source filename
        };

        template <typename Ch, typename String>
        StringBuffer<Ch, String>::StringBuffer(const string_type &buf,
                                               const std::string &filename)
            : buffer_{buf + " \n"}, index{0},
              currentLine{1}, currentColumn{1},
              currentFile{filename} {}

        template <typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::getch() {
            if (index < buffer_.length()) {
                Ch ch = buffer_[index++];
                if (ch == '\n') {
                    currentLine++;
                    currentColumn = 1;
                } else {
                    currentColumn++;
                }
                return ch;
            }
            return std::nullopt;
        }

        template <typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::curch() {
            if (index < buffer_.length())
                return buffer_[index];
            return std::nullopt;
        }

        template <typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::peekch(int num) {
            if (index + num < buffer_.length())
                return buffer_[index + num];
            return std::nullopt;
        }

        template <typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::prevch() {
            if (index > 0)
                return buffer_[index - 1];
            return std::nullopt;
        }

        template <typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::forward_step(int num) {
            if (index + num < buffer_.length()) {
                index += num;
                return buffer_[index];
            }
            return std::nullopt;
        }

        template <typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::backward_step(int num) {
            if (static_cast<int64_t>(index) >= num) {
                index -= num;
                if (buffer_[index] == '\n') {
                    currentLine--;
                    currentColumn = 1;
                }
                return buffer_[index];
            }
            return std::nullopt;
        }

        template <typename Ch, typename String>
        std::pair<uint64_t, uint64_t> StringBuffer<Ch, String>::cur_line() {
            uint64_t line = (currentColumn == 1 && index > 0) ? currentLine - 1 : currentLine;
            uint64_t col = (currentColumn > 1 ? currentColumn - 1 : 1);
            return std::make_pair(line, col);
        }

        template <typename Ch, typename String>
        std::string StringBuffer<Ch, String>::cur_file() const {
            return currentFile;
        }

        template <typename Ch, typename String>
        void StringBuffer<Ch, String>::process_line_directive(uint64_t newLine,
                                                              const std::string &newFile) {
            currentLine = newLine;
            currentColumn = 1;
            currentFile = newFile;
        }

        template <typename Ch, typename String>
        bool StringBuffer<Ch, String>::eof(uint64_t pos) {
            return (pos + index >= buffer_.size());
        }

        template <typename Ch, typename String>
        void StringBuffer<Ch, String>::reset(uint64_t pos) {
            index = pos;
            currentLine = 1;
            currentColumn = 1;
        }
    } // namespace buffer

    namespace token {

        using types::CharType;

        /**
         * @brief Maps characters to their lexical category (letter, digit, symbol, etc.)
         * @tparam Ch Character type
         * @tparam MAX_SIZE Maximum character code value to classify
         */
        template <typename Ch, int MAX_SIZE = 256>
        class TokenMap {
          public:
            /** @brief Construct the map with default ASCII character classifications */
            TokenMap();
            /**
             * @brief Look up the character type for an 8-bit character
             * @param c Character to classify
             * @return The CharType, or nullopt if not mapped
             */
            std::optional<types::CharType> lookup_int8(int8_t c);

          private:
            std::unordered_map<Ch, types::CharType> token_map;
        };

        template <typename Ch, int MAX_CHARS>
        TokenMap<Ch, MAX_CHARS>::TokenMap() {
            std::size_t i = 0;
            for (i = 0; i < MAX_CHARS; ++i) {
                token_map[i] = CharType::TT_NULL;
            }
            for (i = 'a'; i <= 'z'; ++i) {
                token_map[i] = CharType::TT_CHAR;
            }
            for (i = 'A'; i <= 'Z'; ++i) {
                token_map[i] = CharType::TT_CHAR;
            }
            for (i = '0'; i <= '9'; ++i) {
                token_map[i] = CharType::TT_DIGIT;
            }
            token_map[' '] = CharType::TT_SPACE;
            token_map['+'] = CharType::TT_SYMBOL;
            token_map['-'] = CharType::TT_SYMBOL;
            token_map['*'] = CharType::TT_SYMBOL;
            token_map['/'] = CharType::TT_SYMBOL;
            token_map['%'] = CharType::TT_SYMBOL;
            token_map['&'] = CharType::TT_SYMBOL;
            token_map['|'] = CharType::TT_SYMBOL;
            token_map['^'] = CharType::TT_SYMBOL;
            token_map['~'] = CharType::TT_SYMBOL;
            token_map['!'] = CharType::TT_SYMBOL;
            token_map['='] = CharType::TT_SYMBOL;
            token_map['<'] = CharType::TT_SYMBOL;
            token_map['>'] = CharType::TT_SYMBOL;
            token_map['('] = CharType::TT_SYMBOL;
            token_map[')'] = CharType::TT_SYMBOL;
            token_map['['] = CharType::TT_SYMBOL;
            token_map[']'] = CharType::TT_SYMBOL;
            token_map['{'] = CharType::TT_SYMBOL;
            token_map['}'] = CharType::TT_SYMBOL;
            token_map[';'] = CharType::TT_SYMBOL;
            token_map[':'] = CharType::TT_SYMBOL;
            token_map[','] = CharType::TT_SYMBOL;
            token_map['.'] = CharType::TT_SYMBOL;
            token_map['?'] = CharType::TT_SYMBOL;
            token_map['#'] = CharType::TT_SYMBOL;
            token_map['_'] = CharType::TT_CHAR;
            token_map['\\'] = CharType::TT_SYMBOL;
            token_map['\''] = CharType::TT_SINGLE;
            token_map['\"'] = CharType::TT_STRING;
            token_map['"'] = CharType::TT_STRING;
            token_map['@'] = CharType::TT_SYMBOL;
            token_map['$'] = CharType::TT_SYMBOL;
        }
        template <typename Ch, int MAX_CHARS>
        std::optional<types::CharType> TokenMap<Ch, MAX_CHARS>::lookup_int8(int8_t c) {
            if (c >= 0 && c < MAX_CHARS) {
                auto f = token_map.find(c);
                if (f != token_map.end())
                    return f->second;
            }
            return std::nullopt;
        }

        /**
         * @brief A scanned token with type, value, and source position
         * @tparam Ch Character type
         * @tparam String String type for token values
         */
        template <typename Ch = char, typename String = std::basic_string<Ch, std::char_traits<Ch>>>
        class Token {
          public:
            using ch_type = Ch;           ///< character type alias
            using string_type = String;    ///< string type alias
            /** @brief Default constructor — creates a null token at line 1, col 1 */
            Token() : line{1}, col{1} {}
            /** @brief Construct a token with a given type */
            Token(const types::TokenType &t) : type{t}, line{1}, col{1} {}
            /** @brief Copy constructor */
            Token(const Token<Ch, String> &t) : type{t.type}, value{t.value}, line{t.line}, col{t.col}, filename{t.filename} {}
            /** @brief Move constructor */
            Token(Token<Ch, String> &&t) : type{t.type}, value{std::move(t.value)}, line{t.line}, col{t.col}, filename{t.filename} {}
            /** @brief Assign a new token type, keeping the current value */
            Token<Ch, String> &operator=(const types::TokenType &type);
            /** @brief Copy assignment */
            Token<Ch, String> &operator=(const Token<Ch, String> &t);
            /** @brief Move assignment */
            Token<Ch, String> &operator=(Token<Ch, String> &&t);
            /**
             * @brief Set the token's type and value
             * @param type New token type
             * @param value New token value string
             */
            void setToken(const types::TokenType &type, const String &value);
            /** @brief Get the token's text value */
            string_type getTokenValue() const { return value; }
            /** @brief Get the token's type classification */
            types::TokenType getTokenType() const { return type; }
            /** @brief Print the token value and type to a stream */
            void print(std::ostream &out);
            /** @brief Get the (line, column) source position */
            const std::pair<uint64_t, uint64_t> get_pos() const { return std::make_pair(line, col); }
            /** @brief Set the source position from a (line, column) pair */
            void set_pos(const std::pair<uint64_t, uint64_t> &p);
            /** @brief Set the source filename for this token */
            void set_filename(const std::string &filename) { this->filename = filename; }
            /** @brief Get the source filename for this token */
            std::string get_filename() const { return this->filename; }
            /** @brief Get the source line number */
            uint64_t getLine() const { return line; }
            /** @brief Get the source column number */
            uint64_t getCol() const { return col; }

          private:
            types::TokenType type;
            string_type value;
            uint64_t line = 1, col = 1;
            std::string filename;
        };

        template <typename Ch, typename String>
        void Token<Ch, String>::setToken(const types::TokenType &type, const String &value) {
            this->type = type;
            this->value = value;
        }

        template <typename Ch, typename String>
        Token<Ch, String> &Token<Ch, String>::operator=(const types::TokenType &type) {
            setToken(type, value);
            return *this;
        }
        template <typename Ch, typename String>
        Token<Ch, String> &Token<Ch, String>::operator=(const Token<Ch, String> &t) {
            setToken(t.type, t.value);
            filename = t.filename;
            return *this;
        }
        template <typename Ch, typename String>
        Token<Ch, String> &Token<Ch, String>::operator=(Token<Ch, String> &&t) {
            setToken(t.type, t.value);
            return *this;
        }

        template <typename Ch, typename String>
        void Token<Ch, String>::print(std::ostream &out) {
            out << value << " -> \t";
            types::print_type_TokenType(out, this->type);
            out << "\n";
        }
        template <typename Ch, typename String>
        void Token<Ch, String>::set_pos(const std::pair<uint64_t, uint64_t> &p) {
            line = p.first;
            col = p.second;
        }
    } // namespace token

} // namespace scan

#endif