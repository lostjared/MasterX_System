#include<iostream>
#include<string>
#include"parser.hpp"
#include<fstream>
#include<sstream>
#include"ir.hpp"
#include"ir_opt.hpp"
#include"interp.hpp"

extern void outputDebugInfo(std::ostream &file, symbol::SymbolTable &table, const ir::IRCode &code);

void test_execute(const std::string &filename, bool debug_info) {

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
                            interp::Interpreter interp_(irContext.table);
                            try {
                                int rt_exit = interp_.execute(irContext.instructions);
                                interp_.outputDebugInfo(std::cout);
                                std::cout << "exited: " << rt_exit << "\n";
                                exit(rt_exit);
                            } catch(const interp::Exception  &e) {
                                std::cerr << "Exception: " << e.why() << "\n";
                            } catch(const interp::Exit_Exception &e) {
                                interp_.outputDebugInfo(std::cout);
                                std::cout << "exited: " << e.status() <<"\n";
                                exit(e.status());
                            }    
                            catch (...) {
                                std::cerr << "Unknown Exception\n";
                            }
                            exit(EXIT_FAILURE);
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