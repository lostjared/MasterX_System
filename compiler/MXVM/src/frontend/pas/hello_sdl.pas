program Hello;
var
    result: integer;
    window_id: integer;
    renderer: integer;
    event_type_result: integer;
    running: boolean;
    
begin
    result := sdl_init();
    if (result <> 0) then
    begin
        writeln('failed to init SDL');
        halt(0);
    end
    else
        writeln('sdl init passed');

    window_id := sdl_create_window('Hello SDL', 100, 100, 640, 480, 4);
    if (window_id = -1) then
    begin
        writeln('failed to create window');
        sdl_quit();
        halt(0);
    end
    else
        writeln('window created');

    renderer := sdl_create_renderer(window_id, -1, 2);
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
            if (event_type_result = 256) then
                running := false;
            if (event_type_result = 768) and (sdl_get_key_code() = 27) then
                running := false;
        end;

        sdl_set_draw_color(renderer, 0, 0, 0, 255);
        sdl_clear(renderer);
        sdl_present(renderer);
    end;

    sdl_destroy_renderer(renderer);
    sdl_destroy_window(window_id);
    sdl_quit();
end.
