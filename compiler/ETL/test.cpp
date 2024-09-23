#include<iostream>
#include<string>
#include"parser.hpp"
#include<fstream>
#include<sstream>
#include"ir.hpp"
#include"ir_opt.hpp"
#include"codegen.hpp"

extern void outputDebugInfo(std::ostream &file, symbol::SymbolTable &table, const ir::IRCode &code);

void test_parse(const std::string &filename, const std::string &out_file, bool debug_info) {
    std::fstream file;
    file.open(filename, std::ios::in);
    if(!file.is_open()) {
        std::cerr << "ETL: Could not open: " << filename << ".\n";
        exit(EXIT_FAILURE);
    }  
    std::ostringstream stream;
    stream << file.rdbuf() << " \n";
    file.close();
    scan::TString p(stream.str());
    bool in_string = false;
    bool escaped = false;
    try {

        if(stream.str().length()>0) {
            parse::Parser parser(new scan::Scanner(p));
            if(parser.parse()) {
                auto rootAST = parser.getAST();  
                if (rootAST) {
                            parse::IRGenerator irGen;
                            auto irContext = irGen.generateIR(rootAST);  
                            ir::IROptimizer opt;
                            irContext.instructions = std::move(opt.optimize(irContext.instructions));
                            std::cout << "ETL: IR code: {\n";
                            for (const auto &instr : irContext.instructions) {
                                std::cout << "\t" << instr.toString() << "\n";
                            }
                            if(debug_info == true) {
                                std::fstream file;
                                file.open("debug.html", std::ios::out);
                                if(!file.is_open()) {
                                    std::cerr << "Error could not open debug output file debug.html\n";
                                    exit(EXIT_FAILURE);
                                }
                                outputDebugInfo(file, irContext.table, irContext.instructions);
                                file.close();
                            }
                            codegen::CodeEmitter emitter(irContext.table, irContext.functionLocalVarCount);
                            std::string text = emitter.emit(irContext.instructions);
                            std::fstream ofile;
                            ofile.open(out_file, std::ios::out);
                            ofile << text << "\n";
                            ofile.close();
                            
                            std::cout << "}\n";
                            if(debug_info == true)
                                std::cout << "ETL: outputted debug info [debug.html]\n";

                            std::cout << "ETL: compiled [" << out_file << "]\n";
                            exit(EXIT_SUCCESS);
                            
                }
            } else {
                std::cerr << "ETL: Parsing failed...\n";
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << "ETL: Zero bytes to read..\n";
            exit(EXIT_FAILURE);
        }
    }
    catch(ir::IRException &e) {
            std::cerr << "ETL: IR Exception: " << e.why() << "\n";
            exit(EXIT_FAILURE);
    }
    catch(parse::ParseException &e) {
        std::cerr << "ETL: Parse Exception: " << e.why() << "\n";
        exit(EXIT_FAILURE);
    }
    catch(...) {
            std::cerr << "ETL: Unknown Exception\n";
            exit(EXIT_FAILURE);
    }
}