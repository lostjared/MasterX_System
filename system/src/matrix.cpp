#include "matrix.hpp"
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

 namespace mx {
    std::unordered_map<std::string, SDL_Texture*> char_textures;
    SDL_Color bright_green = {180, 255, 180, 255};
    SDL_Color dim_green = {0, 255, 0, 150};
        
    void releaseMatrix() {
        for(auto &i : char_textures) {
            if(i.second != nullptr) {
                SDL_DestroyTexture(i.second);
            }
        }
    }
    // Utility function to convert a Unicode code point to a UTF-8 string
    std::string unicodeToUTF8(int codepoint) {
        std::string utf8;
        if (codepoint <= 0x7F) {
            utf8 += static_cast<char>(codepoint);
        } else if (codepoint <= 0x7FF) {
            utf8 += static_cast<char>((codepoint >> 6) | 0xC0);
            utf8 += static_cast<char>((codepoint & 0x3F) | 0x80);
        } else if (codepoint <= 0xFFFF) {
            utf8 += static_cast<char>((codepoint >> 12) | 0xE0);
            utf8 += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
            utf8 += static_cast<char>((codepoint & 0x3F) | 0x80);
        } else if (codepoint <= 0x10FFFF) {
            utf8 += static_cast<char>((codepoint >> 18) | 0xF0);
            utf8 += static_cast<char>(((codepoint >> 12) & 0x3F) | 0x80);
            utf8 += static_cast<char>(((codepoint >> 6) & 0x3F) | 0x80);
            utf8 += static_cast<char>((codepoint & 0x3F) | 0x80);
        }
        return utf8;
    }

    // Hiragana character code points (U+3040 to U+309F)
    const int hiragana_range[] = {
        0x3041, 0x3042, 0x3043, 0x3044, 0x3045, 0x3046, 0x3047, 0x3048, 0x3049, 0x304A,
        0x304B, 0x304C, 0x304D, 0x304E, 0x304F, 0x3050, 0x3051, 0x3052, 0x3053, 0x3054,
        0x3055, 0x3056, 0x3057, 0x3058, 0x3059, 0x305A, 0x305B, 0x305C, 0x305D, 0x305E,
        0x305F, 0x3060, 0x3061, 0x3062, 0x3063, 0x3064, 0x3065, 0x3066, 0x3067, 0x3068,
        0x3069, 0x306A, 0x306B, 0x306C, 0x306D, 0x306E, 0x306F, 0x3070, 0x3071, 0x3072,
        0x3073, 0x3074, 0x3075, 0x3076, 0x3077, 0x3078, 0x3079, 0x307A, 0x307B, 0x307C,
        0x307D, 0x307E, 0x307F, 0x3080, 0x3081, 0x3082, 0x3083, 0x3084, 0x3085, 0x3086,
        0x3087, 0x3088, 0x3089, 0x308A, 0x308B, 0x308C, 0x308D, 0x308E, 0x308F, 0x3090,
        0x3091, 0x3092, 0x3093
    };
    int hiragana_count = sizeof(hiragana_range) / sizeof(hiragana_range[0]);

    void createMatrixRainTexture(SDL_Renderer* renderer, SDL_Texture* texture, TTF_Font* font, int screen_width, int screen_height) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);  
        SDL_Rect screenRect = {0, 0, screen_width, screen_height};
        SDL_RenderFillRect(renderer, &screenRect);

        int char_width = 0;
        int char_height = 0;
        TTF_SizeUTF8(font, unicodeToUTF8(hiragana_range[0]).c_str(), &char_width, &char_height);
        int num_columns = screen_width / char_width;
        int num_rows = screen_height / char_height;

        static std::vector<float> fall_positions(num_columns, 0.0f);
        static std::vector<int> fall_speeds(num_columns, 0);
        static std::vector<int> trail_lengths(num_columns, 0);
        static Uint32 last_time = 0;
        float speed_multiplier = 2.0f;

        if (fall_positions[0] == 0.0f) {
            for (int col = 0; col < num_columns; ++col) {
                fall_positions[col] = static_cast<float>(rand() % num_rows);
                fall_speeds[col] = (rand() % 7 + 3) * speed_multiplier;
                trail_lengths[col] = rand() % 10 + 5;
            }
            last_time = SDL_GetTicks();
        }

        Uint32 current_time = SDL_GetTicks();
        float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;

        for (int col = 0; col < num_columns; ++col) {
            fall_positions[col] += fall_speeds[col] * delta_time;
            if (fall_positions[col] >= num_rows) {
                fall_positions[col] -= num_rows;
            }

            for (int trail_offset = 0; trail_offset < trail_lengths[col]; ++trail_offset) {
                int row = static_cast<int>(fall_positions[col] - trail_offset + num_rows) % num_rows;
                
                int random_char_code = hiragana_range[rand() % hiragana_count];
                std::string random_char = unicodeToUTF8(random_char_code);
                SDL_Texture* char_texture = nullptr;
                if (char_textures.find(random_char) == char_textures.end()) {
                    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, random_char.c_str(), trail_offset == 0 ? bright_green : dim_green);
                    char_texture = SDL_CreateTextureFromSurface(renderer, surface);
                    SDL_FreeSurface(surface);
                    char_textures[random_char] = char_texture;
                } else {
                    char_texture = char_textures[random_char];
                }

                SDL_Rect dst_rect = {col * char_width, row * char_height, char_width, char_height};
                SDL_RenderCopy(renderer, char_texture, nullptr, &dst_rect);
            }
        }
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
    }
}
 
 