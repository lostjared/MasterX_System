#ifndef __LOADPNG_H__
#define __LOADPNG_H__
#include"SDL.h"
namespace mx {
    SDL_Surface *LoadPNG(const char* file);
}
#endif