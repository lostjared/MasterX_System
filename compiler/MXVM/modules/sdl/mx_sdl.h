/**
 * @file mx_sdl.h
 * @brief SDL2 module C API — function declarations for SDL2 bindings exported to MXVM programs
 * @author Jared Bruni
 */
#ifndef __SDL2__H_
#define __SDL2__H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @name Core SDL functions */
/** @{ */

/** @brief Initialize SDL2 subsystems
 * @return 0 on success, negative on failure
 */
int64_t init(void);

/** @brief Shut down SDL2 and release all resources */
void quit(void);
/** @} */

/** @name Window management */
/** @{ */

/** @brief Create an SDL window
 * @param title Window title
 * @param x Initial x position
 * @param y Initial y position
 * @param w Width in pixels
 * @param h Height in pixels
 * @param flags SDL window flags
 * @return Window ID handle
 */
int64_t create_window(const char *title, int64_t x, int64_t y, int64_t w, int64_t h, int64_t flags);

/** @brief Destroy a window by ID */
void destroy_window(int64_t window_id);

/** @brief Set the title of a window */
void set_window_title(int64_t window_id, const char *title);

/** @brief Set the position of a window */
void set_window_position(int64_t window_id, int64_t x, int64_t y);

/** @brief Get the size of a window
 * @param[out] w Receives width
 * @param[out] h Receives height
 */
void get_window_size(int64_t window_id, int64_t *w, int64_t *h);

/** @brief Set or clear fullscreen mode for a window */
void set_window_fullscreen(int64_t window_id, int64_t fullscreen);

/** @brief Set the window icon from an image file */
void set_window_icon(uint64_t window_id, const char *path);
/** @} */
/** @name Renderer management */
/** @{ */

/** @brief Create a hardware-accelerated renderer for a window
 * @param window_id Target window
 * @param index Rendering driver index (-1 for first available)
 * @param flags SDL renderer flags
 * @return Renderer ID handle
 */
int64_t create_renderer(int64_t window_id, int64_t index, int64_t flags);

/** @brief Destroy a renderer by ID */
void destroy_renderer(int64_t renderer_id);

/** @brief Set the current draw color (RGBA) */
void set_draw_color(int64_t renderer_id, int64_t r, int64_t g, int64_t b, int64_t a);

/** @brief Clear the renderer with the current draw color */
void clear(int64_t renderer_id);

/** @brief Present the renderer's back buffer to the screen */
void present(int64_t renderer_id);

/** @brief Get the renderer output size
 * @param[out] w Receives width
 * @param[out] h Receives height
 */
void get_renderer_output_size(int64_t renderer_id, int64_t *w, int64_t *h);
/** @} */

/** @name Drawing functions */
/** @{ */

/** @brief Draw a single pixel */
void draw_point(int64_t renderer_id, int64_t x, int64_t y);

/** @brief Draw a line between two points */
void draw_line(int64_t renderer_id, int64_t x1, int64_t y1, int64_t x2, int64_t y2);

/** @brief Draw a rectangle outline */
void draw_rect(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h);

/** @brief Draw a filled rectangle */
void fill_rect(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h);
/** @} */

/** @name Event handling */
/** @{ */

/** @brief Poll for a pending SDL event
 * @return 1 if an event was available, 0 otherwise
 */
int64_t poll_event(void);

/** @brief Get the type of the last polled event */
int64_t get_event_type(void);

/** @brief Get the key code from the last keyboard event */
int64_t get_key_code(void);

/** @brief Get the mouse x coordinate from the last mouse event */
int64_t get_mouse_x(void);

/** @brief Get the mouse y coordinate from the last mouse event */
int64_t get_mouse_y(void);

/** @brief Get the mouse button from the last mouse event */
int64_t get_mouse_button(void);
/** @} */

/** @name Surface functions */
/** @{ */

/** @brief Create an RGB surface
 * @return Surface pointer cast to int64_t
 */
int64_t create_rgb_surface(int64_t width, int64_t height, int64_t depth);

/** @brief Free a surface */
void free_surface(int64_t surf_ptr);

/** @brief Blit a source surface onto a destination surface at (x, y) */
int64_t blit_surface(int64_t src_ptr, int64_t dst_ptr, int64_t x, int64_t y);
/** @} */

/** @name Mouse state functions */
/** @{ */

/** @brief Get the current mouse state with pointer coordinates
 * @param[out] x Receives mouse x position
 * @param[out] y Receives mouse y position
 * @return Button bitmask
 */
int64_t get_mouse_state(int64_t *x, int64_t *y);

/** @brief Get the relative mouse state
 * @param[out] x Receives relative x delta
 * @param[out] y Receives relative y delta
 * @return Button bitmask
 */
int64_t get_relative_mouse_state(int64_t *x, int64_t *y);

/** @brief Get the current mouse button bitmask (pointerless variant) */
int64_t get_mouse_buttons(void);

/** @brief Get the relative mouse x delta (pointerless variant) */
int64_t get_relative_mouse_x(void);

/** @brief Get the relative mouse y delta (pointerless variant) */
int64_t get_relative_mouse_y(void);

/** @brief Get the relative mouse button bitmask (pointerless variant) */
int64_t get_relative_mouse_buttons(void);
/** @} */

/** @name Keyboard state functions */
/** @{ */

/** @brief Get the keyboard state array
 * @param[out] numkeys Receives number of keys
 * @return Pointer to key state array cast to int64_t
 */
int64_t get_keyboard_state(int64_t *numkeys);

/** @brief Check whether a specific key is currently pressed
 * @param scancode SDL scancode
 * @return Non-zero if pressed
 */
int64_t is_key_pressed(int64_t scancode);

/** @brief Get the total number of keyboard keys */
int64_t get_num_keys(void);
/** @} */

/** @name Clipboard functions */
/** @{ */

/** @brief Set the system clipboard text */
void set_clipboard_text(const char *text);

/** @brief Get the system clipboard text */
const char *get_clipboard_text(void);
/** @} */

/** @name Cursor functions */
/** @{ */

/** @brief Show or hide the mouse cursor */
void show_cursor(int64_t show);
/** @} */

/** @name Texture functions */
/** @{ */

/** @brief Create a blank texture
 * @param renderer_id Renderer handle
 * @param format Pixel format
 * @param access Texture access pattern (static, streaming, target)
 * @param w Width
 * @param h Height
 * @return Texture ID handle
 */
int64_t create_texture(int64_t renderer_id, int64_t format, int64_t access, int64_t w, int64_t h);

/** @brief Destroy a texture */
void destroy_texture(int64_t texture_id);

/** @brief Load a texture from an image file
 * @param renderer_id Renderer handle
 * @param file_path Path to the image file
 * @return Texture ID handle
 */
int64_t load_texture(int64_t renderer_id, const char *file_path);

/** @brief Load a BMP file as a texture with black color-key transparency */
int64_t load_texture_color_key(int64_t renderer_id, const char *file_path);

/** @brief Load a BMP file as a texture with a custom RGB color-key transparency */
int64_t load_texture_color_key_rgb(int64_t renderer_id, const char *file_path, int64_t r, int64_t g, int64_t b);

/** @brief Render a portion of a texture to a destination rectangle */
void render_texture(int64_t renderer_id, int64_t texture_id, int64_t src_x, int64_t src_y, int64_t src_w, int64_t src_h, int64_t dst_x, int64_t dst_y, int64_t dst_w, int64_t dst_h);
/** @} */

/** @name Timing functions */
/** @{ */

/** @brief Get the number of milliseconds since SDL initialization */
int64_t get_ticks(void);

/** @brief Pause execution for the given number of milliseconds */
void delay(int64_t ms);
/** @} */

/** @name Audio functions */
/** @{ */
/** @brief Open the audio device
 * @param freq Sample rate (e.g. 44100)
 * @param format Audio format
 * @param channels Number of channels
 * @param samples Buffer size in samples
 * @return 0 on success
 */
int64_t open_audio(int64_t freq, int64_t format, int64_t channels, int64_t samples);

/** @brief Close the audio device */
void close_audio(void);

/** @brief Pause or unpause audio playback */
void pause_audio(int64_t pause_on);

/** @brief Load a WAV file
 * @param file_path Path to the WAV file
 * @param[out] audio_buf Receives the audio buffer pointer
 * @param[out] audio_len Receives the audio length in bytes
 * @param[out] audio_spec Receives the audio spec pointer
 * @return Non-zero on success
 */
int64_t load_wav(const char *file_path, int64_t *audio_buf, int64_t *audio_len, int64_t *audio_spec);

/** @brief Free a loaded WAV buffer */
void free_wav(int64_t audio_buf);

/** @brief Queue audio data for playback */
int64_t queue_audio(const void *data, int64_t len);

/** @brief Get the number of bytes of queued audio */
int64_t get_queued_audio_size(void);

/** @brief Clear all queued audio data */
void clear_queued_audio(void);
/** @} */

/** @name Texture pixel access */
/** @{ */

/** @brief Update a texture with new pixel data */
int64_t update_texture(int64_t texture_id, const void *pixels, int64_t pitch);

/** @brief Lock a texture for direct pixel access
 * @param[out] pixels Receives pointer to pixel data
 * @param[out] pitch Receives row pitch in bytes
 */
int64_t lock_texture(int64_t texture_id, void **pixels, int64_t *pitch);

/** @brief Unlock a previously locked texture */
void unlock_texture(int64_t texture_id);
/** @} */

/** @name Text rendering (SDL_ttf) */
/** @{ */

/** @brief Initialize the SDL_ttf text rendering subsystem */
int64_t init_text(void);

/** @brief Shut down the SDL_ttf subsystem */
void quit_text(void);

/** @brief Load a TrueType font
 * @param file Font file path
 * @param ptsize Font point size
 * @return Font ID handle
 */
int64_t load_font(const char *file, int64_t ptsize);

/** @brief Draw text with a font at the given position and color */
void draw_text(int64_t renderer_id, int64_t font_id, const char *text, int64_t x, int64_t y, int64_t r, int64_t g, int64_t b, int64_t a);
/** @} */

/** @name Render targets */
/** @{ */

/** @brief Create an off-screen render target texture */
int64_t create_render_target(int64_t renderer_id, int64_t width, int64_t height);

/** @brief Set the current render target (0 to render to the window) */
void set_render_target(int64_t renderer_id, int64_t target_id);

/** @brief Destroy a render target texture */
void destroy_render_target(int64_t target_id);

/** @brief Present a render target scaled to the given dimensions */
void present_scaled(int64_t renderer_id, int64_t target_id, int64_t scale_width, int64_t scale_height);

/** @brief Present a render target stretched to the given dimensions */
void present_stretched(int64_t renderer_id, int64_t target_id, int64_t dst_width, int64_t dst_height);
/** @} */

#ifdef __cplusplus
}
#endif

#endif
