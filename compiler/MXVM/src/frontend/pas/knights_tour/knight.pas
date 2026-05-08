{ Knight's Tour -- SDL2 game
  Uses Warnsdorff's heuristic with backtracking to find a
  Hamiltonian path on an 8x8 chess board.  The player watches
  the pre-computed tour unfold one move at a time. }
program KnightsTour;

const
  BOARD_SIZE = 8;
  TOTAL_MOVES = 65;       { 64 squares + 1 (move_count starts at 1) }
  START_X = 140;           { board left edge in pixels }
  START_Y = 40;            { board top edge (leaves room for HUD text) }
  CELL_SIZE = 85;          { step between cell origins }
  CELL_DRAW_SIZE = 80;     { drawn square size (5 px gap) }
  KNIGHT_SIZE = 60;        { knight sprite size }

  WIN_W = 960;
  WIN_H = 720;

  SDL_QUIT_EVENT = 256;        { SDL event constants }
  SDL_KEYDOWN = 768;
  SDL_MOUSEBUTTONDOWN = 1025;
  SDLK_ESCAPE = 27;            { key codes }
  SDLK_RETURN = 13;

  SDL_SCANCODE_SPACE = 44;     { scancode for space bar polling }

  STATE_INTRO = 0;             { game state: showing intro screen }
  STATE_PLAYING = 1;           { game state: tour in progress }

  MAX_SEQ = 64;

var
  window_id, renderer_id: integer;
  font_id: integer;
  knight_tex: integer;
  logo_tex: integer;
  running: boolean;
  game_state: integer;

  { board stored as flat array: board[row * BOARD_SIZE + col] }
  board: array[0..63] of integer;

  { move sequence stored as separate row/col arrays }
  seq_row: array[0..63] of integer;
  seq_col: array[0..63] of integer;
  seq_count: integer;

  knight_row, knight_col: integer;
  moves: integer;
  tour_over: boolean;

  { knight move offsets }
  horizontal: array[0..7] of integer;
  vertical: array[0..7] of integer;

  { sorting scratch arrays for Warnsdorff's heuristic }
  sort_deg: array[0..7] of integer;
  sort_row: array[0..7] of integer;
  sort_col: array[0..7] of integer;

  { intro timing }
  intro_start: integer;

{ Populate the eight L-shaped knight move offset tables }
procedure init_move_offsets;
begin
  horizontal[0] := 2;  vertical[0] := -1;
  horizontal[1] := 1;  vertical[1] := -2;
  horizontal[2] := -1; vertical[2] := -2;
  horizontal[3] := -2; vertical[3] := -1;
  horizontal[4] := -2; vertical[4] := 1;
  horizontal[5] := -1; vertical[5] := 2;
  horizontal[6] := 1;  vertical[6] := 2;
  horizontal[7] := 2;  vertical[7] := 1;
end;

{ Zero every cell on the board }
procedure clear_board;
var
  i: integer;
begin
  for i := 0 to 63 do
    board[i] := 0;
end;

{ Read the board value at (row, col) }
function get_board(row, col: integer): integer;
begin
  get_board := board[row * BOARD_SIZE + col];
end;

{ Write val into the board at (row, col) }
procedure set_board(row, col, val: integer);
begin
  board[row * BOARD_SIZE + col] := val;
end;

{ Return 1 if (row, col) is on-board and unvisited, 0 otherwise.
  Uses early exit instead of compound 'and' which does not
  short-circuit in MXVM Pascal. }
function is_valid_move(row, col: integer): integer;
begin
  if (row < 0) or (row >= BOARD_SIZE) then
  begin
    is_valid_move := 0;
    exit;
  end;
  if (col < 0) or (col >= BOARD_SIZE) then
  begin
    is_valid_move := 0;
    exit;
  end;
  if get_board(row, col) <> 0 then
    is_valid_move := 0
  else
    is_valid_move := 1;
end;

{ Count valid onward moves from (row, col) -- Warnsdorff degree }
function get_degree(row, col: integer): integer;
var
  i, nr, nc, count: integer;
begin
  count := 0;
  for i := 0 to 7 do
  begin
    nr := row + vertical[i];
    nc := col + horizontal[i];
    if is_valid_move(nr, nc) = 1 then
      count := count + 1;
  end;
  get_degree := count;
end;

{ Selection sort the candidate moves by ascending degree.
  Warnsdorff's rule: always visit the square with fewest onward
  moves first, which dramatically reduces backtracking. }
procedure sort_moves(n: integer);
var
  i, j, min_idx, tmp: integer;
begin
  { selection sort by degree (ascending) }
  i := 0;
  while i < n - 1 do
  begin
    min_idx := i;
    j := i + 1;
    while j < n do
    begin
      if sort_deg[j] < sort_deg[min_idx] then
        min_idx := j;
      j := j + 1;
    end;
    if min_idx <> i then
    begin
      tmp := sort_deg[i];
      sort_deg[i] := sort_deg[min_idx];
      sort_deg[min_idx] := tmp;
      tmp := sort_row[i];
      sort_row[i] := sort_row[min_idx];
      sort_row[min_idx] := tmp;
      tmp := sort_col[i];
      sort_col[i] := sort_col[min_idx];
      sort_col[min_idx] := tmp;
    end;
    i := i + 1;
  end;
end;

{ Recursively solve the knight's tour via depth-first search
  with Warnsdorff-sorted candidates.  Records each move in the
  seq_row / seq_col arrays for later playback.
  Returns 1 on success, 0 on dead end (triggers backtracking). }
function solve_tour(pos_row, pos_col, move_count: integer): integer;
var
  i, nr, nc, n: integer;
begin
  if move_count = TOTAL_MOVES then
  begin
    solve_tour := 1;
    exit;
  end;

  n := 0;
  for i := 0 to 7 do
  begin
    nr := pos_row + vertical[i];
    nc := pos_col + horizontal[i];
    if is_valid_move(nr, nc) = 1 then
    begin
      sort_deg[n] := get_degree(nr, nc);
      sort_row[n] := nr;
      sort_col[n] := nc;
      n := n + 1;
    end;
  end;

  sort_moves(n);

  i := 0;
  while i < n do
  begin
    set_board(sort_row[i], sort_col[i], move_count);
    seq_row[seq_count] := sort_row[i];
    seq_col[seq_count] := sort_col[i];
    seq_count := seq_count + 1;

    if solve_tour(sort_row[i], sort_col[i], move_count + 1) = 1 then
    begin
      solve_tour := 1;
      exit;
    end
    else
    begin
      set_board(sort_row[i], sort_col[i], 0);
      seq_count := seq_count - 1;
    end;
    i := i + 1;
  end;

  solve_tour := 0;
end;

{ Generate a fresh tour from a random starting square }
procedure reset_tour;
var
  dummy: integer;
begin
  clear_board;
  knight_row := rand() mod BOARD_SIZE;
  knight_col := rand() mod BOARD_SIZE;
  set_board(knight_row, knight_col, 1);
  seq_count := 0;
  seq_row[0] := knight_row;
  seq_col[0] := knight_col;
  seq_count := 1;
  dummy := solve_tour(knight_row, knight_col, 2);
  moves := 1;
  tour_over := false;
end;

{ Advance the knight one step along the pre-computed tour.
  Marks the old square as visited (-1) and places the knight
  on the next square.  No-op if the tour is already complete. }
procedure next_move;
var
  nr, nc: integer;
begin
  if moves >= 64 then
    exit;
  if seq_count <= 0 then
    exit;
  if tour_over then
    exit;
  nr := seq_row[moves];
  nc := seq_col[moves];
  set_board(knight_row, knight_col, -1);
  knight_row := nr;
  knight_col := nc;
  moves := moves + 1;
  set_board(knight_row, knight_col, moves);
  if moves >= 64 then
    tour_over := true;
end;

{ Draw the 8x8 checkerboard.  Visited squares are black,
  unvisited squares alternate white and red. }
procedure draw_board;
var
  i, j, rx, ry: integer;
begin
  for i := 0 to BOARD_SIZE - 1 do
  begin
    for j := 0 to BOARD_SIZE - 1 do
    begin
      rx := START_X + j * CELL_SIZE;
      ry := START_Y + i * CELL_SIZE;

      if get_board(i, j) = -1 then
        sdl_set_draw_color(renderer_id, 0, 0, 0, 255)
      else if (i + j) mod 2 = 0 then
        sdl_set_draw_color(renderer_id, 255, 255, 255, 255)
      else
        sdl_set_draw_color(renderer_id, 255, 0, 0, 255);

      sdl_fill_rect(renderer_id, rx, ry, CELL_DRAW_SIZE, CELL_DRAW_SIZE);
    end;
  end;
end;

{ Render the knight sprite centered on its current cell }
procedure draw_knight;
var
  dx, dy: integer;
begin
  dx := START_X + knight_col * CELL_SIZE + 10;
  dy := START_Y + knight_row * CELL_SIZE + 10;
  sdl_render_texture(renderer_id, knight_tex, -1, -1, -1, -1,
                     dx, dy, KNIGHT_SIZE, KNIGHT_SIZE);
end;

{ main event loop }
var
  ev_type, kc: integer;
  frame_start, frame_time, delay_time: integer;
  cur_time: integer;
  mouse_btn: integer;
  space_was_down: integer;
  space_now: integer;
  tour_ready: integer;
  last_move_time: integer;
  nr, nc: integer;

begin
  sdl_init();
  window_id := sdl_create_window('Knights Tour', 100, 100, WIN_W, WIN_H, 0);
  renderer_id := sdl_create_renderer(window_id, -1, 0);

  sdl_init_text();
  font_id := sdl_load_font('data/font.ttf', 14);
  if font_id = -1 then
    writeln('Could not open font..');

  { load knight bitmap with white (255,255,255) as transparent color key }
  knight_tex := sdl_load_texture_color_key_rgb(renderer_id, 'data/knight.bmp', 255, 255, 255);
  if knight_tex = -1 then
    writeln('Warning: could not load knight.bmp');

  logo_tex := sdl_load_texture(renderer_id, 'data/logo.bmp');
  if logo_tex = -1 then
    writeln('Warning: could not load logo.bmp');

  seed_random;
  init_move_offsets;

  game_state := STATE_INTRO;
  intro_start := sdl_get_ticks();
  running := true;
  space_was_down := 0;
  tour_ready := 0;
  last_move_time := 0;

  while running do
  begin
    frame_start := sdl_get_ticks();

    { poll events }
    while sdl_poll_event() <> 0 do
    begin
      ev_type := sdl_get_event_type();

      if ev_type = SDL_QUIT_EVENT then
        running := false;

      if ev_type = SDL_KEYDOWN then
      begin
        kc := sdl_get_key_code();
        if kc = SDLK_ESCAPE then
          running := false
        else if game_state = STATE_INTRO then
        begin
          game_state := STATE_PLAYING;
          if tour_ready = 0 then
          begin
            reset_tour;
            tour_ready := 1;
          end;
        end
        else
        begin
          if kc = SDLK_RETURN then
            reset_tour;
        end;
      end;

      if (ev_type = SDL_MOUSEBUTTONDOWN) then
      begin
        if game_state = STATE_INTRO then
        begin
          game_state := STATE_PLAYING;
          if tour_ready = 0 then
          begin
            reset_tour;
            tour_ready := 1;
          end;
        end
        else
        begin
          mouse_btn := sdl_get_mouse_button();
          if mouse_btn = 1 then
          begin
            next_move;
          end
          else if mouse_btn = 3 then
            reset_tour;
        end;
      end;
    end;

    { space bar: first press moves immediately, holding repeats every 150ms }
    if game_state = STATE_PLAYING then
    begin
      space_now := sdl_is_key_pressed(SDL_SCANCODE_SPACE);
      if space_now <> 0 then
      begin
        cur_time := sdl_get_ticks();
        if (space_was_down = 0) or (cur_time - last_move_time >= 150) then
        begin
          next_move;
          last_move_time := cur_time;
        end;
      end;
      space_was_down := space_now;
    end;

    { render }
    sdl_set_draw_color(renderer_id, 0, 0, 0, 255);
    sdl_clear(renderer_id);

    if game_state = STATE_INTRO then
    begin
      { show logo briefly then transition }
      if logo_tex <> -1 then
        sdl_render_texture(renderer_id, logo_tex, -1, -1, -1, -1,
                           0, 0, WIN_W, WIN_H);
      sdl_draw_text(renderer_id, font_id,
        'Press any key to start',
        380, 680, 255, 255, 255, 255);
      cur_time := sdl_get_ticks();
      if cur_time - intro_start >= 2000 then
      begin
        game_state := STATE_PLAYING;
        if tour_ready = 0 then
        begin
          reset_tour;
          tour_ready := 1;
        end;
      end;
    end
    else
    begin
      draw_board;
      draw_knight;

      if moves < TOTAL_MOVES then
      begin
        sdl_draw_text(renderer_id, font_id,
          'Knights Tour - Tap Space, Press Return to Reset',
          15, 5, 255, 255, 255, 255);
        sdl_draw_text(renderer_id, font_id,
          'Moves: ' + inttostr(moves),
          400, 5, 255, 255, 255, 255);
      end
      else
        sdl_draw_text(renderer_id, font_id,
          '-[ Tour Complete ]- Press Return to Reset',
          15, 5, 255, 255, 255, 255);
    end;

    sdl_present(renderer_id);

    frame_time := sdl_get_ticks() - frame_start;
    delay_time := 16 - frame_time;
    if delay_time > 0 then
      sdl_delay(delay_time);
  end;

  { clean up SDL resources }
  if knight_tex <> -1 then sdl_destroy_texture(knight_tex);
  if logo_tex <> -1 then sdl_destroy_texture(logo_tex);
  sdl_quit_text();
  sdl_destroy_renderer(renderer_id);
  sdl_destroy_window(window_id);
  sdl_quit();
end.
