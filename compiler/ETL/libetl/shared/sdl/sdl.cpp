#include"SDL.h"
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

extern "C" void sdl_init();
extern "C" void sdl_init_size(long fs);
extern "C" void sdl_quit();
extern "C" void sdl_pump();
extern "C" void sdl_release();
extern "C" void sdl_create(const char *name, long  tw, long th, long w, long h);
extern "C" void sdl_setcolor(long r, long g, long b, long a);
extern "C" void sdl_fillrect(long x, long y, long w, long h);
extern "C" void sdl_flip();
extern "C" void sdl_clear();
extern "C" long sdl_keydown(long key);
extern "C" long sdl_getticks();
extern "C" SDL_Texture *sdl_loadtex(const char *src);
extern "C" void sdl_destroytex(SDL_Texture *tex);
extern "C" void sdl_copytex(SDL_Texture *tex, long x, long y, long w, long h);
extern "C" void sdl_settarget(SDL_Texture *t);
extern "C" void sdl_cleartarget();
extern "C" void sdl_printtext(long x, long y, const char *src);
extern "C" void sdl_settextcolor(long r, long g, long b, long a);
extern "C" void sdl_drawline(long x1, long y1, long x2, long y2);
extern "C" void sdl_drawcircle(long center_x, long center_y, long radius);
extern "C" long sdl_mousedown();
extern "C" long sdl_mousex();
extern "C" long sdl_mousey();
extern "C" void sdl_messagebox(const char *src);
extern "C" void sdl_delay(long waitx);
extern "C" void sdl_setstartcolor(long r, long g, long b, long a);
extern "C" void sdl_setendcolor(long r, long  g, long b, long a);
extern "C" void sdl_draw_gradient(long x, long y, long w, long h);


interp::Var func_sdl_init(const std::vector<interp::Var> &v) {
    sdl_init();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_init_size(const std::vector<interp::Var> &v) {
    check_args("sdl_init_size", v, {ast::VarType::NUMBER});
    sdl_init_size(v.at(0).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_quit(const std::vector<interp::Var> &v) {
    sdl_quit();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_pump(const std::vector<interp::Var> &v) {
    sdl_pump();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_release(const std::vector<interp::Var> &v) {
    sdl_release();
    return interp::Var("return", (long)0);
}

extern "C" void initTable(addFunction func) {
    func("sdl_init", (void*)func_sdl_init);
    func("sdl_init_size", (void*)func_sdl_init_size);
    func("sdl_quit", (void*)func_sdl_quit);
    func("sdl_pump", (void*)func_sdl_pump);
    func("sdl_release", (void*)func_sdl_release);
}