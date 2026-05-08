{ MasterPiece - Main program with game loop and input handling }
program MasterPiece;

uses BlockTypes, PieceUnit, GridUnit, GameUI;

var
  running: integer;
  ev: integer;
  ev_type: integer;
  kc: integer;
  load_result: integer;

procedure HandleIntroInput;
begin
  if ev_type = SDL_KEYDOWN then
  begin
    kc := sdl_get_key_code();
    if kc = SDLK_RETURN then
      GameUI.current_screen := SCREEN_START
    else if kc = SDLK_ESCAPE then
      GameUI.current_screen := SCREEN_START;
  end;
end;

procedure HandleStartInput;
begin
  if ev_type = SDL_KEYDOWN then
  begin
    kc := sdl_get_key_code();
    if kc = SDLK_UP then
    begin
      if GameUI.cursor_pos > 0 then
        GameUI.cursor_pos := GameUI.cursor_pos - 1;
    end
    else if kc = SDLK_DOWN then
    begin
      if GameUI.cursor_pos < 3 then
        GameUI.cursor_pos := GameUI.cursor_pos + 1;
    end
    else if kc = SDLK_RETURN then
    begin
      if GameUI.cursor_pos = 0 then
      begin
        GridUnit.ResetGame;
        GameUI.current_screen := SCREEN_GAME;
      end
      else if GameUI.cursor_pos = 1 then
        GameUI.current_screen := SCREEN_OPTIONS
      else if GameUI.cursor_pos = 2 then
        GameUI.current_screen := SCREEN_CREDITS
      else if GameUI.cursor_pos = 3 then
        running := 0;
    end
    else if kc = SDLK_ESCAPE then
      running := 0;
  end;
end;

procedure HandleGameInput;
begin
  if ev_type = SDL_KEYDOWN then
  begin
    kc := sdl_get_key_code();
    if kc = SDLK_ESCAPE then
      GameUI.current_screen := SCREEN_START
    else if (kc = SDLK_p) or (kc = SDLK_P_UPPER) then
      GameUI.paused := 1 - GameUI.paused
    else if GameUI.paused = 0 then
    begin
      if GridUnit.game_over = 1 then
      begin
        if kc = SDLK_RETURN then
        begin
          GameUI.current_screen := SCREEN_HIGHSCORES;
        end;
      end
      else
      begin
        if kc = SDLK_LEFT then
        begin
          if GridUnit.CanMoveLeft() = 1 then
            PieceUnit.PieceMoveLeft;
        end
        else if kc = SDLK_RIGHT then
        begin
          if GridUnit.CanMoveRight() = 1 then
            PieceUnit.PieceMoveRight;
        end
        else if kc = SDLK_DOWN then
          GridUnit.DropPiece
        else if (kc = SDLK_UP) or (kc = SDLK_s) or (kc = SDLK_S_UPPER) then
          PieceUnit.PieceShiftUp
        else if (kc = SDLK_a) or (kc = SDLK_A_UPPER) then
          PieceUnit.PieceShiftDown
        else if kc = SDLK_SPACE then
          GridUnit.HardDrop
        else if kc = SDLK_RETURN then
        begin
          if GridUnit.CanRotate() = 1 then
            PieceUnit.PieceRotate;
        end;
      end;
    end;
  end;
end;

procedure HandleOptionsInput;
begin
  if ev_type = SDL_KEYDOWN then
  begin
    kc := sdl_get_key_code();
    if kc = SDLK_ESCAPE then
      GameUI.current_screen := SCREEN_START
    else if kc = SDLK_RIGHT then
    begin
      if GameUI.opt_lines < 15 then
        GameUI.opt_lines := GameUI.opt_lines + 1;
    end
    else if kc = SDLK_LEFT then
    begin
      if GameUI.opt_lines > 5 then
        GameUI.opt_lines := GameUI.opt_lines - 1;
    end
    else if kc = SDLK_RETURN then
    begin
      GridUnit.lines_per_speed := GameUI.opt_lines;
      GameUI.current_screen := SCREEN_START;
    end;
  end;
end;

procedure HandleCreditsInput;
begin
  if ev_type = SDL_KEYDOWN then
  begin
    kc := sdl_get_key_code();
    if kc = SDLK_ESCAPE then
      GameUI.current_screen := SCREEN_START;
  end;
end;

procedure HandleHighScoresInput;
begin
  if ev_type = SDL_KEYDOWN then
  begin
    kc := sdl_get_key_code();
    if kc = SDLK_RETURN then
      GameUI.current_screen := SCREEN_START
    else if kc = SDLK_ESCAPE then
      GameUI.current_screen := SCREEN_START;
  end;
end;

procedure HandleInput;
begin
  ev := sdl_poll_event();
  while ev <> 0 do
  begin
    ev_type := sdl_get_event_type();
    if ev_type = SDL_QUIT_EVENT then
    begin
      running := 0;
      exit;
    end;
    if GameUI.current_screen = SCREEN_INTRO then
      HandleIntroInput
    else if GameUI.current_screen = SCREEN_START then
      HandleStartInput
    else if GameUI.current_screen = SCREEN_GAME then
      HandleGameInput
    else if GameUI.current_screen = SCREEN_OPTIONS then
      HandleOptionsInput
    else if GameUI.current_screen = SCREEN_CREDITS then
      HandleCreditsInput
    else if GameUI.current_screen = SCREEN_HIGHSCORES then
      HandleHighScoresInput;
    ev := sdl_poll_event();
  end;
end;

begin
  seed_random();
  GameUI.InitSDL;
  load_result := GameUI.LoadTextures();
  if load_result <> 0 then
  begin
    writeln('Failed to load textures, exiting.');
    GameUI.ShutdownSDL;
    halt(1);
  end;
  GridUnit.ResetGame;
  running := 1;
  while running = 1 do
  begin
    HandleInput;
    if (GameUI.current_screen = SCREEN_GAME) and (GameUI.paused = 0) then
      GridUnit.UpdateGame;
    GameUI.RenderFrame;
    sdl_delay(FRAME_DELAY);
  end;
  writeln('MasterPiece: Goodbye');
  GameUI.ShutdownSDL;
end.
