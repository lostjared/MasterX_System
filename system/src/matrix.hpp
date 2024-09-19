#ifndef __MATRIX_H__
#define __MATRIX_H__


#include "window.hpp"
#include "mx_window.hpp"

namespace mx {
    void createMatrixRainTexture(SDL_Renderer* renderer, SDL_Texture *tex, TTF_Font* font, int screen_width, int screen_height);
    void releaseMatrix();
}

#endif