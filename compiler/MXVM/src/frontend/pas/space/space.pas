program Space;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDL_KEYUP = 769;
  SDLK_ESCAPE = 27;
  WIDTH = 1280;
  HEIGHT = 720;
  title = 'Space';
  filename = 'font.ttf';
  EXIT_FAILURE = 1;
  SCALE_W = 640;
  SCALE_H = 360;

  SDLK_LEFT  = 1073741904;
  SDLK_RIGHT = 1073741903;
  SDLK_UP    = 1073741906;

  PI = 3.14159265358979323846;

type
  Ship = record
    x, y: real;
    vx, vy: real;
    angle: real; { 0 = pointing up }
    lives: integer;
  end;

var
  running: boolean;
  event_type_result: integer;
  window_id, renderer, target_id: integer;
  font_id: integer;
  key: integer;
  the_ship: Ship;
  keyLeft, keyRight, keyThrust: boolean;

procedure init_space;
begin
  the_ship.x := SCALE_W / 2;
  the_ship.y := SCALE_H / 2;
  the_ship.vx := 0;
  the_ship.vy := 0;
  the_ship.angle := 0; { pointing up }
  the_ship.lives := 3;
end;



procedure draw_ship;
var
  { Local model points (lx, ly) }
  lx1, ly1, lx2, ly2, lx3, ly3: real;
  x1, y1, x2, y2, x3, y3: integer;
  c, s: real;
begin
  lx1 := 0;  ly1 := -12;  { nose }
  lx2 := -8; ly2 := 8;    { left rear }
  lx3 := 8;  ly3 := 8;    { right rear }

  c := cos(the_ship.angle);
  s := sin(the_ship.angle);

  x1 := trunc(the_ship.x + (lx1 * c - ly1 * s));
  y1 := trunc(the_ship.y + (lx1 * s + ly1 * c));
  x2 := trunc(the_ship.x + (lx2 * c - ly2 * s));
  y2 := trunc(the_ship.y + (lx2 * s + ly2 * c));
  x3 := trunc(the_ship.x + (lx3 * c - ly3 * s));
  y3 := trunc(the_ship.y + (lx3 * s + ly3 * c));

  sdl_set_draw_color(renderer, 255, 255, 255, 255);
  sdl_draw_line(renderer, x1, y1, x2, y2);
  sdl_draw_line(renderer, x2, y2, x3, y3);
  sdl_draw_line(renderer, x3, y3, x1, y1);

  if keyThrust then
  begin
    { Flame (triangle) }
    sdl_set_draw_color(renderer, 255, 160, 0, 255);
    { Flame points in local space: (-4,8) (0,16) (4,8) }
    x1 := trunc(the_ship.x + ((-4)*c - 8*s));
    y1 := trunc(the_ship.y + ((-4)*s + 8*c));
    x2 := trunc(the_ship.x + (0*c - 16*s));
    y2 := trunc(the_ship.y + (0*s + 16*c));
    x3 := trunc(the_ship.x + (4*c - 8*s));
    y3 := trunc(the_ship.y + (4*s + 8*c));
    sdl_draw_line(renderer, x1, y1, x2, y2);
    sdl_draw_line(renderer, x2, y2, x3, y3);
  end;
end;

procedure update_ship;
var
  accel, drag: real;
begin
  if keyLeft then
    the_ship.angle := the_ship.angle - 0.08;
  if keyRight then
    the_ship.angle := the_ship.angle + 0.08;

  accel := 0.14;
  if keyThrust then
  begin
    the_ship.vx := the_ship.vx + sin(the_ship.angle) * accel; { note: y grows downward, so rotate axes }
    the_ship.vy := the_ship.vy - cos(the_ship.angle) * accel;
  end;

  drag := 0.995;
  the_ship.vx := the_ship.vx * drag;
  the_ship.vy := the_ship.vy * drag;

  the_ship.x := the_ship.x + the_ship.vx;
  the_ship.y := the_ship.y + the_ship.vy;

  { Screen wrap }
  if the_ship.x < 0 then the_ship.x := the_ship.x + SCALE_W;
  if the_ship.x >= SCALE_W then the_ship.x := the_ship.x - SCALE_W;
  if the_ship.y < 0 then the_ship.y := the_ship.y + SCALE_H;
  if the_ship.y >= SCALE_H then the_ship.y := the_ship.y - SCALE_H;
end;

procedure render;
begin
  sdl_set_render_target(renderer, target_id);
  sdl_set_draw_color(renderer, 0, 0, 0, 255);
  sdl_clear(renderer);

  draw_ship;
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

  keyLeft := false;
  keyRight := false;
  keyThrust := false;
  init_space;
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
        if key = SDLK_LEFT then keyLeft := true;
        if key = SDLK_RIGHT then keyRight := true;
        if key = SDLK_UP then keyThrust := true;
      end;
      if event_type_result = SDL_KEYUP then
      begin
        key := sdl_get_key_code();
        if key = SDLK_LEFT then keyLeft := false;
        if key = SDLK_RIGHT then keyRight := false;
        if key = SDLK_UP then keyThrust := false;
      end;
    end;

    update_ship;
    render;
    sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    sdl_delay(16);
  end;
  cleanup;
end.
