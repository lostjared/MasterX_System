/**
 * @file html.cpp
 * @brief mxvm-html command-line tool — converts MXVM source to syntax-highlighted HTML
 * @author Jared Bruni
 *
 * Usage:
 *   mxvm-html -i input.mxvm  [-o output.html] [-s style.css]
 *   mxvm-html -i input.pas   [-o output.html] [-s style.css]
 *   mxvm-html -c file1.mxvm file2.pas ...     [-s style.css]
 *   mxvm-html -h
 *
 * For .pas input the output is Pascal-syntax-highlighted using PasHTMLGen.
 * An external stylesheet is linked from the HTML so you can change the look
 * without recompiling.  By default mxvm-highlight.css is written to the same
 * directory as the first output file (only when the file does not exist yet).
 * Use -s to point to an existing stylesheet instead.
 *
 * Output filenames are auto-derived from the program/unit/object declaration
 * in the source unless -o is supplied.
 */
#include "mxvm/html_gen.hpp"
#include "argz.hpp"
#include <cctype>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

static std::string readFile(const std::string &path) {
    std::fstream f(path, std::ios::in);
    if (!f.is_open()) {
        std::cerr << "mxvm-html: could not open file: " << path << "\n";
        exit(EXIT_FAILURE);
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

static bool endsWith(const std::string &s, const std::string &suffix) {
    return s.size() >= suffix.size() &&
           s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/** Return the directory portion of a path, including the trailing slash. */
static std::string dirOf(const std::string &path) {
    auto pos = path.find_last_of("/\\");
    return pos == std::string::npos ? "./" : path.substr(0, pos + 1);
}

static bool fileExists(const std::string &path) {
    std::ifstream f(path);
    return f.good();
}

/** Write mxvm-highlight.css next to outPath if it does not already exist. */
static void ensureCss(const std::string &outPath) {
    std::string cssPath = dirOf(outPath) + "mxvm-highlight.css";
    if (!fileExists(cssPath)) {
        std::ofstream f(cssPath);
        if (f.is_open()) {
            f << mxvm::PasHTMLGen::defaultCss();
            std::cout << "mxvm-html: wrote " << cssPath << "\n";
        }
    }
}

/**
 * Render src to outPath.
 * For .pas input PasHTMLGen is used and the HTML links to cssHref;
 * if cssHref is empty the default stylesheet is written alongside the output.
 */
static void writeHtml(const std::string &src, const std::string &inPath,
                      const std::string &outPath, const std::string &cssHref = "") {
    std::fstream ofile(outPath, std::ios::out);
    if (!ofile.is_open()) {
        std::cerr << "mxvm-html: could not open output file: " << outPath << "\n";
        exit(EXIT_FAILURE);
    }

    if (endsWith(inPath, ".pas")) {
        std::string href = cssHref.empty() ? "mxvm-highlight.css" : cssHref;
        if (cssHref.empty()) ensureCss(outPath);
        mxvm::PasHTMLGen gen(src);
        gen.output(ofile, inPath, href);
    } else {
        mxvm::HTMLGen gen(src);
        gen.output(ofile, inPath);
    }

    ofile.close();
    std::cout << "mxvm-html: wrote " << outPath << "\n";
}

/** Scan source for the first "program|unit|object <Identifier>" and return the identifier.
 *  Skips //-line comments, MXVM #-comments, Pascal {}-comments, and Pascal (*)-comments. */
static std::string extractDeclName(const std::string &src) {
    const std::string decls[] = {"program", "unit", "object"};
    std::size_t pos = 0, n = src.size();
    while (pos < n) {
        char c = src[pos];
        // whitespace
        if (std::isspace(static_cast<unsigned char>(c))) { ++pos; continue; }
        // // line comment
        if (c == '/' && pos + 1 < n && src[pos + 1] == '/') {
            while (pos < n && src[pos] != '\n') ++pos;
            continue;
        }
        // MXVM # comment
        if (c == '#') {
            while (pos < n && src[pos] != '\n') ++pos;
            continue;
        }
        // Pascal { } comment
        if (c == '{') {
            while (pos < n && src[pos] != '}') ++pos;
            if (pos < n) ++pos;
            continue;
        }
        // Pascal (* *) comment
        if (c == '(' && pos + 1 < n && src[pos + 1] == '*') {
            pos += 2;
            while (pos + 1 < n && !(src[pos] == '*' && src[pos + 1] == ')')) ++pos;
            if (pos + 1 < n) pos += 2;
            continue;
        }
        // MXVM /* */ comment
        if (c == '/' && pos + 1 < n && src[pos + 1] == '*') {
            pos += 2;
            while (pos + 1 < n && !(src[pos] == '*' && src[pos + 1] == '/')) ++pos;
            if (pos + 1 < n) pos += 2;
            continue;
        }
        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            std::size_t start = pos;
            while (pos < n && (std::isalnum(static_cast<unsigned char>(src[pos])) || src[pos] == '_'))
                ++pos;
            std::string tok = src.substr(start, pos - start);
            // case-insensitive match for Pascal (unit/program)
            std::string ltok = tok;
            for (auto &ch : ltok) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            for (const auto &kw : decls) {
                if (ltok == kw) {
                    while (pos < n && std::isspace(static_cast<unsigned char>(src[pos]))) ++pos;
                    if (pos < n && (std::isalpha(static_cast<unsigned char>(src[pos])) || src[pos] == '_')) {
                        std::size_t ns = pos;
                        while (pos < n && (std::isalnum(static_cast<unsigned char>(src[pos])) || src[pos] == '_'))
                            ++pos;
                        return src.substr(ns, pos - ns);
                    }
                }
            }
            continue;
        }
        ++pos;
    }
    return {};
}

int main(int argc, char **argv) {
    mx::Argz<std::string> args(argc, argv);
    args.addOptionSingleValue('i', "input source file (.mxvm or .pas)")
        .addOptionSingleValue('o', "output .html file (single-file mode only)")
        .addOptionSingleValue('s', "link to this CSS file instead of writing mxvm-highlight.css")
        .addOptionSingle('c', "batch: convert each listed file, auto-name output from program/unit/object")
        .addOptionSingle('h', "show this help");

    std::string inputFile, outputFile, cssHref;
    bool batchMode = false;
    std::vector<std::string> positional;

    mx::Argument<std::string> arg;
    int opt;
    try {
        while ((opt = args.proc(arg)) != -1) {
            switch (opt) {
            case 'i': inputFile  = arg.arg_value; break;
            case 'o': outputFile = arg.arg_value; break;
            case 's': cssHref    = arg.arg_value; break;
            case 'c': batchMode  = true; break;
            case 'h':
                std::cout << "mxvm-html: MXVM / Pascal source to HTML converter\n\n";
                args.help(std::cout);
                return EXIT_SUCCESS;
            case '-':
                positional.push_back(arg.arg_value);
                break;
            default: break;
            }
        }
    } catch (const mx::ArgException<std::string> &e) {
        std::cerr << "mxvm-html: " << e.text() << "\n";
        return EXIT_FAILURE;
    }

    if (batchMode) {
        if (positional.empty()) {
            std::cerr << "mxvm-html: -c requires at least one input file\n";
            return EXIT_FAILURE;
        }
        for (const auto &path : positional) {
            std::string src  = readFile(path);
            std::string name = extractDeclName(src);
            if (name.empty()) {
                std::cerr << "mxvm-html: no program/unit/object declaration found in: " << path << "\n";
                continue;
            }
            writeHtml(src, path, name + ".html", cssHref);
        }
        return EXIT_SUCCESS;
    }

    if (!inputFile.empty()) {
        std::string src = readFile(inputFile);
        if (outputFile.empty()) {
            std::string name = extractDeclName(src);
            if (name.empty()) {
                std::cerr << "mxvm-html: no program/unit/object declaration found in: " << inputFile
                          << " — use -o to specify output filename\n";
                return EXIT_FAILURE;
            }
            outputFile = name + ".html";
        }
        writeHtml(src, inputFile, outputFile, cssHref);
        return EXIT_SUCCESS;
    }

    std::cerr << "mxvm-html: no input specified. Use -i <file> [-o <output>] [-s <css>] or -c <file1> [file2...]\n";
    std::cerr << "Run with -h for help.\n";
    return EXIT_FAILURE;
}