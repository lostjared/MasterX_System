#include<libetl/etl.e>
#include<libetl/sdl.e>

proc init() {
	sdl_init();
	sdl_create("Hello, World", 640, 480, 960, 720);
	while(sdl_pump()) {
		sdl_clear();
		sdl_settextcolor(255,255,255,255);
		sdl_printtext(45, 45, "Hello, World!");
		sdl_flip();
	}
	return 0;
}
