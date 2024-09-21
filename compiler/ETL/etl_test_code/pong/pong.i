# 0 "pong.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "pong.e"
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
define @meminsert(@buffer, index, value, element, @current_size);
define @memremove(@buffer,index, element, @current_size);
define @memmove(@dest, @src, size);
define @memcpy(@dest, @src, size);
# 2 "pong.e" 2
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
define sdl_drawline(x1, y1, x2, y2);
define sdl_drawcircle(center_x, center_y, radius);
define sdl_mousedown();
define sdl_mousex();
define sdl_mousey();
define sdl_messagebox($message);
define sdl_delay(wait);
# 3 "pong.e" 2

proc draw_paddle(x, y) {
    sdl_setcolor(255, 255, 255, 255);
    sdl_fillrect(x, y, 10, 60);
    return 0;
}


proc draw_ball(x, y) {
    sdl_setcolor(255, 255, 255, 255);
    sdl_fillrect(x, y, 16, 16);
    return 0;
}

proc init() {
    sdl_init();
    sdl_create("Pong", 640, 480);
    let paddle1_y = 200;
    let paddle2_y = 200;
    let ball_x = 320;
    let ball_y = 240;
    let ball_vel_x = 2;
    let ball_vel_y = 2;
    let prev_time = sdl_getticks();
    while(sdl_pump()) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();
        draw_paddle(10, paddle1_y);
        draw_paddle(620, paddle2_y);
        draw_ball(ball_x, ball_y);
        let current_time = sdl_getticks();
        let adjusted = current_time - prev_time;
        if (adjusted >= 4) {
            prev_time = current_time;
            ball_x = ball_x + ball_vel_x;
            ball_y = ball_y + ball_vel_y;
             if (ball_y <= 0 || ball_y >= 470) {
                ball_vel_y = -ball_vel_y;
            }
            if (ball_x <= 20 && ball_y >= paddle1_y && ball_y <= (paddle1_y + 60)) {
                ball_vel_x = -ball_vel_x;
            }
            if (ball_x >= 610 && ball_y >= paddle2_y && ball_y <= (paddle2_y + 60)) {
                ball_vel_x = -ball_vel_x;
            }
            if (paddle2_y + 30 < ball_y) {
                paddle2_y = paddle2_y + 2;
            }
            if (paddle2_y + 30 > ball_y) {
                paddle2_y = paddle2_y - 2;
            }
            if (sdl_keydown(82)) {
                if (paddle1_y > 0) {
                    paddle1_y = paddle1_y - 5;
                }
            }
            if (sdl_keydown(81)) {
                if (paddle1_y < 420) {
                    paddle1_y = paddle1_y + 5;
                }
            }
        }
        sdl_flip();
    }
    sdl_release();
    sdl_quit();
    return 0;
}
