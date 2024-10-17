#include"embed_func.hpp"
#include<initializer_list>

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

    std::unordered_map<std::string, interp::FuncPtr> func_table  { 
        {"puts", func_print}, 
        {"str", func_str},
        {"malloc", func_malloc},
        {"free", func_free},
    };

}