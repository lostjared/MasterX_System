#include<libetl/etl.e>
#include<libetl/sdl.e>

#define BLOCK_WIDTH 640/32
#define BLOCK_HEIGHT 200/16
#define GRID_SIZE ((BLOCK_WIDTH * BLOCK_HEIGHT) * 8)

proc @allocate_grid() {
    let grid = malloc(GRID_SIZE);
    memclr(grid, 0);
    return grid;
}

proc draw_grid(@grid) {
    for(let x = 0; x < BLOCK_WIDTH; x = x + 1) {
        for(let y = 0;  y < BLOCK_HEIGHT; y = y + 1) {
            let index = y * BLOCK_WIDTH + x;
            if(mematl(grid, index) == 0) {
                sdl_setcolor(255, 0, 0, 255);
                sdl_fillrect(x*32, y*16, 32, 16);
            }
        }
    }
    return 0;
}

proc check_collision_with_bricks(@grid, ball_x, ball_y, ball_size) {
    let grid_x = ball_x / 32;
    let grid_y = ball_y / 16;
    
    if (grid_x >= 0 && grid_x < BLOCK_WIDTH && grid_y >= 0 && grid_y < BLOCK_HEIGHT) {
        let index = grid_y * BLOCK_WIDTH + grid_x;
        if (mematl(grid, index) == 0) {
            memstorel(grid, index, 1); 
            return 1; 
        }
    }
    return 0;
}

proc init() {
    sdl_init();
    sdl_create("Breakout", 640, 480, 640, 480);
    let grid = allocate_grid();
    let paddle_x = (640-100)/2;
    let ball_x = (640-10)/2;
    let ball_y = 300;
    let ball_vx = 2;
    let ball_vy = 2;
    let paddle_width = 100;
    let paddle_height = 10;
    let ball_size = 5;

    while(sdl_pump()) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();
        
        ball_x = ball_x + ball_vx;
        ball_y = ball_y + ball_vy;

        if (ball_x <= 0 || ball_x + ball_size >= 640) {
            ball_vx = -ball_vx;
        }
        if (ball_y <= 0) {
            ball_vy = -ball_vy;
        }
        if (ball_y + ball_size >= 380 && ball_y + ball_size <= 380 + paddle_height && 
            ball_x >= paddle_x && ball_x <= paddle_x + paddle_width) {
            ball_vy = -ball_vy;
        }

        if (check_collision_with_bricks(grid, ball_x, ball_y, ball_size) == 1) {
            ball_vy = -ball_vy; 
        }

        if (ball_y + ball_size > 480) {
            ball_x = (640-10)/2;
            ball_y = 300;
            ball_vx = 2;
            ball_vy = 2;
        } else {
            if(sdl_keydown(79)) {
                if(paddle_x < 640-100) {
                    paddle_x = paddle_x + 5;
                }
            }
            
            if(sdl_keydown(80)) {
                if(paddle_x > 0) {
                    paddle_x = paddle_x - 5;
                }
            }
        }
        draw_grid(grid);
        sdl_setcolor(255, 255, 255, 255);
        sdl_fillrect(paddle_x, 380, paddle_width, paddle_height);
        sdl_setcolor(255, 255, 255, 255);
        sdl_fillrect(ball_x, ball_y, ball_size, ball_size);
        sdl_flip();
        sdl_delay(1000/60);
    }
    free(grid);
    sdl_release();
    sdl_quit();
    return 0;
}
