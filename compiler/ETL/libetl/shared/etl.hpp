#ifndef __ETL_H___
#define __ETL_H___

#include<string>
#include<vector>
#include<iostream>
#include<sstream>

namespace interp {

    inline void check_args(const std::string &n, const std::vector<interp::Var>  &v, std::initializer_list<ast::VarType> args) {
        if(v.size() != args.size()) {
            std::ostringstream stream;
            stream << "In function: " << n << " argument count mismatch.\n";
            std::cout << stream.str();
            exit(EXIT_FAILURE);
        }
        size_t i = 0;
        for(auto a = args.begin(); a != args.end(); ++a) {
            if(v.at(i).type != *a) {
                std::ostringstream stream;
                stream << "In function: " << n << " argumment type mismatch " << static_cast<int>(v.at(i).type) << ":" << static_cast<int>(*a) << "\n";
                std::cout << stream.str();
                exit(EXIT_FAILURE);
            }
            i ++;
        }
    }
}

typedef void  (*addFunction)(const char *src, void *ptr);

#endif