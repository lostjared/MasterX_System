#ifndef _EMBED_FUNC_H_
#define _EMBED_FUNC_H_

#include"interp.hpp"

namespace lib {
    extern std::unordered_map<std::string, interp::FuncPtr> func_table;
}

#endif