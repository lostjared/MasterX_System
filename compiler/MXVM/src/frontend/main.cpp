/**
 * @file main.cpp
 * @brief Pascal-to-MXVM compiler command-line entry point
 * @author Jared Bruni
 */
#include "icode.hpp"
#include "parser.hpp"
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_set>
#include "argz.hpp"
#include "version_info.hpp"



std::string removeComments(const std::string &text) {
    std::ostringstream stream;
    enum State {
        CODE,
        BRACE_COMMENT,
        PAREN_COMMENT,
        STRING
    };
    State state = CODE;

    for (size_t i = 0; i < text.length(); ++i) {
        char c = text[i];
        char next = (i + 1 < text.length()) ? text[i + 1] : '\0';

        switch (state) {
        case CODE:
            if (c == '{') {
                state = BRACE_COMMENT;
            } else if (c == '(' && next == '*') {
                state = PAREN_COMMENT;
                i++;
            } else if (c == '\'') {
                stream << c;
                state = STRING;
            } else if (c == '/' && next == '/') {

                while (i < text.length() && text[i] != '\n') {
                    i++;
                }
                if (i < text.length()) {
                    stream << '\n';
                }
            } else {
                stream << c;
            }
            break;

        case BRACE_COMMENT:
            if (c == '}') {
                state = CODE;
            }
            break;

        case PAREN_COMMENT:
            if (c == '*' && next == ')') {
                state = CODE;
                i++;
            }
            break;

        case STRING:
            stream << c;
            if (c == '\'') {
                if (next == '\'') {
                    stream << next;
                    i++;
                } else {
                    state = CODE;
                }
            }
            break;
        }
    }

    return stream.str();
}

static bool compileFile(const std::string &inputPath, const std::string &outputPath) {
    try {
        std::ifstream file(inputPath);
        if (!file.is_open()) {
            std::cerr << "mxx: cannot open file: " << inputPath << "\n";
            return false;
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();

        if (buffer.str().empty()) {
            std::cerr << "mxx: file is empty: " << inputPath << "\n";
            return false;
        }

        std::string source = buffer.str();
        pascal::PascalParser parser(removeComments(source));
        if (!parser.validator.validate(inputPath)) {
            std::cerr << "mxx: validation failed: " << inputPath << "\n";
            return false;
        }

        pascal::CodeGenVisitor emiter;

        if (parser.isUnitSource()) {
            auto ast = parser.parseUnit();
            if (ast) {
                // For units with uses clause, parse each dependency's interface
                // to import their declarations (vars, arrays, consts, types, funcs)
                std::string inputDir = inputPath.substr(0, inputPath.find_last_of("/\\") + 1);
                if (inputDir.empty()) inputDir = "./";

                static const std::unordered_set<std::string> nativeModules = {"io", "std", "string", "sdl", "strlib"};
                for (const auto &dep : ast->uses) {
                    if (nativeModules.count(dep)) continue;

                    std::string unitFile = inputDir + dep + ".pas";
                    std::ifstream uf(unitFile);
                    if (!uf.is_open()) {
                        std::string lowerDep = dep;
                        std::transform(lowerDep.begin(), lowerDep.end(), lowerDep.begin(), ::tolower);
                        unitFile = inputDir + lowerDep + ".pas";
                        uf.open(unitFile);
                    }
                    if (uf.is_open()) {
                        std::ostringstream ubuf;
                        ubuf << uf.rdbuf();
                        uf.close();
                        try {
                            pascal::PascalParser unitParser(removeComments(ubuf.str()));
                            auto unitAst = unitParser.parseUnit();
                            if (unitAst) {
                                for (auto &decl : unitAst->interfaceDecls) {
                                    if (auto *fd = dynamic_cast<pascal::FuncDeclNode *>(decl.get()))
                                        emiter.registerExternalFunc(fd->name, dep);
                                    else if (auto *pd = dynamic_cast<pascal::ProcDeclNode *>(decl.get()))
                                        emiter.registerExternalFunc(pd->name, dep);
                                    else if (auto *vd = dynamic_cast<pascal::VarDeclNode *>(decl.get())) {
                                        emiter.registerImportedVar(dep, *vd);
                                        continue;
                                    } else if (auto *cd = dynamic_cast<pascal::ConstDeclNode *>(decl.get())) {
                                        emiter.registerImportedConst(dep, *cd);
                                        continue;
                                    }
                                    ast->interfaceDecls.push_back(std::move(decl));
                                }
                            }
                        } catch (...) {
                        }
                    }
                }

                emiter.generate(ast.get());
            }
        } else {
            auto ast = parser.parseProgram();
            if (ast) {
                // For programs with unit imports, parse each unit's interface
                // to register function/procedure signatures
                std::string inputDir = inputPath.substr(0, inputPath.find_last_of("/\\") + 1);
                if (inputDir.empty()) inputDir = "./";

                for (const auto &dep : ast->uses) {
                    // Skip native modules
                    static const std::unordered_set<std::string> nativeModules = {"io", "std", "string", "sdl", "strlib"};
                    if (nativeModules.count(dep)) continue;

                    // Try to find and parse the unit's .pas file for interface declarations
                    std::string unitFile = inputDir + dep + ".pas";
                    std::ifstream uf(unitFile);
                    if (!uf.is_open()) {
                        // Try lowercase
                        std::string lowerDep = dep;
                        std::transform(lowerDep.begin(), lowerDep.end(), lowerDep.begin(), ::tolower);
                        unitFile = inputDir + lowerDep + ".pas";
                        uf.open(unitFile);
                    }
                    if (uf.is_open()) {
                        std::ostringstream ubuf;
                        ubuf << uf.rdbuf();
                        uf.close();
                        try {
                            pascal::PascalParser unitParser(removeComments(ubuf.str()));
                            auto unitAst = unitParser.parseUnit();
                            if (unitAst) {
                                // Register interface declarations as forward declarations in the main AST
                                // and track which functions/procedures are external
                                for (auto &decl : unitAst->interfaceDecls) {
                                    if (auto *fd = dynamic_cast<pascal::FuncDeclNode *>(decl.get()))
                                        emiter.registerExternalFunc(fd->name, dep);
                                    else if (auto *pd = dynamic_cast<pascal::ProcDeclNode *>(decl.get()))
                                        emiter.registerExternalFunc(pd->name, dep);
                                    else if (auto *vd = dynamic_cast<pascal::VarDeclNode *>(decl.get())) {
                                        emiter.registerImportedVar(dep, *vd);
                                        continue;
                                    } else if (auto *cd = dynamic_cast<pascal::ConstDeclNode *>(decl.get())) {
                                        emiter.registerImportedConst(dep, *cd);
                                        continue;
                                    }
                                    ast->block->declarations.push_back(std::move(decl));
                                }
                            }
                        } catch (...) {
                            // Unit source not available; user must ensure correct call signatures
                        }
                    }
                }

                emiter.generate(ast.get());
            }
        }

        {
            std::fstream outFile;
            outFile.open(outputPath, std::ios::out);
            if (!outFile.is_open()) {
                std::cerr << "mxx: cannot open output file: " << outputPath << "\n";
                return false;
            }
            std::ostringstream output;
            emiter.writeTo(output);
            std::string opt = pascal::mxvmOpt(output.str());
            outFile << opt << "\n";
        }
        return true;
    } catch (const pascal::ParseException &e) {
        std::cerr << "Parse Error: " << e.what() << "\n";
        return false;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return false;
    } catch (const scan::ScanExcept &e) {
        std::cerr << "Syntax Error: " << e.why() << "\n";
        return false;
    }
}

// Extract the declared program or unit name from a Pascal source file.
// Returns empty string if not found.
static std::string extractPasName(const std::string &filePath) {
    std::ifstream f(filePath);
    if (!f.is_open()) return {};
    std::ostringstream buf;
    buf << f.rdbuf();
    std::string src = removeComments(buf.str());
    // Scan tokens: skip whitespace, look for "program"|"unit" then the identifier before ';'
    size_t i = 0;
    auto skipWS = [&]() {
        while (i < src.size() && (src[i] == ' ' || src[i] == '\t' || src[i] == '\r' || src[i] == '\n'))
            ++i;
    };
    auto readIdent = [&]() -> std::string {
        skipWS();
        std::string id;
        while (i < src.size() && (std::isalnum(static_cast<unsigned char>(src[i])) || src[i] == '_'))
            id += src[i++];
        return id;
    };
    std::string kw = readIdent();
    std::string lkw = kw;
    std::transform(lkw.begin(), lkw.end(), lkw.begin(), ::tolower);
    if (lkw == "program" || lkw == "unit") {
        std::string name = readIdent();
        if (!name.empty()) return name;
    }
    return {};
}

int main(int argc, char **argv) {
    mx::Argz<std::string> args(argc, argv);
    args.addOptionSingle('c', "Compile one or more Pascal source files to .mxvm bytecode")
        .addOptionSingleValue('i', "Input Pascal source file")
        .addOptionSingleValue('o', "Output .mxvm file");

    bool compile_mode = false;
    std::string input_file;
    std::string output_file;
    std::vector<std::string> files;

    try {
        mx::Argument<std::string> arg;
        int r;
        while ((r = args.proc(arg)) != -1) {
            switch (r) {
            case 'c':
                compile_mode = true;
                break;
            case 'i':
                input_file = arg.arg_value;
                break;
            case 'o':
                output_file = arg.arg_value;
                break;
            case '-':
                files.push_back(arg.arg_value);
                break;
            }
        }
    } catch (const mx::ArgException<std::string> &e) {
        std::cerr << "mxx: " << e.text() << "\n";
        return EXIT_FAILURE;
    }

    if (!input_file.empty()) {
        // -i / -o mode
        if (output_file.empty()) {
            std::string name = extractPasName(input_file);
            if (!name.empty()) {
                output_file = name + ".mxvm";
            } else {
                output_file = input_file;
                if (output_file.size() > 4 && output_file.substr(output_file.size() - 4) == ".pas")
                    output_file = output_file.substr(0, output_file.size() - 4) + ".mxvm";
                else
                    output_file += ".mxvm";
            }
        }
        return compileFile(input_file, output_file) ? 0 : EXIT_FAILURE;
    } else if (compile_mode) {
        if (files.empty()) {
            std::cerr << "mxx: -c requires at least one source file\n";
            return EXIT_FAILURE;
        }
        for (const auto &src : files) {
            std::string name = extractPasName(src);
            std::string out;
            if (!name.empty()) {
                out = name + ".mxvm";
            } else {
                out = src;
                if (out.size() > 4 && out.substr(out.size() - 4) == ".pas")
                    out = out.substr(0, out.size() - 4) + ".mxvm";
                else
                    out += ".mxvm";
            }
            if (!compileFile(src, out))
                return EXIT_FAILURE;
        }
    } else {
        // Legacy mode: mxx <source> <output>
        if (files.size() != 2) {
            std::cerr << argv[0] << " v" << VERSION_INFO << "\n(C) 2026 LostSideDead Software\n\n";
            std::cerr << "Usage:\n"
                      << "  " << argv[0] << " <source.pas> <output.mxvm>\n"
                      << "  " << argv[0] << " -i <source.pas> [-o <output.mxvm>]\n"
                      << "  " << argv[0] << " -c <source1.pas> [source2.pas ...]\n";
            return EXIT_FAILURE;
        }
        if (!compileFile(files[0], files[1]))
            return EXIT_FAILURE;
    }

    return 0;
}
