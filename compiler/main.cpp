#include"scanner.hpp"
#include<fstream>
#include<sstream>
#include<iostream>
#include<memory>
#include"argz.hpp"
#include<cstdlib>
#include<string>

extern int html_main(const char *filename, std::ostream &out);

int scanFile(const std::string &contents) {
    try {
        std::unique_ptr<scan::Scanner> scan(new scan::Scanner(scan::TString(contents)));
        uint64_t tokens = scan->scan();
        std::cout << "Sucessfull scan returned: " << tokens << " token(s)...\n";

        for(size_t i = 0; i < scan->size(); ++i) {
            scan->operator[](i).print(std::cout);
        }
        return static_cast<int>(tokens);
    } catch(scan::ScanExcept &e) {
        std::cerr << "Fatal error: " << e.why() << "\n";
    }
    return 0;
}

int main(int argc, char **argv) {
    Argz<std::string> argz(argc, argv);
    argz.addOptionSingleValue('i', "input text").addOptionSingleValue('o', "output file").addOptionSingle('h', "help").addOptionSingle('v', "help");
    std::string in_file, out_file;
    int value = 0;

    Argument<std::string> arg;
    try {
        while((value = argz.proc(arg)) != -1) {
            switch(value) {
                case 'h':
                case 'v':
                    argz.help(std::cout);
                    exit(EXIT_SUCCESS);
                    break;
                case 'i':
                    in_file = arg.arg_value;
                    break;
                case 'o':
                    out_file = arg.arg_value;
                    break;
            }
        }
    } catch(const ArgException<std::string> &e) {
        std::cerr << "Syntax Error: " << e.text() << "\n";
    }

    if(in_file.length() == 0) {
        std::cerr << "Input file must be provided...use -i \n";
        exit(EXIT_FAILURE);
    }

    std::fstream file;
    file.open(in_file, std::ios::in);
    std::ostringstream stream;
    stream << file.rdbuf();
    file.close();

    if(in_file.length()>0 && out_file.length()==0) {
        if(stream.str().length()>0) {
            return scanFile(stream.str());
        }
    } else if(out_file.length()>0) {
        std::fstream file;
        file.open(out_file, std::ios::out);
        if(!file.is_open()) {
            std::cerr << "Error could not open output file: " << out_file << "\n";
            exit(EXIT_FAILURE);
        }
        html_main(in_file.c_str(), file);
    }
    return 0;
}