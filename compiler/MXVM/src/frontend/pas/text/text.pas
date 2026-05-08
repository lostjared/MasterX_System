program AppSkeleton;
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
  title = 'Logo';
  filename = 'font.ttf';
  EXIT_FAILURE = 1;
  SCALE_W = 640;
  SCALE_H = 360;
var
running : boolean;
event_type_result: integer;
color_value_r,  color_value_g, color_value_b: integer;
window_id, renderer, target_id:  integer;
font_id: integer;

procedure render();
var
begin
    sdl_set_render_target(renderer, target_id);
    sdl_draw_text(renderer, font_id, 'Hello, World in Pascal MXVM',25, 25, 255, 255, 255, 255);
end;
procedure init(title: string; x,y,w,h: integer);
var
result: integer;
begin
    result := sdl_init();
    if result <> 0 then 
    begin
        writeln('failed to init SDL');
        halt(EXIT_FAILURE);
    end;
    window_id := sdl_create_window(title, x, y, w, h, SDL_WINDOW_SHOWN);
    if window_id = -1 then 
    begin 
        writeln('Could not create window');
        sdl_quit(); 
        halt(EXIT_FAILURE); 
    end;
    renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
    if renderer = -1 then 
    begin
        writeln('could not create  renderer');
        sdl_destroy_window(window_id);  
        sdl_quit();
        halt(EXIT_FAILURE);
    end;  
        
    target_id := sdl_create_render_target(renderer, SCALE_W, SCALE_H);
    if target_id = -1 then 
    begin 
        writeln('could not create render target');
        sdl_destroy_window(window_id); 
        sdl_destroy_renderer(renderer);
        
        halt(1); 
        sdl_quit();
        
    end;

    srand(sdl_get_ticks());
    if (sdl_init_text() <> 1) then
    begin
        writeln('Font subsystem failed to load');
        halt(EXIT_FAILURE);
    end;

    font_id := sdl_load_font(filename, 32);
    if (font_id = -1)  then  
    begin
        writeln('failed to open: ', filename);
        halt(EXIT_FAILURE);
    end;
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
            if event_type_result = SDL_QUIT then 
                running := false;
            if (event_type_result = SDL_KEYDOWN) and (sdl_get_key_code() = SDLK_ESCAPE) then 
                running := false;
        end;
        sdl_set_draw_color(renderer, 0, 0, 0, 255);
        sdl_clear(renderer);
        render();
        sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    end;
    cleanup;
end.
                
