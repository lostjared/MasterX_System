{ PuzzleDrop - SDL2 MXVM Pascal port }
program PuzzleDrop;

uses BlockTypes, PieceUnit, GridUnit;

const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT_EVENT = 256;
  SDL_KEYDOWN = 768;
  SDLK_ESCAPE = 27;
  SDLK_a = 97;
  SDLK_s = 115;
  SDLK_n = 110;
  SDLK_UP = 1073741906;
  SDL_SCANCODE_LEFT  = 80;
  SDL_SCANCODE_RIGHT = 79;
  SDL_SCANCODE_DOWN  = 81;
  SDL_SCANCODE_A = 4;
  SDL_SCANCODE_S = 22;
  SDL_SCANCODE_Z = 29;
  SDL_SCANCODE_X = 27;
  KEY_REPEAT_MS = 100;
  WIN_W = 1280;
  WIN_H = 720;
  BLOCK_W = 64;
  BLOCK_H = 32;
  EXIT_FAILURE = 1;
  FONT_FILE = 'font.ttf';
  FONT_SIZE = 28;
  DROP_INTERVAL = 1000;
  INTRO_DURATION = 2500;

var
  window_id, renderer: integer;
  font_id: integer;
  logo_tex: integer;
  running: integer;
  game_started: integer;
  event_type_result: integer;
  key_code: integer;

  block_tex: array[0..8] of integer;
  bg_tex: array[0..7] of integer;

  last_drop_tick: integer;
  last_proc_tick: integer;
  last_render_tick: integer;
  last_key_tick: integer;
  cur_tick: integer;
  drop_speed: integer;

procedure LoadTextures;
var
  i: integer;
begin
  block_tex[0] := sdl_load_texture(renderer, 'img/red1.bmp');
  block_tex[1] := sdl_load_texture(renderer, 'img/red2.bmp');
  block_tex[2] := sdl_load_texture(renderer, 'img/red3.bmp');
  block_tex[3] := sdl_load_texture(renderer, 'img/green1.bmp');
  block_tex[4] := sdl_load_texture(renderer, 'img/green2.bmp');
  block_tex[5] := sdl_load_texture(renderer, 'img/green3.bmp');
  block_tex[6] := sdl_load_texture(renderer, 'img/blue1.bmp');
  block_tex[7] := sdl_load_texture(renderer, 'img/blue2.bmp');
  block_tex[8] := sdl_load_texture(renderer, 'img/blue3.bmp');
  for i := 0 to 8 do
  begin
    if block_tex[i] = -1 then
    begin
      writeln('Failed to load block texture ', i);
      halt(EXIT_FAILURE);
    end;
  end;
  bg_tex[0] := sdl_load_texture(renderer, 'img/level1.bmp');
  bg_tex[1] := sdl_load_texture(renderer, 'img/level2.bmp');
  bg_tex[2] := sdl_load_texture(renderer, 'img/level3.bmp');
  bg_tex[3] := sdl_load_texture(renderer, 'img/level4.bmp');
  bg_tex[4] := sdl_load_texture(renderer, 'img/level5.bmp');
  bg_tex[5] := sdl_load_texture(renderer, 'img/level6.bmp');
  bg_tex[6] := sdl_load_texture(renderer, 'img/level7.bmp');
  bg_tex[7] := sdl_load_texture(renderer, 'img/level8.bmp');
  for i := 0 to 7 do
  begin
    if bg_tex[i] = -1 then
    begin
      writeln('Failed to load background texture ', i);
      halt(EXIT_FAILURE);
    end;
  end;
end;

procedure DestroyTextures;
var
  i: integer;
begin
  for i := 0 to 8 do
    sdl_destroy_texture(block_tex[i]);
  for i := 0 to 7 do
    sdl_destroy_texture(bg_tex[i]);
  sdl_destroy_texture(logo_tex);
end;

procedure Render;
var
  x, y, idx, btype, image, flash_phase: integer;
  b1, b2, b3: integer;
begin
  sdl_set_draw_color(renderer, 0, 0, 0, 255);
  sdl_clear(renderer);

  { Draw background for current level }
  sdl_render_texture(renderer, bg_tex[GridUnit.GetLevel() - 1], -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);

  { Draw grid blocks - direct array access to avoid function call overhead }
  for x := 0 to GRID_W - 1 do
  begin
    for y := 0 to GRID_H - 1 do
    begin
      idx := y * GRID_W + x;
      btype := GridUnit.grid_blocks[idx];
      if btype = BLOCK_NULL then
      begin
        sdl_set_draw_color(renderer, 0, 0, 0, 255);
        sdl_fill_rect(renderer, x * BLOCK_W + 1, y * BLOCK_H + 1, BLOCK_W - 1, BLOCK_H - 1);
      end
      else if btype = BLOCK_CLEAR then
      begin
        flash_phase := GridUnit.grid_flash[idx] mod 6;
        if flash_phase < 3 then
        begin
          { Show original block }
          image := GridUnit.grid_original[idx] - FIRST_COLOR;
          if (image >= 0) and (image <= 8) then
            sdl_render_texture(renderer, block_tex[image], -1, -1, -1, -1,
              x * BLOCK_W, y * BLOCK_H, BLOCK_W, BLOCK_H)
          else
          begin
            sdl_set_draw_color(renderer, 255, 255, 255, 255);
            sdl_fill_rect(renderer, x * BLOCK_W, y * BLOCK_H, BLOCK_W, BLOCK_H);
          end;
        end
        else
        begin
          { Show random colored rectangle }
          sdl_set_draw_color(renderer, rand() mod 256, rand() mod 256, rand() mod 256, 255);
          sdl_fill_rect(renderer, x * BLOCK_W, y * BLOCK_H, BLOCK_W, BLOCK_H);
        end;
      end
      else if (btype >= FIRST_COLOR) and (btype <= LAST_COLOR) then
      begin
        image := btype - FIRST_COLOR;
        sdl_render_texture(renderer, block_tex[image], -1, -1, -1, -1,
          x * BLOCK_W, y * BLOCK_H, BLOCK_W, BLOCK_H);
      end
      else if btype = BLOCK_MATCH then
      begin
        image := rand() mod 9;
        sdl_render_texture(renderer, block_tex[image], -1, -1, -1, -1,
          x * BLOCK_W, y * BLOCK_H, BLOCK_W, BLOCK_H);
      end;
    end;
  end;

  { Draw active piece }
  b1 := PieceUnit.piece_type[0] - FIRST_COLOR;
  b2 := PieceUnit.piece_type[1] - FIRST_COLOR;
  b3 := PieceUnit.piece_type[2] - FIRST_COLOR;
  if PieceUnit.piece_type[0] = BLOCK_MATCH then b1 := rand() mod 9;
  if PieceUnit.piece_type[1] = BLOCK_MATCH then b2 := rand() mod 9;
  if PieceUnit.piece_type[2] = BLOCK_MATCH then b3 := rand() mod 9;
  sdl_render_texture(renderer, block_tex[b1], -1, -1, -1, -1,
    PieceUnit.piece_x[0] * BLOCK_W, PieceUnit.piece_y[0] * BLOCK_H, BLOCK_W, BLOCK_H);
  sdl_render_texture(renderer, block_tex[b2], -1, -1, -1, -1,
    PieceUnit.piece_x[1] * BLOCK_W, PieceUnit.piece_y[1] * BLOCK_H, BLOCK_W, BLOCK_H);
  sdl_render_texture(renderer, block_tex[b3], -1, -1, -1, -1,
    PieceUnit.piece_x[2] * BLOCK_W, PieceUnit.piece_y[2] * BLOCK_H, BLOCK_W, BLOCK_H);

  { Draw HUD text }
  if game_started = 0 then
  begin
    sdl_draw_text(renderer, font_id, 'Puzzle Drop', WIN_W div 2 - 120, WIN_H div 3, 255, 255, 255, 255);
    sdl_draw_text(renderer, font_id, 'Press N to Start', WIN_W div 2 - 160, WIN_H div 3 + 48, 255, 255, 255, 255);
  end
  else
  begin
    sdl_draw_text(renderer, font_id, 'Level: ' + inttostr(GridUnit.GetLevel()) + '  Lines: ' + inttostr(GridUnit.GetLines()),
      16, 8, 255, 255, 255, 255);
  end;

  if GridUnit.IsGameOver() = 1 then
  begin
    sdl_draw_text(renderer, font_id, 'GAME OVER', WIN_W div 2 - 100, WIN_H div 2, 255, 80, 80, 255);
    sdl_draw_text(renderer, font_id, 'Press N for New Game', WIN_W div 2 - 180, WIN_H div 2 + 48, 255, 255, 255, 255);
  end;
end;

procedure NewGame;
begin
  GridUnit.GridClearAll;
  game_started := 1;
  drop_speed := DROP_INTERVAL;
  last_drop_tick := sdl_get_ticks();
  last_proc_tick := sdl_get_ticks();
  last_render_tick := sdl_get_ticks();
  last_key_tick := sdl_get_ticks();
end;

procedure HandleKey(k: integer);
begin
  if k = SDLK_n then
  begin
    NewGame;
    exit;
  end;
  if game_started = 0 then exit;
  if GridUnit.IsGameOver() = 1 then exit;

  if (k = SDLK_a) or (k = SDLK_UP) then GridUnit.GridKeyShiftUp
  else if k = SDLK_s then GridUnit.GridKeyShiftDown;
end;

procedure Init;
var
  result: integer;
begin
  result := sdl_init();
  if result <> 0 then
  begin
    writeln('Failed to init SDL');
    halt(EXIT_FAILURE);
  end;

  window_id := sdl_create_window('PuzzleDrop', 100, 100, WIN_W, WIN_H, SDL_WINDOW_SHOWN);
  if window_id = -1 then
  begin
    writeln('Could not create window');
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
  if renderer = -1 then
  begin
    writeln('Could not create renderer');
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  if sdl_init_text() <> 1 then
  begin
    writeln('Font subsystem failed to load');
    halt(EXIT_FAILURE);
  end;

  font_id := sdl_load_font(FONT_FILE, FONT_SIZE);
  if font_id = -1 then
  begin
    writeln('Failed to load font: ', FONT_FILE);
    halt(EXIT_FAILURE);
  end;

  seed_random;
  LoadTextures;

  logo_tex := sdl_load_texture(renderer, 'img/logo.bmp');
  if logo_tex = -1 then
  begin
    writeln('Failed to load logo texture');
    halt(EXIT_FAILURE);
  end;

  GridUnit.GridClearAll;
  game_started := 0;
  running := 1;
  drop_speed := DROP_INTERVAL;
  last_drop_tick := sdl_get_ticks();
  last_proc_tick := sdl_get_ticks();
  last_render_tick := sdl_get_ticks();
  last_key_tick := sdl_get_ticks();
end;

procedure Cleanup;
begin
  DestroyTextures;
  sdl_destroy_renderer(renderer);
  sdl_destroy_window(window_id);
  sdl_quit_text();
  sdl_quit();
end;

begin
  Init;

  { Show intro logo for 5 seconds }
  cur_tick := sdl_get_ticks();
  last_render_tick := cur_tick;
  while (sdl_get_ticks() - cur_tick) < INTRO_DURATION do
  begin
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();
      if event_type_result = SDL_QUIT_EVENT then
      begin
        running := 0;
        cur_tick := sdl_get_ticks() - INTRO_DURATION;
      end;
      if event_type_result = SDL_KEYDOWN then
      begin
        key_code := sdl_get_key_code();
        if key_code = SDLK_ESCAPE then
        begin
          running := 0;
          cur_tick := sdl_get_ticks() - INTRO_DURATION;
        end;
      end;
    end;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);
    sdl_render_texture(renderer, logo_tex, -1, -1, -1, -1, 0, 0, WIN_W, WIN_H);
    sdl_present(renderer);
    sdl_delay(33);
  end;

  while running = 1 do
  begin
    { Drain all events but only keep the last key press }
    key_code := 0;
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();
      if event_type_result = SDL_QUIT_EVENT then
        running := 0;
      if event_type_result = SDL_KEYDOWN then
      begin
        key_code := sdl_get_key_code();
        if key_code = SDLK_ESCAPE then
          running := 0;
      end;
    end;
    { Apply the last key once per frame }
    if (key_code <> 0) and (key_code <> SDLK_ESCAPE) then
      HandleKey(key_code);
    cur_tick := sdl_get_ticks();

    { Poll keyboard state for movement keys (no event queue lag) }
    if (game_started = 1) and (GridUnit.IsGameOver() = 0) then
    begin
      if (cur_tick - last_key_tick) >= KEY_REPEAT_MS then
      begin
        if sdl_is_key_pressed(SDL_SCANCODE_LEFT) <> 0 then
        begin GridUnit.GridKeyLeft; last_key_tick := cur_tick; end
        else if sdl_is_key_pressed(SDL_SCANCODE_RIGHT) <> 0 then
        begin GridUnit.GridKeyRight; last_key_tick := cur_tick; end
        else if sdl_is_key_pressed(SDL_SCANCODE_DOWN) <> 0 then
        begin GridUnit.GridKeyDown; last_key_tick := cur_tick; end
        else if sdl_is_key_pressed(SDL_SCANCODE_Z) <> 0 then
        begin GridUnit.GridKeyRotateLeft; last_key_tick := cur_tick; end
        else if sdl_is_key_pressed(SDL_SCANCODE_X) <> 0 then
        begin GridUnit.GridKeyRotateRight; last_key_tick := cur_tick; end;
      end;
    end;

    { Auto drop piece on timer }
    if (game_started = 1) and (GridUnit.IsGameOver() = 0) then
    begin
      if (cur_tick - last_drop_tick) >= drop_speed then
      begin
        GridUnit.GridKeyDown;
        last_drop_tick := cur_tick;
        if GridUnit.IsGameOver() = 1 then
          game_started := 0;
      end;
      { Background processing: match clearing and gravity }
      if (cur_tick - last_proc_tick) >= 2 then
      begin
        GridUnit.ProcBlocks();
        GridUnit.ProcMoveDown();
        last_proc_tick := cur_tick;
      end;
    end;

    { Only render at ~30fps to reduce interpreter load }
    Render;
    sdl_present(renderer);
    last_render_tick := cur_tick;
    sdl_delay(16);
  end;
  Cleanup;
end.
