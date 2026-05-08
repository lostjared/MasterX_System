/**
 * @file io.cpp
 * @brief I/O module C++ runtime bindings for the MXVM interpreter
 * @author Jared Bruni
 */
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <mxvm/icode.hpp>
#include <mxvm/instruct.hpp>
#include <string>
#include <vector>

extern "C" void mxvm_io_fopen(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() == 2) {
        std::string &filename = operand[0].op;
        std::string &mode = operand[1].op;
        if (program->isVariable(filename)) {
            mxvm::Variable &v = program->getVariable(filename);
            if (v.type != mxvm::VarType::VAR_STRING) {
                throw mx::Exception("Requires string variable type for fopen.\n");
            }
            filename = v.var_value.str_value;
        } else {
            throw mx::Exception("Variable required for fopen.");
        }
        if (program->isVariable(mode)) {
            mxvm::Variable &v = program->getVariable(mode);
            if (v.type != mxvm::VarType::VAR_STRING) {
                throw mx::Exception("Requires string variable type for fopen.\n");
            }
            mode = v.var_value.str_value;
        } else {
            throw mx::Exception("Variable required for fopen.");
        }
        program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
        program->vars["%rax"].var_name = "%rax";
        program->vars["%rax"].var_value.ptr_value = (void *)fopen(filename.c_str(), mode.c_str());
    } else {
        throw mx::Exception("Error invalid types for fopen.\n");
    }
}

extern "C" void mxvm_io_fprintf(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() < 2) {
        throw mx::Exception("Requires at least two arguments for fprintf.");
    }
    if (program->isVariable(operand[0].op)) {
        std::vector<mxvm::Variable *> v;
        for (size_t i = 2; i < operand.size(); ++i) {
            if (program->isVariable(operand[i].op)) {
                mxvm::Variable &varval = program->getVariable(operand[i].op);
                v.push_back(&varval);
            } else {
                if (operand[i].type == mxvm::OperandType::OP_CONSTANT) {
                    mxvm::Variable val = program->createTempVariable(mxvm::VarType::VAR_INTEGER, operand[i].op);
                    v.push_back(&val);
                }
            }
        }
        if (program->isVariable(operand[1].op)) {
            mxvm::Variable &fmtv = program->getVariable(operand[1].op);
            if (fmtv.type != mxvm::VarType::VAR_STRING) {
                throw mx::Exception("fprintf requires format to be a string.");
            }
            std::string value = program->printFormatted(fmtv.var_value.str_value, v, false);
            mxvm::Variable &vx = program->getVariable(operand[0].op);
            if (vx.type == mxvm::VarType::VAR_POINTER || vx.type == mxvm::VarType::VAR_EXTERN) {
                FILE *fptr = reinterpret_cast<FILE *>(vx.var_value.ptr_value);
                fprintf(fptr, "%s", value.c_str());
            }
        } else {
            throw mx::Exception("fprintf requires format to be a variable");
        }
    } else {
        throw mx::Exception("fprintf requires first argument to be valid variable found: " + operand[0].op);
    }
}

extern "C" void mxvm_io_fclose(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("fclose requires a single file pointer argument.");
    }
    std::string &file_var = operand[0].op;
    if (!program->isVariable(file_var)) {
        throw mx::Exception("fclose argument must be a variable (file pointer).");
    }
    mxvm::Variable &file_v = program->getVariable(file_var);
    if (file_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fclose argument must be a pointer variable.");
    }
    FILE *fp = reinterpret_cast<FILE *>(file_v.var_value.ptr_value);
    int result = fclose(fp);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_fsize(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("fsize requires a single file pointer argument.");
    }
    std::string &file_var = operand[0].op;
    if (!program->isVariable(file_var)) {
        throw mx::Exception("fsize argument must be a variable pointer.");
    }
    mxvm::Variable &file_v = program->getVariable(file_var);
    if (file_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fsize argument must be a pointer variable.");
    }
    FILE *fptr = reinterpret_cast<FILE *>(file_v.var_value.ptr_value);
    if (fptr == NULL) {
        throw mx::Exception("Error pointer handle must not be null");
    }
    fseek(fptr, 0, SEEK_END);
    size_t result = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_fread(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("fread requires four arguments.");
    }
    std::string &buf_var = operand[0].op;
    if (!program->isVariable(buf_var)) {
        throw mx::Exception("fread argument must be a variable pointer.");
    }
    mxvm::Variable &buf_v = program->getVariable(buf_var);
    if (buf_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fread argument must be a pointer variable.");
    }

    mxvm::Variable vsize, vcount, file_ptr;
    vsize = program->variableFromOperand(operand[1]);
    vcount = program->variableFromOperand(operand[2]);
    file_ptr = program->variableFromOperand(operand[3]);

    if (vsize.type != mxvm::VarType::VAR_INTEGER || vcount.type != mxvm::VarType::VAR_INTEGER) {
        throw mx::Exception("Argument type mismatch expected integer for fread");
    }
    if (file_ptr.type != mxvm::VarType::VAR_POINTER && file_ptr.type != mxvm::VarType::VAR_EXTERN) {
        throw mx::Exception("Final argument for fread requires pointer");
    }
    FILE *fptr = reinterpret_cast<FILE *>(file_ptr.var_value.ptr_value);
    size_t result = fread(buf_v.var_value.ptr_value, vsize.var_value.int_value, vcount.var_value.int_value, fptr);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_fwrite(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("fwrite requires four arguments.");
    }
    std::string &buf_var = operand[0].op;
    if (!program->isVariable(buf_var)) {
        throw mx::Exception("fwrite argument must be a variable pointer.");
    }
    mxvm::Variable &buf_v = program->getVariable(buf_var);
    if (buf_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fwrite argument must be a pointer variable.");
    }

    mxvm::Variable vsize, vcount, file_ptr;
    vsize = program->variableFromOperand(operand[1]);
    vcount = program->variableFromOperand(operand[2]);
    file_ptr = program->variableFromOperand(operand[3]);

    if (vsize.type != mxvm::VarType::VAR_INTEGER || vcount.type != mxvm::VarType::VAR_INTEGER) {
        throw mx::Exception("Argument type mismatch expected integer for fwrite");
    }
    if (file_ptr.type != mxvm::VarType::VAR_POINTER && file_ptr.type != mxvm::VarType::VAR_EXTERN) {
        throw mx::Exception("Final argument for fwrite requires pointer");
    }
    FILE *fptr = reinterpret_cast<FILE *>(file_ptr.var_value.ptr_value);
    size_t result = fwrite(buf_v.var_value.ptr_value, vsize.var_value.int_value, vcount.var_value.int_value, fptr);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_fseek(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("fseek requires three arguments got: " + std::to_string(operand.size()));
    }
    std::string &buf_var = operand[0].op;
    if (!program->isVariable(buf_var)) {
        throw mx::Exception("fseek argument must be a variable found: " + buf_var);
    }
    mxvm::Variable &buf_v = program->getVariable(buf_var);
    if (buf_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fseek argument must be a pointer variable.");
    }

    mxvm::Variable vsize, vcount, file_ptr;
    file_ptr = program->variableFromOperand(operand[0]);
    vsize = program->variableFromOperand(operand[1]);
    vcount = program->variableFromOperand(operand[2]);

    if (vsize.type != mxvm::VarType::VAR_INTEGER || vcount.type != mxvm::VarType::VAR_INTEGER) {
        throw mx::Exception("Argument type mismatch expec  ted integer for fread");
    }
    if (file_ptr.type != mxvm::VarType::VAR_POINTER && file_ptr.type != mxvm::VarType::VAR_EXTERN) {
        throw mx::Exception("Final argument for fread requires pointer");
    }
    FILE *fptr = reinterpret_cast<FILE *>(file_ptr.var_value.ptr_value);
    int result = fseek(fptr, vsize.var_value.int_value, vcount.var_value.int_value);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_rand_number(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("rand_number requires one argument (size).");
    }
    int64_t rsize = 1;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &vsize = program->getVariable(operand[0].op);
        if (vsize.type != mxvm::VarType::VAR_INTEGER) {
            throw mx::Exception("rand_number argument must be integer (size).");
        }
        rsize = vsize.var_value.int_value;
    } else {

        if (operand[0].type == mxvm::OperandType::OP_CONSTANT)
            rsize = std::stoll(operand[0].op, nullptr, 0);
        else
            throw mx::Exception("rand_number expected variable or constant: " + operand[0].op);
    }
    if (rsize <= 0) {
        throw mx::Exception("rand_number: size must be positive, got " + std::to_string(rsize));
    }
    int result = std::rand() % rsize;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_seed_random(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    uint64_t mixed = static_cast<uint64_t>(now);
    mixed ^= static_cast<uint64_t>(reinterpret_cast<std::uintptr_t>(program));
    mixed ^= (mixed >> 33);
    mixed *= 0xff51afd7ed558ccdULL;
    mixed ^= (mixed >> 33);
    mixed *= 0xc4ceb9fe1a85ec53ULL;
    mixed ^= (mixed >> 33);
    std::srand(static_cast<unsigned int>(mixed));
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = 0;

}

extern "C" void mxvm_io_feof(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("feof requires one file pointer argument.");
    }
    std::string &file_var = operand[0].op;
    if (!program->isVariable(file_var)) {
        throw mx::Exception("feof argument must be a variable (file pointer).");
    }
    mxvm::Variable &file_v = program->getVariable(file_var);
    if (file_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("feof argument must be a pointer variable.");
    }
    FILE *fp = reinterpret_cast<FILE *>(file_v.var_value.ptr_value);
    int result = feof(fp) ? 1 : 0;
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_io_mxvm_fgets(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("mxvm_fgets requires one file pointer argument.");
    }
    std::string &file_var = operand[0].op;
    if (!program->isVariable(file_var)) {
        throw mx::Exception("fgets argument must be a variable (file pointer).");
    }
    mxvm::Variable &file_v = program->getVariable(file_var);
    if (file_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fgets argument must be a pointer variable.");
    }
    FILE *fp = reinterpret_cast<FILE *>(file_v.var_value.ptr_value);
    char buf[4096];
    char *res = fgets(buf, sizeof(buf), fp);
    if (res) {
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[len - 1] = '\0';
        char *str = strdup(buf);
        program->vars["%rax"].type = mxvm::VarType::VAR_STRING;
        program->vars["%rax"].var_value.type = mxvm::VarType::VAR_STRING;
        program->vars["%rax"].var_value.str_value = str;
    } else {
        program->vars["%rax"].type = mxvm::VarType::VAR_STRING;
        program->vars["%rax"].var_value.type = mxvm::VarType::VAR_STRING;
        program->vars["%rax"].var_value.str_value = strdup("");
    }
}

extern "C" void mxvm_io_fputs(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("fputs requires two arguments (string, file).");
    }
    std::string str_val;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &v = program->getVariable(operand[0].op);
        if (v.type == mxvm::VarType::VAR_STRING) {
            str_val = v.var_value.str_value;
        } else {
            throw mx::Exception("fputs first argument must be a string variable.");
        }
    } else {
        throw mx::Exception("fputs first argument must be a variable.");
    }
    std::string &file_var = operand[1].op;
    if (!program->isVariable(file_var)) {
        throw mx::Exception("fputs second argument must be a variable (file pointer).");
    }
    mxvm::Variable &file_v = program->getVariable(file_var);
    if (file_v.type != mxvm::VarType::VAR_POINTER) {
        throw mx::Exception("fputs second argument must be a pointer variable.");
    }
    FILE *fp = reinterpret_cast<FILE *>(file_v.var_value.ptr_value);
    int result = fputs(str_val.c_str(), fp);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}