/**
 * @file sdl.c
 * @brief SDL2 module C implementation — window, renderer, texture, and event handling
 * @author Jared Bruni
 */
#include "mx_sdl.h"
#include "SDL.h"
#include "SDL_ttf.h"


static SDL_Event g_event;
static SDL_Window **g_windows = NULL;
static SDL_Renderer **g_renderers = NULL;
static SDL_Texture **g_textures = NULL;
static int64_t g_window_count = 0;
static int64_t g_renderer_count = 0;
static int64_t g_texture_count = 0;
static TTF_Font **g_fonts = NULL;
static int64_t g_font_count = 0;

static SDL_Texture **g_render_targets = NULL;
static int64_t *g_target_widths = NULL;
static int64_t *g_target_heights = NULL;
static int64_t g_render_target_count = 0;

int64_t init(void) {
    SDL_SetMainReady();
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) == 0) {
        return 0;
    }
    return 1;
}

void quit(void) {
    if (g_fonts) {
        for (int64_t i = 0; i < g_font_count; ++i) {
            if (g_fonts[i])
                TTF_CloseFont(g_fonts[i]);
        }
        free(g_fonts);
        g_fonts = NULL;
        g_font_count = 0;
    }

    if (g_target_widths) {
        free(g_target_widths);
        g_target_widths = NULL;
    }
    if (g_target_heights) {
        free(g_target_heights);
        g_target_heights = NULL;
    }
    g_render_target_count = 0;

    if (g_textures) {
        for (int64_t i = 0; i < g_texture_count; ++i) {
            if (g_textures[i])
                SDL_DestroyTexture(g_textures[i]);
            g_textures[i] = NULL;
        }
        free(g_textures);
        g_textures = NULL;
        g_texture_count = 0;
    }
    if (g_renderers) {
        for (int64_t i = 0; i < g_renderer_count; ++i) {
            if (g_renderers[i])
                SDL_DestroyRenderer(g_renderers[i]);
        }
        free(g_renderers);
        g_renderers = NULL;
        g_renderer_count = 0;
    }
    if (g_windows) {
        for (int64_t i = 0; i < g_window_count; ++i) {
            if (g_windows[i])
                SDL_DestroyWindow(g_windows[i]);
        }
        free(g_windows);
        g_windows = NULL;
        g_window_count = 0;
    }
    SDL_Quit();
}

int64_t create_window(const char *title, int64_t x, int64_t y, int64_t w, int64_t h, int64_t flags) {
    SDL_Window *window = SDL_CreateWindow(title, (int)x, (int)y, (int)w, (int)h, (Uint32)flags);
    if (!window)
        return -1;

    void *tmp = realloc(g_windows, sizeof(SDL_Window *) * (g_window_count + 1));
    if (!tmp) {
        SDL_DestroyWindow(window);
        return -1;
    }
    g_windows = tmp;
    g_windows[g_window_count] = window;
    SDL_Surface *surf = SDL_LoadBMP("icon.bmp");
    if (surf != NULL) {
        SDL_SetWindowIcon(window, surf);
        SDL_FreeSurface(surf);
    }
    return g_window_count++;
}

void destroy_window(int64_t window_id) {
    if (window_id >= 0 && window_id < g_window_count && g_windows[window_id]) {
        SDL_DestroyWindow(g_windows[window_id]);
        g_windows[window_id] = NULL;
    }
}

void set_window_icon(uint64_t window_id, const char *path) {
    SDL_Surface *surf = SDL_LoadBMP(path);
    if (!surf)
        return;
    SDL_SetWindowIcon(g_windows[window_id], surf);
    SDL_FreeSurface(surf);
}

int64_t create_renderer(int64_t window_id, int64_t index, int64_t flags) {
    if (window_id < 0 || window_id >= g_window_count || !g_windows[window_id])
        return -1;

    SDL_Renderer *renderer = SDL_CreateRenderer(g_windows[window_id], (int)index, (Uint32)flags);
    if (!renderer)
        return -1;

    void *tmp = realloc(g_renderers, sizeof(SDL_Renderer *) * (g_renderer_count + 1));
    if (!tmp) {
        SDL_DestroyRenderer(renderer);
        return -1;
    }
    g_renderers = tmp;
    g_renderers[g_renderer_count] = renderer;
    return g_renderer_count++;
}

void destroy_renderer(int64_t renderer_id) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_DestroyRenderer(g_renderers[renderer_id]);
        g_renderers[renderer_id] = NULL;
    }
}

int64_t create_render_target(int64_t renderer_id, int64_t width, int64_t height) {
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return -1;

    SDL_RenderSetLogicalSize(g_renderers[renderer_id], (int)width, (int)height);

    void *tmp2 = realloc(g_target_widths, sizeof(int64_t) * (g_render_target_count + 1));
    void *tmp3 = realloc(g_target_heights, sizeof(int64_t) * (g_render_target_count + 1));
    if (!tmp2 || !tmp3) {
        if (tmp2)
            g_target_widths = tmp2;
        if (tmp3)
            g_target_heights = tmp3;
        return -1;
    }
    g_target_widths = tmp2;
    g_target_heights = tmp3;

    g_target_widths[g_render_target_count] = width;
    g_target_heights[g_render_target_count] = height;

    SDL_SetRenderDrawColor(g_renderers[renderer_id], 0, 0, 0, 255);
    SDL_RenderClear(g_renderers[renderer_id]);

    return g_render_target_count++;
}

void set_render_target(int64_t renderer_id, int64_t target_id) {
    (void)renderer_id;
    (void)target_id;
}

void destroy_render_target(int64_t target_id) {
    (void)target_id;
}

int64_t poll_event(void) {
    return SDL_PollEvent(&g_event);
}

int64_t get_event_type(void) {
    return g_event.type;
}

int64_t get_key_code(void) {
    return g_event.key.keysym.sym;
}

int64_t get_mouse_x(void) {
    return g_event.motion.x;
}

int64_t get_mouse_y(void) {
    return g_event.motion.y;
}

int64_t get_mouse_button(void) {
    return g_event.button.button;
}

void set_draw_color(int64_t renderer_id, int64_t r, int64_t g, int64_t b, int64_t a) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_SetRenderDrawColor(g_renderers[renderer_id], (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
    }
}

void clear(int64_t renderer_id) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_RenderClear(g_renderers[renderer_id]);
    }
}

void present(int64_t renderer_id) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_RenderPresent(g_renderers[renderer_id]);
    }
}

void present_scaled(int64_t renderer_id, int64_t target_id, int64_t scale_width, int64_t scale_height) {
    (void)target_id;
    (void)scale_width;
    (void)scale_height;
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return;
    SDL_RenderPresent(g_renderers[renderer_id]);
}

void present_stretched(int64_t renderer_id, int64_t target_id, int64_t dst_width, int64_t dst_height) {
    (void)target_id;
    (void)dst_width;
    (void)dst_height;
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return;
    SDL_RenderPresent(g_renderers[renderer_id]);
}

void draw_point(int64_t renderer_id, int64_t x, int64_t y) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_RenderDrawPoint(g_renderers[renderer_id], (int)x, (int)y);
    }
}

void draw_line(int64_t renderer_id, int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_RenderDrawLine(g_renderers[renderer_id], (int)x1, (int)y1, (int)x2, (int)y2);
    }
}

void draw_rect(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_Rect rect = {(int)x, (int)y, (int)w, (int)h};
        SDL_RenderDrawRect(g_renderers[renderer_id], &rect);
    }
}

void fill_rect(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        SDL_Rect rect = {(int)x, (int)y, (int)w, (int)h};
        SDL_RenderFillRect(g_renderers[renderer_id], &rect);
    }
}

int64_t create_texture(int64_t renderer_id, int64_t format, int64_t access, int64_t w, int64_t h) {
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return -1;

    SDL_Texture *texture = SDL_CreateTexture(g_renderers[renderer_id], (Uint32)format, (int)access, (int)w, (int)h);
    if (!texture)
        return -1;

    void *tmp = realloc(g_textures, sizeof(SDL_Texture *) * (g_texture_count + 1));
    if (!tmp) {
        SDL_DestroyTexture(texture);
        return -1;
    }
    g_textures = tmp;
    g_textures[g_texture_count] = texture;
    return g_texture_count++;
}

void destroy_texture(int64_t texture_id) {
    if (texture_id >= 0 && texture_id < g_texture_count && g_textures[texture_id]) {
        SDL_DestroyTexture(g_textures[texture_id]);
        g_textures[texture_id] = NULL;
    }
}

/**
 * @brief Load a BMP file as an SDL texture with black color-key transparency.
 *
 * Loads the bitmap via SDL_LoadBMP, sets pure black (0,0,0) as the
 * transparent color key with SDL_SetColorKey, then creates an
 * accelerated texture from the surface.  The surface is freed after
 * texture creation.
 *
 * @param renderer_id  Index into the renderer array.
 * @param file_path    Path to the .bmp file.
 * @return Texture index on success, -1 on bad renderer, -2 on load failure,
 *         -3 on texture creation failure.
 */
int64_t load_texture(int64_t renderer_id, const char *file_path) {
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return -1;

    SDL_Surface *surface = SDL_LoadBMP(file_path);
    if (!surface)
        return -2;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderers[renderer_id], surface);
    SDL_FreeSurface(surface);
    if (!texture)
        return -3;

    void *tmp = realloc(g_textures, sizeof(SDL_Texture *) * (g_texture_count + 1));
    if (!tmp) {
        SDL_DestroyTexture(texture);
        return -1;
    }
    g_textures = tmp;
    g_textures[g_texture_count] = texture;

    return g_texture_count++;
}

int64_t load_texture_color_key(int64_t renderer_id, const char *file_path) {
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return -1;

    SDL_Surface *surface = SDL_LoadBMP(file_path);
    if (!surface)
        return -2;

    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, 0, 0, 0));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderers[renderer_id], surface);
    SDL_FreeSurface(surface);
    if (!texture)
        return -3;
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    void *tmp = realloc(g_textures, sizeof(SDL_Texture *) * (g_texture_count + 1));
    if (!tmp) {
        SDL_DestroyTexture(texture);
        return -1;
    }
    g_textures = tmp;
    g_textures[g_texture_count] = texture;

    return g_texture_count++;
}

/**
 * @brief Load a BMP file as an SDL texture with a custom RGB color-key transparency.
 *
 * Loads the bitmap via SDL_LoadBMP, sets the specified (r, g, b) color as the
 * transparent color key with SDL_SetColorKey, then creates an accelerated
 * texture from the surface.  The surface is freed after texture creation.
 *
 * @param renderer_id  Index into the renderer array.
 * @param file_path    Path to the .bmp file.
 * @param r            Red component of the color key (0–255).
 * @param g            Green component of the color key (0–255).
 * @param b            Blue component of the color key (0–255).
 * @return Texture index on success, -1 on bad renderer or allocation failure,
 *         -2 on load failure, -3 on texture creation failure.
 */
int64_t load_texture_color_key_rgb(int64_t renderer_id, const char *file_path, int64_t r, int64_t g, int64_t b) {
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return -1;

    SDL_Surface *surface = SDL_LoadBMP(file_path);
    if (!surface)
        return -2;

    SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, (Uint8)r, (Uint8)g, (Uint8)b));
    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderers[renderer_id], surface);
    SDL_FreeSurface(surface);
    if (!texture)
        return -3;
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    void *tmp = realloc(g_textures, sizeof(SDL_Texture *) * (g_texture_count + 1));
    if (!tmp) {
        SDL_DestroyTexture(texture);
        return -1;
    }
    g_textures = tmp;
    g_textures[g_texture_count] = texture;

    return g_texture_count++;
}

void render_texture(int64_t renderer_id, int64_t texture_id, int64_t src_x, int64_t src_y, int64_t src_w, int64_t src_h, int64_t dst_x, int64_t dst_y, int64_t dst_w, int64_t dst_h) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id] &&
        texture_id >= 0 && texture_id < g_texture_count && g_textures[texture_id]) {
        SDL_Renderer *renderer = g_renderers[renderer_id];
        SDL_Texture *texture = g_textures[texture_id];

        SDL_Rect *psrc = NULL;
        SDL_Rect src_rect;
        if (!((src_x == -1 || src_y == -1) && src_w == -1 && src_h == -1)) {
            src_rect.x = (int)src_x;
            src_rect.y = (int)src_y;
            src_rect.w = (int)src_w;
            src_rect.h = (int)src_h;
            psrc = &src_rect;
        }

        SDL_Rect *pdst = NULL;
        SDL_Rect dst_rect;
        if (!(dst_x == -1 && dst_y == -1 && dst_w == -1 && dst_h == -1)) {
            dst_rect.x = (int)dst_x;
            dst_rect.y = (int)dst_y;
            dst_rect.w = (int)dst_w;
            dst_rect.h = (int)dst_h;
            pdst = &dst_rect;
        }

        SDL_RenderCopy(renderer, texture, psrc, pdst);
    }
}

int64_t update_texture(int64_t texture_id, const void *pixels, int64_t pitch) {
    if (texture_id >= 0 && texture_id < g_texture_count && g_textures[texture_id]) {
        return SDL_UpdateTexture(g_textures[texture_id], NULL, pixels, (int)pitch) == 0 ? 1 : 0;
    }
    return 0;
}

int64_t lock_texture(int64_t texture_id, void **pixels, int64_t *pitch) {
    if (texture_id >= 0 && texture_id < g_texture_count && g_textures[texture_id]) {
        int p;
        int result = SDL_LockTexture(g_textures[texture_id], NULL, pixels, &p);
        *pitch = p;
        return result == 0 ? 1 : 0;
    }
    return 0;
}

void unlock_texture(int64_t texture_id) {
    if (texture_id >= 0 && texture_id < g_texture_count && g_textures[texture_id]) {
        SDL_UnlockTexture(g_textures[texture_id]);
    }
}

int64_t get_ticks(void) {
    return SDL_GetTicks();
}

void delay(int64_t ms) {
    SDL_Delay((Uint32)ms);
}

int64_t open_audio(int64_t freq, int64_t format, int64_t channels, int64_t samples) {
    SDL_AudioSpec wanted_spec;
    wanted_spec.freq = (int)freq;
    wanted_spec.format = (SDL_AudioFormat)format;
    wanted_spec.channels = (Uint8)channels;
    wanted_spec.samples = (Uint16)samples;
    wanted_spec.callback = NULL;
    wanted_spec.userdata = NULL;

    return SDL_OpenAudio(&wanted_spec, NULL) == 0 ? 1 : 0;
}

void close_audio(void) {
    SDL_CloseAudio();
}

void pause_audio(int64_t pause_on) {
    SDL_PauseAudio(pause_on ? 1 : 0);
}

int64_t load_wav(const char *file_path, int64_t *audio_buf, int64_t *audio_len, int64_t *audio_spec) {
    SDL_AudioSpec *spec = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
    if (!spec)
        return 0;
    Uint8 *buf;
    Uint32 len;

    if (SDL_LoadWAV(file_path, spec, &buf, &len) == NULL) {
        free(spec);
        return 0;
    }

    *audio_buf = (int64_t)buf;
    *audio_len = len;
    *audio_spec = (int64_t)spec;
    return 1;
}

void free_wav(int64_t audio_buf) {
    if (audio_buf) {
        SDL_FreeWAV((Uint8 *)audio_buf);
    }
}

int64_t queue_audio(const void *data, int64_t len) {
    return SDL_QueueAudio(1, data, (Uint32)len) == 0 ? 1 : 0;
}

int64_t get_queued_audio_size(void) {
    return SDL_GetQueuedAudioSize(1);
}

void clear_queued_audio(void) {
    SDL_ClearQueuedAudio(1);
}

int64_t get_mouse_buttons(void) {
    int x, y;
    return SDL_GetMouseState(&x, &y);
}

int64_t get_relative_mouse_x(void) {
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    return x;
}

int64_t get_relative_mouse_y(void) {
    int x, y;
    SDL_GetRelativeMouseState(&x, &y);
    return y;
}

int64_t get_relative_mouse_buttons(void) {
    int x, y;
    return SDL_GetRelativeMouseState(&x, &y);
}

int64_t get_keyboard_state(int64_t *numkeys) {
    int nk;
    const Uint8 *state = SDL_GetKeyboardState(&nk);
    *numkeys = nk;
    return (int64_t)state;
}

int64_t is_key_pressed(int64_t scancode) {
    int numkeys;
    const Uint8 *state = SDL_GetKeyboardState(&numkeys);
    if (scancode < 0 || scancode >= numkeys)
        return 0;
    return state[scancode] ? 1 : 0;
}

int64_t get_num_keys(void) {
    int numkeys;
    SDL_GetKeyboardState(&numkeys);
    return numkeys;
}

void set_clipboard_text(const char *text) {
    SDL_SetClipboardText(text);
}

const char *get_clipboard_text(void) {
    static char *last_clipboard = NULL;
    if (last_clipboard)
        SDL_free(last_clipboard);
    last_clipboard = SDL_GetClipboardText();
    return last_clipboard;
}

void set_window_title(int64_t window_id, const char *title) {
    if (window_id >= 0 && window_id < g_window_count && g_windows[window_id]) {
        SDL_SetWindowTitle(g_windows[window_id], title);
    }
}

void set_window_position(int64_t window_id, int64_t x, int64_t y) {
    if (window_id >= 0 && window_id < g_window_count && g_windows[window_id]) {
        SDL_SetWindowPosition(g_windows[window_id], (int)x, (int)y);
    }
}

void get_window_size(int64_t window_id, int64_t *w, int64_t *h) {
    if (window_id >= 0 && window_id < g_window_count && g_windows[window_id]) {
        int ww, hh;
        SDL_GetWindowSize(g_windows[window_id], &ww, &hh);
        *w = ww;
        *h = hh;
    }
}

void set_window_fullscreen(int64_t window_id, int64_t fullscreen) {
    if (window_id >= 0 && window_id < g_window_count && g_windows[window_id]) {
        SDL_SetWindowFullscreen(g_windows[window_id], fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    }
}

void get_renderer_output_size(int64_t renderer_id, int64_t *w, int64_t *h) {
    if (renderer_id >= 0 && renderer_id < g_renderer_count && g_renderers[renderer_id]) {
        int ww, hh;
        SDL_GetRendererOutputSize(g_renderers[renderer_id], &ww, &hh);
        *w = ww;
        *h = hh;
    }
}

void show_cursor(int64_t show) {
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

int64_t init_text(void) {
    return TTF_Init() == 0 ? 1 : 0;
}

void quit_text(void) {
    if (g_fonts) {
        for (int64_t i = 0; i < g_font_count; ++i) {
            if (g_fonts[i])
                TTF_CloseFont(g_fonts[i]);
        }
        free(g_fonts);
        g_fonts = NULL;
        g_font_count = 0;
    }
    TTF_Quit();
}

int64_t load_font(const char *file, int64_t ptsize) {
    TTF_Font *font = TTF_OpenFont(file, (int)ptsize);
    if (!font)
        return -1;
    void *tmp = realloc(g_fonts, sizeof(TTF_Font *) * (g_font_count + 1));
    if (!tmp) {
        TTF_CloseFont(font);
        return -1;
    }
    g_fonts = tmp;
    g_fonts[g_font_count] = font;
    return g_font_count++;
}

void draw_text(int64_t renderer_id, int64_t font_id, const char *text, int64_t x, int64_t y, int64_t r, int64_t g, int64_t b, int64_t a) {
    if (renderer_id < 0 || renderer_id >= g_renderer_count || !g_renderers[renderer_id])
        return;
    if (font_id < 0 || font_id >= g_font_count || !g_fonts[font_id])
        return;

    SDL_Color color = {(Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a};
    SDL_Surface *surface = TTF_RenderUTF8_Blended(g_fonts[font_id], text, color);
    if (!surface)
        return;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(g_renderers[renderer_id], surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect dst = {(int)x, (int)y, surface->w, surface->h};
    SDL_RenderCopy(g_renderers[renderer_id], texture, NULL, &dst);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

int64_t create_rgb_surface(int64_t width, int64_t height, int64_t depth) {
    SDL_Surface *s = NULL;
    int w = (int)width, h = (int)height, d = (int)depth;

    if (d == 32)
        s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_RGBA32);
    else if (d == 24)
        s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 24, SDL_PIXELFORMAT_RGB24);
    else if (d == 16)
        s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 16, SDL_PIXELFORMAT_RGB565);
    else
        return 0;

    return (int64_t)s;
}

void free_surface(int64_t surf_ptr) {
    if (surf_ptr)
        SDL_FreeSurface((SDL_Surface *)surf_ptr);
}

int64_t blit_surface(int64_t src_ptr, int64_t dst_ptr, int64_t x, int64_t y) {
    if (!src_ptr || !dst_ptr)
        return 0;
    SDL_Surface *src = (SDL_Surface *)src_ptr;
    SDL_Surface *dst = (SDL_Surface *)dst_ptr;
    SDL_Rect dst_rc = {(int)x, (int)y, 0, 0};
    return SDL_BlitSurface(src, NULL, dst, &dst_rc) == 0 ? 1 : 0;
}

int64_t get_mouse_state(int64_t *x, int64_t *y) {
    int ix, iy;
    Uint32 m = SDL_GetMouseState(&ix, &iy);
    if (x)
        *x = ix;
    if (y)
        *y = iy;
    return (int64_t)m;
}

int64_t get_relative_mouse_state(int64_t *x, int64_t *y) {
    int ix, iy;
    Uint32 m = SDL_GetRelativeMouseState(&ix, &iy);
    if (x)
        *x = ix;
    if (y)
        *y = iy;
    return (int64_t)m;
}
