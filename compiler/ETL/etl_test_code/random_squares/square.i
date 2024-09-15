# 0 "square.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "square.e"
# 1 "/usr/local/include/libetl/etl.e" 1 3



# 3 "/usr/local/include/libetl/etl.e" 3
define scan_integer();
define len($value);
define $str(value);
define exit(code);
define puts($output);
define srand(seed);
define rand();
define time(value);
define strlen($value);
define strcmp($value1, $value2);
define at($value, index);
define @malloc(bytes);
define @calloc(bytes, size);
define free(@memory);
define mematl(@memory, index);
define mematb(@memory, index);
define memclr(@memory, size);
define memstorel(@memory, index, value);
define memstoreb(@memory, index, value);
# 2 "square.e" 2
# 1 "sdl.e" 1




# 4 "sdl.e"
define sdl_init();
define sdl_quit();
define sdl_pump();
define sdl_release();
define sdl_create($name, width, height);
define sdl_setcolor(r, g, b, a);
define sdl_fillrect(x, y, w, h);
define sdl_flip();
define sdl_clear();
define sdl_keydown(key);
define sdl_getticks();
# 3 "square.e" 2



proc draw_grid(w, h) {


    for(let x = 0; x < w; x = x + 4) {
        for(let y = 0; y < h; y = y + 4) {
            sdl_setcolor(rand()%255,rand()%255,rand()%255, 255);
            sdl_fillrect(x, y, 4, 4);
        }
    }
    return 0;
}


proc init() {

    let width = 800;
    let height = 600;
    sdl_init();
    sdl_create("Random Squares", width, height);
    srand(time(0));
    while (sdl_pump()) {
        sdl_clear();
        draw_grid(width, height);
        sdl_flip();
    }
    sdl_release();
    sdl_quit();
    return 0;
}
