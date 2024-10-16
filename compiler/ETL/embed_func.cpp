#include"embed_func.hpp"

namespace lib { 

    interp::Var func_print(const std::vector<interp::Var> &v) {
        if(v.size() == 1 && v.at(0).type == ast::VarType::STRING) {
            std::cout << v.at(0).string_value;
        }
        return interp::Var("return", 1);
    }

    interp::Var func_str(const std::vector<interp::Var> &v) {
        std::string value;
        if(v.size() == 1 && v.at(0).type == ast::VarType::NUMBER) {
            value = std::to_string(v.at(0).numeric_value);
        }
        return interp::Var("return", value);
    }

    std::unordered_map<std::string, interp::FuncPtr> func_table  { 
        {"puts", func_print}, 
        {"str", func_str} 
    };

}