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
extern "C" long sdl_pump();
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
    check_args("sdl_init", v, {});
    sdl_init();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_init_size(const std::vector<interp::Var> &v) {
    check_args("sdl_init_size", v, {ast::VarType::NUMBER});
    sdl_init_size(v.at(0).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_quit(const std::vector<interp::Var> &v) {
    check_args("sdl_quit", v, {});
    sdl_quit();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_pump(const std::vector<interp::Var> &v) {
    check_args("sdl_pump", v, {});
    return interp::Var("return", (long)sdl_pump());
}

interp::Var func_sdl_release(const std::vector<interp::Var> &v) {
    check_args("sdl_release", v, {});
    sdl_release();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_create(const std::vector<interp::Var> &v) {
    check_args("sdl_create", v, {ast::VarType::STRING, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_create(v.at(0).string_value.c_str(), v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value, v.at(4).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_setcolor(const std::vector<interp::Var> &v) {
    check_args("sdl_setcolor", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_setcolor(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_fillrect(const std::vector<interp::Var> &v) {
    check_args("sdl_fillrect", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_fillrect(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_flip(const std::vector<interp::Var> &v) {
    check_args("sdl_flip", v, {});
    sdl_flip();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_clear(const std::vector<interp::Var> &v) {
    check_args("sdl_clear", v, {});
    sdl_clear();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_keydown(const std::vector<interp::Var> &v) {
    check_args("sdl_keydown", v, {ast::VarType::NUMBER});
    return interp::Var("return", (long)sdl_keydown(v.at(0).numeric_value));
}

interp::Var func_sdl_getticks(const std::vector<interp::Var> &v) {
    check_args("sdl_getticks", v, {});
    return interp::Var("return", (long)sdl_getticks());
}

interp::Var func_sdl_loadtex(const std::vector<interp::Var> &v) {
    check_args("sdl_loadtex", v, {ast::VarType::STRING});
    return interp::Var("return", (void*)sdl_loadtex(v.at(0).string_value.c_str()));
}

interp::Var func_sdl_destroytex(const std::vector<interp::Var> &v) {
    check_args("sdl_destroytex", v, {ast::VarType::POINTER});
    sdl_destroytex((SDL_Texture*)v.at(0).ptr_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_copytex(const std::vector<interp::Var> &v) {
    check_args("sdl_copytex", v, {ast::VarType::POINTER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_copytex((SDL_Texture*)v.at(0).ptr_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value, v.at(4).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_settarget(const std::vector<interp::Var> &v) {
    check_args("sdl_settarget", v, {ast::VarType::POINTER});
    sdl_settarget((SDL_Texture*)v.at(0).ptr_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_cleartarget(const std::vector<interp::Var> &v) {
    check_args("sdl_cleartarget", v, {});
    sdl_cleartarget();
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_printtext(const std::vector<interp::Var> &v) {
    check_args("sdl_printtext", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::STRING});
    sdl_printtext(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).string_value.c_str());
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_settextcolor(const std::vector<interp::Var> &v) {
    check_args("sdl_settextcolor", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_settextcolor(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_drawline(const std::vector<interp::Var> &v) {
    check_args("sdl_drawline", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_drawline(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_drawcircle(const std::vector<interp::Var> &v) {
    check_args("sdl_drawcircle", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_drawcircle(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_mousedown(const std::vector<interp::Var> &v) {
    check_args("sdl_mousedown", v, {});
    return interp::Var("return", (long)sdl_mousedown());
}

interp::Var func_sdl_mousex(const std::vector<interp::Var> &v) {
    check_args("sdl_mousex", v, {});
    return interp::Var("return", (long)sdl_mousex());
}

interp::Var func_sdl_mousey(const std::vector<interp::Var> &v) {
    check_args("sdl_mousey", v, {});
    return interp::Var("return", (long)sdl_mousey());
}

interp::Var func_sdl_messagebox(const std::vector<interp::Var> &v) {
    check_args("sdl_messagebox", v, {ast::VarType::STRING});
    sdl_messagebox(v.at(0).string_value.c_str());
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_delay(const std::vector<interp::Var> &v) {
    check_args("sdl_delay", v, {ast::VarType::NUMBER});
    sdl_delay(v.at(0).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_setstartcolor(const std::vector<interp::Var> &v) {
    check_args("sdl_setstartcolor", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_setstartcolor(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_setendcolor(const std::vector<interp::Var> &v) {
    check_args("sdl_setendcolor", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_setendcolor(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}

interp::Var func_sdl_draw_gradient(const std::vector<interp::Var> &v) {
    check_args("sdl_draw_gradient", v, {ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER, ast::VarType::NUMBER});
    sdl_draw_gradient(v.at(0).numeric_value, v.at(1).numeric_value, v.at(2).numeric_value, v.at(3).numeric_value);
    return interp::Var("return", (long)0);
}


extern "C" void initTable(addFunction func) {
    func("sdl_init", (void*)func_sdl_init);
    func("sdl_init_size", (void*)func_sdl_init_size);
    func("sdl_quit", (void*)func_sdl_quit);
    func("sdl_pump", (void*)func_sdl_pump);
    func("sdl_release", (void*)func_sdl_release);
    func("sdl_create", (void*)func_sdl_create);
    func("sdl_setcolor", (void*)func_sdl_setcolor);
    func("sdl_fillrect", (void*)func_sdl_fillrect);
    func("sdl_flip", (void*)func_sdl_flip);
    func("sdl_clear", (void*)func_sdl_clear);
    func("sdl_keydown", (void*)func_sdl_keydown);
    func("sdl_getticks", (void*)func_sdl_getticks);
    func("sdl_loadtex", (void*)func_sdl_loadtex);
    func("sdl_destroytex", (void*)func_sdl_destroytex);
    func("sdl_copytex", (void*)func_sdl_copytex);
    func("sdl_settarget", (void*)func_sdl_settarget);
    func("sdl_cleartarget", (void*)func_sdl_cleartarget);
    func("sdl_printtext", (void*)func_sdl_printtext);
    func("sdl_settextcolor", (void*)func_sdl_settextcolor);
    func("sdl_drawline", (void*)func_sdl_drawline);
    func("sdl_drawcircle", (void*)func_sdl_drawcircle);
    func("sdl_mousedown", (void*)func_sdl_mousedown);
    func("sdl_mousex", (void*)func_sdl_mousex);
    func("sdl_mousey", (void*)func_sdl_mousey);
    func("sdl_messagebox", (void*)func_sdl_messagebox);
    func("sdl_delay", (void*)func_sdl_delay);
    func("sdl_setstartcolor", (void*)func_sdl_setstartcolor);
    func("sdl_setendcolor", (void*)func_sdl_setendcolor);
    func("sdl_draw_gradient", (void*)func_sdl_draw_gradient);
}