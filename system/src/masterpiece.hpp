#ifndef __MASTERPIECE__H_
#define __MASTERPIECE__H_

#include"window.hpp"
#include"dimension.hpp"
#include"mx_window.hpp"

namespace mx {

    class MasterPiece : public Window {
    public:
        MasterPiece(mxApp &);
        virtual ~MasterPiece();
        virtual bool event(mxApp &app, SDL_Event &e) override;
        virtual void draw(mxApp &app) override;
        virtual void activate() override;
        void newGame();
        void resetGame();
    private:
        void load_gfx(mxApp &);
        void release_gfx();
        void draw_intro(mxApp &);
        void draw_start(mxApp &);
        void draw_game(mxApp &);
        void draw_credits(mxApp &);
        void draw_options(mxApp  &);
        int cur_screen = 0;
        SDL_Texture *draw_tex, *logo, *start, *cursor, *gamebg, *blocks[12], *alien;
        Uint32 startTime = 0; 
        Uint32 waitTime = 3000;
        int cursor_pos = 0;
        Uint32 startTime2 = 0;
        Uint32 waitTime2 = 1000;
    
        class TileMatrix {
        public:

            
            struct GameData {
                unsigned long score;
                int lines;
                int speed;
                int lineamt;
                int linenum = 0;

                bool game_is_over = false;

                void gameover() {
                     game_is_over = true;
                }

                bool isGameOver() const {
                    return game_is_over;
                }


                GameData() {
                    newgame();
                }

                void newgame() {
                    score = 0;
                    lines = 0;
                    speed = 1500;
                    lineamt = 0;
                }

                void addline() {
                    lines++;
                    score += 6;
                    lineamt++;

                    if (lineamt >= linenum) {
                        lineamt = 0;
                        speed -= 101;
                        if (speed < 100) {
                            speed = 100;
                        }
                    }
                }
            };

            struct GameBlock {

                struct Color {
                    int c1, c2, c3;

                    void randcolor() {
                        c1 = rand() % 10;
                        c2 = rand() % 10;
                        c3 = rand() % 10;
                        if (c1 == 0) { c1++; }
                        if (c2 == 0) { c2++; }
                        if (c3 == 0) { c3++; }

                        if (c1 == c2 && c1 == c3) {
                            randcolor();
                            return;
                        }
                    }

                    void copycolor(Color* c) {
                        c1 = c->c1;
                        c2 = c->c2;
                        c3 = c->c3;
                    }

                    void shiftcolor(bool dir) {
                        int ic1 = c1, ic2 = c2, ic3 = c3;

                        if (dir) {
                            c1 = ic3;
                            c2 = ic1;
                            c3 = ic2;
                        } else {
                            c1 = ic2;
                            c2 = ic3;
                            c3 = ic1;
                        }
                    }
                };

                Color color;
                Color nextcolor;
                int x, y;

                GameBlock() {
                    color.randcolor();
                    nextcolor.randcolor();
                }

                void NextBlock() {
                    color.copycolor(&nextcolor);
                    nextcolor.randcolor();
                    x = 3;
                    y = 0;
                }

                void MoveDown() {
                    if (y+2 < 16) {
                        y++;
                    }
                }

                void MoveLeft() {
                    if (x > 0) {
                        x--;
                    }
                }

                void MoveRight() {
                    if (x < 7) {
                        x++;
                    }
                }
            };

            GameData Game;
            GameBlock block;
            int Tiles[17][8];  // BR removed, strictly 17x8 bounds

            void cleartiles() {
                for (int i = 0; i < 17; i++) {
                    for (int j = 0; j < 8; j++) {
                        Tiles[i][j] = 0;
                    }
                }
            }

            void init_matrix() {
                cleartiles();
                Game.newgame();
                block.NextBlock();
            }

            void setblock() {
                if (block.y <= 0) {
                    Game.gameover();
                }

                if (block.y < 15) {  // Ensuring block.y + 2 stays within bounds
                    Tiles[block.y][block.x] = block.color.c1;
                    Tiles[block.y + 1][block.x] = block.color.c2;
                    Tiles[block.y + 2][block.x] = block.color.c3;
                    block.NextBlock();
                }
            }

            TileMatrix() {
                cleartiles();
            }

            void ProccessBlocks() {
                for (int z = 0; z < 8; z++) {
                    for (int i = 0; i < 16; i++) {
                        if (Tiles[i][z] != 0 && Tiles[i + 1][z] == 0) {
                            Tiles[i + 1][z] = Tiles[i][z];
                            Tiles[i][z] = 0;
                        }
                    }
                }
            }

            void clearBlocks() {
                for (int z = 0; z < 8; z++) {
                    for (int i = 0; i < 17; i++) {
                        if (Tiles[i][z] == -1) {
                            Tiles[i][z] = 0;
                        }
                    }
                }
            }
        };

        TileMatrix matrix;
        void blockProc();

    };



}





#endif