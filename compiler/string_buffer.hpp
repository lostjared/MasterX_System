#ifndef _STR_BUF_X
#define _STR_BUF_X

#include<string>
#include<sstream>
#include<cstdint>
#include<optional>
#include<unordered_map>
#include"types.hpp"


namespace scan {
    namespace buffer {
        template<typename Ch = char, typename String = std::basic_string<Ch, std::char_traits<Ch>>>
        class StringBuffer {
        public:
            using ch_type = Ch;
            using string_type = String;
            StringBuffer(const string_type &buf) : buffer_{buf+" \n"}, index{0} {}
            StringBuffer(const StringBuffer<Ch,String> &sb) :  buffer_{sb.buffer_}, index{0} {}   
            StringBuffer(StringBuffer<Ch,String> &&sb) :  buffer_{std::move(sb.buffer_)}, index{0} {}
            StringBuffer<Ch,String> &operator=(const StringBuffer<Ch,String> &sb);                                      
            StringBuffer<Ch,String> &operator=(StringBuffer<Ch,String> &&b);
            StringBuffer<Ch,String> &operator=(const String &b);

            std::optional<ch_type> getch();
            std::optional<ch_type> curch();
            std::optional<ch_type> peekch(int num=1);
            std::optional<ch_type> prevch();
            std::optional<ch_type> forward_step(int num=1);
            std::optional<ch_type> backward_step(int num=1);

            std::pair<uint64_t, uint64_t> cur_line();
            bool eof(uint64_t pos);
            void reset(uint64_t pos = 0);

            private:
               string_type buffer_;           
               uint64_t index{};
        };

        template<typename Ch, typename String>
        StringBuffer<Ch,String> &StringBuffer<Ch,String>::operator=(const StringBuffer<Ch,String> &sb) {
            buffer_ = sb.buffer_;
            index = sb.index;
            return *this;
        }            
        template<typename Ch, typename String>         
        StringBuffer<Ch,String> &StringBuffer<Ch,String>::operator=(StringBuffer<Ch,String> &&b) { //rval
            buffer_ = std::move(b.buffer_);
            index = b.index;
                return *this;
        }
        template<typename Ch, typename String>         
        StringBuffer<Ch,String> &StringBuffer<Ch,String>::operator=(const String &buf) {
            buffer_ =  buf;
            index = 0;
            return *this;
        }

        template<typename Ch, typename String>         
        std::optional<Ch> StringBuffer<Ch,String>::getch() {
            if(index + 1 < buffer_.length()) {
                return buffer_[index++];
            }
            return std::nullopt;
        }
        template<typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch,String>::curch() {
            if(index < buffer_.length())
                return buffer_[index];
            return std::nullopt;
        }
        template<typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::peekch(int num) {
            if(index + num < buffer_.length())
                return buffer_[index+num];
            return std::nullopt;
        }

        template<typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::prevch() {
            if(index - 1 >= 0)
                return buffer_[index-1];
            return std::nullopt;
        }
        template<typename Ch, typename String> 
        std::optional<Ch> StringBuffer<Ch, String>::forward_step(int num) {
            if(index + num < buffer_.length()) {
                index = index + num;
                return buffer_[index];
            }
            return std::nullopt;

        }                                                                                                                                                              
        template<typename Ch, typename String>
        std::optional<Ch> StringBuffer<Ch, String>::backward_step(int num) {
            if(index - num >= 0) {
                index = index - num;
                return buffer_[index];
            }
            return std::nullopt;
        }

        template<typename Ch, typename String>
        std::pair<uint64_t, uint64_t> StringBuffer<Ch,String>::cur_line() {
            uint64_t pos = index;
            uint64_t count = 1;
            uint64_t col = 0;
            for(uint64_t i = 0; i < pos; ++i) {
                if(buffer_[i] == '\n') {
                    ++count;
                    col = 0;
                } else ++col;
            }
            return std::make_pair(count, col);
        }

        template<typename Ch, typename String>
        bool StringBuffer<Ch,String>::eof(uint64_t pos) {
            if(pos+index > buffer_.size()) {
                return true;
            } else {
                return false;
            }
        }
        template<typename Ch, typename String>
        void StringBuffer<Ch,String>::reset(uint64_t pos) {
        
            index = pos;
        }
    }

    namespace token {
      
        using types::CharType;
   
        template<typename Ch, int MAX_SIZE=256>
        class TokenMap {
        public:
                    
            TokenMap();
            std::optional<types::CharType> lookup_int8(int8_t c);
        private:
            std::unordered_map<Ch, types::CharType> token_map;
        };

        template<typename Ch, int MAX_CHARS>
        TokenMap<Ch, MAX_CHARS>::TokenMap() {
            std::size_t i = 0;
            for(i = 0;  i < MAX_CHARS; ++i) {
                token_map[i] = CharType::TT_NULL;
            }
            for(i = 'a'; i <= 'z'; ++i) {
                token_map[i] = CharType::TT_CHAR;
            }
            for(i = 'A'; i <= 'Z'; ++i) {
                token_map[i] = CharType::TT_CHAR;
            }
            for(i = '0'; i <= '9'; ++i) {
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
            token_map['@'] = CharType::TT_SYMBOL;   
            token_map['$'] = CharType::TT_SYMBOL;
        }
        template<typename Ch, int MAX_CHARS>
        std::optional<types::CharType> TokenMap<Ch, MAX_CHARS>::lookup_int8(int8_t c) {
            if(c >= 0 && c < MAX_CHARS) {
                auto f = token_map.find(c);
                if(f != token_map.end())
                    return f->second;
            }
            return std::nullopt;
        }
        
        template<typename Ch = char, typename String = std::basic_string<Ch, std::char_traits<Ch>>>
        class Token {
        public:
            using ch_type = Ch;
            using string_type = String;
            Token() = default;
            Token(const types::TokenType &t) : type{t} {}
            Token(const Token<Ch,String> &t) : type{t.type}, value{t.value}, line{t.line}, col{t.col} {}
            Token(Token<Ch,String>  &&t) : type{t.type}, value{std::move(t.value)}, line{t.line}, col{t.col} {}
            Token<Ch,String> &operator=(const types::TokenType &type);
            Token<Ch,String> &operator=(const Token<Ch,String> &t);
            Token<Ch,String> &operator=(Token<Ch,String> &&t);
            void setToken(const types::TokenType &type, const String &value);
            string_type getTokenValue() const { return value; }
            types::TokenType getTokenType() const { return type; }
            void print(std::ostream &out);
            const std::pair<uint64_t, uint64_t> get_pos() const { return std::make_pair(line, col); }
            void set_pos(const std::pair<uint64_t, uint64_t> &p);
        private:
            types::TokenType type;
            string_type value;
            uint64_t line, col;
        };
        
        template<typename Ch, typename String>
        void Token<Ch, String>::setToken(const types::TokenType &type, const String &value) {
            this->type = type;
            this->value = value;
        }

        template<typename Ch, typename String>
        Token<Ch,String> &Token<Ch,String>::operator=(const types::TokenType &type) {
            setToken(type, value);
            return *this;
        }
        template<typename Ch, typename String>
        Token<Ch,String> &Token<Ch,String>::operator=(const Token<Ch,String> &t) {
            setToken(t.type, t.value);
            return *this;
        }
        template<typename Ch, typename String>
        Token<Ch,String> &Token<Ch,String>::operator=(Token<Ch,String> &&t) {
            setToken(t.type, t.value);
            return *this;
        }

        template<typename Ch, typename String>
        void Token<Ch, String>::print(std::ostream &out) {
            out << value << " -> \t";
            types::print_type_TokenType(out, this->type);
            out << "\n"; 
        }
        template<typename Ch, typename String>
        void Token<Ch, String>::set_pos(const std::pair<uint64_t, uint64_t> &p) {
            line = p.first;
            col = p.second;
        }
    }
       
  }

#endif