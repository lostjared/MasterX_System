#ifndef __SDLE_H__
#define __SDLE_H__
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
define sdl_drawline(x1, y1, x2, y2);
define sdl_drawcircle(center_x, center_y, radius);
define sdl_mousedown();
define sdl_mousex();
define sdl_mousey();
define sdl_messagebox($message);
#endif