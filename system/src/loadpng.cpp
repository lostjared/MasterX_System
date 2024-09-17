#include"loadpng.hpp"
#include <png.h>
#include "SDL.h"
#include <cstdio>
#include <cstdlib>

namespace mx {
    SDL_Surface* LoadPNG(const char* file) {
        FILE* fp = fopen(file, "rb");
        if (!fp) {
            printf("Failed to open file: %s\n", file);
            return nullptr;
        }

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png) {
            fclose(fp);
            return nullptr;
        }

        png_infop info = png_create_info_struct(png);
        if (!info) {
            png_destroy_read_struct(&png, nullptr, nullptr);
            fclose(fp);
            return nullptr;
        }

        if (setjmp(png_jmpbuf(png))) {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(fp);
            return nullptr;
        }

        png_init_io(png, fp);
        png_read_info(png, info);

        int width = png_get_image_width(png, info);
        int height = png_get_image_height(png, info);
        png_byte color_type = png_get_color_type(png, info);
        png_byte bit_depth = png_get_bit_depth(png, info);

        if (bit_depth == 16) {
            png_set_strip_16(png);
        }

        if (color_type == PNG_COLOR_TYPE_PALETTE) {
            png_set_palette_to_rgb(png);
        }

        if (png_get_valid(png, info, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png);
        }

        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
            png_set_gray_to_rgb(png);
        }

        if (color_type == PNG_COLOR_TYPE_RGB) {
            png_set_add_alpha(png, 0xFF, PNG_FILLER_AFTER);
        }

        png_read_update_info(png, info);

        int pitch = png_get_rowbytes(png, info);
        Uint32 rmask, gmask, bmask, amask;

    #if SDL_BYTEORDER == SDL_BIG_ENDIAN
        rmask = 0xFF000000;
        gmask = 0x00FF0000;
        bmask = 0x0000FF00;
        amask = 0x000000FF;
    #else
        rmask = 0x000000FF;
        gmask = 0x0000FF00;
        bmask = 0x00FF0000;
        amask = 0xFF000000;
    #endif

        SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, rmask, gmask, bmask, amask);
        if (!surface) {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(fp);
            return nullptr;
        }

        png_bytep* row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
        Uint8* pixels = (Uint8*) surface->pixels;

        for (int y = 0; y < height; ++y) {
            row_pointers[y] = pixels + y * pitch;
        }

        png_read_image(png, row_pointers);
        png_read_end(png, nullptr);

        free(row_pointers);
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(fp);
        return surface;
    }
}