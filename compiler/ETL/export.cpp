#define VERSION_INFO "1.0"
#include<unordered_map>
#include<emscripten.h>
#include<emscripten/bind.h>
#include<iostream>
#include<string>
#include<sstream>
#include"parser.hpp"
#include"ir.hpp"
#include"ir_opt.hpp"

extern void outputDebugInfo(std::ostream &file, symbol::SymbolTable &table, const ir::IRCode &code);

std::string test_parse(const std::string &data) {
    std::ostringstream stream;
    scan::TString p(data);
    std::ostringstream code;
    bool in_string = false;
    bool escaped = false;
    while (1) {
        auto c = p.getch();
        if (c.has_value()) {
            if (escaped) {
                code << *c;
                escaped = false;
            } else if (*c == '\\') {
                escaped = true;
                code << *c;
            } else if (*c == '"') {
                in_string = !in_string;
                code << *c;
            } else if (*c == '#' && !in_string) {
                do {
                    c = p.getch();
                } while (c.has_value() && *c != '\n');
            } else {
                code << *c;
            }
        } else break;
    }
    try { 
        if(data.length()>0) {
            parse::Parser parser(new scan::Scanner(scan::TString(code.str())));
            if(parser.parse()) {
                auto rootAST = parser.getAST();  
                if (rootAST) {
                            parse::IRGenerator irGen;
                            auto irContext = irGen.generateIR(rootAST);  
                            ir::IROptimizer opt;
                            irContext.instructions = std::move(opt.optimize(irContext.instructions));
                            outputDebugInfo(stream, irContext.table, irContext.instructions);
                            return stream.str();
                }
            } else {
                stream << "<b>ETL: Parsing failed...</b>\n";
                return stream.str();
            }
        } else {
            stream << "<b>ETL: Zero bytes to read..</b>\n";
            return stream.str();
        }

        } catch(ir::IRException &e) {
            stream << "<b>ETL: IR Exception: " << e.why() << "</b>\n";
            return stream.str();
        } catch(parse::ParseException &e) {
            stream << "<b>ETL: Parse Exception: " << e.why() << "</b>\n";
            return stream.str();
        } 
        catch(...) {
            stream << "<b>ETL: Unknown Exception</b>\n";
            return stream.str();
        }
    
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
