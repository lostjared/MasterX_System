#include "mxvm/html_gen.hpp"
#include <emscripten/bind.h>
#include <iostream>

class MXVM_HTML {
  public:
    MXVM_HTML() = default;
    std::string codeToHTML(const std::string &code) {
        mxvm::HTMLGen gen(code);
        std::ostringstream out;
        gen.output(out, "Filename");
        return out.str();
    }
};

EMSCRIPTEN_BINDINGS(MXVM_HTML_bindings) {
    emscripten::class_<MXVM_HTML>("MXVM_HTML")
        .constructor<>()
        .function("codeToHTML", &MXVM_HTML::codeToHTML);
}