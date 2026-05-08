program RandomSquaresExpand;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDLK_ESCAPE = 27;
  WIDTH = 800;
  HEIGHT = 800;
  TILE_W = 32;
  TILE_H = 32;
var
  result, window_id, renderer, event_type_result: integer;
  running: boolean;
  color_value_r, color_value_g, color_value_b: integer;
  i, z: integer;
  max_cols, max_rows: integer;
  index_w: integer;
  index_h: integer;

procedure render(cols, rows: integer);
begin
  for z := 0 to rows do
    for i := 0 to cols do
    begin
      color_value_r := rand() mod 255;
      color_value_g := rand() mod 255;
      color_value_b := rand() mod 255;
      sdl_set_draw_color(renderer, color_value_r, color_value_g, color_value_b, 255);
      sdl_fill_rect(renderer, i * TILE_W, z * TILE_H, TILE_W, TILE_H);
    end;
end;

procedure init;
begin
  result := sdl_init();
  if result <> 0 then halt(0);
  window_id := sdl_create_window('Random Squares', 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
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
  index_w := 2;
  index_h := 2;
  max_cols :=(WIDTH div TILE_W)-1;
  max_rows := (HEIGHT div TILE_H)-1;
  init;
  running := true;
  while running do
  begin
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();
      if event_type_result = SDL_QUIT then running := false;
      if (event_type_result = SDL_KEYDOWN) and (sdl_get_key_code() = SDLK_ESCAPE) then running := false;
    end;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);
    render(index_w, index_h);
    if (index_w >= max_cols) or (index_h >= max_rows) then
    begin
      index_w := 2;
      index_h := 2;
    end
    else
    begin
      index_w := index_w + 1;
      index_h := index_h + 1;
    end;
    sdl_present(renderer);
    sdl_delay(16);
  end;
  cleanup;
end.

