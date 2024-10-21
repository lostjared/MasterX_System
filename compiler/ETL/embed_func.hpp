#ifndef _EMBED_FUNC_H_
#define _EMBED_FUNC_H_

#include"interp.hpp"

namespace lib {
    extern std::unordered_map<std::string, interp::FuncPtr> func_table;
    void initSharedObject(const std::string &name, const std::string &table_name);
    void loadSharedObjects(const std::string &lib_path, const std::string &filename);
    void releaseSharedObjects();
    void initStatic();
}
typedef void  (*addFunction)(const char *src, void *ptr);

#ifdef WITH_STATIC_SDL
extern "C" void libsdl_rt_initTable(addFunction f);
extern "C" void libio_rt_initTable(addFunction f);
#endif

#endif