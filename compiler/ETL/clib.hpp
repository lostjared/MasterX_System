#ifndef C_LIB_H_
#define C_LIB_H_

#include<unordered_map>
#include<string>
#include<sstream>

namespace clib {

    enum class VarType {
        POINTER,
        INTEGER,
        FLOAT,
        DOUBLE,
        VARIABLE,
    };

    struct FunctionDef {
        std::string name;
        VarType return_type;
        std::vector<VarType> args;
        bool allocated;
    }; 
 
    inline std::unordered_map<std::string, FunctionDef> clibrary = {
        //{ "puts",   { "puts",   VarType::INTEGER, { VarType::POINTER }, false } },  
        { "printf", { "printf", VarType::INTEGER, { VarType::POINTER, VarType::VARIABLE }, false } },  
        /*
        { "rand",   { "rand",   VarType::INTEGER, {} , false } },                    
        { "srand",  { "srand",  VarType::POINTER, { VarType::INTEGER }, false } },  
        { "malloc", { "malloc", VarType::POINTER, { VarType::INTEGER }, true } },  
        { "free",   { "free",   VarType::POINTER, { VarType::POINTER }, false } },  
        { "fopen",  { "fopen",  VarType::POINTER, { VarType::POINTER, VarType::POINTER }, false } },  
        // libetl
        { "str", { "str", VarType::POINTER, { VarType::INTEGER }, true } },
        { "scan_integer", { "scan_integer", VarType::INTEGER, {}, false } },
        { "len", {"len", VarType::INTEGER,{ VarType::POINTER }, false } },
        */
    };              
}



#endif
