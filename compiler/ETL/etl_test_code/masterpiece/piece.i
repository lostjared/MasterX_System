# 0 "piece.e"
# 0 "<built-in>"
# 0 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 0 "<command-line>" 2
# 1 "piece.e"
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
define @realloc(@memory, size);
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
# 2 "piece.e" 2
# 1 "sdl.e" 1



# 3 "sdl.e"
define sdl_init();
define sdl_quit();
define sdl_pump();
define sdl_release();
define sdl_create($name, text_width, texture_height, width, height);
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
# 3 "piece.e" 2






proc set_block_color(color) {
    if(color == 0) {
        return 0;
    }
    if(color == 1) {
        sdl_setcolor(255,0,0,255);
        return 1;
    }
    if(color == 2) {
        sdl_setcolor(0,255,0,255);
        return 2;
    }
    if(color == 3) {
        sdl_setcolor(0,0,255,255);
        return 3;
    }
    if(color == 4) {
        sdl_setcolor(255,0,255,25);
        return 4;
    }
    return 5;
}

proc set_block_at(@block, index) {
    let bx = mematb(block, index+1);
    let by = mematb(block, index+2);
    let x = 32 * 4;
    let y = 16 * 4;
    set_block_color(mematb(block, index));
    sdl_fillrect(x*bx+20, y*by+30, 32 * 4, 16 * 4);
    return 0;
}

proc draw_grid(@grid, @block) {
    let block_size_w = 32 * 4;
    let block_size_h = 16 * 4;
    let offset_left = 20;
    let offset_top = 30;
    for(let x = 0; x < 11; x = x + 1) {
        for(let y = 0; y < 16; y = y + 1) {
            let grid_x = (x*block_size_w)+offset_left;
            let grid_y = (y*block_size_h)+offset_top;
            let color = mematb(grid, x * 16 + y);

            if(set_block_color(color) != 0) {
                sdl_fillrect(grid_x, grid_y, block_size_w, block_size_h);
            }
        }
    }
    set_block_at(block,0);
    set_block_at(block,3);
    set_block_at(block,6);
    return 0;
}

proc check_blocks(@grid, @score) {
    let bx = 0;
    let by = 0;
    for(let x = 0; x < 11; x = x + 1) {
        for(let y = 0; y < 14; y = y + 1) {
            bx = x;
            by = y;
            let color1 = mematb(grid, bx * 16 + by);
            let color2 = mematb(grid, bx * 16 + (by+1));
            let color3 = mematb(grid, bx * 16 + (by+2));
             if(color1 != 0 && color2 != 0 && color3 != 0 && color1 == color2 && color1 == color3) {
                memstoreb(grid, bx * 16 + by, 0);
                memstoreb(grid, bx * 16 + (by+1), 0);
                memstoreb(grid, bx * 16 + (by+2), 0);
                memstorel(score, 0, mematl(score, 0) + 100);
                return 0;
            }
        }
    }
    let x1 = 0;
    let y1 = 0;
    let color1x = 0;
    let color2x = 0;
    let color3x = 0;

    for(y1 = 0; y1 < 16; y1 = y1 + 1) {
        for(x1 = 0; x1 < 9; x1 = x1 + 1) {
            bx = x1;
            by = y1;
            color1x = mematb(grid, bx * 16 + by);
            color2x = mematb(grid, (bx+1) * 16 + (by));
            color3x = mematb(grid, (bx+2) * 16 + (by));
            if(color1x != 0 && color2x != 0 && color3x != 0 && color1x == color2x && color1x == color3x) {
                memstoreb(grid, bx * 16 + by, 0);
                memstoreb(grid, (bx+1) * 16 + by, 0);
                memstoreb(grid, (bx+2) * 16 + by, 0);
                memstorel(score, 0, mematl(score, 0) + 100);
                return 0;
            }
        }
    }

    for (y1 = 0; y1 < 14; y1 = y1 + 1) {
        for (x1 = 0; x1 < 7; x1 = x1 + 1) {
            color1x = mematb(grid, x1 * 16 + y1);
            color2x = mematb(grid, (x1 + 1) * 16 + (y1 + 1));
            color3x = mematb(grid, (x1 + 2) * 16 + (y1 + 2));
            if (color1x != 0 && color2x != 0 && color3x != 0 && color1x == color2x && color1x == color3x) {
                memstoreb(grid, x1 * 16 + y1, 0);
                memstoreb(grid, (x1 + 1) * 16 + (y1 + 1), 0);
                memstoreb(grid, (x1 + 2) * 16 + (y1 + 2), 0);
                memstorel(score, 0, mematl(score, 0) + 100);
                return 0;
            }
        }
    }

    for (y1 = 0; y1 < 14; y1 = y1 + 1) {
        for (x1 = 2; x1 < 9; x1 = x1 + 1) {
            color1x = mematb(grid, x1 * 16 + y1);
            color2x = mematb(grid, (x1 - 1) * 16 + (y1 + 1));
            color3x = mematb(grid, (x1 - 2) * 16 + (y1 + 2));
            if (color1x != 0 && color2x != 0 && color3x != 0 && color1x == color2x && color1x == color3x) {
                memstoreb(grid, x1 * 16 + y1, 0);
                memstoreb(grid, (x1 - 1) * 16 + (y1 + 1), 0);
                memstoreb(grid, (x1 - 2) * 16 + (y1 + 2), 0);
                memstorel(score, 0, mematl(score, 0) + 100);
                return 0;
            }
        }
    }
    return 0;
}

proc move_blocks(@grid) {
    for(let x = 0; x < 11; x = x + 1) {
        for(let y = 0; y < 15; y = y + 1) {
            let color1 = mematb(grid, x * 16 + y);
            let color2 = mematb(grid, x * 16 + (y+1));

            if(color2 == 0 && color1 != 0) {
                memstoreb(grid, x * 16 + y, 0);
                memstoreb(grid, x * 16 + (y+1), color1);
                return 0;
            }
        }
    }
    return 0;
}

proc @allocate_grid(w, h) {
    let grid = malloc (w * h);
    memclr(grid, w * h);
    return grid;
}

proc @allocate_block() {
    let block = malloc (11);
    memclr(block, 10);
    return block;
}

proc setup_block(@block) {
    memstoreb(block,0, 1+(rand()%3));
    memstoreb(block,1, 11/2 - 1 );
    memstoreb(block,2, 0);
    memstoreb(block,3, 1+(rand()%3));
    memstoreb(block,4, 11/2 - 1);
    memstoreb(block,5, 1);
    memstoreb(block,6, 1+(rand()%3));
    memstoreb(block,7, 11/2 - 1);
    memstoreb(block,8, 2);
    return 0;
}

proc set_block_in_grid(@grid, @block) {
    let val = mematb(block, 0);
    let bx = mematb(block, 1);
    let by = mematb(block, 2);

    if(by < 2) {
        return 1;
    }

    memstoreb(grid, bx * 16 + by, val);
    val = mematb(block, 3);
    bx = mematb(block, 4);
    by = mematb(block, 5);
    memstoreb(grid, bx * 16 + by, val);
    val = mematb(block,6);
    bx = mematb(block, 7);
    by = mematb(block, 8);
    memstoreb(grid, bx * 16 + by, val);
    return 0;
}

proc move_block_down(@grid, @block) {
    let bx = mematb(block, 7);
    let by = mematb(block, 8)+1;
    let pos = bx * 16 + by;
    let test_block = mematb(grid, pos);
    if(mematb(block, 8) < 15 && test_block == 0) {
        memstoreb(block, 2, mematb(block, 2) + 1);
        memstoreb(block, 5, mematb(block, 5) + 1);
        memstoreb(block, 8, mematb(block, 8) + 1);
    } else {


       if(set_block_in_grid(grid, block) == 1) {
           return 1;
       }
       setup_block(block); 
    }
    return 0;
}

proc swap_colors(@block) {

    let c1 = mematb(block, 0);
    let c2 = mematb(block, 3);
    let c3 = mematb(block, 6);

    memstoreb(block, 0, c3);
    memstoreb(block, 3, c1);
    memstoreb(block, 6, c2);

    return 0;
}

proc init() {
    sdl_init();
    sdl_create("MasterPiece", 1440, 1080, 960, 720);
    srand(time(0));
    let grid = allocate_grid(11, 16);
    let block = allocate_block();
    setup_block(block);
    let prev_time = sdl_getticks();
    let update_time = sdl_getticks();

    let score = malloc(8 * 2);
    memstorel(score, 0, 0);
    let game_over = 0;

    let puzzle_bg = sdl_loadtex("img/puzzle.bmp");
    while (sdl_pump() && game_over == 0) {
        sdl_clear();
        sdl_copytex(puzzle_bg, 0, 0, 1440, 1080);
        draw_grid(grid, block);
        sdl_settextcolor(255, 255, 255, 255);
        sdl_printtext(25, 25, "Score: " + str(mematl(score, 0)));
        check_blocks(grid, score);
        move_blocks(grid);
        sdl_flip();
        let ctime = sdl_getticks();
        if((ctime - update_time) >= 750) {
            if(move_block_down(grid, block) == 1) {
                game_over = 1;
            }
            update_time = ctime;
        }
        let current_time = sdl_getticks();
        if ((current_time - prev_time) >= 100) {
            prev_time = current_time;
            let bx = mematb(block, 7)+1;
            let by = mematb(block, 8);
            let pos = bx * 16 + by;
            let test_block = mematb(grid, pos);
            if (sdl_keydown(79) && test_block == 0 && mematb(block, 1) < (1440 / (32 * 4) - 1))
                    memstoreb(block, 1, mematb(block, 1) + 1);
                    memstoreb(block, 4, mematb(block, 4) + 1);
                    memstoreb(block, 7, mematb(block, 7) + 1);
            } else {
                bx = mematb(block, 7)-1;
                by = mematb(block, 8);
                pos = bx * 16 + by;
                test_block = mematb(grid, pos);
                if (sdl_keydown(80) && test_block == 0 && mematb(block, 1) > 0) {
                    memstoreb(block, 1, mematb(block, 1) - 1);
                    memstoreb(block, 4, mematb(block, 4) - 1);
                    memstoreb(block, 7, mematb(block, 7) - 1);
                } else {
                    if(sdl_keydown(81)) {
                         move_block_down(grid, block);
                    } else {
                        if(sdl_keydown(82)) {
                            swap_colors(block);
                        }
                    }
                }
            }
        }
    }

    let game_over_screen = sdl_loadtex("img/gameover.bmp");

    while(sdl_pump()) {
        sdl_clear();
        sdl_copytex(game_over_screen, 0, 0, 1440, 1080);
        sdl_setcolor(0, 0, 0, 255);
        sdl_settextcolor(255, 255, 255 ,255);
        sdl_printtext(45, 45, "Game Over Score: " + str(mematl(score, 0)));
        sdl_flip();
    }
    sdl_destroytex(puzzle_bg);
    sdl_destroytex(game_over_screen);
    free(grid);
    free(block);
    free(score);
    sdl_release();
    sdl_quit();
    return 0;
}
