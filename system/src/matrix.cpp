#include "matrix.hpp"
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

 namespace mx {
    std::unordered_map<char, SDL_Texture*> char_textures;
    SDL_Color bright_green = {180, 255, 180, 255};
    SDL_Color dim_green = {0, 255, 0, 150};
        
    void releaseMatrix() {
        for(auto &i : char_textures) {
            if(i.second != nullptr) {
                SDL_DestroyTexture(i.second);
            }
        }
    }

    void createMatrixRainTexture(SDL_Renderer* renderer, SDL_Texture* texture, TTF_Font* font, int screen_width, int screen_height) {
        const char *s = "a";
        int char_width = 0;
        int char_height = 0;
        TTF_SizeText(font, s, &char_width, &char_height);
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
                char random_char = static_cast<char>(rand() % 93 + 33);

                SDL_Texture* char_texture = nullptr;

                if (char_textures.find(random_char) == char_textures.end()) {
                    SDL_Surface* surface = TTF_RenderText_Solid(font, std::string(1, random_char).c_str(), trail_offset == 0 ? bright_green : dim_green);
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
    }
}
 
 