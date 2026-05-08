/**
 * @file main.cpp
 * @brief MXVM command-line entry point — argument parsing, interpretation, and compilation dispatch
 * @author Jared Bruni
 */
#include "version_info.hpp"
#include "argz.hpp"
#include <csignal>
#include <dlfcn.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mxvm/mxvm.hpp>
#include <set>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif

enum class vm_action {
    null_action = 0,
    translate,
    interpret,
    compile
};
enum class vm_target {
    x86_64_linux,
    x86_64_macos,
    x86_64_windows
};

std::ostream &operator<<(std::ostream &out, const vm_action &ae) {
    switch (ae) {
    case vm_action::null_action:
        out << "null action";
        break;
    case vm_action::translate:
        out << "translate";
        break;
    case vm_action::interpret:
        out << "interpret";
        break;
    case vm_action::compile:
        out << "compille";
        break;
    default:
        std::cerr << "Error: ";
        break;
    }
    return out;
}

std::ostream &operator<<(std::ostream &out, const vm_target &t) {
    switch (t) {
    case vm_target::x86_64_linux:
        out << "linux";
        break;
    case vm_target::x86_64_macos:
        out << "macos";
        break;
    case vm_target::x86_64_windows:
        out << "windows";
        break;
    }
    return out;
}

struct Args {
    std::string source_file;
    std::string output_file;
    std::string module_path = "/usr/local/lib";
    std::string object_path = ".";
    std::string include_path = "/usr/local/include/mxvm/modules";
    vm_action action = vm_action::null_action;
    vm_target target = vm_target::x86_64_linux;
    std::vector<std::string> argv;
    mxvm::Platform platform = mxvm::Platform::LINUX;
    int platform_argc = 0;
    char **platform_argv = nullptr;
    bool Makefile = false;
    bool only_test = false;
    std::string toolchain;
};

template <typename T>
void print_help(T &type) {
    std::cout << Col("MXVM: Compiler/Interpreter ", mx::Color::BRIGHT_CYAN) << "v" << VERSION_INFO << "\n";
    std::cout << "(C) 2026 " << Col("LostSideDead Software", mx::Color::BRIGHT_BLUE) << "\n";
    std::cout << "https://lostsidedead.biz\n";
    type.help(std::cout);
}

int process_arguments(Args *args);
int action_translate(const mxvm::Platform &platform, std::unique_ptr<mxvm::Program> &program, Args *args);
int action_interpret(bool only_test, std::string_view include_path, std::string_view object_path, const std::vector<std::string> &argv, std::string_view input, std::string_view mod_path);
int translate_x64(const mxvm::Platform &platform, std::unique_ptr<mxvm::Program> &program, Args *args);
void collectAndRegisterAllExterns(std::unique_ptr<mxvm::Program> &program);
void createMakefile(Args *args);

Args proc_args(int argc, char **argv) {
    Args args;
    std::string p = "linux";
#ifdef _WIN32
    p = "windows";
#endif
#ifdef __APPLE__
    p = "macos";
#endif
#ifdef __linux__
    p = "linux";
#endif
    if (p == "macos") {
        args.platform = mxvm::Platform::DARWIN;
        args.target = vm_target::x86_64_macos;
    } else if (p == "linux") {
        args.platform = mxvm::Platform::LINUX;
        args.target = vm_target::x86_64_linux;
    } else if (p == "windows") {
        args.platform = mxvm::Platform::WINX64;
        args.target = vm_target::x86_64_windows;
    }

    std::vector<char *> v;
    int stop = 0;
    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) != "--args") {
            v.push_back(argv[i]);
        } else {
            stop = i;
            break;
        }
    }
    std::vector<std::string> arg_v;
    for (int i = stop + 1; i < argc; ++i) {
        args.argv.push_back(argv[i]);
    }
    mx::Argz<std::string> argz(static_cast<int>(v.size()), v.data());
    args.platform_argc = argc;
    args.platform_argv = argv;
    argz.addOptionSingleValue('o', "output file")
        .addOptionSingleValue('a', "action")
        .addOptionDoubleValue(128, "action", "action to take [translate,  interpret]")
        .addOptionSingleValue('t', "target")
        .addOptionDoubleValue(129, "target", "output target: [linux, macos]")
        .addOptionSingle('d', "debug mode")
        .addOptionDouble(130, "debug", "debug mode")
        .addOptionSingle('i', "instruction trace mode")
        .addOptionDouble(131, "instruction", "instruction trace on")
        .addOptionSingle('h', "Help")
        .addOptionDouble(132, "help", "Help output")
        .addOptionDouble(133, "html", "debug html")
        .addOptionSingle('D', "print html")
        .addOptionSingleValue('x', "object path")
        .addOptionDoubleValue(136, "object-path", "Object path")
        .addOptionSingleValue('p', "path")
        .addOptionDoubleValue(134, "path", "module path")
        .addOptionSingleValue('I', "include path")
        .addOptionDoubleValue(137, "include", "include path")
        .addOptionSingle('m', "gemerate Makefile")
        .addOptionDouble(140, "makefile", "generate Makefile")
        .addOptionDouble(141, "dry-run", "check for correctness do not execute")
        .addOptionSingleValue('T', "toolchain prefix")
        .addOptionDoubleValue(142, "toolchain", "cross-compilation toolchain prefix (e.g. x86_64-w64-mingw32)");

    if (argc == 1) {
        print_help(argz);
    }
    mx::Argument<std::string> arg;
    int value = 0;
    try {
        while ((value = argz.proc(arg)) != -1) {
            switch (value) {
            case 141:
                args.only_test = true;
                break;
            case 'T':
            case 142:
                args.toolchain = arg.arg_value;
                break;
            case 'm':
            case 140:
                args.Makefile = true;
                break;
            case 'I':
            case 137:
                args.include_path = arg.arg_value;
                break;
            case 'x':
            case 136:
                args.object_path = arg.arg_value;
                break;
            case 134:
            case 'p':
                args.module_path = arg.arg_value;
                break;
            case 133:
            case 'D':
                mxvm::html_mode = true;
                break;
            case 'h':
            case 132:
                print_help(argz);
                break;
            case 'i':
            case 131:
                mxvm::instruct_mode = true;
                break;
            case 'd':
            case 130:
                mxvm::debug_mode = true;
                break;
            case 'o':
                args.output_file = arg.arg_value;
                break;
            case 'a':
            case 128:
                if (arg.arg_value == "translate") {
                    args.action = vm_action::translate;
                } else if (arg.arg_value == "interpret") {
                    args.action = vm_action::interpret;
                } else if (arg.arg_value == "compile") {
                    args.action = vm_action::compile;
                } else {
                    throw mx::ArgException<std::string>("Error invalid action value");
                }
                break;
            case 't':
            case 129:
                if (arg.arg_value == "macos") {
                    args.platform = mxvm::Platform::DARWIN;
                    args.target = vm_target::x86_64_macos;
                } else if (arg.arg_value == "linux") {
                    args.platform = mxvm::Platform::LINUX;
                    args.target = vm_target::x86_64_linux;
                } else if (arg.arg_value == "windows") {
                    args.platform = mxvm::Platform::WINX64;
                    args.target = vm_target::x86_64_windows;
                } else {
                    throw mx::Exception("Error platform unsupported.\n");
                }
                break;
            default:
            case '-':
                args.source_file = arg.arg_value;
                break;
            }
        }
    } catch (mx::ArgException<std::string> &exec) {
        std::cerr << Col("MXVM: ", mx::Color::RED) << "Command Line Argument Parsing Sytnax Error: " << exec.text() << "\n";
        exit(EXIT_FAILURE);
    } catch (const mx::Exception &e) {
        std::cerr << Col("MXVM: ", mx::Color::RED) << " Exception: " << e.what() << "\n";
    }

    if (!args.source_file.empty()) {
        constexpr std::string_view ext = ".mxvm";
        if (args.source_file.size() < ext.size() ||
            std::string_view(args.source_file).substr(args.source_file.size() - ext.size()) != ext) {
            args.source_file += ext;
        }
    }
    if (args.source_file.empty()) {
        std::cerr << Col("MXVM: Error ", mx::Color::RED) << "source file required..\n";
        exit(EXIT_FAILURE);
    }
    if (args.module_path.empty()) {
        std::cerr << Col("MXVM: Errror: ", mx::Color::RED) << "please set module path.\n";
        exit(EXIT_FAILURE);
    }

    return args;
}

int main(int argc, char **argv) {
    Args args = proc_args(argc, argv);
    return process_arguments(&args);
}

int process_arguments(Args *args) {
    if (args->Makefile) {
        createMakefile(args);
        return EXIT_SUCCESS;
    }
    int exitCode = 0;
    if (!std::filesystem::is_regular_file(args->source_file) || !std::filesystem::exists(args->source_file)) {
        std::cerr << Col("MXVM: Error: ", mx::Color::RED) << "input file: " << args->source_file << " does not exist or is not regular file.\n";
        return EXIT_FAILURE;
    }

    std::unique_ptr<mxvm::Program> program(new mxvm::Program());
    program->setMainBase(program.get());
    program->platform = args->platform;
    if (args->action == vm_action::compile) {
        exitCode = action_translate(args->platform, program, args);
        if (exitCode == 0) {
            if (mxvm::Program::base != nullptr && !mxvm::Program::base->root_name.empty()) {
                if (args->platform == mxvm::Platform::LINUX || args->platform == mxvm::Platform::WINX64) {
                    std::vector<std::string> object_files;
                    std::string assembler = args->toolchain.empty() ? "as" : args->toolchain + "-as";
                    const char *as_env = getenv("AS");
                    if (as_env != nullptr) {
                        assembler = as_env;
                    }
                    std::string asflags;
                    const char *asf = getenv("ASFLAGS");
                    if (asf != nullptr) {
                        asflags = asf;
                    }

                    for (auto &f : mxvm::Program::base->filenames) {
                        std::string obj_file = f;
                        size_t pos = obj_file.rfind(".s");
                        if (pos != std::string::npos) {
                            obj_file.replace(pos, 2, ".o");
                        } else {
                            obj_file += ".o";
                        }
                        std::ostringstream as_cmd;
                        as_cmd << assembler << " " << asflags << " " << f << " -o " << obj_file;
                        std::cout << as_cmd.str() << "\n";

                        int as_result = system(as_cmd.str().c_str());
                        if (as_result != 0) {
                            std::cerr << Col("MXVM: ", mx::Color::RED) << "Assembly failed\n";
                            return EXIT_FAILURE;
                        }
                        object_files.push_back(obj_file);
                    }

                    std::string linker = args->toolchain.empty() ? "cc" : args->toolchain + "-gcc";
                    const char *cc_env = getenv("CC");
                    if (cc_env != nullptr) {
                        linker = cc_env;
                    }
                    std::string ldflags;
                    const char *ldf = getenv("LDFLAGS");
                    if (ldf != nullptr) {
                        ldflags = ldf;
                    }
                    ldflags += " -lm ";

                    std::ostringstream modules_archives;
                    std::set<std::string> arch;
                    for (auto &m : mxvm::Program::base->external) {
                        if (m.module == true && m.mod != "main" && m.name != "strlen") {
                            arch.insert(m.mod);
                        }
                    }

                    for (auto &m : arch) {
                        modules_archives << args->module_path << "/modules/" << m << "/libmxvm_" << m << "_static.a ";
                    }

                    std::ostringstream cc_cmd;
                    cc_cmd << linker << " ";

                    for (auto &obj : object_files) {
                        cc_cmd << obj << " ";
                    }

                    cc_cmd << modules_archives.str() << " "
                           << ldflags << " "
                           << "-o " << mxvm::Program::base->root_name;

                    std::cout << cc_cmd.str() << "\n";
                    int cc_result = system(cc_cmd.str().c_str());
                    if (cc_result != 0) {
                        std::cerr << Col("MXVM: ", mx::Color::RED) << "Linking failed\n";
                        return EXIT_FAILURE;
                    }
                } else if (args->platform == mxvm::Platform::DARWIN) {
                    std::vector<std::string> object_files;
                    std::string clang = args->toolchain.empty() ? "clang" : args->toolchain + "-clang";
                    const char *clang_env = getenv("CLANG");
                    if (clang_env != nullptr) {
                        clang = clang_env;
                    }
                    std::string cflags;
                    const char *cflags_env = getenv("CFLAGS");
                    if (cflags_env != nullptr) {
                        cflags = cflags_env;
                    }

                    for (auto &f : mxvm::Program::base->filenames) {
                        std::string obj_file = f;
                        size_t pos = obj_file.rfind(".s");
                        if (pos != std::string::npos) {
                            obj_file.replace(pos, 2, ".o");
                        } else {
                            obj_file += ".o";
                        }
                        std::ostringstream clang_cmd;
                        clang_cmd << clang << " -c " << cflags << " " << f << " -o " << obj_file;
                        std::cout << clang_cmd.str() << "\n";
                        int clang_result = system(clang_cmd.str().c_str());
                        if (clang_result != 0) {
                            std::cerr << Col("MXVM: ", mx::Color::RED) << "Assembly failed\n";
                            return EXIT_FAILURE;
                        }
                        object_files.push_back(obj_file);
                    }

                    std::string ldflags;
                    const char *ldf = getenv("LDFLAGS");
                    if (ldf != nullptr) {
                        ldflags = ldf;
                    }

                    ldflags += " -lm ";

                    std::ostringstream modules_archives;
                    std::set<std::string> arch;
                    for (auto &m : mxvm::Program::base->external) {
                        if (m.module == true && m.mod != "main" && m.name != "strlen") {
                            arch.insert(m.mod);
                        }
                    }
                    for (auto &m : arch) {
                        modules_archives << args->module_path << "/modules/" << m << "/libmxvm_" << m << "_static.a ";
                    }
                    std::ostringstream clang_link_cmd;
                    clang_link_cmd << clang << " ";
                    for (auto &obj : object_files) {
                        clang_link_cmd << obj << " ";
                    }
                    clang_link_cmd << modules_archives.str() << " "
                                   << ldflags << " "
                                   << "-o " << mxvm::Program::base->root_name;
                    std::cout << clang_link_cmd.str() << "\n";
                    int clang_link_result = system(clang_link_cmd.str().c_str());
                    if (clang_link_result != 0) {
                        std::cerr << Col("MXVM: ", mx::Color::RED) << "Linking failed\n";
                        return EXIT_FAILURE;
                    }
                }
            }
        }
    } else if (args->action == vm_action::translate) {
        exitCode = action_translate(args->platform, program, args);
    } else if (args->action == vm_action::interpret && !args->source_file.empty()) {
        exitCode = action_interpret(args->only_test, args->include_path, args->object_path, args->argv, args->source_file, args->module_path);
    } else if (args->action == vm_action::null_action && !args->source_file.empty()) {
        exitCode = action_interpret(args->only_test, args->include_path, args->object_path, args->argv, args->source_file, args->module_path);
    } else {
        std::cerr << Col("MXVM: Error ", mx::Color::RED) << "invalid action/command\n";
        return EXIT_FAILURE;
    }
    return exitCode;
}

int action_translate(const mxvm::Platform &platform, std::unique_ptr<mxvm::Program> &program, Args *args) {
    return translate_x64(platform, program, args);
}

int translate_x64(const mxvm::Platform &platform, std::unique_ptr<mxvm::Program> &program, Args *args) {
    std::string_view include_path = args->include_path;
    std::string_view object_path = args->object_path;
    std::string_view input = args->source_file;
    std::string_view mod_path = args->module_path;
    std::string_view output = args->output_file;
    try {

        std::string input_file(input);
        std::fstream file;
        file.open(input_file, std::ios::in);
        if (!file.is_open()) {
            throw mx::Exception("Error could not open file: " + input_file);
        }
        std::ostringstream stream;
        stream << file.rdbuf();
        file.close();
        mxvm::Parser parser(stream.str());
        parser.platform = platform;
        parser.scan();

        program->filename = input_file;
        parser.module_path = std::string(mod_path);
        parser.object_path = std::string(object_path);
        parser.include_path = include_path;
        if (parser.generateProgramCode(mxvm::Mode::MODE_COMPILE, program)) {
            collectAndRegisterAllExterns(program);
            std::string output_file(output);
            std::string program_name = output_file.empty() ? program->name + ".s" : output_file;
            std::fstream file;
            if (program->root_name == program->name)
                program->object = false;
            else
                program->object = true;
            file.open(program_name, std::ios::out);
            if (file.is_open()) {
                std::ostringstream code_v;
                program->generateCode(platform, program->object, code_v);
                program->assembly_code = code_v.str();
                std::string opt_code = program->gen_optimize(program->assembly_code, platform);
                file << opt_code;
                if (mxvm::html_mode) {
                    std::ofstream htmlFile(program->name + ".html");
                    if (htmlFile.is_open()) {
                        parser.generateDebugHTML(htmlFile, program);
                        std::cout << Col("MXVM: Generated Debug HTML for: ", mx::Color::BRIGHT_GREEN) << program->name << "\n";
                    }
                    htmlFile.close();
                }
                file.close();
                if (mxvm::Program::base != nullptr) {
                    mxvm::Program::base->add_filename(program->name + ".s");
                }
            }
        } else {
            std::cerr << Col("MXVM: Exception: ", mx::Color::RED) << "Failed to generate intermedaite code" << "\n";
            exit(EXIT_FAILURE);
        }
    } catch (const mx::Exception &e) {
        std::cerr << Col("MXVM: Exception: ", mx::Color::RED) << e.what() << "\n";
        if (mxvm::debug_mode) {
            program->print(std::cerr);
            program->memoryDump(std::cerr);
        }
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << Col("MXVM: Runtime Error: ", mx::Color::RED) << e.what() << "\n";
        if (mxvm::debug_mode) {
            program->print(std::cerr);
            program->memoryDump(std::cerr);
        }
        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << Col("MXVM: Exception: ", mx::Color::RED) << e.what() << "\n";
        if (mxvm::debug_mode) {
            program->print(std::cerr);
            program->memoryDump(std::cerr);
        }
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << Col("MXVM: ", mx::Color::RED) << "Unknown Exception.\n";
        if (mxvm::debug_mode) {
            program->print(std::cerr);
            program->memoryDump(std::cerr);
        }
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

mxvm::Program *signal_program = nullptr;

#ifndef _WIN32
void signal_action(int signum) {
    if (signum == SIGINT) {
        std::cout << "MXVM: Signal SIGINT Received Exiting...\n";
        if (mxvm::debug_mode) {
            std::cout << "MXVM: Debug Mode Dumping Memory.\n";
            if (signal_program != nullptr)
                signal_program->memoryDump(std::cout);
        }
        exit(EXIT_FAILURE);
    }
}
#else
BOOL WINAPI CtrlHandler(DWORD ctrlType) {
    switch (ctrlType) {
    case CTRL_C_EVENT: {
        std::cout << "MXVM: Signal SIGINT Received Exiting...\n";
        if (mxvm::debug_mode) {
            std::cout << "MXVM: Debug Mode Dumping Memory.\n";
            if (signal_program != nullptr)
                signal_program->memoryDump(std::cout);
        }
        exit(EXIT_FAILURE);
    }
        return TRUE;
    case CTRL_CLOSE_EVENT:
        return TRUE;
    default:
        return FALSE;
    }
}
#endif

int action_interpret(bool only_test, std::string_view include_path, std::string_view object_path, const std::vector<std::string> &argv, std::string_view input, std::string_view mod_path) {
    int exitCode = 0;
    std::unique_ptr<mxvm::Program> program(new mxvm::Program());
    program->setArgs(argv);
    program->setMainBase(program.get());
    std::fstream debug_output;
    if (mxvm::debug_mode) {
        debug_output.open("debug_info.txt", std::ios::out);
        if (!debug_output.is_open()) {
            std::cerr << Col("MXVM: Error ", mx::Color::RED) << "could not open debug_info.txt\n";
        }
    }
    signal_program = program.get();
#ifndef _WIN32
    struct sigaction sa;
    sa.sa_handler = signal_action;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, nullptr);
#else
    SetConsoleCtrlHandler(CtrlHandler, TRUE);
#endif

    try {
        std::string input_file(input);
        std::fstream file;
        file.open(input_file, std::ios::in);
        if (!file.is_open()) {
            throw mx::Exception("Error could not open file: " + input_file);
        }
        program->filename = input_file;
        std::ostringstream stream;
        stream << file.rdbuf();
        file.close();
        mxvm::Parser parser(stream.str());
        parser.scan();
        bool uses_std_module = false;
        parser.module_path = std::string(mod_path);
        parser.object_path = std::string(object_path);
        parser.include_path = std::string(include_path);

        class ArgsRaii {
          public:
            ArgsRaii() = default;
            void init(const std::vector<std::string> &argv, void *handle) {
                std::vector<const char *> c_argv;
                c_argv.push_back("mxvmc");
                for (const auto &arg : argv) {
                    c_argv.push_back(arg.c_str());
                }

                void *set_args = (void *)dlsym(handle, "set_program_args");
                using set_program_args_t = void (*)(int, const char **);
                if (set_args) {
                    auto func = reinterpret_cast<set_program_args_t>(set_args);
                    func(static_cast<int>(c_argv.size()), c_argv.data());
                    set = true;
                    handle_ = handle;
                }
            }
            ~ArgsRaii() {
                void *free_args = (void *)dlsym(handle_, "free_program_args");
                using set_program_args_t = void (*)();
                if (free_args) {
                    auto func = reinterpret_cast<set_program_args_t>(free_args);
                    func();
                    set = false;
                }
            }

            bool set = false;
            void *handle_ = nullptr;
        };

        if (parser.generateProgramCode(mxvm::Mode::MODE_INTERPRET, program)) {
            if (program->object) {
                throw mx::Exception("Requires one program object to execute");
            }
            ArgsRaii args_raii;

            if (mxvm::Program::base != nullptr) {
                void *handle = nullptr;
                for (const auto &ext : mxvm::Program::base->external_functions) {
                    if (ext.second.mod_name == "std") {
                        uses_std_module = true;
                        handle = ext.second.handle;
                        break;
                    }
                }
                if (uses_std_module) {
                    args_raii.init(argv, handle);
                }
            }
            program->flatten(program.get());
            if (only_test == false)
                exitCode = program->exec();

            if (mxvm::debug_mode) {
                if (debug_output.is_open()) {
                    program->print(debug_output);
                    program->post(debug_output);
                    program->memoryDump(debug_output);
                }
            }

            if (mxvm::html_mode) {
                std::cout << Col("MXVM: Generated ", mx::Color::BRIGHT_GREEN) << program->name << ".html\n";
                std::ofstream fout(program->name + ".html");
                parser.generateDebugHTML(fout, program);
                fout.close();
            }
        } else {
            std::cerr << Col("MXVM: Error: ", mx::Color::RED) << "Failed to generate intermediate code.\n";
            return EXIT_FAILURE;
        }
    } catch (const mx::Exception &e) {
        std::cerr << Col("MXVM: Exception: ", mx::Color::RED) << e.what() << "\n";
        if (mxvm::debug_mode) {
            if (debug_output.is_open()) {
                program->print(debug_output);
                program->memoryDump(debug_output);
            }
        }
        return EXIT_FAILURE;
    } catch (const std::runtime_error &e) {
        std::cerr << Col("MXVM: Runtime Error: ", mx::Color::RED) << e.what() << "\n";
        if (mxvm::debug_mode) {
            if (debug_output.is_open()) {
                program->print(debug_output);
                program->memoryDump(debug_output);
            }
        }
        return EXIT_FAILURE;
    } catch (const std::exception &e) {
        std::cerr << Col("MXVM: Exception: ", mx::Color::RED) << e.what() << "\n";
        if (mxvm::debug_mode) {
            if (debug_output.is_open()) {
                program->print(debug_output);
                program->memoryDump(debug_output);
            }
        }
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << Col("MXVM: ", mx::Color::RED) << "Unknown Exception.\n";
        if (mxvm::debug_mode) {
            if (debug_output.is_open()) {
                program->print(debug_output);
                program->memoryDump(debug_output);
            }
        }
        return EXIT_FAILURE;
    }

    if (mxvm::debug_mode) {
        if (debug_output.is_open()) {
            debug_output.close();
            std::cout << Col("MXVM: Generated: ", mx::Color::BRIGHT_GREEN) << "debug information: debug_info.txt\n";
        }
    }
    if (mxvm::debug_mode) {
        std::cout << Col("MXVM: ", mx::Color::RED) << "Program exited with: " << exitCode << "\n";
    }
    return exitCode;
}

void collectAndRegisterAllExterns(std::unique_ptr<mxvm::Program> &program) {
    for (const auto &obj : program->objects) {
        for (auto &lbl : obj->labels) {
            if (lbl.second.second)
                mxvm::Program::base->add_extern(obj->name, lbl.first, false);
        }
        for (auto &v : obj->vars) {
            mxvm::Program::base->add_extern(obj->name, v.first, false);
        }
    }
}

void createMakefile(Args *args) {

    if(args == nullptr) {
        std::cerr << "Error arguments null.\n";
        return;
    }

    std::ofstream file("Makefile");
    if (!file.is_open()) {
        std::cerr << Col("MXVM Error: ", mx::Color::BRIGHT_RED) << " could not open file\n";
        exit(EXIT_FAILURE);
    }
    char *ldflags = getenv("LDFLAGS");
    std::string ld_flags;
    if (ldflags != nullptr)
        ld_flags = ldflags;

    // Derive program name from source file (strip path and .mxvm extension)
    std::string prog_name = args->source_file;
    {
        size_t slash = prog_name.rfind('/');
        if (slash != std::string::npos)
            prog_name = prog_name.substr(slash + 1);
        size_t dot = prog_name.rfind(".mxvm");
        if (dot != std::string::npos)
            prog_name = prog_name.substr(0, dot);
    }

    // Build the mxvmc compile command with all relevant flags
    std::ostringstream compile_cmd;
    compile_cmd << "mxvmc \"" << args->source_file << "\""
                << " --path \"" << args->module_path << "\""
                << " --object-path \"" << args->object_path << "\""
                << " --action compile"
                << " --target " << args->target;
    if (!args->toolchain.empty())
        compile_cmd << " --toolchain " << args->toolchain;
    if (!args->include_path.empty())
        compile_cmd << " --include \"" << args->include_path << "\"";

    file << "### Makefile generated by MXVM\n";
    file << "### Source: " << args->source_file << "\n";
    file << "### Target: " << args->target << "\n";
    if (!args->toolchain.empty())
        file << "### Toolchain: " << args->toolchain << "\n";
    file << "\n";
    file << "PROGRAM = " << prog_name << "\n";
    file << "LDFLAGS = " << ld_flags << "\n";
    file << "\n";
    file << ".PHONY: all run clean\n\n";
    file << "all:\n";
    file << "\tLDFLAGS=\"$(LDFLAGS)\" " << compile_cmd.str() << "\n\n";
    file << "run:\n";
    file << "\tmxvmc \"" << args->source_file << "\""
         << " --path \"" << args->module_path << "\""
         << " --object-path \"" << args->object_path << "\""
         << " --target " << args->target;
    if (!args->toolchain.empty())
        file << " --toolchain " << args->toolchain;
    file << "\n\n";
    file << "clean:\n";
    file << "\trm -f $(PROGRAM) $(PROGRAM).exe *.o *.s\n";
    file.close();
    std::cout << Col("MXVM: ", mx::Color::BRIGHT_GREEN) << "Created Makefile for " << prog_name << "\n";
}
