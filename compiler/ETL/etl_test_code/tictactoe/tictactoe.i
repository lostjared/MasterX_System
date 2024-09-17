# 0 "tictactoe.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "tictactoe.e"
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
# 2 "tictactoe.e" 2
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
# 3 "tictactoe.e" 2


proc @allocate_grid(size) {
    let grid = malloc(size * size);
    memclr(grid, size * size);
    return grid;
}


proc draw_grid(@grid) {
    let GRID_SIZE = 3;
    sdl_setcolor(0, 0, 0, 255);
    sdl_fillrect(0, 0, 600, 600);

    sdl_setcolor(255, 225, 255, 255);


    for(let i1 = 1; i1 < GRID_SIZE; i1 = i1 + 1) {
        sdl_drawline(200 * i1, 0, 200 * i1, 600);
    }


    for(let i2 = 1; i2 < GRID_SIZE; i2 = i2 + 1) {
        sdl_drawline(0, 200 * i2, 600, 200 * i2);
    }


    for(let x1 = 0; x1 < GRID_SIZE; x1 = x1 + 1) {
        for(let y1 = 0; y1 < GRID_SIZE; y1 = y1 + 1) {
            let mark = mematb(grid, y1 * GRID_SIZE + x1);
            if(mark == 1) {
                draw_x(x1, y1);
            }
            if(mark == 2) {
                draw_o(x1, y1);
            }
        }
    }
    return 0;
}


proc draw_x(grid_x, grid_y) {
    let GRID_SIZE = 3;
    sdl_setcolor(255, 0, 0, 255);
    let offset = 20;
    let x1 = grid_x * 200 + offset;
    let y1 = grid_y * 200 + offset;
    let x2 = (grid_x + 1) * 200 - offset;
    let y2 = (grid_y + 1) * 200 - offset;
    sdl_drawline(x1, y1, x2, y2);
    sdl_drawline(x1, y2, x2, y1);
    return 0;
}


proc draw_o(grid_x, grid_y) {
    let GRID_SIZE = 3;
    sdl_setcolor(0, 0, 255, 255);
    let center_x = grid_x * 200 + 100;
    let center_y = grid_y * 200 + 100;
    sdl_drawcircle(center_x, center_y, 80);
    return 0;
}


proc check_winner(@grid) {
    let GRID_SIZE = 3;

    for(let i3 = 0; i3 < GRID_SIZE; i3 = i3 + 1) {

        if(mematb(grid, i3 * GRID_SIZE + 0) != 0 &&
           mematb(grid, i3 * GRID_SIZE + 0) == mematb(grid, i3 * GRID_SIZE + 1) &&
           mematb(grid, i3 * GRID_SIZE + 1) == mematb(grid, i3 * GRID_SIZE + 2)) {
            return mematb(grid, i3 * GRID_SIZE + 0);
        }

        if(mematb(grid, 0 * GRID_SIZE + i3) != 0 &&
           mematb(grid, 0 * GRID_SIZE + i3) == mematb(grid, 1 * GRID_SIZE + i3) &&
           mematb(grid, 1 * GRID_SIZE + i3) == mematb(grid, 2 * GRID_SIZE + i3)) {
            return mematb(grid, 0 * GRID_SIZE + i3);
        }
    }


    if(mematb(grid, 0) != 0 &&
       mematb(grid, 0) == mematb(grid, 4) &&
       mematb(grid, 4) == mematb(grid, 8)) {
        return mematb(grid, 0);
    }
    if(mematb(grid, 2) != 0 &&
       mematb(grid, 2) == mematb(grid, 4) &&
       mematb(grid, 4) == mematb(grid, 6)) {
        return mematb(grid, 2);
    }


    let is_draw = 1;
    for(let i4 = 0; i4 < GRID_SIZE * GRID_SIZE; i4 = i4 + 1) {
        if(mematb(grid, i4) == 0) {
            is_draw = 0;
            break;
        }
    }
    if(is_draw == 1) {
        return 3;
    }

    return 0;
}


proc init() {
    let GRID_SIZE = 3;
    sdl_init();
    sdl_create("Tic-Tac-Toe", 600, 600);

    let grid = allocate_grid(GRID_SIZE);
    let current_player = 1;
    let game_over = 0;
    let game_index = 0;

    while(sdl_pump()) {
        sdl_clear();
        draw_grid(grid);
        if(game_over == 1) {
            sdl_settextcolor(255, 255, 255, 255);
            if(game_index == 1) {
                sdl_printtext(15, 15, "Player 1 Wins!");
            }
            if(game_index == 2) {
                sdl_printtext(15, 15, "Player 2 Wins!");
            }
            if(game_index == 3) {
                sdl_printtext(15, 15,"It's a Draw!");
            }
        }
        sdl_flip();
        if(game_over == 0) {

            if(sdl_mousedown()) {
                let mx = sdl_mousex();
                let my = sdl_mousey();
                let grid_x = mx / 200;
                let grid_y = my / 200;

                if(grid_x >= 0 && grid_x < GRID_SIZE && grid_y >= 0 && grid_y < GRID_SIZE) {
                    let index = grid_y * GRID_SIZE + grid_x;
                    if(mematb(grid, index) == 0) {
                        memstoreb(grid, index, current_player);

                        let result = check_winner(grid);
                        if(result != 0) {
                            game_over = 1;
                            game_index = result;
                        } else {

                            if(current_player == 1) {
                                current_player = 2;
                            } else {
                                current_player = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    if(game_index == 1 || game_index == 2) {
        printf("Game Over: Winner was Player: %d\n", game_index);
    } else {
        printf("Game Over: Was a draw.\n");
    }

    free(grid);
    sdl_release();
    sdl_quit();
    return 0;
}
