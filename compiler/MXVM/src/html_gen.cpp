/**
 * @file html_gen.cpp
 * @brief Syntax-highlighted HTML generation from MXVM source code
 * @author Jared Bruni
 */
#include "mxvm/html_gen.hpp"
#include "mxvm/instruct.hpp"
#include <algorithm>
#include <cctype>
#include <unordered_set>

namespace {
    inline bool isIdentStart(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
    inline bool isIdentChar(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_'; }
    std::string lower(std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
        return s;
    }
    std::string html_escape(const std::string &s) {
        std::string out;
        out.reserve(s.size() * 12 / 10 + 8);
        for (char c : s) {
            switch (c) {
            case '&':
                out += "&amp;";
                break;
            case '<':
                out += "&lt;";
                break;
            case '>':
                out += "&gt;";
                break;
            case '"':
                out += "&quot;";
                break;
            case '\'':
                out += "&#39;";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\t':
                out += "\\t";
                break;
            default:
                out.push_back(c);
                break;
            }
        }
        return out;
    }
} // namespace

namespace mxvm {

    static std::unordered_set<std::string> buildInstructionSet() {
        std::unordered_set<std::string> s;
        for (const auto &name : IncType) {
            if (name == "NULL")
                continue;
            s.insert(lower(name));
        }
        return s;
    }

    static std::unordered_set<std::string> buildKeywordSet() {
        std::unordered_set<std::string> s;
        for (const auto &k : keywords)
            s.insert(lower(k));
        const char *extra[] = {
            "function", "export", "return", "start", "done", "true", "false", "null",
            "string", "int", "byte", "float", "ptr", "object", "program", "module", "data", "code", "section"};
        for (auto *p : extra)
            s.insert(p);
        return s;
    }

    static const std::unordered_set<std::string> kInstructions = buildInstructionSet();
    static const std::unordered_set<std::string> kKeywords = buildKeywordSet();
    static const std::unordered_set<std::string> kTypes = [] {
        return std::unordered_set<std::string>{"string", "int", "byte", "float", "ptr"};
    }();

    HTMLGen::HTMLGen(const std::string &text) : source(text) {}

    void HTMLGen::output(std::ostream &out, std::string filename) {
        out << "<!doctype html><html><head><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">";
        out << "<title>" << filename << " - [ MXVM Source ]</title>";
        out << "<style>";
        out << "body{background:#FFFFFF;color:#000000;margin:0;font:13px/1.6 \"Courier New\",Courier,monospace}";
        out << ".wrap{max-width:1100px;margin:0 auto;padding:24px}";
        out << "pre.code{white-space:pre-wrap;word-wrap:break-word;background:#FFFFFF;padding:16px;border:1px solid #CCCCCC;border-radius:2px}";
        out << ".kw{color:#000080;font-weight:bold}";
        out << ".op{color:#000080;font-weight:bold}";
        out << ".id{color:#000000}";
        out << ".num{color:#800000}";
        out << ".str{color:#008080}";
        out << ".com{color:#808080;font-style:italic}";
        out << ".lbl{color:#008000;font-weight:bold}";
        out << ".typ{color:#000080;font-weight:bold}";
        out << "</style></head><body><div class=\"wrap\"><pre class=\"code\"><code>";
        const std::string &s = source;
        size_t i = 0, n = s.size();

        // State for suppressing "string" highlighting inside  section module { … }
        enum class ModState { None, SawSection, SawModule, Inside };
        ModState modState = ModState::None;

        while (i < n) {
            char c = s[i];
            if (c == '\r') {
                ++i;
                continue;
            }
            if (c == '\n') {
                out << "\n";
                ++i;
                continue;
            }
            if (c == '\t') {
                out << "\t";
                ++i;
                continue;
            }
            if (c == ' ') {
                out << " ";
                ++i;
                continue;
            }

            if (c == '#') {
                size_t j = i;
                while (j < n && s[j] != '\n')
                    ++j;
                std::string t = s.substr(i, j - i);
                out << "<span class=\"com\">" << html_escape(t) << "</span>";
                i = j;
                continue;
            }

            if (c == '"') {
                size_t j = i + 1;
                bool esc = false;
                while (j < n) {
                    if (!esc && s[j] == '"') {
                        ++j;
                        break;
                    }
                    esc = (!esc && s[j] == '\\');
                    ++j;
                }
                std::string t = s.substr(i, j - i);
                out << "<span class=\"str\">" << html_escape(t) << "</span>";
                i = j;
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(c)) || (c == '-' && i + 1 < n && std::isdigit(static_cast<unsigned char>(s[i + 1])))) {
                size_t j = i;
                if (s[j] == '-')
                    ++j;
                if (j + 1 < n && s[j] == '0' && (s[j + 1] == 'x' || s[j + 1] == 'X')) {
                    j += 2;
                    while (j < n && std::isxdigit(static_cast<unsigned char>(s[j])))
                        ++j;
                } else {
                    while (j < n && std::isdigit(static_cast<unsigned char>(s[j])))
                        ++j;
                    if (j < n && s[j] == '.') {
                        ++j;
                        while (j < n && std::isdigit(static_cast<unsigned char>(s[j])))
                            ++j;
                    }
                }
                std::string t = s.substr(i, j - i);
                out << "<span class=\"num\">" << html_escape(t) << "</span>";
                i = j;
                continue;
            }

            if (isIdentStart(c)) {
                size_t j = i + 1;
                while (j < n && isIdentChar(s[j]))
                    ++j;
                bool is_label = (j < n && s[j] == ':');
                std::string ident = s.substr(i, j - i);
                std::string lid = lower(ident);
                if (is_label) {
                    out << "<span class=\"lbl\">" << html_escape(ident) << "</span>:";
                    i = j + 1;
                    modState = ModState::None;
                    continue;
                }

                // Advance the  section module { … }  state machine
                if (lid == "section" && modState == ModState::None) {
                    modState = ModState::SawSection;
                } else if (lid == "module" && modState == ModState::SawSection) {
                    modState = ModState::SawModule;
                } else if (modState != ModState::Inside) {
                    modState = ModState::None;
                }

                // Inside a module section, suppress type/keyword highlighting
                // so that module names like "string" render as plain identifiers.
                bool suppress = (modState == ModState::Inside);

                if (kInstructions.count(lid)) {
                    out << "<span class=\"op\">" << html_escape(ident) << "</span>";
                } else if (!suppress && (kTypes.count(lid) || kKeywords.count(lid))) {
                    if (kTypes.count(lid))
                        out << "<span class=\"typ\">" << html_escape(ident) << "</span>";
                    else
                        out << "<span class=\"kw\">" << html_escape(ident) << "</span>";
                } else {
                    out << "<span class=\"id\">" << html_escape(ident) << "</span>";
                }
                i = j;
                continue;
            }

            if (c == '{' || c == '}' || c == ',' || c == '(' || c == ')' || c == '[' || c == ']') {
                if (c == '{' && modState == ModState::SawModule) {
                    modState = ModState::Inside;
                } else if (c == '}' && modState == ModState::Inside) {
                    modState = ModState::None;
                }
                out << html_escape(std::string(1, c));
                ++i;
                continue;
            }

            out << html_escape(std::string(1, c));
            ++i;
        }
        out << "</code></pre></div></body></html>";
    }

} // namespace mxvm

// ============================================================
//  Pascal highlighter
// ============================================================

namespace {

    /** HTML-escape without mangling newlines or tabs (for Pascal output). */
    std::string pas_html_escape(const std::string &s) {
        std::string out;
        out.reserve(s.size() + 16);
        for (char c : s) {
            switch (c) {
            case '&':  out += "&amp;";  break;
            case '<':  out += "&lt;";   break;
            case '>':  out += "&gt;";   break;
            case '"':  out += "&quot;"; break;
            case '\'': out += "&#39;";  break;
            default:   out.push_back(c); break;
            }
        }
        return out;
    }

    // ---- Pascal keyword sets ----------------------------------------

    /// Reserved words → .kw
    const std::unordered_set<std::string> &kPasKeywords() {
        static const std::unordered_set<std::string> s = {
            "program", "unit", "interface", "implementation", "uses",
            "var", "const", "type", "procedure", "function", "forward",
            "begin", "end",
            "if", "then", "else", "while", "do",
            "for", "to", "downto", "repeat", "until",
            "case", "of", "with", "goto", "label",
            "in", "and", "or", "not", "div", "mod", "xor", "shl", "shr",
            "packed", "set", "file", "array", "record",
        };
        return s;
    }

    /// Built-in types → .typ
    const std::unordered_set<std::string> &kPasTypes() {
        static const std::unordered_set<std::string> s = {
            "integer", "real", "boolean", "char",
            "byte", "word", "longint", "shortint", "smallint", "cardinal",
            "string", "text", "double", "single", "extended",
            "comp", "currency", "ptr", "pointer",
        };
        return s;
    }

    /// Built-in functions, procedures, and predefined constants → .bi
    const std::unordered_set<std::string> &kPasBuiltins() {
        static const std::unordered_set<std::string> s = {
            // I/O
            "writeln", "write", "readln", "read",
            "assign", "reset", "rewrite", "append", "close", "eof", "eoln",
            // memory / dynamic
            "new", "dispose", "setlength", "high", "low",
            // control
            "exit", "break", "continue", "halt",
            // predefined consts
            "nil", "true", "false",
            // special function return var
            "result",
            // math / conversion
            "ord", "chr", "succ", "pred",
            "inc", "dec",
            "abs", "sqr", "sqrt", "trunc", "round",
            "odd", "upcase",
            "random", "randomize", "seed_random", "rand_number",
            // string
            "length", "copy", "pos", "concat", "delete", "insert", "str", "val",
            // set
            "include", "exclude",
        };
        return s;
    }

} // anonymous namespace

namespace mxvm {

    PasHTMLGen::PasHTMLGen(const std::string &src) : source(src) {}

    std::string PasHTMLGen::defaultCss() {
        return
            "/* mxvm-highlight.css — syntax highlight stylesheet for mxvm-html\n"
            "   Inspired by the Borland Delphi 7 editor default colour scheme.\n"
            "   Edit this file to change colours; it takes precedence over the\n"
            "   inline fallback styles that are embedded in every HTML file.    */\n"
            "\n"
            "body {\n"
            "    background: #FFFFFF;\n"
            "    color: #000000;\n"
            "    margin: 0;\n"
            "    font: 13px/1.6 \"Courier New\", Courier, monospace;\n"
            "}\n"
            ".wrap {\n"
            "    max-width: 1100px;\n"
            "    margin: 0 auto;\n"
            "    padding: 24px;\n"
            "}\n"
            "pre.code {\n"
            "    white-space: pre-wrap;\n"
            "    word-wrap: break-word;\n"
            "    background: #FFFFFF;\n"
            "    padding: 16px;\n"
            "    border: 1px solid #CCCCCC;\n"
            "    border-radius: 2px;\n"
            "}\n"
            "\n"
            "/* ---- token spans ---- */\n"
            "\n"
            "/* reserved keywords — bold navy (classic Delphi blue) */\n"
            ".kw  { color: #000080; font-weight: bold; }\n"
            "/* built-in types — same as keywords */\n"
            ".typ { color: #000080; font-weight: bold; }\n"
            "/* built-in routines and predefined constants */\n"
            ".bi  { color: #000080; }\n"
            "/* string / char literals — teal */\n"
            ".str { color: #008080; }\n"
            "/* numeric literals — maroon */\n"
            ".num { color: #800000; }\n"
            "/* comments — gray italic */\n"
            ".com { color: #808080; font-style: italic; }\n"
            "/* plain identifiers — black */\n"
            ".id  { color: #000000; }\n"
            "\n"
            "/* ---- mxvm-specific (also used by .mxvm output) ---- */\n"
            "\n"
            "/* VM instructions */\n"
            ".op  { color: #000080; font-weight: bold; }\n"
            "/* VM labels */\n"
            ".lbl { color: #008000; font-weight: bold; }\n";
    }

    void PasHTMLGen::output(std::ostream &out, const std::string &filename,
                            const std::string &cssHref) {
        out << "<!doctype html><html><head>"
            << "<meta charset=\"utf-8\">"
            << "<meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">"
            << "<title>" << pas_html_escape(filename) << " - [ Pascal Source ]</title>"
            // Inline fallback: guarantees highlighting even if the CSS file is absent
            << "<style>\n" << defaultCss() << "</style>"
            // External file: user edits this to customise; it overrides the inline block
            << "<link rel=\"stylesheet\" href=\"" << pas_html_escape(cssHref) << "\">"
            << "</head><body><div class=\"wrap\"><pre class=\"code\"><code>";

        const std::string &s = source;
        size_t i = 0, n = s.size();

        while (i < n) {
            char c = s[i];

            // -- whitespace --------------------------------------------------
            if (c == '\r') { ++i; continue; }
            if (c == '\n') { out << '\n'; ++i; continue; }
            if (c == '\t') { out << '\t'; ++i; continue; }
            if (c == ' ')  { out << ' ';  ++i; continue; }

            // -- line comment  // ... ----------------------------------------
            if (c == '/' && i + 1 < n && s[i + 1] == '/') {
                size_t j = i;
                while (j < n && s[j] != '\n') ++j;
                out << "<span class=\"com\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- block comment  { ... } --------------------------------------
            if (c == '{') {
                size_t j = i + 1;
                while (j < n && s[j] != '}') ++j;
                if (j < n) ++j;  // include '}'
                out << "<span class=\"com\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- block comment  (* ... *) ------------------------------------
            if (c == '(' && i + 1 < n && s[i + 1] == '*') {
                size_t j = i + 2;
                while (j + 1 < n && !(s[j] == '*' && s[j + 1] == ')')) ++j;
                if (j + 1 < n) j += 2;  // include '*)'
                out << "<span class=\"com\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- Pascal char literal  #65  or  #$41 --------------------------
            if (c == '#') {
                size_t j = i + 1;
                if (j < n && s[j] == '$') {
                    ++j;
                    while (j < n && std::isxdigit(static_cast<unsigned char>(s[j]))) ++j;
                } else {
                    while (j < n && std::isdigit(static_cast<unsigned char>(s[j]))) ++j;
                }
                out << "<span class=\"num\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- string literal  'hello'  with  ''  escape -------------------
            if (c == '\'') {
                size_t j = i + 1;
                while (j < n) {
                    if (s[j] == '\'') {
                        ++j;
                        if (j < n && s[j] == '\'') { ++j; continue; }  // '' → single quote
                        break;
                    }
                    ++j;
                }
                out << "<span class=\"str\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- hex literal  $FF --------------------------------------------
            if (c == '$') {
                size_t j = i + 1;
                while (j < n && std::isxdigit(static_cast<unsigned char>(s[j]))) ++j;
                out << "<span class=\"num\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- decimal / float  (don't consume '..' as a decimal point) ----
            if (std::isdigit(static_cast<unsigned char>(c))) {
                size_t j = i;
                while (j < n && std::isdigit(static_cast<unsigned char>(s[j]))) ++j;
                if (j < n && s[j] == '.' && (j + 1 >= n || s[j + 1] != '.')) {
                    ++j;
                    while (j < n && std::isdigit(static_cast<unsigned char>(s[j]))) ++j;
                }
                if (j < n && (s[j] == 'e' || s[j] == 'E')) {
                    ++j;
                    if (j < n && (s[j] == '+' || s[j] == '-')) ++j;
                    while (j < n && std::isdigit(static_cast<unsigned char>(s[j]))) ++j;
                }
                out << "<span class=\"num\">" << pas_html_escape(s.substr(i, j - i)) << "</span>";
                i = j;
                continue;
            }

            // -- identifier / keyword ----------------------------------------
            if (isIdentStart(c)) {
                size_t j = i + 1;
                while (j < n && isIdentChar(s[j])) ++j;
                std::string ident = s.substr(i, j - i);
                std::string lid   = lower(ident);
                i = j;

                if (kPasKeywords().count(lid))
                    out << "<span class=\"kw\">"  << pas_html_escape(ident) << "</span>";
                else if (kPasTypes().count(lid))
                    out << "<span class=\"typ\">" << pas_html_escape(ident) << "</span>";
                else if (kPasBuiltins().count(lid))
                    out << "<span class=\"bi\">"  << pas_html_escape(ident) << "</span>";
                else
                    out << "<span class=\"id\">"  << pas_html_escape(ident) << "</span>";
                continue;
            }

            // -- everything else (operators, punctuation) --------------------
            out << pas_html_escape(std::string(1, c));
            ++i;
        }

        out << "</code></pre></div></body></html>";
    }

} // namespace mxvm
