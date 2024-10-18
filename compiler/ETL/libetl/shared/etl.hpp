#ifndef __ETL_H___
#define __ETL_H___

#include<string>
#include<vector>
#include<iostream>
#include<sstream>

namespace ast {
    enum class VarType { NUMBER, STRING, POINTER, TYPE_NULL };
}
namespace interp {
    struct Var {
        Var() = default;
        Var(const std::string &n, ast::VarType v) : name{n}, numeric_value{0}, ptr_value{nullptr}, type{v} {}
        Var(const std::string &n, const std::string &value) : name{n}, string_value{value}, type{ast::VarType::STRING} {}
        Var(const std::string &n, long value) : name{n}, numeric_value{value}, type{ast::VarType::NUMBER} {}
        Var(const std::string &n, void *ptr) : name{n}, ptr_value{ptr}, type {ast::VarType::POINTER} {}
        Var(const Var &v) : name{v.name}, numeric_value{v.numeric_value}, string_value{v.string_value}, ptr_value{v.ptr_value}, type{v.type} {}
        Var(Var &&v) : name{std::move(v.name)}, numeric_value{v.numeric_value}, string_value{std::move(v.string_value)}, ptr_value{v.ptr_value}, type{v.type} {}

        Var &operator=(const Var &v) {
            name = v.name;
            numeric_value = v.numeric_value;
            string_value = v.string_value;
            ptr_value = v.ptr_value;
            type = v.type;
            return *this;
        }

        Var &operator=(Var &&v) {
            name = std::move(v.name);
            numeric_value = v.numeric_value;
            string_value = std::move(v.string_value);
            ptr_value = v.ptr_value;
            type = v.type;
            return *this;
        }

        std::string name;
        long numeric_value = 0;
        std::string string_value;
        void *ptr_value = nullptr;
        ast::VarType type;
    };

    void check_args(const std::string &n, const std::vector<interp::Var>  &v, std::initializer_list<ast::VarType> args) {
        if(v.size() != args.size()) {
            std::ostringstream stream;
            stream << "In function: " << n << " argument count mismatch.";
            std::cout << stream.str();
            exit(EXIT_FAILURE);
        }
        size_t i = 0;
        for(auto a = args.begin(); a != args.end(); ++a) {
            if(v.at(i).type != *a) {
                std::ostringstream stream;
                stream << "In function: " << n << " argumment type mismatch " << static_cast<int>(v.at(i).type) << ":" << static_cast<int>(*a);
                std::cout << stream.str();
                exit(EXIT_FAILURE);
            }
            i ++;
        }
    }
}

typedef void  (*addFunction)(const char *src, void *ptr);

#endif