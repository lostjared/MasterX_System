#include"scanner.hpp"
#include<fstream>
#include<sstream>
#include<iostream>
#include<memory>
#include<set>
#include<string>

std::string htmlPage_Header = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Token Table</title>
    <style>
        table {
            width: 50%;
            margin: 50px auto;
            border-collapse: collapse;
        }
        th, td {
            border: 1px solid #000;
            padding: 10px;
            text-align: center;
            background-color: #f2f2f2;
        }
        th {
            background-color: #000000;
            color: #f2f2f2;
        }
    </style>
</head>
<body>
<table>
)";

std::string htmlPage_Footer = R"(
</table>
</body>
</html>
)";

std::string convertToHTML(const std::string &text) {
    std::ostringstream stream;
    for(size_t i = 0; i < text.length(); ++i) {
        if(text[i] == '<') {
            stream << "&lt;";
        } else if(text[i] == '>') {
            stream << "&gt;";
        } else {
            stream << text[i];
        }
    }
    return stream.str();
}

std::set<std::string> createKeywordSet() {
    return {
        // C++ Keywords
        "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept",
        "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
        "class", "compl", "concept", "const", "consteval", "constexpr", "constinit", "const_cast", "continue",
        "co_await", "co_return", "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast",
        "else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline",
        "int", "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or",
        "or_eq", "private", "protected", "public", "reflexpr", "register", "reinterpret_cast", "requires", "return",
        "short", "signed", "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized",
        "template", "this", "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union",
        "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",

        // JavaScript Keywords
        "await", "debugger", "delete", "export", "extends", "finally", "function", "import", "in", "instanceof", 
        "let", "new", "null", "super", "this", "throw", "typeof", "var", "void", "with", "yield",

        // C Keywords (remaining after duplicates removed)
        "restrict", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Decimal128", "_Decimal32", "_Decimal64",
        "_Generic", "_Imaginary", "_Noreturn", "_Static_assert", "_Thread_local"
    };
}

bool isKeyword(const std::string& word) {
    static std::set<std::string> keywordSet = createKeywordSet();
    return keywordSet.find(word) != keywordSet.end();
}

int html_scanFile(const std::string &contents, std::ostream &file) {
    try {
        file << htmlPage_Header << "\n";
        std::unique_ptr<scan::Scanner> scan(new scan::Scanner(scan::TString(contents)));
        uint64_t tokens = scan->scan();
        std::cout << "scan: Sucessfull scan returned: " << tokens << " token(s)...\n";

        file << "<tr><th>Index/Line:Col</th><th>Token</th><th>Type</th></tr>\n";

        for(size_t i = 0; i < scan->size(); ++i) {
            auto posx = scan->operator[](i).get_pos();
            file << "<tr><td>" << i << "/" << posx.first << ":" << posx.second << "</td><td>";
            if(!isKeyword(scan->operator[](i).getTokenValue())) {
                file << convertToHTML(scan->operator[](i).getTokenValue()) << "</td><td>";
                if(scan->operator[](i).getTokenType() == types::TokenType::TT_SYM) {
                    auto t = types::lookUp(scan->operator[](i).getTokenValue());
                    if(t.has_value()) {
                        file << types::opName[static_cast<int>(*t)];
                    } else {
                        file << "Not Recognized";
                    }
                }
                else 
                    types::print_type_TokenType(file, scan->operator[](i).getTokenType());
            } else {
                file << "<b>" << convertToHTML(scan->operator[](i).getTokenValue()) << "</b>" << "</td><td>";
                file << "Keyword";
            }
            file << "</td></tr>\n";
        }
        file << htmlPage_Footer << "\n";
        return static_cast<int>(tokens);

    } catch(scan::ScanExcept &e) {
        std::cerr << "Fatal error: " << e.why() << "\n";
    }
    return 0;
}


extern int html_main(const char *filename, std::ostream &out) {
    std::fstream file;
    file.open(filename, std::ios::in);
    std::ostringstream stream;
    stream << file.rdbuf();
    file.close();
    if(stream.str().length()>0) {
        return html_scanFile(stream.str(), out);
    }
    return 0;
}