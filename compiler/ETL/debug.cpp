#include<iostream>
#include<fstream>
#include<sstream>
#include"types.hpp"
#include"ir.hpp"

std::string htmlPage_Header = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Debug Info</title>
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
        .centert {
            text-align: center;
        }
    </style>
</head>
<body>
)";

std::string htmlPage_Footer = R"(
</body>
</html>
)";

void outputDebugInfo(std::ostream &file, symbol::SymbolTable &table, const ir::IRCode &code) {
    file << htmlPage_Header << "\n";
    file << "<div class=\"centert\"><h1>Instruction Table</h1></div>\n";
    file << "<table>\n";
    file << "<tr><th>Index</th><th>Instruction</th>\n";
    uint64_t index = 0;
      for(const auto &i : code) {
        file << "<tr><td>" << index << "</td><td>" << i.toString() << "</td></tr>\n";
        index++;
    }
    file << "</table>\n";
    file << "<div class=\"centert\"><h1>Symbol Table</h1></div>\n";
    file <<  "<table>\n";
    file << "<tr><th>Function</th><th>Symbol</th><th>Type</th></tr>\n";
    std::string curFunction;
    for(const auto  &t: table.getTable()) {
        curFunction = t.first;
        for(const auto &s : t.second) {
            file << "<tr><td>" << curFunction << "</td><td>" << s.first << "</td><td>" << ast::VarString.at(static_cast<int>(s.second.vtype)) << "</td></tr>\n";
        }
    }
    file << "</table>\n";
    file << htmlPage_Footer << "\n";
}
