#include"embed_func.hpp"
#include<initializer_list>
#include<algorithm>
#include<cstring>
#include<ctime>
#include<cstdlib>

namespace lib { 

    void check_args(const std::string &n, const std::vector<interp::Var>  &v, std::initializer_list<ast::VarType> args) {
        if(v.size() != args.size()) {
            std::ostringstream stream;
            stream << "In function: " << n << " argument count mismatch.";
            throw interp::Exception(stream.str());
        }
        size_t i = 0;
        for(auto a = args.begin(); a != args.end(); ++a) {
            if(v.at(i).type != *a) {
                std::ostringstream stream;
                stream << "In function: " << n << " argumment type mismatch " << static_cast<int>(v.at(i).type) << ":" << static_cast<int>(*a);
                throw interp::Exception(stream.str());
            }
            i ++;
        }
    }

    interp::Var func_print(const std::vector<interp::Var> &v) {
        check_args("puts", v, {ast::VarType::STRING});
        std::cout << v.at(0).string_value;
        return interp::Var("return", (long)0);
    }

    interp::Var func_str(const std::vector<interp::Var> &v) {
        check_args("str", v, {ast::VarType::NUMBER});
        std::string value;
        value = std::to_string(v.at(0).numeric_value);
        return interp::Var("return", value);
    }

    interp::Var func_malloc(const std::vector<interp::Var> &v) {
        check_args("malloc", v, {ast::VarType::NUMBER});
        void *buf = nullptr;
        buf = (void*) malloc(v.at(0).numeric_value);
        return interp::Var("return", (void*)buf);
    }

    interp::Var func_free(const std::vector<interp::Var> &v) {
        check_args("free", v, {ast::VarType::POINTER});
        free(v.at(0).ptr_value);   
        return interp::Var("return", (long)0);
    }

    interp::Var func_scan_integer(const std::vector<interp::Var> &v) {
        check_args("scan_integer", v, {});
        std::string line;
        std::getline(std::cin, line);
        return interp::Var("return", (long)std::atol(line.c_str()));
    }

    interp::Var func_len(const std::vector<interp::Var> &v) {
        check_args("len", v, {ast::VarType::STRING});
        return interp::Var("return", (long)v.at(0).string_value.length());
    }

    interp::Var func_exit(const std::vector<interp::Var> &v) {
        check_args("exit", v, {ast::VarType::NUMBER});
        exit(v.at(0).numeric_value);
        return interp::Var("return", (long)0);
    }
   
    interp::Var func_srand(const std::vector<interp::Var> &v) {
        check_args("srand", v, {ast::VarType::NUMBER});
        srand(v.at(0).numeric_value);
        return interp::Var("return", (long)0);
    }

    interp::Var func_rand(const std::vector<interp::Var> &v) {
        check_args("rand", v, {});
        return interp::Var("return", (long)rand());
    }

    interp::Var func_time(const std::vector<interp::Var> &v) {
        check_args("time", v, {ast::VarType::NUMBER});
        int t = time(0);
        return interp::Var("return", (long)t);
    }

    interp::Var func_strlen(const std::vector<interp::Var> &v) {
        check_args("strlen", v, {ast::VarType::STRING});
        return interp::Var("return", (long)v.at(0).string_value.length());
    }

    interp::Var func_strcmp(const std::vector<interp::Var> &v) {
        check_args("strcmp", v, {ast::VarType::STRING, ast::VarType::STRING});
        return interp::Var("return", (long)strcmp(v.at(0).string_value.c_str(), v.at(1).string_value.c_str()));
    }

    interp::Var func_at(const std::vector<interp::Var> &v) {
        check_args("at", v,{ast::VarType::STRING, ast::VarType::NUMBER});
        return interp::Var("return", (long)v.at(0).string_value.at(v.at(1).numeric_value));
    }

    interp::Var func_char_at(const std::vector<interp::Var> &v) {
        check_args("charAt", v,{ast::VarType::STRING, ast::VarType::NUMBER});
        char *buf = (char*)malloc(2);
        buf[0] = v.at(0).string_value.at(v.at(1).numeric_value);
        return interp::Var("return", (void*)buf);
    }

    interp::Var func_ptr(const std::vector<interp::Var> &v) {
        check_args("ptr", v,{ast::VarType::STRING});
        return interp::Var("return", (void*)v.at(0).string_value.data());
    }

    interp::Var func_string(const std::vector<interp::Var> &v) {
        check_args("string", v,{ast::VarType::POINTER});
        return interp::Var("return", std::string((char*)v.at(0).ptr_value));
    }

    interp::Var func_calloc(const std::vector<interp::Var> &v) {
        check_args("calloc", v, {ast::VarType::NUMBER});
        void *buf = nullptr;
        buf = (void*) calloc(1, v.at(0).numeric_value);
        return interp::Var("return", (void*)buf);
    }

    interp::Var func_mematl(const std::vector<interp::Var> &v) {
        check_args("mematl", v, {ast::VarType::POINTER, ast::VarType::NUMBER});
        long *value = (long *)v.at(0).ptr_value;
        return interp::Var("return", (long)value[v.at(1).numeric_value]);
    }

    interp::Var func_mematb(const std::vector<interp::Var> &v) {
        check_args("mematb", v, {ast::VarType::POINTER, ast::VarType::NUMBER});
        char *value = (char *)v.at(0).ptr_value;
        return interp::Var("return", (long)value[v.at(1).numeric_value]);
    }

    interp::Var func_memclr(const std::vector<interp::Var> &v) {
        check_args("memclr", v, {ast::VarType::POINTER, ast::VarType::NUMBER});
        memset(v.at(0).ptr_value, 0, v.at(1).numeric_value);
        return interp::Var("return", (long)0);
    }

    interp::Var func_memstorel(const std::vector<interp::Var> &v) {
        check_args("memstorel", v, {ast::VarType::POINTER, ast::VarType::NUMBER, ast::VarType::NUMBER});
        long *value = (long *)v.at(0).ptr_value;
        value[v.at(1).numeric_value] = v.at(2).numeric_value;
        return interp::Var("return", (long)0);
    }

    interp::Var func_memstoreb(const std::vector<interp::Var> &v) {
        check_args("memstoreb", v, {ast::VarType::POINTER, ast::VarType::NUMBER, ast::VarType::NUMBER});
        char *value = (char *)v.at(0).ptr_value;
        value[v.at(1).numeric_value] = (char)v.at(2).numeric_value;
        return interp::Var("return", (long)0);
    }

    interp::Var func_memcpy(const std::vector<interp::Var> &v) {
        check_args("memcpy", v, {ast::VarType::POINTER, ast::VarType::POINTER, ast::VarType::NUMBER});
        void *b = memcpy(v.at(0).ptr_value, v.at(1).ptr_value, v.at(2).numeric_value);
        return interp::Var("return", (void *)b);
    }

    std::unordered_map<std::string, interp::FuncPtr> func_table  { 
        {"puts", func_print}, 
        {"str", func_str},
        {"malloc", func_malloc},
        {"free", func_free},
        {"scan_integer", func_scan_integer},
        {"len", func_len},
        {"exit", func_exit},
        {"srand",  func_srand},
        {"rand", func_rand},
        {"time", func_time},
        {"strlen", func_strlen},
        {"strcmp", func_strcmp},
        {"at", func_at},
        {"charAt", func_char_at},
        {"ptr", func_ptr},
        {"string", func_string},
        {"calloc", func_calloc},
        {"mematl", func_mematl},
        {"mematb", func_mematb},
        {"memclr", func_memclr},
        {"memstorel", func_memstorel},
        {"memstoreb", func_memstoreb},
        {"memcpy", func_memcpy}
    };

}