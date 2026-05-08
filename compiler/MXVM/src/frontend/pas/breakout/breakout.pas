program Breakout;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDL_KEYUP = 769;
  SDLK_ESCAPE = 27;

  WIDTH = 1280;
  HEIGHT = 720;
  SCALE_W = 640;
  SCALE_H = 360;

  SDL_SCANCODE_LEFT = 80;
  SDL_SCANCODE_RIGHT = 79;

  GRID_COLS = 10;
  GRID_ROWS = 6;
  BLOCK_W = 60;
  BLOCK_H = 16;
  BLOCK_SPACING_X = 4;
  BLOCK_SPACING_Y = 4;
  GRID_OFFSET_X = 2;
  GRID_OFFSET_Y = 40;

  PADDLE_W = 72;
  PADDLE_H = 10;
  PADDLE_STEP = 6;

  BALL_W = 6;
  BALL_H = 6;
  BALL_SPEED_X = 3;
  BALL_SPEED_Y = 3;

  EXIT_FAILURE = 1;
  filename = 'font.ttf';

type
  Rect = record
    x: integer;
    y: integer;
    w: integer;
    h: integer;
  end;

var
  running: boolean;
  window_id, renderer, target_id: integer;
  font_id: integer;
  event_type_result: integer;
  key: integer;

  paddle: Rect;
  ball: Rect;
  ball_dx: integer;
  ball_dy: integer;

  blocks: array[0..(GRID_COLS*GRID_ROWS)-1] of integer;
  lives: integer;
  score: integer;

procedure draw_box(px, py, pw, ph: integer);
begin
  sdl_fill_rect(renderer, px, py, pw, ph);
end;

function aabb(ax, ay, aw, ah, bx, by, bw, bh: integer): boolean;
begin
  aabb := not ((ax + aw <= bx) or (bx + bw <= ax) or (ay + ah <= by) or (by + bh <= ay));
end;

procedure reset_ball_paddle;
begin
  paddle.x := (SCALE_W div 2) - (PADDLE_W div 2);
  paddle.y := SCALE_H - 24;
  paddle.w := PADDLE_W;
  paddle.h := PADDLE_H;
  ball.x := paddle.x + (PADDLE_W div 2) - (BALL_W div 2);
  ball.y := paddle.y - BALL_H - 2;
  ball.w := BALL_W;
  ball.h := BALL_H;
  if (sdl_get_ticks() mod 2) = 0 then ball_dx := BALL_SPEED_X else ball_dx := -BALL_SPEED_X;
  ball_dy := -BALL_SPEED_Y;
end;

procedure reset_level;
var
  x, y, i: integer;
begin
  i := 0;
  for y := 0 to GRID_ROWS-1 do
  begin
    for x := 0 to GRID_COLS-1 do
    begin
      blocks[i] :=1 + (rand() mod 3);
      i := i + 1;
    end;
  end;
end;

procedure clamp_paddle;
begin
  if paddle.x < 0 then paddle.x := 0;
  if paddle.x + paddle.w > SCALE_W then paddle.x := SCALE_W - paddle.w;
end;

procedure update_input;
begin
  if sdl_is_key_pressed(SDL_SCANCODE_LEFT) <> 0 then paddle.x := paddle.x - PADDLE_STEP;
  if sdl_is_key_pressed(SDL_SCANCODE_RIGHT) <> 0 then paddle.x := paddle.x + PADDLE_STEP;
  clamp_paddle;
end;

procedure update_ball;
var
  nx, ny: integer;
  i, gx, gy: integer;
  bx, by, bw, bh: integer;
  hitOnce: integer;
  anyLeft: integer;
  topHit, bottomHit: integer;
begin
  nx := ball.x + ball_dx;
  ny := ball.y + ball_dy;

  if ny <= 0 then
  begin
    ny := 0;
    ball_dy := -ball_dy;
  end;

  if nx <= 0 then
  begin
    nx := 0;
    ball_dx := -ball_dx;
  end;

  if nx + ball.w >= SCALE_W then
  begin
    nx := SCALE_W - ball.w;
    ball_dx := -ball_dx;
  end;

  if ny + ball.h >= SCALE_H then
  begin
    lives := lives - 1;
    if lives <= 0 then
    begin
      lives := 3;
      score := 0;
      reset_level;
    end;
    reset_ball_paddle;
    exit;
  end;

  if aabb(nx, ny, ball.w, ball.h, paddle.x, paddle.y, paddle.w, paddle.h) then
  begin
    ny := paddle.y - ball.h - 1;
    ball_dy := -ball_dy;
    if nx + (ball.w div 2) < paddle.x + (paddle.w div 2) then
    begin
      if ball_dx > 0 then ball_dx := -ball_dx;
    end
    else
    begin
      if ball_dx < 0 then ball_dx := -ball_dx;
    end;
  end;

  hitOnce := 0;
  i := 0;
  while i < GRID_COLS * GRID_ROWS do
  begin
    if hitOnce = 0 then
    begin
      if blocks[i] >= 1 then
      begin
        gy := i div GRID_COLS;
        gx := i - gy * GRID_COLS;
        bx := GRID_OFFSET_X + gx * (BLOCK_W + BLOCK_SPACING_X);
        by := GRID_OFFSET_Y + gy * (BLOCK_H + BLOCK_SPACING_Y);
        bw := BLOCK_W;
        bh := BLOCK_H;

        if aabb(nx, ny, ball.w, ball.h, bx, by, bw, bh) then
        begin
          blocks[i] := 0;
          score := score + 10;
          topHit := 0;
          bottomHit := 0;
          if ball.y + ball.h <= by then topHit := 1;
          if ball.y >= by + bh then bottomHit := 1;
          if (topHit >= 1) or (bottomHit >= 1) then
            ball_dy := -ball_dy
          else
            ball_dx := -ball_dx;
          hitOnce := 1;
        end;
      end;
    end;
    i := i + 1;
  end;

  ball.x := nx;
  ball.y := ny;

  anyLeft := 0;
  i := 0;
  while i < GRID_COLS * GRID_ROWS do
  begin
    if blocks[i] >= 1 then anyLeft := 1;
    i := i + 1;
  end;
  if anyLeft = 0 then
  begin
    reset_level;
    reset_ball_paddle;
  end;
end;

procedure setcolor(index: integer);
begin

case index of 
1: sdl_set_draw_color(renderer, 255, 0, 0,255);
2: sdl_set_draw_color(renderer, 0, 255, 0,255);
3: sdl_set_draw_color(renderer, 0, 0, 255,255);
else
sdl_set_draw_color(renderer, 255, 255, 255, 255);
end;



end;

procedure render;
var
  i, gx, gy, bx, by: integer;
begin
  sdl_set_render_target(renderer, target_id);
  sdl_set_draw_color(renderer, 0, 0, 0, 255);
  sdl_clear(renderer);

  sdl_set_draw_color(renderer, 255, 255, 255, 255);
  draw_box(paddle.x, paddle.y, paddle.w, paddle.h);
  draw_box(ball.x, ball.y, BALL_W, BALL_H);

  sdl_set_draw_color(renderer, 200, 200, 200, 255);
  i := 0;
  while i < GRID_COLS * GRID_ROWS do
  begin
    if blocks[i] >= 1 then
    begin
      gy := i div GRID_COLS;
      gx := i - gy * GRID_COLS;
      bx := GRID_OFFSET_X + gx * (BLOCK_W + BLOCK_SPACING_X);
      by := GRID_OFFSET_Y + gy * (BLOCK_H + BLOCK_SPACING_Y);
      setcolor(blocks[i]);
      draw_box(bx, by, BLOCK_W, BLOCK_H);
    end;
    i := i + 1;
  end;

  sdl_draw_text(renderer, font_id, 'Score: ' + inttostr(score), 8, 8, 255, 255, 255, 255);
  sdl_draw_text(renderer, font_id, 'Lives: ' + inttostr(lives), 120, 8, 255, 255, 255, 255);
end;

procedure init(title: string; xval, yval, wval, hval: integer);
var
  result: integer;
begin
  result := sdl_init();
  if result <> 0 then
  begin
    writeln('failed to init SDL');
    halt(EXIT_FAILURE);
  end;

  window_id := sdl_create_window(title, xval, yval, wval, hval, SDL_WINDOW_SHOWN);
  if window_id = -1 then
  begin
    writeln('Could not create window');
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
  if renderer = -1 then
  begin
    writeln('could not create renderer');
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  target_id := sdl_create_render_target(renderer, SCALE_W, SCALE_H);
  if target_id = -1 then
  begin
    writeln('could not create render target');
    sdl_destroy_renderer(renderer);
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  if sdl_init_text() <> 1 then
  begin
    writeln('Font subsystem failed to load');
    halt(EXIT_FAILURE);
  end;

  font_id := sdl_load_font(filename, 16);
  if font_id = -1 then
  begin
    writeln('failed to open: ', filename);
    halt(EXIT_FAILURE);
  end;

  lives := 3;
  score := 0;
  reset_level;
  reset_ball_paddle;

  running := true;
end;

procedure cleanup;
begin
  sdl_destroy_render_target(target_id);
  sdl_destroy_renderer(renderer);
  sdl_destroy_window(window_id);
  sdl_quit_text();
  sdl_quit();
end;

begin
  init('Breakout', 100, 100, WIDTH, HEIGHT);
  while running = true do
  begin
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();
      if event_type_result = SDL_QUIT then running := false;
      if event_type_result = SDL_KEYDOWN then
      begin
        key := sdl_get_key_code();
        if key = SDLK_ESCAPE then running := false;
      end;
      if event_type_result = SDL_KEYUP then
      begin
        key := sdl_get_key_code();
      end;
    end;

    update_input;
    update_ball;
    render;
    sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    sdl_delay(16);
  end;
  cleanup;
end.
