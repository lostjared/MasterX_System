program Skeleton;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDLK_ESCAPE = 27;
  WIDTH = 1280;
  HEIGHT = 720;
  TILE_W = 32;
  TILE_H = 32;
  GRID_W = (WIDTH div TILE_W);
  GRID_H = (HEIGHT div TILE_H);
  title = 'Hello, World';
var
running : boolean;
event_type_result: integer;
color_value_r,  color_value_g, color_value_b: integer;
window_id, renderer:  integer;
procedure render();
var
i, z: integer;
begin
  for z := 0 to GRID_H do
  begin
    for i := 0 to GRID_W do
    begin
      sdl_set_draw_color(renderer, rand() mod 255, rand() mod 255, rand() mod 255, 255);
      sdl_fill_rect(renderer, i * TILE_W, z * TILE_H, TILE_W, TILE_H);
    end;
  end;
end;
procedure init(title: string; x,y,w,h: integer);
var
result: integer;
begin
  result := sdl_init();
  if result <> 0 then halt(0);
  window_id := sdl_create_window(title, x, y, w, h, SDL_WINDOW_SHOWN);
  if window_id = -1 then begin sdl_quit(); halt(0); end;
  renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
  if renderer = -1 then begin sdl_destroy_window(window_id); sdl_quit(); halt(0); end;
  srand(sdl_get_ticks());
end;
procedure cleanup;
begin
  sdl_destroy_renderer(renderer);
  sdl_destroy_window(window_id);
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
      if (event_type_result = SDL_KEYDOWN) and (sdl_get_key_code() = SDLK_ESCAPE) then running := false;
    end;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);
    render();
    sdl_present(renderer);
 end;
cleanup;
end.
                
