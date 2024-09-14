#include "parser.hpp"
#include "ir.hpp"

namespace ir {
    std::string escapeString(const std::string &input) {
        std::ostringstream escaped;
        if(input.length() > 0 && input.at(0) == '\"') {
            for (size_t i = 1; i < input.length()-1; ++i) {
                char c = input.at(i);
                switch (c) {
                    case '\n': escaped << "\\n"; break;
                    case '\t': escaped << "\\t"; break;
                    case '\r': escaped << "\\r"; break;
                    case '\v': escaped << "\\v"; break;
                    case '\b': escaped << "\\b"; break;
                    case '\f': escaped << "\\f"; break;
                    case '\a': escaped << "\\a"; break;
                    case '\\': escaped << "\\\\"; break;
                    case '\"': escaped << "\\\""; break;
                    case '\0': escaped << "\\0"; break;
                    default:
                        if (std::isprint(static_cast<unsigned char>(c))) {
                            escaped << c;
                        } else {
                            escaped << "\\x" << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(static_cast<unsigned char>(c));
                        }
                        break;
                }
            }
            return "\"" + escaped.str() + "\"";
        }
        return input;
    }
}