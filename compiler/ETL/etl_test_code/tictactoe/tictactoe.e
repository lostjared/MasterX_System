#include <libetl/etl.e>
#include "sdl.e"

// Function to allocate the game grid
proc @allocate_grid(size) {
    let grid = malloc(size * size);
    memclr(grid, size * size);
    return grid;
}

// Function to draw the game grid and marks
proc draw_grid(@grid) {
    let GRID_SIZE = 3;
    sdl_setcolor(0, 0, 0, 255); // Black background
    sdl_fillrect(0, 0, 600, 600); // Clear the screen

    sdl_setcolor(255, 225, 255, 255); // White lines

    // Draw vertical lines
    for(let i1 = 1; i1 < GRID_SIZE; i1 = i1 + 1) {
        sdl_drawline(200 * i1, 0, 200 * i1, 600);
    }

    // Draw horizontal lines
    for(let i2 = 1; i2 < GRID_SIZE; i2 = i2 + 1) {
        sdl_drawline(0, 200 * i2, 600, 200 * i2);
    }

    // Draw the marks (X and O)
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

// Function to draw an 'X' at the given grid position
proc draw_x(grid_x, grid_y) {
    let GRID_SIZE = 3;
    sdl_setcolor(255, 0, 0, 255); // Red color for X
    let offset = 20;
    let x1 = grid_x * 200 + offset;
    let y1 = grid_y * 200 + offset;
    let x2 = (grid_x + 1) * 200 - offset;
    let y2 = (grid_y + 1) * 200 - offset;
    sdl_drawline(x1, y1, x2, y2);
    sdl_drawline(x1, y2, x2, y1);
    return 0;
}

// Function to draw an 'O' at the given grid position
proc draw_o(grid_x, grid_y) {
    let GRID_SIZE = 3;
    sdl_setcolor(0, 0, 255, 255); // Blue color for O
    let center_x = grid_x * 200 + 100;
    let center_y = grid_y * 200 + 100;
    sdl_drawcircle(center_x, center_y, 80);
    return 0;
}

// Function to check for a winner or draw
proc check_winner(@grid) {
    let GRID_SIZE = 3;
    // Check rows and columns
    for(let i3 = 0; i3 < GRID_SIZE; i3 = i3 + 1) {
        // Check row
        if(mematb(grid, i3 * GRID_SIZE + 0) != 0 &&
           mematb(grid, i3 * GRID_SIZE + 0) == mematb(grid, i3 * GRID_SIZE + 1) &&
           mematb(grid, i3 * GRID_SIZE + 1) == mematb(grid, i3 * GRID_SIZE + 2)) {
            return mematb(grid, i3 * GRID_SIZE + 0);
        }
        // Check column
        if(mematb(grid, 0 * GRID_SIZE + i3) != 0 &&
           mematb(grid, 0 * GRID_SIZE + i3) == mematb(grid, 1 * GRID_SIZE + i3) &&
           mematb(grid, 1 * GRID_SIZE + i3) == mematb(grid, 2 * GRID_SIZE + i3)) {
            return mematb(grid, 0 * GRID_SIZE + i3);
        }
    }

    // Check diagonals
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

    // Check for draw
    let is_draw = 1;
    for(let i4 = 0; i4 < GRID_SIZE * GRID_SIZE; i4 = i4 + 1) {
        if(mematb(grid, i4) == 0) {
            is_draw = 0;
            break;
        }
    }
    if(is_draw == 1) {
        return 3; // Indicate a draw
    }

    return 0; // No winner yet
}

// Main function
proc init() {
    let GRID_SIZE = 3;
    sdl_init();
    sdl_create("Tic-Tac-Toe", 600, 600);

    let grid = allocate_grid(GRID_SIZE);
    let current_player = 1; // Player 1 starts
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
            // Handle mouse input for placing marks
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
                            // Switch players
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
    free(grid);
    sdl_release();
    sdl_quit();
    return 0;
}
