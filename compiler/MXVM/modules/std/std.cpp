/**
 * @file std.cpp
 * @brief Standard library module C++ runtime bindings for the MXVM interpreter
 * @author Jared Bruni
 */
#include "mx_std.h"
#include <mxvm/icode.hpp>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>

extern "C" void mxvm_std_abs(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("abs requires 1 argument (value).");
    }

    int64_t value = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = std::abs(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_fabs(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("fabs requires 1 argument (value).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("fabs argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::fabs(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_sqrt(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sqrt requires 1 argument (value).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("sqrt argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::sqrt(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_pow(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("pow requires 2 arguments (base, exponent).");
    }

    double base, exponent;

    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        base = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);
    } else {
        base = static_cast<double>(operand[0].op_value);
    }

    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        exponent = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);
    } else {
        exponent = static_cast<double>(operand[1].op_value);
    }

    double result = std::pow(base, exponent);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_sin(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sin requires 1 argument (angle).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("sin argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::sin(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_cos(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("cos requires 1 argument (angle).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("cos argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::cos(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_tan(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("tan requires 1 argument (angle).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("tan argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::tan(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_floor(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("floor requires 1 argument (value).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("floor argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::floor(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

extern "C" void mxvm_std_ceil(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("ceil requires 1 argument (value).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("ceil argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    double value = (var.type == mxvm::VarType::VAR_FLOAT) ? var.var_value.float_value : static_cast<double>(var.var_value.int_value);

    double result = std::ceil(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

// Random number functions
extern "C" void mxvm_std_rand(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = std::rand();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_srand(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("srand requires 1 argument (seed).");
    }

    int64_t seed = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    std::srand(static_cast<unsigned int>(seed));
}

// Memory functions
extern "C" void mxvm_std_malloc(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("malloc requires 1 argument (size).");
    }

    int64_t size = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    void *result = std::malloc(static_cast<size_t>(size));
    if (result == nullptr) {
        throw mx::Exception("malloc failed to allocate " + std::to_string(size) + " bytes");
    }
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = result;
    program->vars["%rax"].var_value.owns = true;
    program->vars["%rax"].var_value.ptr_size = static_cast<uint64_t>(size);
    program->vars["%rax"].var_value.ptr_count = 1;
}

extern "C" void mxvm_std_calloc(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("calloc requires 2 arguments (count, size).");
    }

    int64_t count = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t size = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;

    void *result = std::calloc(static_cast<size_t>(count), static_cast<size_t>(size));
    if (result == nullptr) {
        throw mx::Exception("calloc failed to allocate " + std::to_string(count) + " * " + std::to_string(size) + " bytes");
    }
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = result;
    program->vars["%rax"].var_value.owns = true;
    program->vars["%rax"].var_value.ptr_size = static_cast<uint64_t>(size);
    program->vars["%rax"].var_value.ptr_count = static_cast<uint64_t>(count);
}

extern "C" void mxvm_std_release(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("free requires 1 argument (pointer).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("free argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    if (var.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("free argument must be a pointer variable.");
    }

    std::free(var.var_value.ptr_value);
    var.var_value.ptr_value = nullptr;
    var.var_value.owns = false;
    var.var_value.ptr_size = 0;
    var.var_value.ptr_count = 0;
}

// Character functions
extern "C" void mxvm_std_toupper(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("toupper requires 1 argument (character).");
    }

    int64_t ch = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = std::toupper(static_cast<int>(ch));
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_tolower(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("tolower requires 1 argument (character).");
    }

    int64_t ch = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = std::tolower(static_cast<int>(ch));
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_isalpha(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("isalpha requires 1 argument (character).");
    }

    int64_t ch = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = std::isalpha(static_cast<int>(ch)) ? 1 : 0;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_isdigit(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("isdigit requires 1 argument (character).");
    }

    int64_t ch = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = std::isdigit(static_cast<int>(ch)) ? 1 : 0;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_isspace(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("isspace requires 1 argument (character).");
    }

    int64_t ch = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = std::isspace(static_cast<int>(ch)) ? 1 : 0;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// String conversion functions
extern "C" void mxvm_std_atoi(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("atoi requires 1 argument (string).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("atoi argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    const char *str = nullptr;

    if (var.type == mxvm::VarType::VAR_STRING) {
        str = var.var_value.str_value.c_str();
    } else if (var.type == mxvm::VarType::VAR_POINTER) {
        if (var.var_value.ptr_value == nullptr) {
            throw mx::Exception("atoi: pointer argument is null");
        }
        str = reinterpret_cast<const char *>(var.var_value.ptr_value);
    } else {
        throw mx::Exception("atoi argument must be a string or pointer variable.");
    }

    int64_t result = std::atoi(str);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_atof(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("atof requires 1 argument (string).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("atof argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    const char *str = nullptr;

    if (var.type == mxvm::VarType::VAR_STRING) {
        str = var.var_value.str_value.c_str();
    } else if (var.type == mxvm::VarType::VAR_POINTER) {
        if (var.var_value.ptr_value == nullptr) {
            throw mx::Exception("atof: pointer argument is null");
        }
        str = reinterpret_cast<const char *>(var.var_value.ptr_value);
    } else {
        throw mx::Exception("atof argument must be a string or pointer variable.");
    }

    double result = std::atof(str);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}

// Utility functions
extern "C" void mxvm_std_exit(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t exit_code = 0;
    if (!operand.empty()) {
        exit_code = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    }

    std::exit(static_cast<int>(exit_code));
}

extern "C" void mxvm_std_system(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("system requires 1 argument (command).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("system argument must be a variable.");
    }

    mxvm::Variable &var = program->getVariable(operand[0].op);
    const char *command = nullptr;

    if (var.type == mxvm::VarType::VAR_STRING) {
        command = var.var_value.str_value.c_str();
    } else if (var.type == mxvm::VarType::VAR_POINTER) {
        if (var.var_value.ptr_value == nullptr) {
            throw mx::Exception("system: pointer argument is null");
        }
        command = reinterpret_cast<const char *>(var.var_value.ptr_value);
    } else {
        throw mx::Exception("system argument must be a string or pointer variable.");
    }

    int64_t result = std::system(command);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// ...existing code...

extern "C" void mxvm_std_memcpy(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("memcpy requires 3 arguments (dest, src, size).");
    }

    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op)) {
        throw mx::Exception("memcpy dest and src must be pointer variables.");
    }

    mxvm::Variable &dest = program->getVariable(operand[0].op);
    mxvm::Variable &src = program->getVariable(operand[1].op);

    if (dest.type != mxvm::VarType::VAR_POINTER || src.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("memcpy dest and src must be pointer variables.");
    }

    mxvm::except_assert("memcpy: dest pointer is null", dest.var_value.ptr_value != nullptr);
    mxvm::except_assert("memcpy: src pointer is null", src.var_value.ptr_value != nullptr);

    size_t n = 0;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &size_var = program->getVariable(operand[2].op);
        n = static_cast<size_t>(size_var.var_value.int_value);
    } else {
        n = static_cast<size_t>(operand[2].op_value);
    }

    if (n > 0) {
        memcpy(dest.var_value.ptr_value, src.var_value.ptr_value, n);
    }

    // Return dest pointer in %rax
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = dest.var_value.ptr_value;
}

extern "C" void mxvm_std_memcmp(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("memcmp requires 3 arguments (ptr1, ptr2, size).");
    }

    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op)) {
        throw mx::Exception("memcmp ptr1 and ptr2 must be pointer variables.");
    }

    mxvm::Variable &ptr1 = program->getVariable(operand[0].op);
    mxvm::Variable &ptr2 = program->getVariable(operand[1].op);

    if (ptr1.type != mxvm::VarType::VAR_POINTER || ptr2.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("memcmp ptr1 and ptr2 must be pointer variables.");
    }

    mxvm::except_assert("memcmp: ptr1 is null", ptr1.var_value.ptr_value != nullptr);
    mxvm::except_assert("memcmp: ptr2 is null", ptr2.var_value.ptr_value != nullptr);

    size_t n = 0;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &size_var = program->getVariable(operand[2].op);
        n = static_cast<size_t>(size_var.var_value.int_value);
    } else {
        n = static_cast<size_t>(operand[2].op_value);
    }

    int result = 0;
    if (n > 0) {
        result = memcmp(ptr1.var_value.ptr_value, ptr2.var_value.ptr_value, n);
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = static_cast<int64_t>(result);
}

extern "C" void mxvm_std_memmove(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("memmove requires 3 arguments (dest, src, size).");
    }

    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op)) {
        throw mx::Exception("memmove dest and src must be pointer variables.");
    }

    mxvm::Variable &dest = program->getVariable(operand[0].op);
    mxvm::Variable &src = program->getVariable(operand[1].op);

    if (dest.type != mxvm::VarType::VAR_POINTER || src.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("memmove dest and src must be pointer variables.");
    }

    mxvm::except_assert("memmove: dest pointer is null", dest.var_value.ptr_value != nullptr);
    mxvm::except_assert("memmove: src pointer is null", src.var_value.ptr_value != nullptr);

    size_t n = 0;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &size_var = program->getVariable(operand[2].op);
        n = static_cast<size_t>(size_var.var_value.int_value);
    } else {
        n = static_cast<size_t>(operand[2].op_value);
    }

    if (n > 0) {
        memmove(dest.var_value.ptr_value, src.var_value.ptr_value, n);
    }

    // Return dest pointer in %rax
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = dest.var_value.ptr_value;
}

extern "C" void mxvm_std_memset(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("memset requires 3 arguments (dest, value, size).");
    }

    if (!program->isVariable(operand[0].op)) {
        throw mx::Exception("memset dest must be a pointer variable.");
    }

    mxvm::Variable &dest = program->getVariable(operand[0].op);

    if (dest.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("memset dest must be a pointer variable.");
    }

    mxvm::except_assert("memset: dest pointer is null", dest.var_value.ptr_value != nullptr);

    int value = 0;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &val_var = program->getVariable(operand[1].op);
        value = static_cast<int>(val_var.var_value.int_value);
    } else {
        value = static_cast<int>(operand[1].op_value);
    }

    size_t n = 0;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &size_var = program->getVariable(operand[2].op);
        n = static_cast<size_t>(size_var.var_value.int_value);
    } else {
        n = static_cast<size_t>(operand[2].op_value);
    }

    if (n > 0) {
        memset(dest.var_value.ptr_value, value, n);
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = dest.var_value.ptr_value;
}

extern "C" void mxvm_std_exp(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("exp requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::exp(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_exp2(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("exp2 requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::exp2(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_log(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("log requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::log(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_log10(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("log10 requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::log10(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_log2(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("log2 requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::log2(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_fmod(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2)
        throw mx::Exception("fmod requires 2 arguments (x, y).");
    double x = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double y = program->isVariable(operand[1].op) ? (program->getVariable(operand[1].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[1].op).var_value.float_value : (double)program->getVariable(operand[1].op).var_value.int_value)
                                                  : (double)operand[1].op_value;
    double r = std::fmod(x, y);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_atan2(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2)
        throw mx::Exception("atan2 requires 2 arguments (y, x).");
    double y = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double x = program->isVariable(operand[1].op) ? (program->getVariable(operand[1].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[1].op).var_value.float_value : (double)program->getVariable(operand[1].op).var_value.int_value)
                                                  : (double)operand[1].op_value;
    double r = std::atan2(y, x);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_asin(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("asin requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::asin(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_acos(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("acos requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::acos(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_atan(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("atan requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::atan(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_sinh(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("sinh requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::sinh(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_cosh(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("cosh requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::cosh(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_tanh(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("tanh requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::tanh(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_hypot(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2)
        throw mx::Exception("hypot requires 2 arguments (x, y).");
    double x = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double y = program->isVariable(operand[1].op) ? (program->getVariable(operand[1].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[1].op).var_value.float_value : (double)program->getVariable(operand[1].op).var_value.int_value)
                                                  : (double)operand[1].op_value;
    double r = std::hypot(x, y);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_round(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("round requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::round(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_trunc(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1)
        throw mx::Exception("trunc requires 1 argument.");
    double v = program->isVariable(operand[0].op) ? (program->getVariable(operand[0].op).type == mxvm::VarType::VAR_FLOAT ? program->getVariable(operand[0].op).var_value.float_value : (double)program->getVariable(operand[0].op).var_value.int_value)
                                                  : (double)operand[0].op_value;
    double r = std::trunc(v);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = r;
}

extern "C" void mxvm_std_argc(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    (void)operand;
    int v = argc();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = (int64_t)v;
}

extern "C" void mxvm_std_argv(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("argv(index) requires 1 argument");
    }

    int idx = 0;
    if (operand[0].type == mxvm::OperandType::OP_CONSTANT) {
        idx = (int)operand[0].op_value;
    } else if (program->isVariable(operand[0].op)) {
        mxvm::Variable &v = program->getVariable(operand[0].op);
        if (v.type != mxvm::VarType::VAR_INTEGER) {
            throw mx::Exception("argv index must be an integer");
        }
        idx = (int)v.var_value.int_value;
    } else {
        throw mx::Exception("argv argument must be an index constant or integer variable");
    }

    const char *s = argv(idx);
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = (void *)s; /* may be NULL */
}

extern "C" void mxvm_std_free_program_args(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    free_program_args();
}

extern "C" void mxvm_std_set_program_args(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("set_program_args requires 2 arguments (argc, argv)");
    }

    if (!program->isVariable(operand[0].op) || !program->isVariable(operand[1].op)) {
        throw mx::Exception("set_program_args arguments must be variables");
    }

    mxvm::Variable &argc_var = program->getVariable(operand[0].op);
    mxvm::Variable &argv_var = program->getVariable(operand[1].op);

    if (argc_var.type != mxvm::VarType::VAR_INTEGER) {
        throw mx::Exception("set_program_args first argument (argc) must be an integer");
    }

    if (argv_var.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("set_program_args second argument (argv) must be a pointer");
    }

    int argc = (int)argc_var.var_value.int_value;
    const char **argv = (const char **)argv_var.var_value.ptr_value;

    if (argv == nullptr && argc > 0) {
        throw mx::Exception("set_program_args: argv pointer is null but argc > 0");
    }

    set_program_args(argc, argv);
}

extern "C" void mxvm_std_float_to_int(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("float_to_int requires 1 argument (float value).");
    }

    double value = 0.0;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_FLOAT) {
            value = var.var_value.float_value;
        } else if (var.type == mxvm::VarType::VAR_INTEGER) {
            value = static_cast<double>(var.var_value.int_value);
        } else {
            throw mx::Exception("float_to_int argument must be a float or integer variable.");
        }
    } else {
        value = static_cast<double>(operand[0].op_value);
    }

    int64_t result = static_cast<int64_t>(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_std_int_to_float(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("int_to_float requires 1 argument (integer value).");
    }

    int64_t value = 0;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_INTEGER) {
            value = var.var_value.int_value;
        } else if (var.type == mxvm::VarType::VAR_FLOAT) {
            value = static_cast<int64_t>(var.var_value.float_value);
        } else {
            throw mx::Exception("int_to_float argument must be an integer or float variable.");
        }
    } else {
        value = operand[0].op_value;
    }

    double result = static_cast<double>(value);
    program->vars["%rax"].type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_FLOAT;
    program->vars["%rax"].var_value.float_value = result;
}