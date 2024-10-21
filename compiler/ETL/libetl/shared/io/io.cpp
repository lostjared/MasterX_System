#include "../../../interp.hpp"
#include "../../../ast.hpp"
#include "../etl.hpp"
#include<cstdio>

extern "C" void *file_open(const char *f, const char *m);
extern "C" void file_print(void *f, const char *data);
extern "C" long file_write(void *f, void *data, long size);
extern "C" long file_read(void *f, void *data, long size);
extern "C" long file_seek(void *f, long offset, long origin);
extern "C" void file_close(void *f);
extern "C" long file_size(void *f);

interp::Var func_file_open(const std::vector<interp::Var> &v) {
    check_args("file_open", v, {ast::VarType::STRING, ast::VarType::STRING});
    return interp::Var("return", (void*)file_open(v.at(0).string_value.c_str(), v.at(1).string_value.c_str()));
}

interp::Var func_file_print(const std::vector<interp::Var> &v) {
    check_args("file_print", v, {ast::VarType::POINTER, ast::VarType::STRING});
    file_print(v.at(0).ptr_value, v.at(1).string_value.c_str());
    return interp::Var("return", (long)0);
}

interp::Var func_file_write(const std::vector<interp::Var> &v) {
    check_args("file_write", v, {ast::VarType::POINTER, ast::VarType::POINTER, ast::VarType::NUMBER});
    return interp::Var("return", (long)file_write(v.at(0).ptr_value, v.at(1).ptr_value, v.at(2).numeric_value));
}

interp::Var func_file_read(const std::vector<interp::Var> &v) {
    check_args("file_read", v, {ast::VarType::POINTER, ast::VarType::POINTER, ast::VarType::NUMBER});
    return interp::Var("return", (long)file_read(v.at(0).ptr_value, v.at(1).ptr_value, v.at(2).numeric_value));
}

interp::Var func_file_seek(const std::vector<interp::Var> &v) {
    check_args("file_seek", v, {ast::VarType::POINTER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    return interp::Var("return", (long)file_seek(v.at(0).ptr_value, v.at(1).numeric_value, v.at(2).numeric_value));
}

interp::Var func_file_close(const std::vector<interp::Var> &v) {
    check_args("file_close", v, {ast::VarType::POINTER});
    file_close(v.at(0).ptr_value);
    return interp::Var("return", (long)0);
}

interp::Var func_file_size(const std::vector<interp::Var> &v) {
    check_args("file_size", v, {ast::VarType::POINTER});
    return interp::Var("return", (long)file_size(v.at(0).ptr_value));
}

extern "C" void libio_rt_initTable(addFunction func) {
    func("file_open", (void*)func_file_open);
    func("file_print", (void*)func_file_print);
    func("file_write", (void*)func_file_write);
    func("file_read", (void*)func_file_read);
    func("file_seek", (void*)func_file_seek);
    func("file_close", (void*)func_file_close);
    func("file_size", (void*)func_file_size);
}