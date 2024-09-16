# 0 "text.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "text.e"
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
# 2 "text.e" 2
# 1 "sdl.e" 1



# 3 "sdl.e"
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
define @sdl_loadtex($source);
define sdl_destroytex(@tex);
define sdl_copytex(@tex, x, y, w, h);
define sdl_settarget(@target);
define sdl_cleartarget();
define sdl_printtext(x, y, $src);
define sdl_settextcolor(r,g,b,a);
# 3 "text.e" 2




proc init() {

    let width = 800;
    let height = 600;
    sdl_init();
    sdl_create("Print Text", width, height);
    sdl_cleartarget();
    sdl_settextcolor(255,255,255,255);
    while (sdl_pump()) {
        sdl_clear();
        sdl_printtext(25, 25, "Hello, World!");
        sdl_flip();
    }
    sdl_release();
    sdl_quit();
    return 0;
}
