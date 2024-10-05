#include<libetl/etl.e>
#include<libetl/sdl.e>

#define BLOCK_WIDTH 640/32
#define BLOCK_HEIGHT 200/16
#define GRID_SIZE ((BLOCK_WIDTH * BLOCK_HEIGHT) * 8)

proc @allocate_grid() {
    let grid = malloc(GRID_SIZE+1);
    memclr(grid, GRID_SIZE);
    return grid;
}

proc draw_grid(@grid) {
    for(let x = 0; x < BLOCK_WIDTH; x = x + 1) {
        for(let y = 0;  y < BLOCK_HEIGHT; y = y + 1) {
            let index = y * BLOCK_WIDTH + x;
            if(mematl(grid, index) == 0) {
                sdl_setstartcolor(150, 0, 0, 255);
                sdl_setendcolor(255, 0, 0, 255);
                sdl_draw_gradient(x*32, y*16, 30, 14);
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

proc check_if_clear(@grid) {
    for(let x = 0;  x < BLOCK_WIDTH; x = x + 1) {
        for(let y = 0; y < BLOCK_HEIGHT; y = y + 1) {
            let index = y * BLOCK_WIDTH + x;
            if(mematl(grid, index) == 0) {
                return 0;
            }
        }
    }
    return 1;
}

proc init() {
    srand(time(0));
    sdl_init();
    sdl_create("Breakout", 640, 480, 960, 720);
    let grid = allocate_grid();
    let paddle_x = (640-100)/2;
    let ball_x = (640-10)/2;
    let ball_y = 300;
    let ball_vx = 2;
    let ball_vy = 2;
    let paddle_width = 100;
    let paddle_height = 10;
    let ball_size = 5;
    let lives = 3;
    let active = 1;
    let score = 0;
    let active_gameover = 1;
    let active_intro = 1;
    let start_tex = sdl_loadtex("start.bmp");

    while(sdl_pump() && active_intro == 1) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();
        sdl_copytex(start_tex, 0, 0, 640, 480);
        for(let key = 0; key < 127; key = key + 1) {
            if(sdl_keydown(key)) {
                active_intro = 0;
            }
        }
        sdl_settextcolor(255, 255, 255, 255);
        sdl_printtext(25, 25, "Press any Key to play");
        sdl_flip();
    }

    sdl_destroytex(start_tex);
    let bg = sdl_loadtex("bg.bmp");

    while(sdl_pump() && active == 1) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();   
        sdl_copytex(bg, 0, 0, 640, 480);
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
            score = score + 100;
        }

        if(check_if_clear(grid) == 1) {
            ball_x = rand()%(640-10);
            ball_y = 220;
            ball_vx = 2;
            ball_vy = 2;
            memclr(grid, GRID_SIZE);
        }

        if (ball_y + ball_size > 480) {
            ball_x = rand()%(640-10);
            ball_y = 220;
            ball_vx = 2;
            ball_vy = 2;
            lives = lives - 1;
            if(lives <= 0) {
                active = 0;
            }
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
        sdl_setstartcolor(150, 150, 150, 255);
        sdl_setendcolor(255,255,255,255);
        sdl_draw_gradient(paddle_x, 380, paddle_width, paddle_height);
        sdl_draw_gradient(ball_x, ball_y, ball_size, ball_size);
        sdl_printtext(10, 400, "Lives: " + str(lives));
        sdl_printtext(10, 425, "Score: " + str(score));
        sdl_flip();
        sdl_delay(1000/60);
    }
    sdl_destroytex(bg);
    let game_over = sdl_loadtex("gameover.bmp");
    while(sdl_pump() && active_gameover == 1) {
        sdl_setcolor(0, 0, 0, 255);
        sdl_clear();
        sdl_copytex(game_over, 0, 0, 640, 480);
        sdl_settextcolor(255, 255, 255, 255);
        sdl_printtext(100, 400, "Press Enter to Quit  [ Game Over Score: " + str(score) + " ] ");
        if(sdl_keydown(40)) {
            active_gameover = 0;
        }
        sdl_flip();
        sdl_delay(1000/60);
    }
    sdl_destroytex(game_over);
    free(grid);
    sdl_release();
    sdl_quit();
    return 0;
}
