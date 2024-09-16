#include"SDL.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

void sdl_init() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error on initilaztion\n");
        exit(EXIT_FAILURE);
    }
    srand(time(0));
}

SDL_Window *window;
SDL_Renderer *ren;
SDL_Texture *tex;
SDL_Event e;
SDL_Color col;

void sdl_setcolor(long r, long g, long b, long a) {
    SDL_SetRenderDrawColor(ren, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a);
}

void sdl_fillrect(long x, long y, long w, long h) {
    SDL_Rect rc;
    rc.x = x;
    rc.y = y;
    rc.w = w;
    rc.h = h;
    SDL_RenderFillRect(ren, &rc);
}

void sdl_create(char *name, long width, long height) {
     window = SDL_CreateWindow(name, 0, 0, width, height, SDL_WINDOW_SHOWN);
     if(window == NULL) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
     }
     ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(ren == NULL) {
        fprintf(stderr, "Error could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, width, height);
    if(tex == NULL) {
        fprintf(stderr, "Error could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

long sdl_keydown(long key) {
    const Uint8 *keys = SDL_GetKeyboardState(0);
    return keys[key];
}

long sdl_getticks() {
    return (long) SDL_GetTicks64();
}

long sdl_pump() {
    while(SDL_PollEvent(&e)) {
            switch(e.type) {
                case SDL_KEYDOWN:
                    if(e.key.keysym.sym == SDLK_ESCAPE) return 0;
                    break;
                case SDL_QUIT:
                    return 0;
                    break;
            }
    }
    return 1;
}

void sdl_clear() {
     SDL_RenderClear(ren);
}

void sdl_flip() {
     SDL_RenderPresent(ren);
}

void sdl_release()  {
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
}

void sdl_quit() {
    SDL_Quit();
}