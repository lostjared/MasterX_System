/**
 * @file string.cpp
 * @brief String module C++ runtime bindings for the MXVM interpreter
 * @author Jared Bruni
 */
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <mxvm/icode.hpp>
#include <mxvm/instruct.hpp>
#include <string>

static std::string getStringFromVar(mxvm::Program *program, const std::string &varName) {
    if (!program->isVariable(varName)) {
        throw mx::Exception("Argument must be a variable, got: " + varName);
    }
    mxvm::Variable &var = program->getVariable(varName);
    if (var.type == mxvm::VarType::VAR_STRING) {
        return var.var_value.str_value;
    }
    if (var.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("Pointer argument '" + varName + "' is null", var.var_value.ptr_value != nullptr);
        return std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
    }
    throw mx::Exception("Argument '" + varName + "' must be a string or pointer variable.");
}

static inline bool isStringLike(mxvm::Variable &v) {
    return v.type == mxvm::VarType::VAR_STRING || v.type == mxvm::VarType::VAR_POINTER;
}
static inline const char *asReadPtr(mxvm::Variable &v) {
    if (v.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("null pointer", v.var_value.ptr_value != nullptr);
        return reinterpret_cast<const char *>(v.var_value.ptr_value);
    }
    return v.var_value.str_value.c_str();
}
static inline char *asWritePtr(mxvm::Variable &v) {
    if (v.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("null pointer", v.var_value.ptr_value != nullptr);
        return reinterpret_cast<char *>(v.var_value.ptr_value);
    }
    return nullptr;
}

extern "C" void mxvm_string_strlen(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("strlen requires exactly 1 argument (string | pointer).");
    }
    std::string &name = operand[0].op;
    if (!program->isVariable(name)) {
        throw mx::Exception("strlen argument must be a declared variable (string | pointer): " + name);
    }
    mxvm::Variable &v = program->getVariable(name);

    const char *src = nullptr;
    switch (v.type) {
    case mxvm::VarType::VAR_STRING:
        src = v.var_value.str_value.c_str();
        break;
    case mxvm::VarType::VAR_POINTER:
    case mxvm::VarType::VAR_EXTERN:
        mxvm::except_assert("strlen: null pointer in variable '" + v.var_name + "'", v.var_value.ptr_value != nullptr);
        src = reinterpret_cast<const char *>(v.var_value.ptr_value);
        break;
    default:
        throw mx::Exception("strlen argument '" + v.var_name + "' must be string or pointer variable (got different type).");
    }

    int64_t length = static_cast<int64_t>(std::strlen(src));
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = length;
}

extern "C" void mxvm_string_strcmp(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("strcmp requires two pointer/string arguments.");
    }
    std::string &var1 = operand[0].op;
    std::string &var2 = operand[1].op;

    if (!program->isVariable(var1) || !program->isVariable(var2)) {
        throw mx::Exception("strcmp arguments must be variables (pointer or string).");
    }

    mxvm::Variable &v1 = program->getVariable(var1);
    mxvm::Variable &v2 = program->getVariable(var2);

    const char *s1 = nullptr;
    const char *s2 = nullptr;

    if (v1.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("strcmp: pointer: " + v1.var_name + " is null", (v1.var_value.ptr_value != nullptr));
        s1 = reinterpret_cast<const char *>(v1.var_value.ptr_value);
    } else if (v1.type == mxvm::VarType::VAR_STRING) {
        s1 = v1.var_value.str_value.c_str();
    } else {
        throw mx::Exception("strcmp first argument must be a pointer or string variable.");
    }

    if (v2.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("strcmp: pointer: " + v2.var_name + " is null", (v2.var_value.ptr_value != nullptr));
        s2 = reinterpret_cast<const char *>(v2.var_value.ptr_value);
    } else if (v2.type == mxvm::VarType::VAR_STRING) {
        s2 = v2.var_value.str_value.c_str();
    } else {
        throw mx::Exception("strcmp second argument must be a pointer or string variable.");
    }

    int64_t cmp_result = strcmp(s1, s2);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = cmp_result;
}

extern "C" void mxvm_string_strncpy(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3)
        throw mx::Exception("strncpy requires (dest, src, len)");
    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op))
        throw mx::Exception("strncpy arguments must be variables");
    mxvm::Variable &dest = program->getVariable(operand[0].op);
    mxvm::Variable &src = program->getVariable(operand[1].op);

    int64_t n = operand[2].op_value;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &iv = program->getVariable(operand[2].op);
        n = iv.var_value.int_value;
    }

    if (!isStringLike(dest))
        throw mx::Exception("strncpy dest must be pointer or string");
    if (!isStringLike(src))
        throw mx::Exception("strncpy src must be pointer or string");

    const char *sptr = asReadPtr(src);
    if (dest.type == mxvm::VarType::VAR_POINTER) {
        if (dest.var_value.buffer_size && (int64_t)dest.var_value.buffer_size < n + 1)
            throw mx::Exception("strncpy dest buffer too small");
        char *dptr = asWritePtr(dest);
        std::strncpy(dptr, sptr, n);
        dptr[n] = '\0';
    } else {
        dest.var_value.str_value.assign(sptr, (size_t)n);
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = n;
}

extern "C" void mxvm_string_strncat(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3)
        throw mx::Exception("strncat requires (dest, src, len)");
    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op))
        throw mx::Exception("strncat arguments must be variables");
    mxvm::Variable &dest = program->getVariable(operand[0].op);
    mxvm::Variable &src = program->getVariable(operand[1].op);

    int64_t n = operand[2].op_value;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &iv = program->getVariable(operand[2].op);
        n = iv.var_value.int_value;
    }

    if (!isStringLike(dest))
        throw mx::Exception("strncat dest must be pointer or string");
    if (!isStringLike(src))
        throw mx::Exception("strncat src must be pointer or string");

    const char *sptr = asReadPtr(src);
    if (dest.type == mxvm::VarType::VAR_POINTER) {
        if (dest.var_value.buffer_size) {
            size_t have = std::strlen(reinterpret_cast<const char *>(dest.var_value.ptr_value));
            if (have + (size_t)n + 1 > dest.var_value.buffer_size)
                throw mx::Exception("strncat dest buffer too small");
        }
        std::strncat(reinterpret_cast<char *>(dest.var_value.ptr_value), sptr, n);
    } else {
        dest.var_value.str_value.append(sptr, (size_t)n);
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = n;
}

extern "C" void mxvm_string_snprintf(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() < 4) {
        throw mx::Exception("snprintf requires at least destination, size, format, and one argument.");
    }
    std::string &dest_var = operand[0].op;
    int64_t n = operand[1].op_value;
    std::string &fmt_var = operand[2].op;

    if (!program->isVariable(dest_var) || !program->isVariable(fmt_var)) {
        throw mx::Exception("snprintf destination: " + dest_var + " and format: " + fmt_var + " must be variables.");
    }

    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &iv = program->getVariable(operand[1].op);
        n = iv.var_value.int_value;
    }

    mxvm::except_assert("snprintf size is zero", n != 0);
    mxvm::Variable &dest = program->getVariable(dest_var);
    mxvm::Variable &fmt = program->getVariable(fmt_var);

    if (fmt.type != mxvm::VarType::VAR_STRING) {
        throw mx::Exception("snprintf format must be a string variable.");
    }

    std::ostringstream oss;
    size_t argIndex = 3;
    const char *format = fmt.var_value.str_value.c_str();
    char buffer[4096];

    for (size_t i = 0; i < fmt.var_value.str_value.length(); ++i) {
        if (format[i] == '%' && i + 1 < fmt.var_value.str_value.length()) {
            size_t start = i;
            size_t j = i + 1;
            while (j < fmt.var_value.str_value.length() &&
                   (format[j] == '-' || format[j] == '+' || format[j] == ' ' || format[j] == '#' || format[j] == '0' ||
                    (format[j] >= '0' && format[j] <= '9') || format[j] == '.' ||
                    format[j] == 'l' || format[j] == 'h' || format[j] == 'z' || format[j] == 'j' || format[j] == 't')) {
                ++j;
            }
            if (j < fmt.var_value.str_value.length() && format[j] == '%') {
                oss << '%';
                i = j;
                continue;
            }
            if (j >= fmt.var_value.str_value.length())
                break;
            if (!std::isalpha(format[j])) {
                oss << format[i];
                continue;
            }
            std::string spec(format + start, format + j + 1);
            if (argIndex < operand.size()) {
                std::string &arg_var = operand[argIndex++].op;
                if (!program->isVariable(arg_var)) {
                    throw mx::Exception("snprintf argument must be a variable.");
                }
                mxvm::Variable &arg = program->getVariable(arg_var);
                if (arg.type == mxvm::VarType::VAR_INTEGER) {
                    std::snprintf(buffer, sizeof(buffer), spec.c_str(), arg.var_value.int_value);
                } else if (arg.type == mxvm::VarType::VAR_POINTER || arg.type == mxvm::VarType::VAR_EXTERN) {
                    std::snprintf(buffer, sizeof(buffer), spec.c_str(), arg.var_value.ptr_value);
                } else if (arg.type == mxvm::VarType::VAR_FLOAT) {
                    std::snprintf(buffer, sizeof(buffer), spec.c_str(), arg.var_value.float_value);
                } else if (arg.type == mxvm::VarType::VAR_STRING) {
                    std::snprintf(buffer, sizeof(buffer), spec.c_str(), arg.var_value.str_value.c_str());
                } else if (arg.type == mxvm::VarType::VAR_BYTE) {
                    std::snprintf(buffer, sizeof(buffer), spec.c_str(), arg.var_value.int_value);
                } else {
                    std::snprintf(buffer, sizeof(buffer), "%s", "(unsupported)");
                }
                oss << buffer;
            } else {
                oss << spec;
            }
            i = j;
        } else {
            oss << format[i];
        }
    }

    if (dest.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("snprintf dest pointer is null", dest.var_value.ptr_value != nullptr);
        std::strncpy(reinterpret_cast<char *>(dest.var_value.ptr_value), oss.str().c_str(), n);
        if (n > 0) {
            reinterpret_cast<char *>(dest.var_value.ptr_value)[n - 1] = '\0';
        }
    } else if (dest.type == mxvm::VarType::VAR_STRING) {
        dest.var_value.str_value = oss.str().substr(0, n);
    } else {
        throw mx::Exception("snprintf destination must be a pointer or string variable.");
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = oss.str().length();
}

extern "C" void mxvm_string_strfind(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3)
        throw mx::Exception("strfind requires (haystack, needle, start)");
    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op))
        throw mx::Exception("strfind arguments must be variables");
    mxvm::Variable &hay = program->getVariable(operand[0].op);
    mxvm::Variable &needle = program->getVariable(operand[1].op);

    int64_t start = operand[2].op_value;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &iv = program->getVariable(operand[2].op);
        start = iv.var_value.int_value;
    }

    if (!isStringLike(hay) || !isStringLike(needle))
        throw mx::Exception("strfind args must be pointer or string");

    std::string H = asReadPtr(hay);
    std::string N = asReadPtr(needle);

    size_t pos = H.find(N, (size_t)start);
    int64_t result = (pos == std::string::npos) ? -1 : (int64_t)pos;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_string_substr(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5)
        throw mx::Exception("substr requires 5 args");
    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[2].op))
        throw mx::Exception("substr dest/src must be variables");
    mxvm::Variable &dest = program->getVariable(operand[0].op);
    mxvm::Variable &src = program->getVariable(operand[2].op);

    if (!isStringLike(dest) || !isStringLike(src))
        throw mx::Exception("substr dest/src must be pointer or string");

    int64_t size = operand[1].op_value;
    int64_t pos = operand[3].op_value;
    int64_t len = operand[4].op_value;
    if (program->isVariable(operand[1].op))
        size = program->getVariable(operand[1].op).var_value.int_value;
    if (program->isVariable(operand[3].op))
        pos = program->getVariable(operand[3].op).var_value.int_value;
    if (program->isVariable(operand[4].op))
        len = program->getVariable(operand[4].op).var_value.int_value;

    std::string S = asReadPtr(src);
    if (pos < 0)
        pos = 0;
    std::string out = (pos > (int64_t)S.size()) ? "" : S.substr((size_t)pos, (size_t)len);

    if (dest.type == mxvm::VarType::VAR_POINTER) {
        mxvm::except_assert("substr dest ptr null", dest.var_value.ptr_value != nullptr);
        if (size <= 0 || (dest.var_value.buffer_size && (size_t)size > dest.var_value.buffer_size))
            size = (int64_t)dest.var_value.buffer_size;
        std::strncpy(reinterpret_cast<char *>(dest.var_value.ptr_value), out.c_str(), (size_t)size);
        if (size > 0)
            reinterpret_cast<char *>(dest.var_value.ptr_value)[size - 1] = '\0';
    } else {
        dest.var_value.str_value = out.substr(0, (size_t)size);
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = (int64_t)out.size();
}

extern "C" void mxvm_string_strat(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() == 2 && program->isVariable(operand[0].op)) {

        int64_t pos = 0;
        if (operand[1].type == mxvm::OperandType::OP_CONSTANT) {
            pos = operand[1].op_value;
        } else {
            mxvm::Variable &v = program->getVariable(operand[1].op);
            pos = v.var_value.int_value;
        }
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (pos < 0 || static_cast<size_t>(pos) >= var.var_value.str_value.size()) {
            throw mx::Exception("strat: index " + std::to_string(pos) + " out of bounds for string of length " + std::to_string(var.var_value.str_value.size()));
        }
        program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
        program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
        program->vars["%rax"].var_value.int_value = static_cast<int64_t>(var.var_value.str_value[static_cast<size_t>(pos)]);
    }
}

extern "C" void mxvm_string_pos(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2)
        throw mx::Exception("pos requires 2 arguments: (substr, s)");

    std::string sub = getStringFromVar(program, operand[0].op);
    std::string s = getStringFromVar(program, operand[1].op);

    size_t position = s.find(sub);
    int64_t result = (position == std::string::npos) ? 0 : static_cast<int64_t>(position + 1);

    program->vars["%rax"].var_value.int_value = result;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
}

extern "C" void mxvm_string_copy(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3)
        throw mx::Exception("copy requires 3 arguments: (s, index, count)");

    std::string s = getStringFromVar(program, operand[0].op);
    int64_t index = program->getVariable(operand[1].op).var_value.int_value;
    int64_t count = program->getVariable(operand[2].op).var_value.int_value;

    if (index < 1 || static_cast<size_t>(index - 1) > s.size()) {
        throw mx::Exception("copy: index " + std::to_string(index) + " out of bounds for string of length " + std::to_string(s.size()));
    }
    if (count < 0) {
        throw mx::Exception("copy: count must be non-negative");
    }
    std::string result_str = s.substr(static_cast<size_t>(index - 1), static_cast<size_t>(count));

    char *new_buf = static_cast<char *>(malloc(result_str.length() + 1));
    if (!new_buf)
        throw mx::Exception("malloc failed in copy()");
    strcpy(new_buf, result_str.c_str());

    program->vars["%rax"].var_value.ptr_value = new_buf;
    program->vars["%rax"].var_value.ptr_size = 1;
    program->vars["%rax"].var_value.ptr_count = static_cast<int64_t>(result_str.length() + 1);
    program->vars["%rax"].var_value.owns = true;
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
}

extern "C" void mxvm_string_insert(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3)
        throw mx::Exception("insert requires 3 arguments: (source, dest, index)");

    std::string source = getStringFromVar(program, operand[0].op);
    std::string dest = getStringFromVar(program, operand[1].op);
    int64_t index = program->getVariable(operand[2].op).var_value.int_value;

    if (index < 1 || static_cast<size_t>(index - 1) > dest.size()) {
        throw mx::Exception("insert: index " + std::to_string(index) + " out of bounds for string of length " + std::to_string(dest.size()));
    }
    dest.insert(static_cast<size_t>(index - 1), source);

    char *new_buf = static_cast<char *>(malloc(dest.length() + 1));
    if (!new_buf)
        throw mx::Exception("malloc failed in insert()");
    strcpy(new_buf, dest.c_str());

    program->vars["%rax"].var_value.ptr_value = new_buf;
    program->vars["%rax"].var_value.ptr_size = 1;
    program->vars["%rax"].var_value.ptr_count = static_cast<int64_t>(dest.length() + 1);
    program->vars["%rax"].var_value.owns = true;
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
}

extern "C" void mxvm_string_delete(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3)
        throw mx::Exception("delete requires 3 arguments: (s, index, count)");

    std::string s = getStringFromVar(program, operand[0].op);
    int64_t index = program->getVariable(operand[1].op).var_value.int_value;
    int64_t count = program->getVariable(operand[2].op).var_value.int_value;

    if (index < 1 || static_cast<size_t>(index - 1) > s.size()) {
        throw mx::Exception("delete: index " + std::to_string(index) + " out of bounds for string of length " + std::to_string(s.size()));
    }
    if (count < 0) {
        throw mx::Exception("delete: count must be non-negative");
    }
    s.erase(static_cast<size_t>(index - 1), static_cast<size_t>(count));

    char *new_buf = static_cast<char *>(malloc(s.length() + 1));
    if (!new_buf)
        throw mx::Exception("malloc failed in delete()");
    strcpy(new_buf, s.c_str());

    program->vars["%rax"].var_value.ptr_value = new_buf;
    program->vars["%rax"].var_value.ptr_size = 1;
    program->vars["%rax"].var_value.ptr_count = static_cast<int64_t>(s.length() + 1);
    program->vars["%rax"].var_value.owns = true;
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
}

/**
 * @brief Convert an integer to its decimal string representation.
 *
 * Allocates a new heap buffer for the result string and returns it
 * via %rax as a pointer.  Frees any previously-owned pointer in %rax
 * before overwriting to prevent memory leaks from repeated calls
 * (e.g. `inttostr(score)` inside a render loop).
 *
 * @param program  VM program context.
 * @param operand  Single operand: the integer variable to convert.
 */
extern "C" void mxvm_string_inttostr(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("inttostr requires 1 argument: (integer)");

    int64_t val = program->getVariable(operand[0].op).var_value.int_value;
    std::string result_str = std::to_string(val);

    char *new_buf = static_cast<char *>(malloc(result_str.length() + 1));
    if (!new_buf)
        throw mx::Exception("malloc failed in inttostr()");
    strcpy(new_buf, result_str.c_str());

    // Free old owned pointer in %rax before overwriting
    auto &rax = program->vars["%rax"];
    if (rax.type == mxvm::VarType::VAR_POINTER && rax.var_value.ptr_value && rax.var_value.owns) {
        std::free(rax.var_value.ptr_value);
    }

    rax.var_value.ptr_value = new_buf;
    rax.var_value.ptr_size = 1;
    rax.var_value.ptr_count = static_cast<int64_t>(result_str.length() + 1);
    rax.var_value.owns = true;
    rax.type = mxvm::VarType::VAR_POINTER;
}

extern "C" void mxvm_string_strtoint(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("strtoint requires 1 argument: (string)");

    std::string s = getStringFromVar(program, operand[0].op);
    int64_t result = 0;
    try {
        result = std::stoll(s);
    } catch (const std::exception &e) {
        result = 0;
    }

    program->vars["%rax"].var_value.int_value = result;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
}
