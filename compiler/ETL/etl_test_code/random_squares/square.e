#include<libetl/etl.e> // include ETL standard library
#include"sdl.e" // include SDL definitions for sdl.c

// draw grid function
// takes width and height as arguments
proc draw_grid(w, h) {
    // two nested for loops drawing the radom Squares
    // use C library rand function
    for(let x = 0; x < w; x = x + 4) {
        for(let y = 0;  y < h;  y = y + 4) {
            sdl_setcolor(rand()%255,rand()%255,rand()%255, 255);
            sdl_fillrect(x, y, 4, 4);
        }
    }
    return 0; // always return something in ETL
}

// init or main function takes no args
proc init() {
    // window/grid width/height
    let width = 800;
    let height = 600;
    sdl_init(); // sdl init defined in sdl.e
    sdl_create("Random Squares", width, height); // sdl create window
    srand(time(0));//  C library seed random with time
    while (sdl_pump()) { // event loop
        sdl_clear(); // clear screen
        draw_grid(width, height);// draw grid function
        sdl_flip(); // flip
    }
    sdl_release();// release textures/window
    sdl_quit();// sdl quit program
    return 0; // always return something
}