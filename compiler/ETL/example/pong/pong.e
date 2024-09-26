#include<libetl/etl.e>
#include<libetl/sdl.e>

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
    sdl_create("Pong", 640, 480, 960, 720);
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

            // Player paddle controls
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

            
            if (ball_x < 0 || ball_x > 640) {
                ball_x = 320;
                ball_y = 240;
                ball_vel_x = 2;  
                ball_vel_y = 2;  
            }
        }
        sdl_flip();
    }
    sdl_release();
    sdl_quit();
    return 0;
}
