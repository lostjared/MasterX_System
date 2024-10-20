#include"embed_func.hpp"
#include<initializer_list>
#include<algorithm>
#include<cstring>
#include<ctime>
#include<cstdlib>
#include<dlfcn.h>

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
                stream << "In function: " << n << " argumment type mismatch found " << ast::VarString[static_cast<int>(v.at(i).type)] << " expected " << ast::VarString[static_cast<int>(*a)];
                throw interp::Exception(stream.str());
            }
            i ++;
        }
    }

    interp::Var func_print(const std::vector<interp::Var> &v) {
        check_args("puts", v, {ast::VarType::STRING});
        std::cout << v.at(0).string_value << "\n";
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
        if(value == nullptr) 
            throw interp::Exception("Null pointer [mematl] " + v.at(0).name);
        
        return interp::Var("return", (long)value[v.at(1).numeric_value]);
    }

    interp::Var func_mematb(const std::vector<interp::Var> &v) {
        check_args("mematb", v, {ast::VarType::POINTER, ast::VarType::NUMBER});
        char *value = (char *)v.at(0).ptr_value;
        if(value == nullptr) 
            throw interp::Exception("Null pointer [mematb] " + v.at(0).name);

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
        if(value == nullptr) {
            throw interp::Exception("Null pointer [memstorel] " + v.at(0).name);
        }
        value[v.at(1).numeric_value] = v.at(2).numeric_value;
        return interp::Var("return", (long)0);
    }

    interp::Var func_memstoreb(const std::vector<interp::Var> &v) {
        check_args("memstoreb", v, {ast::VarType::POINTER, ast::VarType::NUMBER, ast::VarType::NUMBER});
        char *value = (char *)v.at(0).ptr_value;
        if(value == nullptr) {
            throw interp::Exception("Null pointer [memstoreb] " + v.at(0).name);
        }
        value[v.at(1).numeric_value] = (char)v.at(2).numeric_value;
        return interp::Var("return", (long)0);
    }

    interp::Var func_memcpy(const std::vector<interp::Var> &v) {
        check_args("memcpy", v, {ast::VarType::POINTER, ast::VarType::POINTER, ast::VarType::NUMBER});
        void *b = memcpy(v.at(0).ptr_value, v.at(1).ptr_value, v.at(2).numeric_value);
        return interp::Var("return", (void *)b);
    }

    interp::Var func_release(const std::vector<interp::Var> &v) {
        check_args("release", v, {ast::VarType::STRING});
        return interp::Var("return", (long)0);
    }

    interp::Var func_string_int(const std::vector<interp::Var> &v) {
        check_args("string_int", v, {ast::VarType::POINTER, ast::VarType::NUMBER});
        std::string value = std::to_string(v.at(1).numeric_value);
        strcpy((char*)v.at(0).ptr_value, value.c_str());
        return interp::Var("return", (long)0);
    }


    interp::Var func_printf(const std::vector<interp::Var> &v) {       
        if(v.size() > 0) {
            std::string input = v.at(0).string_value;
            size_t index = 1;
            for(size_t i = 0; i < input.length(); ++i) {
                if(input.at(i) != '%') {
                    std::cout << input.at(i);
                    continue;
                } 
                if(i+1 < input.length() && input.at(i+1) == '%') {
                    std::cout << '%';
                    i++;
                    continue;
                } else if(i+1 < input.length()) {
                    char c = input.at(i+1);
                    if(index < v.size()) {
                        switch(v.at(index).type) {
                            case ast::VarType::STRING:
                            std::cout << v.at(index).string_value;
                            break;
                            case ast::VarType::NUMBER:
                                if(c == 'c')
                                    std::cout << (char)v.at(index).numeric_value;
                                else 
                                    std::cout << (long)v.at(index).numeric_value;
                            break;
                            case ast::VarType::POINTER:
                            std::cout << (long)v.at(index).ptr_value;
                            break;
                            default:
                            break;
                        }
                    }
                    index ++;
                    i ++;
                }
            }
        }
        return interp::Var("return", (long)0);
    }

     interp::Var func_sprintf(const std::vector<interp::Var> &v) {
        if(v.size() >= 2) {
            void *buf = v.at(0).ptr_value;
            std::ostringstream stream;
            std::string input = v.at(1).string_value;
            size_t index = 2;
            for(size_t i = 0; i < input.length(); ++i) {
                if(input.at(i) != '%') {
                    stream << input.at(i);
                    continue;
                } 
                if(i+1 < input.length() && input.at(i+1) == '%') {
                    stream << '%';
                    i++;
                    continue;
                } else if(i+1 < input.length()) {
                    char c = input.at(i+1);
                    if(index < v.size()) {
                        switch(v.at(index).type) {
                            case ast::VarType::STRING:
                            stream << v.at(index).string_value;
                            break;
                            case ast::VarType::NUMBER:
                                if(c == 'c')
                                    stream << (char)v.at(index).numeric_value;
                                else 
                                    stream << (long)v.at(index).numeric_value;
                            break;
                            case ast::VarType::POINTER:
                            stream << (long)v.at(index).ptr_value;
                            break;
                            default:
                            break;
                        }
                    }
                    index ++;
                    i ++;
                }
                strcpy((char*)buf, stream.str().c_str());
            }
            return interp::Var("return", stream.str());
        }
        return interp::Var("return", "");
        
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
        {"memcpy", func_memcpy},
        {"release", func_release},
        {"printf", func_printf},
        {"sprintf", func_sprintf}
    };

    std::vector<void *> shared_objects;

    void addFunc(const char *src, void *ptr) {
        func_table[std::string(src)] = (interp::FuncPtr)ptr;
    }
        
     void initSharedObject(const std::string &n) {
        void *obj = dlopen(n.c_str(), RTLD_NOW);
        if(!obj) {
            std::cerr << "ETL: Error opening shared library: " << n << "\n";
            char *e = dlerror();
            if(e) {
                std::cerr << "Error: " << e << "\n";
            }
            std::cerr.flush();
            exit(EXIT_FAILURE);
        }
        dlerror();
        shared_objects.push_back(obj);
        typedef void  (*addFunction)(const char *src, void *ptr);
        typedef void (*initTable)(addFunction);
        initTable table = (initTable)dlsym(obj, "initTable");
        char *err = dlerror();
        if(err) {
            std::cerr << "ETL: Could not get Pointer to table function: " << err << "\n";
            std::cerr.flush();
            exit(EXIT_FAILURE);
        }
        std::cout << "ETL: Loaded Shared Library: " << n << "\n";
        table(addFunc);
    }

    void releaseSharedObjects() {
        for(auto &i : shared_objects) {
            dlclose(i);
        }
    }
    
}