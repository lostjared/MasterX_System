/**
 * @file web.cpp
 * @brief WebAssembly (Emscripten) bindings for running MXVM programs in the browser
 * @author Jared Bruni
 */
#include <emscripten/bind.h>
#include <mxvm/mxvm.hpp>
#include <scanner/exception.hpp>
#include <sstream>
#include <string>

namespace {

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
            default:
                out.push_back(c);
                break;
            }
        }
        return out;
    }

    struct BaseGuard {
        mxvm::Base *prev;
        explicit BaseGuard(mxvm::Base *p)
            : prev(mxvm::Program::base) {
            if (p)
                mxvm::Program::base = p;
        }
        ~BaseGuard() {
            mxvm::Program::base = prev;
        }
    };
} // namespace

static void collectAndRegisterAllExterns(mxvm::Program &program) {
    for (const auto &obj : program.objects) {
        for (auto &lbl : obj->labels) {
            if (lbl.second.second) {
                program.add_extern(obj->name, lbl.first, false);
            }
        }
    }
}

class MXVM_Debug {
  public:
    MXVM_Debug() {}

    std::string parseToHTML(int output_type, const std::string &code) {
        std::unique_ptr<mxvm::Program> program(new mxvm::Program());
        BaseGuard guard(program.get());
        mxvm::Base::object_map.clear();
        const bool html_mode_prev = mxvm::html_mode;
        mxvm::html_mode = true;

        std::ostringstream html;
        std::ostringstream html_err;
        std::streambuf *old_cerr = std::cerr.rdbuf();
        std::cerr.rdbuf(html_err.rdbuf());

        mxvm::Platform platform;

        switch (output_type) {
        case 0:
            platform = mxvm::Platform::LINUX;
            break;
        case 1:
            platform = mxvm::Platform::DARWIN;
            break;
        case 2:
            platform = mxvm::Platform::WINX64;
            break;
        }

        try {
            mxvm::Parser parser(code);
            parser.platform = platform;
            parser.include_path = "include";
            parser.scan();
            program->platform = platform;

            if (parser.generateProgramCode(mxvm::Mode::MODE_COMPILE, program)) {
                std::ostringstream code_v;

                collectAndRegisterAllExterns(*program);

                program->object = (program->root_name != program->name);
                program->generateCode(platform, program->object, code_v);
                program->assembly_code = program->gen_optimize(code_v.str(), platform);

                parser.generateDebugHTML(html, program);
            } else {
                html << "Parse Error";
            }
        } catch (mx::Exception &e) {
            html << "Error: " << html_escape(e.what()) << "<br>\n";
        } catch (std::exception &e) {
            html << "Exception: " << html_escape(e.what()) << "<br>\n";
        } catch (scan::ScanExcept &e) {
            html << html_escape(e.why()) << "\n";
        } catch (...) {
            html << "Unknown Error: <br>";
        }

        std::cerr.rdbuf(old_cerr);
        mxvm::html_mode = html_mode_prev;

        if (!html_err.str().empty()) {
            return R"(
                <div class="section">
                    <div class="section-header" style="background:#b71c1c;color:#fff;padding:20px 30px;font-size:1.4rem;font-weight:600;">
                        <span style="font-size:1.5em;">❌</span> Error
                    </div>
                    <div class="section-content" style="padding:30px;">
                        <div class="error" style="background:#2c1810;border:1px solid #d32f2f;color:#ff8a80;padding:20px;margin:20px;border-radius:8px;font-family:'Courier New',monospace;">
                            )" +
                   html_escape(html_err.str()) + R"(
                        </div>
                    </div>
                </div>
            )";
        }

        return html.str();
    }
};

EMSCRIPTEN_BINDINGS(MXVM_Debug_bindings) {
    emscripten::class_<MXVM_Debug>("MXVM_Debug")
        .constructor<>()
        .function("parseToHTML", &MXVM_Debug::parseToHTML);
}