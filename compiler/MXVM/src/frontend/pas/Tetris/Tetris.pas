program Tetris;
{ Tetris in MXVM Pascal/Bytecode }
const
  BOARD_W = 10;
  BOARD_H = 20;
  CELL_SIZE = 40;
  WIN_W = 400;
  WIN_H = 800;
  FRAME_DELAY = 16;
  SPEED = 800;
  SDL_QUIT_EVENT = 256;
  SDL_KEYDOWN = 768;
  KEY_ESCAPE = 27;
  KEY_RETURN = 13;
  KEY_LEFT  = 1073741904;
  KEY_RIGHT = 1073741903;
  KEY_DOWN  = 1073741905;
  KEY_UP    = 1073741906;

var
  window_id: integer;
  renderer_id: integer;
  board: array[0..199] of integer;
  pieces: array[0..111] of integer;
  piece_id: integer;
  rotation: integer;
  piece_x: integer;
  piece_y: integer;
  score: integer;
  game_over: integer;
  show_start: integer;
  last_drop: integer;
  font_id: integer;
  start_texture: integer;
  running: integer;
  ev: integer;
  col_result: integer;
  frame_start, frame_time, delay_time: integer;
  tick: integer;
  score_text: string;

procedure InitPieces;
begin
  { I-piece }
  pieces[0] := 4;  pieces[1] := 5;  pieces[2] := 6;  pieces[3] := 7;
  pieces[4] := 2;  pieces[5] := 6;  pieces[6] := 10; pieces[7] := 14;
  pieces[8] := 8;  pieces[9] := 9;  pieces[10] := 10; pieces[11] := 11;
  pieces[12] := 1; pieces[13] := 5; pieces[14] := 9;  pieces[15] := 13;
  { L-piece }
  pieces[16] := 1;  pieces[17] := 5;  pieces[18] := 6;  pieces[19] := 7;
  pieces[20] := 2;  pieces[21] := 3;  pieces[22] := 6;  pieces[23] := 10;
  pieces[24] := 5;  pieces[25] := 6;  pieces[26] := 7;  pieces[27] := 11;
  pieces[28] := 2;  pieces[29] := 6;  pieces[30] := 10; pieces[31] := 9;
  { J-piece }
  pieces[32] := 3;  pieces[33] := 5;  pieces[34] := 6;  pieces[35] := 7;
  pieces[36] := 2;  pieces[37] := 6;  pieces[38] := 10; pieces[39] := 11;
  pieces[40] := 5;  pieces[41] := 6;  pieces[42] := 7;  pieces[43] := 9;
  pieces[44] := 1;  pieces[45] := 2;  pieces[46] := 6;  pieces[47] := 10;
  { S-piece }
  pieces[48] := 5;  pieces[49] := 6;  pieces[50] := 9;  pieces[51] := 10;
  pieces[52] := 5;  pieces[53] := 6;  pieces[54] := 9;  pieces[55] := 10;
  pieces[56] := 5;  pieces[57] := 6;  pieces[58] := 9;  pieces[59] := 10;
  pieces[60] := 5;  pieces[61] := 6;  pieces[62] := 9;  pieces[63] := 10;
  { Z-piece }
  pieces[64] := 2;  pieces[65] := 3;  pieces[66] := 5;  pieces[67] := 6;
  pieces[68] := 1;  pieces[69] := 5;  pieces[70] := 6;  pieces[71] := 10;
  pieces[72] := 6;  pieces[73] := 7;  pieces[74] := 9;  pieces[75] := 10;
  pieces[76] := 2;  pieces[77] := 6;  pieces[78] := 7;  pieces[79] := 11;
  { T-piece }
  pieces[80] := 2;  pieces[81] := 5;  pieces[82] := 6;  pieces[83] := 7;
  pieces[84] := 2;  pieces[85] := 6;  pieces[86] := 7;  pieces[87] := 10;
  pieces[88] := 5;  pieces[89] := 6;  pieces[90] := 7;  pieces[91] := 10;
  pieces[92] := 2;  pieces[93] := 5;  pieces[94] := 6;  pieces[95] := 10;
  { O-piece }
  pieces[96] := 1;   pieces[97] := 2;   pieces[98] := 6;   pieces[99] := 7;
  pieces[100] := 3;  pieces[101] := 6;  pieces[102] := 7;  pieces[103] := 10;
  pieces[104] := 5;  pieces[105] := 6;  pieces[106] := 10; pieces[107] := 11;
  pieces[108] := 2;  pieces[109] := 5;  pieces[110] := 6;  pieces[111] := 9;
end;

procedure AllocClearBoard;
var
  ci: integer;
begin
  for ci := 0 to (BOARD_W * BOARD_H - 1) do
    board[ci] := 0;
end;

function CheckCollision(cx, cy: integer): integer;
var
  ci, cidx, cv, cpx, cpy, ccx, ccy, bidx: integer;
begin
  for ci := 0 to 3 do
  begin
    cidx := piece_id * 16 + rotation * 4 + ci;
    cv := pieces[cidx];
    cpy := cv div 4;
    cpx := cv mod 4;
    ccx := cx + cpx;
    ccy := cy + cpy;
    if ccx < 0 then
    begin
      CheckCollision := 1;
      exit;
    end;
    if ccx >= BOARD_W then
    begin
      CheckCollision := 1;
      exit;
    end;
    if ccy >= BOARD_H then
    begin
      CheckCollision := 1;
      exit;
    end;
    if ccy >= 0 then
    begin
      bidx := ccy * BOARD_W + ccx;
      if board[bidx] <> 0 then
      begin
        CheckCollision := 1;
        exit;
      end;
    end;
  end;
  CheckCollision := 0;
end;

procedure LockPiece;
var
  li, lidx, lv, lpx, lpy, lx, ly, lbi: integer;
begin
  for li := 0 to 3 do
  begin
    lidx := piece_id * 16 + rotation * 4 + li;
    lv := pieces[lidx];
    lpy := lv div 4;
    lpx := lv mod 4;
    lx := piece_x + lpx;
    ly := piece_y + lpy;
    if (ly >= 0) and (ly < BOARD_H) then
    begin
      lbi := ly * BOARD_W + lx;
      board[lbi] := piece_id + 1;
    end;
  end;
end;

procedure ClearLines;
var
  cy, cx, cidx2, cv2, cj, cbi, cbi2: integer;
  is_full: integer;
begin
  cy := BOARD_H - 1;
  while cy >= 0 do
  begin
    is_full := 1;
    for cx := 0 to BOARD_W - 1 do
    begin
      cidx2 := cy * BOARD_W + cx;
      if board[cidx2] = 0 then
        is_full := 0;
    end;
    if is_full = 1 then
    begin
      score := score + 10;
      cj := cy;
      while cj > 0 do
      begin
        for cx := 0 to BOARD_W - 1 do
        begin
          cbi2 := (cj - 1) * BOARD_W + cx;
          cv2 := board[cbi2];
          cbi := cj * BOARD_W + cx;
          board[cbi] := cv2;
        end;
        cj := cj - 1;
      end;
      for cx := 0 to BOARD_W - 1 do
        board[cx] := 0;
      cy := cy + 1;
    end;
    cy := cy - 1;
  end;
end;

procedure DrawCell(dx, dy, color: integer);
var
  dr, dg, db, dtx, dty, dts, dm: integer;
begin
  dm := color * 35;
  dr := dm;
  dg := 255 - dm;
  db := 128 + dm;
  dtx := dx * CELL_SIZE;
  dty := dy * CELL_SIZE;
  dts := CELL_SIZE - 1;
  sdl_set_draw_color(renderer_id, dr, dg, db, 255);
  sdl_fill_rect(renderer_id, dtx, dty, dts, dts);
end;

procedure DrawEmpty(dx, dy: integer);
var
  dtx, dty, dts: integer;
begin
  dtx := dx * CELL_SIZE;
  dty := dy * CELL_SIZE;
  dts := CELL_SIZE - 1;
  sdl_set_draw_color(renderer_id, 30, 30, 30, 255);
  sdl_fill_rect(renderer_id, dtx, dty, dts, dts);
end;

procedure SpawnPiece;
begin
  piece_id := rand() mod 7;
  if piece_id < 0 then
    piece_id := piece_id + 7;
  rotation := 0;
  piece_x := 3;
  piece_y := 0;
  col_result := CheckCollision(piece_x, piece_y);
  if col_result = 1 then
    game_over := 1;
end;

procedure MoveDown;
begin
  col_result := CheckCollision(piece_x, piece_y + 1);
  if col_result <> 0 then
  begin
    LockPiece;
    ClearLines;
    SpawnPiece;
  end
  else
    piece_y := piece_y + 1;
end;

procedure Render;
var
  ri, ridx, rv, rpx, rpy, rx, ry: integer;
begin
  if show_start = 1 then
  begin
    sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
    sdl_clear(renderer_id);
    sdl_render_texture(renderer_id, start_texture, 0, 0, WIN_W, WIN_H, 0, 0, WIN_W, WIN_H);
    sdl_present(renderer_id);
    exit;
  end;

  sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
  sdl_clear(renderer_id);
  sdl_fill_rect(renderer_id, 0, 0, WIN_W, WIN_H);

  score_text := 'Score: ' + inttostr(score);

  { draw board }
  for ry := 0 to BOARD_H - 1 do
  begin
    for rx := 0 to BOARD_W - 1 do
    begin
      ridx := ry * BOARD_W + rx;
      rv := board[ridx];
      if rv = 0 then
        DrawEmpty(rx, ry)
      else
        DrawCell(rx, ry, rv);
    end;
  end;

  { draw current piece }
  for ri := 0 to 3 do
  begin
    ridx := piece_id * 16 + rotation * 4 + ri;
    rv := pieces[ridx];
    rpy := rv div 4;
    rpx := rv mod 4;
    rx := piece_x + rpx;
    ry := piece_y + rpy;
    if ry >= 0 then
      DrawCell(rx, ry, piece_id + 1);
  end;

  sdl_draw_text(renderer_id, font_id, score_text, 15, 15, 255, 255, 255, 255);
  sdl_present(renderer_id);
end;

procedure GameReset;
begin
  score := 0;
  game_over := 0;
  piece_x := 3;
  piece_y := 0;
  rotation := 0;
  last_drop := sdl_get_ticks();
  AllocClearBoard;
  SpawnPiece;
end;

procedure HandleInput;
var
  ev_type, kc, old_rot: integer;
begin
  ev := sdl_poll_event();
  while ev <> 0 do
  begin
    ev_type := sdl_get_event_type();
    if ev_type = SDL_QUIT_EVENT then
    begin
      running := 0;
      exit;
    end;
    if ev_type = SDL_KEYDOWN then
    begin
      kc := sdl_get_key_code();
      if kc = KEY_ESCAPE then
      begin
        if show_start = 1 then
        begin
          running := 0;
          exit;
        end
        else
          show_start := 1;
      end
      else if show_start = 1 then
      begin
        if kc = KEY_RETURN then
          show_start := 0;
      end
      else if kc = KEY_LEFT then
      begin
        col_result := CheckCollision(piece_x - 1, piece_y);
        if col_result = 0 then
          piece_x := piece_x - 1;
      end
      else if kc = KEY_RIGHT then
      begin
        col_result := CheckCollision(piece_x + 1, piece_y);
        if col_result = 0 then
          piece_x := piece_x + 1;
      end
      else if kc = KEY_DOWN then
        MoveDown
      else if kc = KEY_UP then
      begin
        old_rot := rotation;
        rotation := (rotation + 1) mod 4;
        col_result := CheckCollision(piece_x, piece_y);
        if col_result <> 0 then
          rotation := old_rot;
      end;
    end;
    ev := sdl_poll_event();
  end;
end;

begin
  sdl_init();
  window_id := sdl_create_window('MXVM Blocks', 100, 100, WIN_W, WIN_H, 0);
  renderer_id := sdl_create_renderer(window_id, -1, 0);

  start_texture := sdl_load_texture(renderer_id, 'blocks.bmp');
  if start_texture = -1 then
  begin
    writeln('Could not load: blocks.bmp');
    sdl_destroy_renderer(renderer_id);
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(1);
  end;

  sdl_init_text();
  font_id := sdl_load_font('font.ttf', 20);
  if font_id = -1 then
  begin
    writeln('Could not open font..');
    sdl_destroy_renderer(renderer_id);
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(1);
  end;

  seed_random();
  score := 0;
  game_over := 0;
  show_start := 1;
  running := 1;

  AllocClearBoard;
  InitPieces;
  last_drop := sdl_get_ticks();
  Render;
  SpawnPiece;

  while running = 1 do
  begin
    frame_start := sdl_get_ticks();

    if game_over = 1 then
      GameReset;

    HandleInput;

    if (running = 1) and (show_start = 0) then
    begin
      tick := frame_start - last_drop;
      if tick >= SPEED then
      begin
        last_drop := frame_start;
        MoveDown;
      end;
    end;

    if running = 1 then
      Render;

    frame_time := sdl_get_ticks() - frame_start;
    delay_time := FRAME_DELAY - frame_time;
    if delay_time > 0 then
      sdl_delay(delay_time);
  end;

  sdl_quit_text();
  sdl_destroy_renderer(renderer_id);
  sdl_destroy_window(window_id);
  sdl_quit();
end.
