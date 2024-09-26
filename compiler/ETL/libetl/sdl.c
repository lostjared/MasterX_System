#include"SDL.h"
#include"SDL_ttf.h"
#include<stdio.h>
#include<time.h>
#include<stdlib.h>

TTF_Font *font = NULL;

void sdl_init() {
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Error on initilaztion\n");
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    if(TTF_Init() < 0) {
        fprintf(stderr, "Error on TTF Init: %s\n", TTF_GetError());
        fflush(stderr);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    font = TTF_OpenFont("font.ttf", 18);
    if(!font) {
        fprintf(stderr, "Error could not open font: %s\n", TTF_GetError());
        fflush(stderr);
        TTF_Quit();
        SDL_Quit();
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

void sdl_drawline(long x1, long y1, long x2, long y2) {
    SDL_RenderDrawLine(ren, x1, y1, x2, y2);
}

void sdl_drawcircle(long center_x, long center_y, long radius) {
    int offset_x = 0;
    int offset_y = radius;
    int d = radius - 1;
    while (offset_y >= offset_x) {
        SDL_RenderDrawPoint(ren, center_x + offset_x, center_y + offset_y);
        SDL_RenderDrawPoint(ren, center_x + offset_y, center_y + offset_x);
        SDL_RenderDrawPoint(ren, center_x - offset_x, center_y + offset_y);
        SDL_RenderDrawPoint(ren, center_x - offset_y, center_y + offset_x);
        SDL_RenderDrawPoint(ren, center_x + offset_x, center_y - offset_y);
        SDL_RenderDrawPoint(ren, center_x + offset_y, center_y - offset_x);
        SDL_RenderDrawPoint(ren, center_x - offset_x, center_y - offset_y);
        SDL_RenderDrawPoint(ren, center_x - offset_y, center_y - offset_x);
        if (d >= 2 * offset_x) {
            d -= 2 * offset_x + 1;
            offset_x += 1;
        } else if (d < 2 * (radius - offset_y)) {
            d += 2 * offset_y - 1;
            offset_y -= 1;
        } else {
            d += 2 * (offset_y - offset_x - 1);
            offset_y -= 1;
            offset_x += 1;
        }
    }
}

long sdl_mousedown() {
    SDL_PumpEvents();
    int x, y;
    Uint32 buttons = SDL_GetMouseState(&x, &y);
    return (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
}

long sdl_mousex() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return x;
}

long sdl_mousey() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    return y;
}

void sdl_messagebox(const char *message) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", message, window);
    printf("%s\n", message);
}


void sdl_create(char *name, long tw, long th, long width, long height) {
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
    
    tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, tw, th);
       
    if(tex == NULL) {
        fprintf(stderr, "Error could not create texture: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

SDL_Texture *sdl_loadtex(const char *src) {
    SDL_Surface *surf = SDL_LoadBMP(src);
    if(!surf) {
        fprintf(stderr, "Error loading surface: %s\n", src);
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surf);
    SDL_FreeSurface(surf);
    if(!tex) {
        fprintf(stderr, "Error creating texture: %s\n", SDL_GetError());
        fflush(stderr);
        exit(EXIT_FAILURE);
    }
    return tex;
}

void sdl_delay(long ms) {
    SDL_Delay(ms);
}

void sdl_destroytex(SDL_Texture *tex) {
    if(tex != NULL)
        SDL_DestroyTexture(tex);
}

void sdl_copytex(SDL_Texture *text, long x, long y, long w, long h) {
    SDL_Rect rc = {(int)x,(int)y,(int)w,(int)h};
    SDL_RenderCopy(ren, text, NULL, &rc);
}

SDL_Color text_color = { 255, 255, 255, 255 };

void sdl_settextcolor(long r, long g, long b, long a) {
    text_color.r = (unsigned char)r;
    text_color.g = (unsigned char)g;
    text_color.b = (unsigned char)b;
    text_color.a = (unsigned char)a;
}

void sdl_printtext(long x, long y, char *src) {
    if(src == NULL | strlen(src) == 0)
        return;
    SDL_Surface *surf = TTF_RenderText_Solid(font, src, text_color);
    if(!surf) {
        fprintf(stderr, "Error render solid failed %s\n", TTF_GetError());
        return;
    }
    SDL_Texture *text = SDL_CreateTextureFromSurface(ren, surf);
    if(!text) {
        SDL_FreeSurface(surf);
        fprintf(stderr, "Error creating texture: %s\n", TTF_GetError());
        return;
    }

    SDL_Rect rc = {(int)x, (int)y, surf->w, surf->h };
    SDL_FreeSurface(surf);
    SDL_RenderCopy(ren, text, NULL, &rc);
    SDL_DestroyTexture(text);
}

long sdl_keydown(long key) {
    const Uint8 *keys = SDL_GetKeyboardState(0);

    if(key >= 0 && key <= 255)
         return keys[key];

    return -1;
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


void sdl_settarget(SDL_Texture *text) {
    SDL_SetRenderTarget(ren, text);
}
void sdl_cleartarget() {
    SDL_SetRenderTarget(ren, NULL);
}

void sdl_clear() {
     SDL_RenderClear(ren);
     sdl_settarget(tex);
     SDL_RenderClear(ren);
}

void sdl_flip() {
    sdl_settarget(NULL);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
}

void sdl_release()  {
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(window);
    if(font != NULL)
        TTF_CloseFont(font);
    TTF_Quit();
}

void sdl_quit() {
    SDL_Quit();
}