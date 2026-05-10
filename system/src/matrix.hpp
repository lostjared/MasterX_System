#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "mx_window.hpp"
#include "window.hpp"

namespace mx {
void createMatrixRainTexture(SDL_Renderer *renderer, SDL_Texture *tex,
                             TTF_Font *font, int screen_width,
                             int screen_height);
void releaseMatrix();
} // namespace mx

#endif