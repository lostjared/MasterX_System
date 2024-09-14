#define VERSION_INFO "1.0"
#include<unordered_map>
#include<emscripten.h>
#include<emscripten/bind.h>
#include<iostream>
#include<string>
#include<sstream>

extern int html_scanFile(const std::string &contents, std::ostream &file);


std::string test_parse(const std::string &data) {
        std::ostringstream stream;
        html_scanFile(data, stream);
        return stream.str();
} 

class ETL_Export {
public:
    ETL_Export() {
    }
    std::string scan(std::string input_line) {
        return test_parse(input_line);
    }
};

using namespace emscripten;

EMSCRIPTEN_BINDINGS(my_ETL) {
    class_<ETL_Export>("ETL")
        .constructor()
        .function("scan", &ETL_Export::scan)
    ;
}
