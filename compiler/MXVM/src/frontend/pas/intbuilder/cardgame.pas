program CardGame;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT_EVENT = 256;
  SDL_KEYDOWN = 768;
  SDLK_ESCAPE = 27;
  SDLK_1 = 49;
  SDLK_2 = 50;
  SDLK_3 = 51;
  SDLK_RETURN = 13;
  WIDTH = 1280;
  HEIGHT = 720;
  SCALE_W = 1280;
  SCALE_H = 720;
  MAX_CARDS = 10;
  EXIT_FAILURE = 1;
  title = 'Intuition Builder';
  fontfile = 'font.ttf';

var
  running: boolean;
  event_type_result: integer;
  window_id, renderer, target_id: integer;
  font_id: integer;
  key: integer;
  deck: array[0..9] of integer;
  deck_index: integer;
  game_over: boolean;
  next_input: boolean;
  correct: integer;
  draw_card: integer;
  game_string: string;
  psychic_string: string;
  card_back, card1, card2, card3: integer;
  delay_timer: integer;
  delay_active: boolean;

procedure init_deck();
var
  i: integer;
begin
  for i := 0 to MAX_CARDS - 1 do
    deck[i] := (rand() mod 3) + 1;
  deck_index := 0;
end;

function cur_card(): integer;
begin
  if deck_index < MAX_CARDS then
    cur_card := deck[deck_index]
  else
    cur_card := 0;
end;

function gen_rating(): string;
begin
  if correct = 0 then
    gen_rating := 'You either have bad luck or things just are not going your way to score zero.'
  else if correct <= 3 then
    gen_rating := 'You are an average player with little psychic abilities.'
  else if correct <= 6 then
    gen_rating := 'You are above average with some mild psychic abilities.'
  else if correct <= 8 then
    gen_rating := 'You are experienced and know what you are doing.'
  else
    gen_rating := 'You are Psychic.';
end;

procedure advance_card();
begin
  draw_card := 0;
  next_input := true;
  deck_index := deck_index + 1;
  if deck_index > MAX_CARDS - 1 then
  begin
    game_string := 'Game Over Your Score: ' + inttostr(correct) + '/' + inttostr(MAX_CARDS) + ' - [Press Enter for New Game]';
    game_over := true;
    psychic_string := gen_rating();
  end
  else
    game_string := 'Guess 1-3 by pressing 1-3 keys ' + inttostr(deck_index + 1) + '/' + inttostr(MAX_CARDS);
end;

procedure handle_guess(guess: integer);
var
  card: integer;
begin
  card := cur_card();
  if card = guess then
  begin
    correct := correct + 1;
    game_string := 'Correct! Your Mental Intuition was correct.';
  end
  else
    game_string := 'Incorrect! Your Mental Intuition was incorrect.';
  draw_card := card;
  next_input := false;
  delay_active := true;
  delay_timer := sdl_get_ticks();
end;

procedure handle_key(k: integer);
begin
  if next_input = false then exit;

  if game_over = true then
  begin
    if k = SDLK_RETURN then
    begin
      game_over := false;
      init_deck();
      correct := 0;
      draw_card := 0;
      psychic_string := '';
      game_string := 'Guess 1-3 by pressing 1-3 keys ' + inttostr(deck_index + 1) + '/' + inttostr(MAX_CARDS);
    end;
  end
  else
  begin
    if k = SDLK_1 then handle_guess(1)
    else if k = SDLK_2 then handle_guess(2)
    else if k = SDLK_3 then handle_guess(3);
  end;
end;

procedure draw_background();
var
  row, col: integer;
  shade: integer;
begin
  { Dark green felt base }
  sdl_set_draw_color(renderer, 20, 70, 35, 255);
  sdl_fill_rect(renderer, 0, 0, SCALE_W, SCALE_H);

  { Subtle diamond pattern }
  sdl_set_draw_color(renderer, 25, 80, 40, 255);
  row := 0;
  while row < SCALE_H do
  begin
    col := 0;
    if (row mod 64) = 32 then col := 32;
    while col < SCALE_W do
    begin
      sdl_fill_rect(renderer, col, row, 30, 30);
      col := col + 64;
    end;
    row := row + 32;
  end;

  { Top bar }
  sdl_set_draw_color(renderer, 40, 25, 15, 255);
  sdl_fill_rect(renderer, 0, 0, SCALE_W, 55);

  { Bottom bar }
  sdl_set_draw_color(renderer, 40, 25, 15, 255);
  sdl_fill_rect(renderer, 0, SCALE_H - 30, SCALE_W, 30);

  { Gold trim lines }
  sdl_set_draw_color(renderer, 180, 140, 60, 255);
  sdl_fill_rect(renderer, 0, 55, SCALE_W, 2);
  sdl_fill_rect(renderer, 0, SCALE_H - 30, SCALE_W, 2);
end;

procedure render();
var
  i, x_pos: integer;
  card_tex: integer;
  card_w, card_h: integer;
begin
  card_w := 160;
  card_h := 240;
  sdl_set_render_target(renderer, target_id);
  draw_background();

  sdl_draw_text(renderer, font_id, 'Intuition Builder', 25, 15, 220, 190, 120, 255);
  sdl_draw_text(renderer, font_id, game_string, 280, 15, 255, 255, 255, 255);

  { Draw deck stack }
  x_pos := 80;
  for i := 0 to (MAX_CARDS - deck_index) - 1 do
  begin
    sdl_render_texture(renderer, card_back, -1, -1, -1, -1, x_pos, 100, card_w, card_h);
    x_pos := x_pos + 18;
  end;

  { Draw revealed card }
  if draw_card <> 0 then
  begin
    if draw_card = 1 then card_tex := card1
    else if draw_card = 2 then card_tex := card2
    else card_tex := card3;
    sdl_render_texture(renderer, card_tex, -1, -1, -1, -1, 800, 100, card_w, card_h);
  end;

  { Game over rating }
  if game_over = true then
  begin
    sdl_set_draw_color(renderer, 0, 0, 0, 160);
    sdl_fill_rect(renderer, 100, 400, SCALE_W - 200, 60);
    sdl_draw_text(renderer, font_id, psychic_string, 130, 415, 255, 80, 80, 255);
  end;
end;

procedure setup();
var
  result: integer;
begin
  result := sdl_init();
  if result <> 0 then
  begin
    writeln('failed to init SDL');
    halt(EXIT_FAILURE);
  end;

  window_id := sdl_create_window(title, 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
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

  font_id := sdl_load_font(fontfile, 24);
  if font_id = -1 then
  begin
    writeln('failed to open: ', fontfile);
    halt(EXIT_FAILURE);
  end;

  card_back := sdl_load_texture(renderer, 'cardback.bmp');
  card1 := sdl_load_texture(renderer, 'card1.bmp');
  card2 := sdl_load_texture(renderer, 'card2.bmp');
  card3 := sdl_load_texture(renderer, 'card3.bmp');

  if (card_back = -1) or (card1 = -1) or (card2 = -1) or (card3 = -1) then
  begin
    writeln('failed to load card textures');
    halt(EXIT_FAILURE);
  end;

  seed_random();
  init_deck();
  correct := 0;
  draw_card := 0;
  next_input := true;
  game_over := false;
  delay_active := false;
  psychic_string := '';
  game_string := 'Guess 1-3 by pressing 1-3 keys ' + inttostr(deck_index + 1) + '/' + inttostr(MAX_CARDS);
end;

procedure cleanup();
begin
  sdl_destroy_texture(card_back);
  sdl_destroy_texture(card1);
  sdl_destroy_texture(card2);
  sdl_destroy_texture(card3);
  sdl_destroy_render_target(target_id);
  sdl_destroy_renderer(renderer);
  sdl_destroy_window(window_id);
  sdl_quit_text();
  sdl_quit();
end;

begin
  setup();
  running := true;
  while running = true do
  begin
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();
      if event_type_result = SDL_QUIT_EVENT then running := false;
      if event_type_result = SDL_KEYDOWN then
      begin
        key := sdl_get_key_code();
        if key = SDLK_ESCAPE then running := false
        else handle_key(key);
      end;
    end;

    if delay_active = true then
    begin
      if (sdl_get_ticks() - delay_timer) >= 3000 then
      begin
        delay_active := false;
        advance_card();
      end;
    end;

    render();
    sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    sdl_delay(16);
  end;
  cleanup();
end.
