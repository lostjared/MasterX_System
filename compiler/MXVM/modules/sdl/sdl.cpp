/**
 * @file sdl.cpp
 * @brief SDL2 module C++ runtime bindings for the MXVM interpreter
 * @author Jared Bruni
 */
#include "mx_sdl.h"
#include <cstdint>
#include <mxvm/icode.hpp>
#include <mxvm/instruct.hpp>

extern "C" void mxvm_sdl_init(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = init();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_Quit
extern "C" void mxvm_sdl_quit(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    quit();
}

// SDL_CreateWindow
extern "C" void mxvm_sdl_create_window(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 6) {
        throw mx::Exception("sdl_create_window requires 6 arguments (title, x, y, w, h, flags).");
    }

    std::string title;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            title = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            title = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        title = operand[0].op;
    }

    int64_t x = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t y = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t w = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t h = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;
    int64_t flags = program->isVariable(operand[5].op) ? program->getVariable(operand[5].op).var_value.int_value : operand[5].op_value;

    int64_t result = create_window(title.c_str(), x, y, w, h, flags);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_DestroyWindow
extern "C" void mxvm_sdl_destroy_window(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_destroy_window requires 1 argument (window_id).");
    }

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    destroy_window(window_id);
}

// SDL_CreateRenderer
extern "C" void mxvm_sdl_create_renderer(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_create_renderer requires 3 arguments (window_id, index, flags).");
    }

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t index = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t flags = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;

    int64_t result = create_renderer(window_id, index, flags);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_DestroyRenderer
extern "C" void mxvm_sdl_destroy_renderer(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_destroy_renderer requires 1 argument (renderer_id).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    destroy_renderer(renderer_id);
}

// Window management functions
extern "C" void mxvm_sdl_set_window_title(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_set_window_title requires 2 arguments (window_id, title).");
    }

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    std::string title;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            title = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            title = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        title = operand[1].op;
    }

    set_window_title(window_id, title.c_str());
}

extern "C" void mxvm_sdl_set_window_position(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_set_window_position requires 3 arguments (window_id, x, y).");
    }

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t x = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t y = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;

    set_window_position(window_id, x, y);
}

extern "C" void mxvm_sdl_get_window_size(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_get_window_size requires 3 arguments (window_id, w_var, h_var).");
    }

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t w, h;
    get_window_size(window_id, &w, &h);

    // Store width in the second operand variable
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &w_var = program->getVariable(operand[1].op);
        w_var.type = mxvm::VarType::VAR_INTEGER;
        w_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        w_var.var_value.int_value = w;
    }

    // Store height in the third operand variable
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &h_var = program->getVariable(operand[2].op);
        h_var.type = mxvm::VarType::VAR_INTEGER;
        h_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        h_var.var_value.int_value = h;
    }
}

extern "C" void mxvm_sdl_set_window_fullscreen(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_set_window_fullscreen requires 2 arguments (window_id, fullscreen).");
    }

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t fullscreen = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;

    set_window_fullscreen(window_id, fullscreen);
}

extern "C" void mxvm_sdl_get_renderer_output_size(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_get_renderer_output_size requires 3 arguments (renderer_id, w_var, h_var).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t w, h;
    get_renderer_output_size(renderer_id, &w, &h);

    // Store width in the second operand variable
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &w_var = program->getVariable(operand[1].op);
        w_var.type = mxvm::VarType::VAR_INTEGER;
        w_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        w_var.var_value.int_value = w;
    }

    // Store height in the third operand variable
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &h_var = program->getVariable(operand[2].op);
        h_var.type = mxvm::VarType::VAR_INTEGER;
        h_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        h_var.var_value.int_value = h;
    }
}

// Mouse functions
extern "C" void mxvm_sdl_get_mouse_buttons(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_mouse_buttons();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_relative_mouse_x(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_relative_mouse_x();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_relative_mouse_y(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_relative_mouse_y();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_relative_mouse_buttons(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_relative_mouse_buttons();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_mouse_state(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_get_mouse_state requires 2 arguments (x_var, y_var).");
    }

    int64_t x, y;
    int64_t result = get_mouse_state(&x, &y);

    // Store x in the first operand variable
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &x_var = program->getVariable(operand[0].op);
        x_var.type = mxvm::VarType::VAR_INTEGER;
        x_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        x_var.var_value.int_value = x;
    }

    // Store y in the second operand variable
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &y_var = program->getVariable(operand[1].op);
        y_var.type = mxvm::VarType::VAR_INTEGER;
        y_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        y_var.var_value.int_value = y;
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_relative_mouse_state(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_get_relative_mouse_state requires 2 arguments (x_var, y_var).");
    }

    int64_t x, y;
    int64_t result = get_relative_mouse_state(&x, &y);

    // Store x in the first operand variable
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &x_var = program->getVariable(operand[0].op);
        x_var.type = mxvm::VarType::VAR_INTEGER;
        x_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        x_var.var_value.int_value = x;
    }

    // Store y in the second operand variable
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &y_var = program->getVariable(operand[1].op);
        y_var.type = mxvm::VarType::VAR_INTEGER;
        y_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        y_var.var_value.int_value = y;
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// Keyboard functions
extern "C" void mxvm_sdl_get_keyboard_state(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_get_keyboard_state requires 1 argument (numkeys_var).");
    }

    int64_t numkeys;
    int64_t result = get_keyboard_state(&numkeys);

    // Store numkeys in the operand variable
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &nk_var = program->getVariable(operand[0].op);
        nk_var.type = mxvm::VarType::VAR_INTEGER;
        nk_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        nk_var.var_value.int_value = numkeys;
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = (void *)result;
}

extern "C" void mxvm_sdl_is_key_pressed(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_is_key_pressed requires 1 argument (scancode).");
    }

    int64_t scancode = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    int64_t result = is_key_pressed(scancode);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_num_keys(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_num_keys();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// Clipboard functions
extern "C" void mxvm_sdl_set_clipboard_text(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_set_clipboard_text requires 1 argument (text).");
    }

    std::string text;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            text = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            text = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        text = operand[0].op;
    }

    set_clipboard_text(text.c_str());
}

extern "C" void mxvm_sdl_get_clipboard_text(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    const char *result = get_clipboard_text();
    // Store result as a string pointer in %rax
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = (void *)result;
}

// Cursor functions
extern "C" void mxvm_sdl_show_cursor(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_show_cursor requires 1 argument (show).");
    }

    int64_t show = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    show_cursor(show);
}

// Surface functions
extern "C" void mxvm_sdl_create_rgb_surface(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_create_rgb_surface requires 3 arguments (width, height, depth).");
    }

    int64_t width = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t height = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t depth = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;

    int64_t result = create_rgb_surface(width, height, depth);
    program->vars["%rax"].type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_POINTER;
    program->vars["%rax"].var_value.ptr_value = (void *)result;
}

extern "C" void mxvm_sdl_free_surface(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_free_surface requires 1 argument (surface_ptr).");
    }

    int64_t surf_ptr = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    free_surface(surf_ptr);
}

extern "C" void mxvm_sdl_blit_surface(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("sdl_blit_surface requires 4 arguments (src_ptr, dst_ptr, x, y).");
    }

    int64_t src_ptr = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t dst_ptr = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t x = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t y = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;

    int64_t result = blit_surface(src_ptr, dst_ptr, x, y);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// Additional functions
extern "C" void mxvm_sdl_free_wav(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_free_wav requires 1 argument (audio_buf).");
    }

    int64_t audio_buf = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    free_wav(audio_buf);
}

// SDL_PollEvent
extern "C" void mxvm_sdl_poll_event(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = poll_event();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_GetEventType
extern "C" void mxvm_sdl_get_event_type(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_event_type();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_GetKeyCode
extern "C" void mxvm_sdl_get_key_code(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_key_code();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_GetMouseX
extern "C" void mxvm_sdl_get_mouse_x(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_mouse_x();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_GetMouseY
extern "C" void mxvm_sdl_get_mouse_y(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_mouse_y();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_GetMouseButton
extern "C" void mxvm_sdl_get_mouse_button(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_mouse_button();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_SetDrawColor
extern "C" void mxvm_sdl_set_draw_color(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5) {
        throw mx::Exception("sdl_set_draw_color requires 5 arguments (renderer_id, r, g, b, a).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t r = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t g = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t b = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t a = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;

    set_draw_color(renderer_id, r, g, b, a);
}

// SDL_Clear
extern "C" void mxvm_sdl_clear(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_clear requires 1 argument (renderer_id).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    clear(renderer_id);
}

// SDL_Present
extern "C" void mxvm_sdl_present(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_present requires 1 argument (renderer_id).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    present(renderer_id);
}

// SDL_DrawPoint
extern "C" void mxvm_sdl_draw_point(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_draw_point requires 3 arguments (renderer_id, x, y).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t x = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t y = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;

    draw_point(renderer_id, x, y);
}

// SDL_DrawLine
extern "C" void mxvm_sdl_draw_line(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5) {
        throw mx::Exception("sdl_draw_line requires 5 arguments (renderer_id, x1, y1, x2, y2).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t x1 = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t y1 = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t x2 = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t y2 = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;

    draw_line(renderer_id, x1, y1, x2, y2);
}

// SDL_DrawRect
extern "C" void mxvm_sdl_draw_rect(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5) {
        throw mx::Exception("sdl_draw_rect requires 5 arguments (renderer_id, x, y, w, h).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t x = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t y = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t w = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t h = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;

    draw_rect(renderer_id, x, y, w, h);
}

// SDL_FillRect
extern "C" void mxvm_sdl_fill_rect(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5) {
        throw mx::Exception("sdl_fill_rect requires 5 arguments (renderer_id, x, y, w, h).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t x = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t y = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t w = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t h = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;

    fill_rect(renderer_id, x, y, w, h);
}

// SDL_CreateTexture
extern "C" void mxvm_sdl_create_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5) {
        throw mx::Exception("sdl_create_texture requires 5 arguments (renderer_id, format, access, w, h).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t format = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t access = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t w = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t h = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;

    int64_t result = create_texture(renderer_id, format, access, w, h);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_DestroyTexture
extern "C" void mxvm_sdl_destroy_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_destroy_texture requires 1 argument (texture_id).");
    }

    int64_t texture_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    destroy_texture(texture_id);
}

// SDL_LoadTexture
extern "C" void mxvm_sdl_load_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_load_texture requires 2 arguments (renderer_id, file_path).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    std::string file_path;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            file_path = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            file_path = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        file_path = operand[1].op;
    }

    int64_t result = load_texture(renderer_id, file_path.c_str());
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_LoadTextureColorKey
extern "C" void mxvm_sdl_load_texture_color_key(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_load_texture_color_key requires 2 arguments (renderer_id, file_path).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    std::string file_path;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            file_path = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            file_path = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        file_path = operand[1].op;
    }

    int64_t result = load_texture_color_key(renderer_id, file_path.c_str());
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

/**
 * @brief MXVM binding for load_texture_color_key_rgb.
 *
 * Loads a BMP texture with a caller-specified RGB color key for transparency.
 * Expects 5 operands: renderer_id, file_path, r, g, b.
 * Returns the texture ID in %%rax.
 *
 * @param program  Pointer to the running MXVM program.
 * @param operand  Vector of 5 operands.
 */
extern "C" void mxvm_sdl_load_texture_color_key_rgb(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 5) {
        throw mx::Exception("sdl_load_texture_color_key_rgb requires 5 arguments (renderer_id, file_path, r, g, b).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    std::string file_path;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            file_path = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            file_path = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        file_path = operand[1].op;
    }

    int64_t r = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t g = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t b = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;

    int64_t result = load_texture_color_key_rgb(renderer_id, file_path.c_str(), r, g, b);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

// SDL_RenderTexture
extern "C" void mxvm_sdl_render_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 10) {
        throw mx::Exception("sdl_render_texture requires 10 arguments (renderer_id, texture_id, src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t texture_id = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t src_x = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t src_y = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t src_w = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;
    int64_t src_h = program->isVariable(operand[5].op) ? program->getVariable(operand[5].op).var_value.int_value : operand[5].op_value;
    int64_t dst_x = program->isVariable(operand[6].op) ? program->getVariable(operand[6].op).var_value.int_value : operand[6].op_value;
    int64_t dst_y = program->isVariable(operand[7].op) ? program->getVariable(operand[7].op).var_value.int_value : operand[7].op_value;
    int64_t dst_w = program->isVariable(operand[8].op) ? program->getVariable(operand[8].op).var_value.int_value : operand[8].op_value;
    int64_t dst_h = program->isVariable(operand[9].op) ? program->getVariable(operand[9].op).var_value.int_value : operand[9].op_value;

    render_texture(renderer_id, texture_id, src_x, src_y, src_w, src_h, dst_x, dst_y, dst_w, dst_h);
}

// Timing functions
extern "C" void mxvm_sdl_get_ticks(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_ticks();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_delay(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_delay requires 1 argument (ms).");
    }

    int64_t ms = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    delay(ms);
}

// Audio functions
extern "C" void mxvm_sdl_open_audio(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("sdl_open_audio requires 4 arguments (freq, format, channels, samples).");
    }

    int64_t frequency = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t format = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t channels = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t samples = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;

    int64_t result = open_audio(frequency, format, channels, samples);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_close_audio(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    close_audio();
}

extern "C" void mxvm_sdl_pause_audio(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_pause_audio requires 1 argument (pause_on).");
    }

    int64_t pause_on = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    pause_audio(pause_on);
}

extern "C" void mxvm_sdl_load_wav(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("sdl_load_wav requires 4 arguments (file_path, audio_buf_var, audio_len_var, audio_spec_var).");
    }

    std::string file_path;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            file_path = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            file_path = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        file_path = operand[0].op;
    }

    int64_t audio_buf, audio_len, audio_spec;
    int64_t result = load_wav(file_path.c_str(), &audio_buf, &audio_len, &audio_spec);

    // Store results in variables
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &buf_var = program->getVariable(operand[1].op);
        buf_var.type = mxvm::VarType::VAR_INTEGER;
        buf_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        buf_var.var_value.int_value = audio_buf;
    }

    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &len_var = program->getVariable(operand[2].op);
        len_var.type = mxvm::VarType::VAR_INTEGER;
        len_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        len_var.var_value.int_value = audio_len;
    }

    if (program->isVariable(operand[3].op)) {
        mxvm::Variable &spec_var = program->getVariable(operand[3].op);
        spec_var.type = mxvm::VarType::VAR_INTEGER;
        spec_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        spec_var.var_value.int_value = audio_spec;
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_queue_audio(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_queue_audio requires 2 arguments (data, len).");
    }

    void *data = nullptr;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_POINTER) {
            data = var.var_value.ptr_value;
        }
    } else {
        data = reinterpret_cast<void *>(static_cast<uintptr_t>(operand[0].op_value));
    }

    int64_t len = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;

    int64_t result = queue_audio(data, len);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_get_queued_audio_size(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = get_queued_audio_size();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_clear_queued_audio(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    clear_queued_audio();
}

// Texture functions
extern "C" void mxvm_sdl_update_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_update_texture requires 3 arguments (texture_id, pixels, pitch).");
    }

    int64_t texture_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    void *pixels = nullptr;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        if (var.type == mxvm::VarType::VAR_POINTER) {
            pixels = var.var_value.ptr_value;
        }
    } else {
        pixels = reinterpret_cast<void *>(static_cast<uintptr_t>(operand[1].op_value));
    }

    int64_t pitch = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;

    int64_t result = update_texture(texture_id, pixels, pitch);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_lock_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_lock_texture requires 3 arguments (texture_id, pixels_var, pitch_var).");
    }

    int64_t texture_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    void *pixels;
    int64_t pitch;
    int64_t result = lock_texture(texture_id, &pixels, &pitch);

    // Store results in variables
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &pixels_var = program->getVariable(operand[1].op);
        pixels_var.type = mxvm::VarType::VAR_POINTER;
        pixels_var.var_value.type = mxvm::VarType::VAR_POINTER;
        pixels_var.var_value.ptr_value = pixels;
    }

    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &pitch_var = program->getVariable(operand[2].op);
        pitch_var.type = mxvm::VarType::VAR_INTEGER;
        pitch_var.var_value.type = mxvm::VarType::VAR_INTEGER;
        pitch_var.var_value.int_value = pitch;
    }

    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_unlock_texture(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_unlock_texture requires 1 argument (texture_id).");
    }

    int64_t texture_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    unlock_texture(texture_id);
}

extern "C" void mxvm_sdl_init_text(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    int64_t result = init_text();
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_quit_text(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    quit_text();
}

extern "C" void mxvm_sdl_load_font(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_load_font requires 2 arguments (file, ptsize).");
    }
    std::string file;
    if (program->isVariable(operand[0].op)) {
        mxvm::Variable &var = program->getVariable(operand[0].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            file = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            file = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        file = operand[0].op;
    }
    int64_t ptsize = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;

    int64_t result = load_font(file.c_str(), ptsize);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_draw_text(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 9) {
        throw mx::Exception("sdl_draw_text requires 9 arguments (renderer_id, font_id, text, x, y, r, g, b, a).");
    }
    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t font_id = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;

    std::string text;
    if (program->isVariable(operand[2].op)) {
        mxvm::Variable &var = program->getVariable(operand[2].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            text = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            text = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        text = operand[2].op;
    }

    int64_t x = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;
    int64_t y = program->isVariable(operand[4].op) ? program->getVariable(operand[4].op).var_value.int_value : operand[4].op_value;
    int64_t r = program->isVariable(operand[5].op) ? program->getVariable(operand[5].op).var_value.int_value : operand[5].op_value;
    int64_t g = program->isVariable(operand[6].op) ? program->getVariable(operand[6].op).var_value.int_value : operand[6].op_value;
    int64_t b = program->isVariable(operand[7].op) ? program->getVariable(operand[7].op).var_value.int_value : operand[7].op_value;
    int64_t a = program->isVariable(operand[8].op) ? program->getVariable(operand[8].op).var_value.int_value : operand[8].op_value;

    draw_text(renderer_id, font_id, text.c_str(), x, y, r, g, b, a);
}

extern "C" void mxvm_sdl_create_render_target(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 3) {
        throw mx::Exception("sdl_create_render_target requires 3 arguments (renderer_id, width, height).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t width = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t height = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;

    int64_t result = create_render_target(renderer_id, width, height);
    program->vars["%rax"].type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.type = mxvm::VarType::VAR_INTEGER;
    program->vars["%rax"].var_value.int_value = result;
}

extern "C" void mxvm_sdl_set_render_target(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 2) {
        throw mx::Exception("sdl_set_render_target requires 2 arguments (renderer_id, target_id).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t target_id = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;

    set_render_target(renderer_id, target_id);
}

extern "C" void mxvm_sdl_destroy_render_target(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 1) {
        throw mx::Exception("sdl_destroy_render_target requires 1 argument (target_id).");
    }

    int64_t target_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    destroy_render_target(target_id);
}

extern "C" void mxvm_sdl_present_scaled(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("sdl_present_scaled requires 4 arguments (renderer_id, target_id, src_width, src_height).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t target_id = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t src_w = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t src_h = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;

    present_scaled(renderer_id, target_id, src_w, src_h);
}

extern "C" void mxvm_sdl_present_stretched(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {
    if (operand.size() != 4) {
        throw mx::Exception("sdl_present_stretched requires 4 arguments (renderer_id, target_id, dst_width, dst_height).");
    }

    int64_t renderer_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;
    int64_t target_id = program->isVariable(operand[1].op) ? program->getVariable(operand[1].op).var_value.int_value : operand[1].op_value;
    int64_t dst_w = program->isVariable(operand[2].op) ? program->getVariable(operand[2].op).var_value.int_value : operand[2].op_value;
    int64_t dst_h = program->isVariable(operand[3].op) ? program->getVariable(operand[3].op).var_value.int_value : operand[3].op_value;

    present_stretched(renderer_id, target_id, dst_w, dst_h);
}

extern "C" void mxvm_sdl_set_window_icon(mxvm::Program *program, std::vector<mxvm::Operand> &operand) {

    int64_t window_id = program->isVariable(operand[0].op) ? program->getVariable(operand[0].op).var_value.int_value : operand[0].op_value;

    std::string path;
    if (program->isVariable(operand[1].op)) {
        mxvm::Variable &var = program->getVariable(operand[1].op);
        if (var.type == mxvm::VarType::VAR_STRING) {
            path = var.var_value.str_value;
        } else if (var.type == mxvm::VarType::VAR_POINTER) {
            path = std::string(reinterpret_cast<const char *>(var.var_value.ptr_value));
        }
    } else {
        path = operand[0].op;
    }

    set_window_icon(window_id, path.c_str());
}