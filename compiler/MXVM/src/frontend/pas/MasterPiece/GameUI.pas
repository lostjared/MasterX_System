{ GameUI - Rendering and UI for MasterPiece screens }
unit GameUI;

interface

uses BlockTypes, PieceUnit, GridUnit;

var
  window_id: integer;
  renderer_id: integer;
  target_id: integer;
  font_id: integer;
  tex_intro: integer;
  tex_start: integer;
  tex_cursor: integer;
  tex_bg: integer;
  block_tex: array[0..10] of integer;
  current_screen: integer;
  cursor_pos: integer;
  intro_wait: integer;
  paused: integer;
  opt_lines: integer;
  opt_fullscr: integer;
  opt_cursor: integer;
  score_buf: string;

procedure InitSDL;
procedure ShutdownSDL;
function LoadTextures: integer;
procedure BeginFrame;
procedure EndFrame;
procedure RenderIntro;
procedure RenderStart;
procedure RenderGame;
procedure RenderOptions;
procedure RenderCredits;
procedure RenderHighScores;
procedure RenderFrame;
procedure DrawBoard;
procedure DrawPiece;
procedure DrawNext;
procedure DrawScore;

implementation

procedure InitSDL;
begin
  sdl_init();
  window_id := sdl_create_window('MasterPiece [MXVM Pascal]', 100, 100, TARGET_W, TARGET_H, 0);
  renderer_id := sdl_create_renderer(window_id, -1, 0);
  target_id := sdl_create_render_target(renderer_id, WIN_W, WIN_H);
  sdl_init_text();
  font_id := sdl_load_font('img/font.ttf', FONT_SIZE);
  if font_id = -1 then
    writeln('Warning: Could not load font');
  current_screen := SCREEN_INTRO;
  cursor_pos := 0;
  intro_wait := 0;
  paused := 0;
  opt_lines := LINES_PER_LEVEL;
  opt_fullscr := 0;
  opt_cursor := 0;
end;

procedure ShutdownSDL;
begin
  if target_id >= 0 then
    sdl_destroy_render_target(target_id);
  sdl_quit_text();
  sdl_destroy_renderer(renderer_id);
  sdl_destroy_window(window_id);
  sdl_quit();
end;

function LoadTextures: integer;
begin
  LoadTextures := 0;
  block_tex[BLOCK_YELLOW] := sdl_load_texture(renderer_id, 'img/block_yellow.bmp');
  block_tex[BLOCK_ORANGE] := sdl_load_texture(renderer_id, 'img/block_orange.bmp');
  block_tex[BLOCK_LTBLUE] := sdl_load_texture(renderer_id, 'img/block_ltblue.bmp');
  block_tex[BLOCK_DBLUE]  := sdl_load_texture(renderer_id, 'img/block_dblue.bmp');
  block_tex[BLOCK_PURPLE] := sdl_load_texture(renderer_id, 'img/block_purple.bmp');
  block_tex[BLOCK_PINK]   := sdl_load_texture(renderer_id, 'img/block_pink.bmp');
  block_tex[BLOCK_GRAY]   := sdl_load_texture(renderer_id, 'img/block_gray.bmp');
  block_tex[BLOCK_RED]    := sdl_load_texture(renderer_id, 'img/block_red.bmp');
  block_tex[BLOCK_GREEN]  := sdl_load_texture(renderer_id, 'img/block_green.bmp');
  block_tex[BLOCK_CLEAR]  := sdl_load_texture(renderer_id, 'img/block_clear.bmp');
  tex_intro  := sdl_load_texture(renderer_id, 'img/intro.bmp');
  tex_start  := sdl_load_texture(renderer_id, 'img/start.bmp');
  tex_cursor := sdl_load_texture_color_key_rgb(renderer_id, 'img/cursor.bmp', 255, 0, 255);
  tex_bg     := sdl_load_texture(renderer_id, 'img/gamebg.bmp');
  if tex_intro = -1 then
  begin
    writeln('Failed to load intro texture');
    LoadTextures := 1;
    exit;
  end;
  if tex_start = -1 then
  begin
    writeln('Failed to load start texture');
    LoadTextures := 1;
    exit;
  end;
  if tex_bg = -1 then
  begin
    writeln('Failed to load gamebg texture');
    LoadTextures := 1;
    exit;
  end;
end;

procedure BeginFrame;
begin
  sdl_set_render_target(renderer_id, target_id);
  sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
  sdl_clear(renderer_id);
end;

procedure EndFrame;
begin
  sdl_present_scaled(renderer_id, target_id, TARGET_W, TARGET_H);
end;

procedure RenderIntro;
begin
  BeginFrame;
  sdl_render_texture(renderer_id, tex_intro, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
  EndFrame;
  intro_wait := intro_wait + 1;
  if intro_wait > 180 then
  begin
    current_screen := SCREEN_START;
    intro_wait := 0;
  end;
end;

procedure RenderStart;
var
  cx, cy: integer;
begin
  BeginFrame;
  sdl_render_texture(renderer_id, tex_start, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
  cx := 250;
  if cursor_pos = 0 then
    cy := 170
  else if cursor_pos = 1 then
    cy := 240
  else if cursor_pos = 2 then
    cy := 310
  else
    cy := 380;
  if tex_cursor >= 0 then
    sdl_render_texture(renderer_id, tex_cursor, -1, -1, -1, -1, cx, cy, 64, 64);
  EndFrame;
end;

procedure DrawBoard;
var
  r, c, val, bx, by, flash_phase, orig: integer;
begin
  for r := 0 to GRID_ROWS - 1 do
  begin
    for c := 0 to GRID_COLS - 1 do
    begin
      val := GridUnit.GridGet(c, r);
      if val = BLOCK_CLEAR then
      begin
        bx := BOARD_OX + c * CELL_W;
        by := BOARD_OY + r * CELL_H;
        flash_phase := GridUnit.GridGetFlash(c, r) mod 6;
        if flash_phase < 3 then
        begin
          orig := GridUnit.GridGetOriginal(c, r);
          if (orig > 0) and (orig <= 10) then
            sdl_render_texture(renderer_id, block_tex[orig], -1, -1, -1, -1, bx, by, CELL_W, CELL_H);
        end
        else
        begin
          sdl_set_draw_color(renderer_id, rand() mod 256, rand() mod 256, rand() mod 256, 255);
          sdl_fill_rect(renderer_id, bx, by, CELL_W, CELL_H);
        end;
      end
      else if (val > BLOCK_BLACK) and (val <= 10) then
      begin
        bx := BOARD_OX + c * CELL_W;
        by := BOARD_OY + r * CELL_H;
        sdl_render_texture(renderer_id, block_tex[val], -1, -1, -1, -1, bx, by, CELL_W, CELL_H);
      end;
    end;
  end;
end;

procedure DrawPiece;
var
  bx, by: integer;
begin
  if GridUnit.game_over = 1 then
    exit;
  bx := BOARD_OX + PieceUnit.piece_x1 * CELL_W;
  by := BOARD_OY + PieceUnit.piece_y1 * CELL_H;
  if (PieceUnit.piece_c1 > 0) and (PieceUnit.piece_c1 <= BLOCK_CLEAR) then
    sdl_render_texture(renderer_id, block_tex[PieceUnit.piece_c1], -1, -1, -1, -1, bx, by, CELL_W, CELL_H);
  bx := BOARD_OX + PieceUnit.piece_x2 * CELL_W;
  by := BOARD_OY + PieceUnit.piece_y2 * CELL_H;
  if (PieceUnit.piece_c2 > 0) and (PieceUnit.piece_c2 <= BLOCK_CLEAR) then
    sdl_render_texture(renderer_id, block_tex[PieceUnit.piece_c2], -1, -1, -1, -1, bx, by, CELL_W, CELL_H);
  bx := BOARD_OX + PieceUnit.piece_x3 * CELL_W;
  by := BOARD_OY + PieceUnit.piece_y3 * CELL_H;
  if (PieceUnit.piece_c3 > 0) and (PieceUnit.piece_c3 <= BLOCK_CLEAR) then
    sdl_render_texture(renderer_id, block_tex[PieceUnit.piece_c3], -1, -1, -1, -1, bx, by, CELL_W, CELL_H);
end;

procedure DrawNext;
var
  ny: integer;
begin
  ny := NEXT_OY;
  if (PieceUnit.next_c1 > 0) and (PieceUnit.next_c1 <= BLOCK_CLEAR) then
    sdl_render_texture(renderer_id, block_tex[PieceUnit.next_c1], -1, -1, -1, -1, NEXT_OX, ny, CELL_W, CELL_H);
  ny := ny + CELL_H;
  if (PieceUnit.next_c2 > 0) and (PieceUnit.next_c2 <= BLOCK_CLEAR) then
    sdl_render_texture(renderer_id, block_tex[PieceUnit.next_c2], -1, -1, -1, -1, NEXT_OX, ny, CELL_W, CELL_H);
  ny := ny + CELL_H;
  if (PieceUnit.next_c3 > 0) and (PieceUnit.next_c3 <= BLOCK_CLEAR) then
    sdl_render_texture(renderer_id, block_tex[PieceUnit.next_c3], -1, -1, -1, -1, NEXT_OX, ny, CELL_W, CELL_H);
end;

procedure DrawScore;
begin
  score_buf := 'Score: ' + inttostr(GridUnit.score);
  if font_id >= 0 then
    sdl_draw_text(renderer_id, font_id, score_buf, 200, 60, 255, 255, 255, 255);
  score_buf := 'Lines: ' + inttostr(GridUnit.lines);
  if font_id >= 0 then
    sdl_draw_text(renderer_id, font_id, score_buf, 310, 60, 255, 255, 255, 255);
end;

procedure RenderGame;
begin
  BeginFrame;
  sdl_render_texture(renderer_id, tex_bg, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
  DrawBoard;
  DrawPiece;
  DrawNext;
  DrawScore;
  if paused = 1 then
  begin
    if font_id >= 0 then
      sdl_draw_text(renderer_id, font_id, 'Game Paused - Press P to Continue', 20, 20, 255, 255, 255, 255);
  end;
  if GridUnit.game_over = 1 then
  begin
    if font_id >= 0 then
      sdl_draw_text(renderer_id, font_id, 'GAME OVER - Press Enter', 200, 240, 255, 255, 255, 255);
  end;
  EndFrame;
end;

procedure RenderOptions;
var
  lines_str, fs_str: string;
begin
  BeginFrame;
  sdl_render_texture(renderer_id, tex_start, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
  sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
  sdl_fill_rect(renderer_id, 35, 82, 586, 370);
  lines_str := 'Lines Until Speed Increase: ' + inttostr(opt_lines);
  if font_id >= 0 then
  begin
    sdl_draw_text(renderer_id, font_id, lines_str, 70, 90, 255, 255, 255, 255);
    sdl_draw_text(renderer_id, font_id, '=)>', 40, 90, 255, 255, 255, 255);
    sdl_draw_text(renderer_id, font_id, 'Arrow keys to change, Enter to apply, Esc to return', 60, 300, 255, 255, 255, 255);
  end;
  EndFrame;
end;

procedure RenderCredits;
begin
  BeginFrame;
  sdl_render_texture(renderer_id, tex_start, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
  sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
  sdl_fill_rect(renderer_id, 35, 82, 586, 370);
  if font_id >= 0 then
  begin
    sdl_draw_text(renderer_id, font_id, 'MasterPiece is written in MXVM Pascal / SDL', 40, 100, 255, 255, 255, 255);
    sdl_draw_text(renderer_id, font_id, 'This program is free.', 40, 120, 255, 255, 255, 255);
    sdl_draw_text(renderer_id, font_id, 'Press Escape to Return', 40, 140, 255, 255, 255, 255);
  end;
  EndFrame;
end;

procedure RenderHighScores;
begin
  BeginFrame;
  sdl_render_texture(renderer_id, tex_start, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
  sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
  sdl_fill_rect(renderer_id, 35, 82, 586, 370);
  if font_id >= 0 then
  begin
    sdl_draw_text(renderer_id, font_id, 'High Scores', 45, 90, 255, 255, 255, 255);
    score_buf := 'Your Score: ' + inttostr(GridUnit.score);
    sdl_draw_text(renderer_id, font_id, score_buf, 45, 120, 255, 255, 255, 255);
    sdl_draw_text(renderer_id, font_id, 'Press Enter to continue...', 280, 120, 255, 255, 255, 255);
  end;
  EndFrame;
end;

procedure RenderFrame;
begin
  if current_screen = SCREEN_INTRO then
    RenderIntro
  else if current_screen = SCREEN_START then
    RenderStart
  else if current_screen = SCREEN_GAME then
    RenderGame
  else if current_screen = SCREEN_OPTIONS then
    RenderOptions
  else if current_screen = SCREEN_CREDITS then
    RenderCredits
  else if current_screen = SCREEN_HIGHSCORES then
    RenderHighScores;
end;

end.
