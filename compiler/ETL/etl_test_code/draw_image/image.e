#include<libetl/etl.e> // include ETL standard library
#include"sdl.e" // include SDL definitions for sdl.c

// init or main function takes no args
proc init() {
    // window/image width/height
    let width = 800;
    let height = 800;
    sdl_init(); // sdl init defined in sdl.e
    sdl_create("draw image", width, height); // sdl create window
    let img = sdl_loadtex("alien.bmp");
    sdl_cleartarget(); // clear the target
    while (sdl_pump()) { // event loop
        sdl_clear(); // clear screen
        sdl_copytex(img, 0, 0, width, height); //copy texture
        sdl_flip(); // flip to screen
    }
    sdl_destroytex(img);
    sdl_release();// release textures/window
    sdl_quit();// sdl quit program
    return 0; // always return something
}