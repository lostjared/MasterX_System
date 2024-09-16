#include<libetl/etl.e> // include ETL standard library
#include"sdl.e" // include SDL definitions for sdl.c



// init or main function takes no args
proc init() {
    // window/grid width/height
    let width = 800;
    let height = 600;
    sdl_init(); // sdl init defined in sdl.e
    sdl_create("Print Text", width, height); // sdl create window
    sdl_cleartarget();
    sdl_settextcolor(255,255,255,255);
    while (sdl_pump()) { // event loop
        sdl_clear(); // clear screen
        sdl_printtext(25, 25, "Hello, World!");
        sdl_flip(); // flip
    }
    sdl_release();// release textures/window
    sdl_quit();// sdl quit program
    return 0; // always return something
}