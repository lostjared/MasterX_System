// Coded by Jared Bruni GPL v3

#include"argz.hpp"
#include"version_info.hpp"

extern void test_parse(const std::string &, const std::string &, bool debug_info, bool quiet_mode);

void print_about() {
    std::cout << "ETL: ETL v" << etl_version << "\n";
    std::cout << "ETL: Writen by Jared Bruni\n";
    std::cout << "ETL: http://lostsidedead.biz\n";
}

int main(int argc, char **argv)  {
    Argz<std::string> argz(argc, argv);
    argz.addOptionSingleValue('i', "input text file")
    .addOptionSingleValue('o', "output file")
    .addOptionSingle('h', "help")
    .addOptionSingle('v', "help")
    .addOptionSingle('d', "Output HTML debug info")
    .addOptionSingle('q', "Quiet mode");

    std::string in_file, out_file;
    int value = 0;
    Argument<std::string> arg;
    bool debug_info = false;
    bool quiet_mode = false;

    try {
        while((value = argz.proc(arg)) != -1) {
            switch(value) {
                case 'h':
                case 'v':
                    print_about();
                    argz.help(std::cout);
                    exit(EXIT_SUCCESS);
                    break;
                case 'i':
                    in_file = arg.arg_value;
                    break;
                case 'o':
                    out_file = arg.arg_value;
                    break;
                case 'd':
                    debug_info = true;
                    break;
                case 'q':
                    quiet_mode = true;
                    break;
            }
        }
    } catch(const ArgException<std::string> &e) {
        std::cerr << "Syntax Error: " << e.text() << "\n";
    }
    if(in_file.length() == 0 || out_file.length() == 0) {
        print_about();
        std::cerr << "Input/Output file(s) must be provided...use -i and -o\n";
        argz.help(std::cerr);
        exit(EXIT_FAILURE);
    }
    test_parse(in_file, out_file, debug_info,quiet_mode);
    return 0;
}