program RandomPixels;
const
    SDL_WINDOW_SHOWN = 4;
    SDL_RENDERER_ACCELLERATED = 2;
    SDL_QUIT = 256;
    SDL_KEYDOWN = 768;
    SDLK_ESCAPE = 27;
var
    result: integer;
    window_id: integer;
    renderer: integer;
    event_type_result: integer;
    running: boolean;
    color_value_r, color_value_g, color_value_b: integer;    
    i, z: integer;
begin
    result := sdl_init();
    if (result <> 0) then
    begin
        writeln('failed to init SDL');
        halt(0);
    end
    else
        writeln('sdl init passed');

    window_id := sdl_create_window('Hello SDL', 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if (window_id = -1) then
    begin
        writeln('failed to create window');
        sdl_quit();
        halt(0);
    end
    else
        writeln('window created');

    renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
    if (renderer = -1) then
    begin
        writeln('failed to create renderer');
        sdl_destroy_window(window_id);
        sdl_quit();
        halt(0);
    end
    else
        writeln('renderer created');

    running := true;
    while running do
    begin
        while sdl_poll_event() <> 0 do
        begin
            event_type_result := sdl_get_event_type();
            if (event_type_result = SDL_QUIT) then
                running := false;
            if (event_type_result = SDL_KEYDOWN) and (sdl_get_key_code() = SDLK_ESCAPE) then
                running := false;
        end;
        sdl_set_draw_color(renderer, 0, 0, 0, 255);
        sdl_clear(renderer);
        for z := 0 to 479 do
        begin
            for i := 0 to 639 do
            begin
                color_value_r := rand() mod 255;
                color_value_g := rand() mod 255;
                color_value_b := rand() mod 255;
                sdl_set_draw_color(renderer, color_value_r, color_value_g, color_value_b, 255);
                sdl_draw_point(renderer, i, z);
            end;
        end;
        sdl_present(renderer);
        sdl_delay(1);
    end;
    sdl_destroy_renderer(renderer);
    sdl_destroy_window(window_id);
    sdl_quit();
end.
