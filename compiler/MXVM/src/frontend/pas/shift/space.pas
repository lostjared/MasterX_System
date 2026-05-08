program AppSkeleton ;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDLK_ESCAPE = 27;
  WIDTH = 1280;
  HEIGHT = 720;
  title = 'Space';
  filename = 'font.ttf';
  EXIT_FAILURE = 1;
  SCALE_W = 640;
  SCALE_H = 360;
  BLOCK_W = 22;
  BLOCK_H = 24;
  BLOCK_TOTAL = BLOCK_W * BLOCK_H;  
var
running: boolean;
event_type_result: integer;
color_value_r,  color_value_g, color_value_b: integer;
window_id, renderer, target_id:  integer;
font_id: integer;
blocks: array[0..BLOCK_TOTAL] of integer;
color: array[0..3] of integer;
function indexAt(xval, yval: integer): integer;
begin
    indexAt := yval * BLOCK_W + xval;
end;
procedure setcolor(color_index: integer);
begin
    case color_index of
        0: begin color[0] := 255;   color[1] := 255;   color[2] := 0;   end;
        1: begin color[0] := 255; color[1] := 0;   color[2] := 0;   end;
        2: begin color[0] := 0;   color[1] := 255; color[2] := 0;   end;
        3: begin color[0] := 0;   color[1] := 0;   color[2] := 255; end;
        4: begin color[0] := 255; color[1] := 255; color[2] := 255; end;
    end;
end;
procedure shift_array_left_wrap();
var
  iq, first: integer;
begin
  first := blocks[0];
  for iq := 0 to BLOCK_TOTAL-1 do
  begin
    blocks[iq] := blocks[iq+1];
  end;
  blocks[BLOCK_TOTAL] := first;
end;
procedure render();
var
    ix, iy, block: integer;
begin
    sdl_set_render_target(renderer, target_id);
    for iy := 0 to 21 do
    begin
        for ix := 0 to 20 do
        begin
            block := blocks[indexAt(ix, iy)];
            setcolor(block);
            sdl_set_draw_color(renderer, color[0], color[1], color[2], 255);
            sdl_fill_rect(renderer, ix * 32, iy * 16, 32, 16);
        end; 
    end;
    sdl_draw_text(renderer, font_id, 'Grid Shift', 15, 15, rand() mod 255, rand() mod 255, rand() mod 255, 255);
    shift_array_left_wrap();

end;
procedure init_array();
var
    index: integer;
begin
    for index := 0 to (BLOCK_TOTAL)-1 do
    begin
        blocks[index] := rand() mod 4;
    end;
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
    init_array();
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
                
