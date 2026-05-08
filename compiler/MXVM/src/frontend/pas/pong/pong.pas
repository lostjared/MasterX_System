program Pong;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDL_KEYUP = 769;
  SDLK_ESCAPE = 27;

  WIDTH = 1280;
  HEIGHT = 720;

  title = 'Pong';
  filename = 'font.ttf';
  EXIT_FAILURE = 1;

  SCALE_W = 640;
  SCALE_H = 360;

  SDL_SCANCODE_DOWN = 81;
  SDL_SCANCODE_UP = 82;

  PADDLE_W = 6;
  PADDLE_H = 56;
  PADDLE_STEP = 4;
  PADDLE_STEP_AI = 4;

  BALL_W = 6;
  BALL_H = 6;
  BALL_SPEED_X = 4;
  BALL_SPEED_Y = 4;

type
  Point = record
    x: integer;
    y: integer;
  end;

var
  running: boolean;
  event_type_result: integer;
  window_id, renderer, target_id: integer;
  font_id: integer;
  key: integer;
  paddle1, paddle2: Point;
  ball: Point;
  ball_dx, ball_dy: integer;
  score_l, score_r: integer;

procedure draw_box(px, py, pw, ph: integer);
begin
  sdl_fill_rect(renderer, px, py, pw, ph);
end;

procedure reset_ball(to_right: boolean);
begin
  ball.x := (SCALE_W div 2) - (BALL_W div 2);
  ball.y := (SCALE_H div 2) - (BALL_H div 2);
  if to_right = true then ball_dx := BALL_SPEED_X else ball_dx := -BALL_SPEED_X;
  if (sdl_get_ticks() mod 2) = 0 then ball_dy := BALL_SPEED_Y else ball_dy := -BALL_SPEED_Y;
end;

procedure clamp_paddle(p: Point);
begin 
  if p.y < 0 then p.y := 0;
  if p.y + PADDLE_H > SCALE_H then p.y := SCALE_H - PADDLE_H;
end;

function aabb(ax, ay, aw, ah, bx, by, bw, bh: integer): boolean;
begin
  aabb := not ((ax + aw <= bx) or (bx + bw <= ax) or (ay + ah <= by) or (by + bh <= ay));
end;

procedure update_input();
begin
  if sdl_is_key_pressed(SDL_SCANCODE_UP) <> 0 then paddle1.y := paddle1.y - PADDLE_STEP;
  if sdl_is_key_pressed(SDL_SCANCODE_DOWN) <> 0 then paddle1.y := paddle1.y + PADDLE_STEP;
    clamp_paddle(paddle1); 
end;

procedure update_ai();
var
  center_ball, center_p2: integer;
begin
  center_ball := ball.y + (BALL_H div 2);
  center_p2 := paddle2.y + (PADDLE_H div 2);
  if center_ball < center_p2 - 4 then paddle2.y := paddle2.y - PADDLE_STEP_AI
  else if center_ball > center_p2 + 4 then paddle2.y := paddle2.y + PADDLE_STEP_AI;
  clamp_paddle(paddle2);
end;

procedure update_ball();
begin
  ball.x := ball.x + ball_dx;
  ball.y := ball.y + ball_dy;

  if ball.y <= 0 then begin ball.y := 0; ball_dy := -ball_dy; end;
  if ball.y + BALL_H >= SCALE_H then begin ball.y := SCALE_H - BALL_H; ball_dy := -ball_dy; end;

  if aabb(ball.x, ball.y, BALL_W, BALL_H, paddle1.x, paddle1.y, PADDLE_W, PADDLE_H) then
  begin
    ball.x := paddle1.x + PADDLE_W;
    ball_dx := -ball_dx;
  end;

 if aabb(ball.x, ball.y, BALL_W, BALL_H, paddle2.x, paddle2.y, PADDLE_W, PADDLE_H) then
  begin
    ball.x := paddle2.x - BALL_W;
    ball_dx := -ball_dx;
  end;

  if ball.x + BALL_W < 0 then
  begin
    score_r := score_r + 1;
    reset_ball(true);
  end;

  if ball.x > SCALE_W then
  begin
    score_l := score_l + 1;
    reset_ball(false);
  end; 
end;

procedure render_center_line();
var
  yy: integer;
begin
  yy := 0;
  while yy < SCALE_H do
  begin
    draw_box((SCALE_W div 2) - 1, yy, 2, 8);
    yy := yy + 16;
  end;
end;

procedure render();
begin
  sdl_set_render_target(renderer, target_id);
  sdl_set_draw_color(renderer, 0, 0, 0, 255);
  sdl_clear(renderer);

  sdl_set_draw_color(renderer, 255, 255, 255, 255);
  render_center_line();

  draw_box(paddle1.x, paddle1.y, PADDLE_W, PADDLE_H);
  draw_box(paddle2.x, paddle2.y, PADDLE_W, PADDLE_H);
  draw_box(ball.x, ball.y, BALL_W, BALL_H);

  sdl_draw_text(renderer, font_id, 'Score: ' + inttostr(score_l), (SCALE_W div 2) - 100, 8, 255, 255, 255, 255);
  sdl_draw_text(renderer, font_id, 'Score: ' + inttostr(score_r), (SCALE_W div 2) + 20, 8, 255, 255, 255, 255);
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

  paddle1.x := 16;
  paddle1.y := (SCALE_H div 2) - (PADDLE_H div 2);
  paddle2.x := SCALE_W - 16 - PADDLE_W;
  paddle2.y := (SCALE_H div 2) - (PADDLE_H div 2);

  reset_ball(true);

  score_l := 0;
  score_r := 0;
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
  init(title, 100, 100, WIDTH, HEIGHT);
  running := true;
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
    end;

    update_input();
    update_ai();
    update_ball();
    render();
    sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    sdl_delay(16);
  end;
  cleanup;
end.
