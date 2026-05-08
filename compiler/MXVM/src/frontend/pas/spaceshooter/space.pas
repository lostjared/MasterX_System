program SpaceShooter;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDL_KEYUP = 769;
  SDLK_ESCAPE = 27;
  SDLK_RETURN = 13;
  SDLK_SPACE = 32;

  WIDTH = 1440;
  HEIGHT = 1080;
  SCALE_W = 1440;
  SCALE_H = 1080;

  EXIT_FAILURE = 1;
  fontfile = 'data/font.ttf';
  logofile = 'data/logo.bmp';
  spacelogofile = 'data/spacelogo.bmp';
  shipfile = 'data/ship.bmp';
  asteroidfile = 'data/asteroid.bmp';
  saucerfile = 'data/saucer.bmp';
  particlefile = 'data/particle.bmp';

  SDL_SCANCODE_LEFT = 80;
  SDL_SCANCODE_RIGHT = 79;
  SDL_SCANCODE_UP = 82;
  SDL_SCANCODE_DOWN = 81;
  SDL_SCANCODE_SPACE = 44;

  PI = 3.14159265358979323846;

  MAX_PROJECTILES = 30;
  MAX_ENEMIES = 20;
  MAX_STARS = 100;
  MAX_EXPLOSIONS = 15;
  MAX_CIRCULAR = 8;

  EXPTYPE_RED = 0;
  EXPTYPE_BLUE = 1;

  { game states }
  STATE_INTRO = 0;
  STATE_PLAYING = 1;
  STATE_DIED = 2;
  STATE_GAMEOVER = 3;

type
  Star = record
    x, y, speed: integer;
  end;

  Projectile = record
    x, y, w, h: integer;
    active: boolean;
  end;

  Enemy = record
    x, y, w, h: integer;
    active: boolean;
  end;

  CircularEnemy = record
    x, y: integer;
    angle: real;
    direction: integer;
    active: boolean;
  end;

  Explosion = record
    x, y: integer;
    duration: integer;
    cr, cg, cb: integer;
    die: boolean;
    active: boolean;
  end;

var
  running: boolean;
  window_id, renderer, target_id: integer;
  font_id: integer;
  logo_tex, spacelogo_tex: integer;
  ship_tex, asteroid_tex, saucer_tex, particle_tex: integer;
  event_type_result: integer;
  key: integer;

  game_state: integer;

  { player }
  player_x, player_y, player_w, player_h: integer;
  player_die: boolean;

  { game data }
  lives, score: integer;
  regular_enemies_destroyed: integer;
  last_shot_time: integer;
  last_spawn_time: integer;

  { intro }
  intro_alpha: integer;
  intro_fading: boolean;
  intro_timer: integer;

  { arrays - use active flag, no shifting }
  stars: array[0..MAX_STARS - 1] of Star;
  projectiles: array[0..MAX_PROJECTILES - 1] of Projectile;
  enemies: array[0..MAX_ENEMIES - 1] of Enemy;
  circular: array[0..MAX_CIRCULAR - 1] of CircularEnemy;
  circular_cx: array[0..MAX_CIRCULAR - 1] of integer;
  explosions: array[0..MAX_EXPLOSIONS - 1] of Explosion;

{ ---- collision detection ---- }
function aabb(ax, ay, aw, ah, bx, by, bw, bh: integer): boolean;
begin
  aabb := not ((ax + aw <= bx) or (bx + bw <= ax) or (ay + ah <= by) or (by + bh <= ay));
end;

{ ---- explosion helpers ---- }
procedure add_explosion(ex, ey, etype: integer; d: boolean);
var
  i: integer;
begin
  i := 0;
  while i < MAX_EXPLOSIONS do
  begin
    if explosions[i].active = false then
    begin
      explosions[i].x := ex;
      explosions[i].y := ey;
      explosions[i].duration := 60;
      explosions[i].die := d;
      explosions[i].active := true;
      if etype = EXPTYPE_RED then
      begin
        explosions[i].cr := 255;
        explosions[i].cg := 0;
        explosions[i].cb := 0;
      end
      else
      begin
        explosions[i].cr := 0;
        explosions[i].cg := 0;
        explosions[i].cb := 255;
      end;
      exit;
    end;
    i := i + 1;
  end;
end;

procedure subtract_life;
begin
  lives := lives - 1;
  if lives <= 0 then
    game_state := STATE_GAMEOVER
  else
    game_state := STATE_DIED;
  player_die := false;
end;

procedure update_explosions;
var
  i: integer;
begin
  for i := 0 to MAX_EXPLOSIONS - 1 do
  begin
    if explosions[i].active then
    begin
      explosions[i].duration := explosions[i].duration - 1;
      if explosions[i].duration <= 0 then
      begin
        explosions[i].active := false;
        if explosions[i].die then
          subtract_life;
      end;
    end;
  end;
end;

procedure draw_explosions;
var
  i, k, dx, dy: integer;
begin
  for i := 0 to MAX_EXPLOSIONS - 1 do
  begin
    if explosions[i].active then
    begin
      if explosions[i].die then
      begin
        { large player death explosion }
        for k := 0 to 39 do
        begin
          dx := (rand() mod 160) - 80;
          dy := (rand() mod 160) - 80;
          sdl_set_draw_color(renderer, 255, 200 + (rand() mod 56), rand() mod 80, 255);
          sdl_draw_line(renderer, explosions[i].x, explosions[i].y,
                        explosions[i].x + dx, explosions[i].y + dy);
        end;
        { white-hot core }
        sdl_set_draw_color(renderer, 255, 255, 200, 255);
        for k := 0 to 14 do
        begin
          dx := (rand() mod 40) - 20;
          dy := (rand() mod 40) - 20;
          sdl_draw_line(renderer, explosions[i].x + dx, explosions[i].y + dy,
                        explosions[i].x + (rand() mod 20) - 10, explosions[i].y + (rand() mod 20) - 10);
        end;
      end
      else
      begin
        { normal enemy explosion }
        sdl_set_draw_color(renderer, explosions[i].cr, explosions[i].cg, explosions[i].cb, 255);
        for k := 0 to 19 do
        begin
          dx := (rand() mod 60) - 30;
          dy := (rand() mod 60) - 30;
          sdl_draw_line(renderer, explosions[i].x, explosions[i].y,
                        explosions[i].x + dx, explosions[i].y + dy);
        end;
      end;
    end;
  end;
end;

{ ---- drawing helpers ---- }
procedure draw_gradient_triangle(x1, y1, y3: integer);
var
  i, r, g, height: integer;
begin
  if player_die then exit;
  height := y3 - y1;
  if height <= 0 then exit;
  for i := 0 to height - 1 do
  begin
    r := 255 - (255 * i div height);
    g := 255 * i div height;
    sdl_set_draw_color(renderer, r, g, 255, 255);
    sdl_draw_line(renderer, x1 - i, y1 + i, x1 + i, y1 + i);
  end;
end;

procedure draw_gradient_diamond(cx, cy, hw, hh: integer);
var
  i, r, b, g2: integer;
begin
  for i := 0 to hh - 1 do
  begin
    r := 255 - (255 * i div hh);
    b := 255 * i div hh;
    sdl_set_draw_color(renderer, r, 255, b, 255);
    sdl_draw_line(renderer, cx - i, cy - i, cx + i, cy - i);
  end;
  for i := 0 to hh - 1 do
  begin
    g2 := 255 - (255 * i div hh);
    b := 255 * i div hh;
    sdl_set_draw_color(renderer, 255, g2, b, 255);
    sdl_draw_line(renderer, cx - i, cy + i, cx + i, cy + i);
  end;
end;

procedure draw_gradient_circle(cx, cy, radius: integer);
var
  i, w, color: integer;
begin
  for i := 0 to radius - 1 do
  begin
    w := radius - i;
    color := 255 * w div radius;
    sdl_set_draw_color(renderer, color, 0, color, 255);
    sdl_draw_line(renderer, cx - w, cy - i, cx + w, cy - i);
    sdl_draw_line(renderer, cx - w, cy + i, cx + w, cy + i);
  end;
end;

{ ---- find free slot helpers ---- }
function find_free_projectile: integer;
var
  i: integer;
begin
  find_free_projectile := -1;
  for i := 0 to MAX_PROJECTILES - 1 do
    if projectiles[i].active = false then
    begin
      find_free_projectile := i;
      exit;
    end;
end;

function find_free_enemy: integer;
var
  i: integer;
begin
  find_free_enemy := -1;
  for i := 0 to MAX_ENEMIES - 1 do
    if enemies[i].active = false then
    begin
      find_free_enemy := i;
      exit;
    end;
end;

function find_free_circular: integer;
var
  i: integer;
begin
  find_free_circular := -1;
  for i := 0 to MAX_CIRCULAR - 1 do
    if circular[i].active = false then
    begin
      find_free_circular := i;
      exit;
    end;
end;

{ ---- init / reset ---- }
procedure init_stars;
var
  i: integer;
begin
  for i := 0 to MAX_STARS - 1 do
  begin
    stars[i].x := rand() mod SCALE_W;
    stars[i].y := rand() mod SCALE_H;
    stars[i].speed := 1 + (rand() mod 3);
  end;
end;

procedure clear_all_entities;
var
  i: integer;
begin
  for i := 0 to MAX_PROJECTILES - 1 do
    projectiles[i].active := false;
  for i := 0 to MAX_ENEMIES - 1 do
    enemies[i].active := false;
  for i := 0 to MAX_CIRCULAR - 1 do
    circular[i].active := false;
  for i := 0 to MAX_EXPLOSIONS - 1 do
    explosions[i].active := false;
end;

procedure reset_game;
begin
  player_x := (SCALE_W div 2) - 24;
  player_y := SCALE_H - 80;
  player_w := 48;
  player_h := 48;
  player_die := false;
  clear_all_entities;
  lives := 5;
  score := 0;
  regular_enemies_destroyed := 0;
  last_shot_time := 0;
  last_spawn_time := sdl_get_ticks();
  init_stars;
  game_state := STATE_PLAYING;
end;

procedure reset_round;
begin
  clear_all_entities;
  player_die := false;
  player_x := (SCALE_W div 2) - 24;
  player_y := SCALE_H - 80;
  game_state := STATE_PLAYING;
end;

{ ---- game update ---- }
procedure update_game;
var
  dx, dy: integer;
  ticks: integer;
  i, ei, ci: integer;
  slot: integer;
  crect_x, crect_y: integer;
  new_cx: real;
begin
  if (game_state <> STATE_PLAYING) then exit;

  update_explosions;

  { player movement via keyboard state }
  dx := 0;
  dy := 0;
  if sdl_is_key_pressed(SDL_SCANCODE_LEFT) <> 0 then dx := dx - 5;
  if sdl_is_key_pressed(SDL_SCANCODE_RIGHT) <> 0 then dx := dx + 5;
  if sdl_is_key_pressed(SDL_SCANCODE_UP) <> 0 then dy := dy - 5;
  if sdl_is_key_pressed(SDL_SCANCODE_DOWN) <> 0 then dy := dy + 5;

  player_x := player_x + dx;
  player_y := player_y + dy;

  if player_x < 0 then player_x := 0;
  if player_x > SCALE_W - player_w then player_x := SCALE_W - player_w;
  if player_y < 0 then player_y := 0;
  if player_y > SCALE_H - player_h then player_y := SCALE_H - player_h;

  { shooting }
  ticks := sdl_get_ticks();
  if (sdl_is_key_pressed(SDL_SCANCODE_SPACE) <> 0) and (ticks - last_shot_time > 300) then
  begin
    slot := find_free_projectile();
    if slot <> -1 then
    begin
      projectiles[slot].x := player_x + (player_w div 2) - 8;
      projectiles[slot].y := player_y - 30;
      projectiles[slot].w := 16;
      projectiles[slot].h := 24;
      projectiles[slot].active := true;
      last_shot_time := ticks;
    end;
  end;

  { update projectiles and check collisions }
  for i := 0 to MAX_PROJECTILES - 1 do
  begin
    if projectiles[i].active then
    begin
      projectiles[i].y := projectiles[i].y - 10;

      { off screen }
      if projectiles[i].y < -24 then
      begin
        projectiles[i].active := false;
      end
      else
      begin
        { check against regular enemies }
        for ei := 0 to MAX_ENEMIES - 1 do
        begin
          if enemies[ei].active and projectiles[i].active then
          begin
            if aabb(projectiles[i].x, projectiles[i].y, projectiles[i].w, projectiles[i].h,
                    enemies[ei].x, enemies[ei].y, enemies[ei].w, enemies[ei].h) then
            begin
              add_explosion(enemies[ei].x + enemies[ei].w div 2,
                            enemies[ei].y + enemies[ei].h div 2, EXPTYPE_RED, false);
              enemies[ei].active := false;
              projectiles[i].active := false;
              score := score + 5;
              regular_enemies_destroyed := regular_enemies_destroyed + 1;
              if (regular_enemies_destroyed mod 5) = 0 then
              begin
                slot := find_free_circular();
                if slot <> -1 then
                begin
                  circular_cx[slot] := 100 + (rand() mod (SCALE_W - 200));
                  circular[slot].x := circular_cx[slot];
                  circular[slot].y := -128;
                  circular[slot].angle := 0.0;
                  circular[slot].direction := 1;
                  circular[slot].active := true;
                end;
              end;
            end;
          end;
        end;

        { check against circular enemies }
        for ci := 0 to MAX_CIRCULAR - 1 do
        begin
          if circular[ci].active and projectiles[i].active then
          begin
            crect_x := circular[ci].x - 64;
            crect_y := circular[ci].y - 64;
            if aabb(projectiles[i].x, projectiles[i].y, projectiles[i].w, projectiles[i].h,
                    crect_x, crect_y, 128, 128) then
            begin
              add_explosion(circular[ci].x, circular[ci].y, EXPTYPE_BLUE, false);
              circular[ci].active := false;
              projectiles[i].active := false;
              score := score + 10;
            end;
          end;
        end;
      end;
    end;
  end;

  { update regular enemies }
  for ei := 0 to MAX_ENEMIES - 1 do
  begin
    if enemies[ei].active then
    begin
      enemies[ei].y := enemies[ei].y + 2;
      if enemies[ei].y > SCALE_H then
      begin
        add_explosion(enemies[ei].x + enemies[ei].w div 2, SCALE_H - 20, EXPTYPE_RED, true);
        enemies[ei].active := false;
      end
      else if aabb(player_x, player_y, player_w, player_h,
                   enemies[ei].x, enemies[ei].y, enemies[ei].w, enemies[ei].h) then
      begin
        add_explosion(player_x + player_w div 2, player_y + player_h div 2, EXPTYPE_RED, true);
        enemies[ei].active := false;
        player_die := true;
      end;
    end;
  end;

  { update circular enemies }
  for ci := 0 to MAX_CIRCULAR - 1 do
  begin
    if circular[ci].active then
    begin
      circular[ci].angle := circular[ci].angle + 0.05;
      new_cx := circular_cx[ci] + 100.0 * cos(circular[ci].angle);
      circular[ci].x := trunc(new_cx);
      circular[ci].y := circular[ci].y + circular[ci].direction * 2;
      if circular[ci].y > SCALE_H then circular[ci].direction := -1;
      if circular[ci].y < 0 then circular[ci].direction := 1;

      crect_x := circular[ci].x - 64;
      crect_y := circular[ci].y - 64;
      if aabb(player_x, player_y, player_w, player_h,
              crect_x, crect_y, 128, 128) then
      begin
        add_explosion(player_x + player_w div 2, player_y + player_h div 2, EXPTYPE_RED, true);
        circular[ci].active := false;
        player_die := true;
      end;
    end;
  end;

  { update stars }
  for i := 0 to MAX_STARS - 1 do
  begin
    stars[i].y := stars[i].y + stars[i].speed;
    if stars[i].y > SCALE_H then
    begin
      stars[i].y := 0;
      stars[i].x := rand() mod SCALE_W;
    end;
  end;

  { spawn enemies }
  if ticks - last_spawn_time > 1000 then
  begin
    slot := find_free_enemy();
    if slot <> -1 then
    begin
      enemies[slot].x := rand() mod (SCALE_W - 64);
      enemies[slot].y := -64;
      enemies[slot].w := 64;
      enemies[slot].h := 64;
      enemies[slot].active := true;
    end;
    last_spawn_time := ticks;
  end;
end;

{ ---- rendering ---- }
procedure render_game;
var
  i: integer;
  brightness: integer;
  r1, g1, b1: integer;
begin
  { draw explosions }
  draw_explosions;

  { draw stars }
  for i := 0 to MAX_STARS - 1 do
  begin
    brightness := 200 + 55 * (stars[i].speed - 1);
    if brightness > 255 then brightness := 255;
    sdl_set_draw_color(renderer, brightness, brightness, brightness, 255);
    sdl_draw_point(renderer, stars[i].x, stars[i].y);
  end;

  if game_state = STATE_PLAYING then
  begin
    { draw player ship }
    if player_die = false then
    begin
      if ship_tex <> -1 then
        sdl_render_texture(renderer, ship_tex, -1, -1, -1, -1,
                           player_x - 8, player_y - 8, 64, 64)
      else
        draw_gradient_triangle(player_x + player_w div 2, player_y,
                               player_y + player_h);
    end;

    { draw projectiles }
    for i := 0 to MAX_PROJECTILES - 1 do
      if projectiles[i].active then
      begin
        if particle_tex <> -1 then
          sdl_render_texture(renderer, particle_tex, -1, -1, -1, -1,
                             projectiles[i].x, projectiles[i].y,
                             projectiles[i].w, projectiles[i].h)
        else
        begin
          sdl_set_draw_color(renderer, 255, 255, 255, 255);
          sdl_fill_rect(renderer, projectiles[i].x, projectiles[i].y,
                        projectiles[i].w, projectiles[i].h);
        end;
      end;

    { draw regular enemies }
    for i := 0 to MAX_ENEMIES - 1 do
      if enemies[i].active then
      begin
        if asteroid_tex <> -1 then
          sdl_render_texture(renderer, asteroid_tex, -1, -1, -1, -1,
                             enemies[i].x, enemies[i].y,
                             enemies[i].w, enemies[i].h)
        else
          draw_gradient_diamond(enemies[i].x + enemies[i].w div 2,
                                enemies[i].y + enemies[i].h div 2,
                                enemies[i].w div 2, enemies[i].h div 2);
      end;

    { draw circular enemies }
    for i := 0 to MAX_CIRCULAR - 1 do
      if circular[i].active then
      begin
        if saucer_tex <> -1 then
          sdl_render_texture(renderer, saucer_tex, -1, -1, -1, -1,
                             circular[i].x - 64, circular[i].y - 64, 128, 128)
        else
          draw_gradient_circle(circular[i].x, circular[i].y, 64);
      end;
  end;

  { draw HUD }
  sdl_draw_text(renderer, font_id, 'Lives: ' + inttostr(lives), 10, 10, 255, 255, 255, 255);
  sdl_draw_text(renderer, font_id, 'Score: ' + inttostr(score), 10, 30, 255, 255, 255, 255);

  { death message }
  if game_state = STATE_DIED then
  begin
    if spacelogo_tex <> -1 then
      sdl_render_texture(renderer, spacelogo_tex, -1, -1, -1, -1, 0, 0, SCALE_W, SCALE_H);
    r1 := rand() mod 255;
    g1 := rand() mod 255;
    b1 := rand() mod 255;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_fill_rect(renderer, 470, 530, 500, 40);
    sdl_draw_text(renderer, font_id, 'You Died! Press ENTER to continue', 490, 538, r1, g1, b1, 255);
  end;

  { game over message }
  if game_state = STATE_GAMEOVER then
  begin
    if spacelogo_tex <> -1 then
      sdl_render_texture(renderer, spacelogo_tex, -1, -1, -1, -1, 0, 0, SCALE_W, SCALE_H);
    r1 := rand() mod 255;
    g1 := rand() mod 255;
    b1 := rand() mod 255;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_fill_rect(renderer, 470, 530, 500, 40);
    sdl_draw_text(renderer, font_id, 'Game Over! Press ENTER to restart', 490, 538, r1, g1, b1, 255);
  end;
end;

{ ---- intro screen ---- }
procedure render_intro;
var
  r1, g1, b1: integer;
begin
  { draw background bitmap }
  if spacelogo_tex <> -1 then
    sdl_render_texture(renderer, spacelogo_tex, -1, -1, -1, -1, 0, 0, 1440, 1080)
  else
  begin
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);
  end;

  { draw logo bitmap }
  if logo_tex <> -1 then
    sdl_render_texture(renderer, logo_tex, -1, -1, -1, -1, 0, 0, SCALE_W, SCALE_H);

  r1 := rand() mod 255;
  g1 := rand() mod 255;
  b1 := rand() mod 255;

  sdl_draw_text(renderer, font_id, 'Press ENTER to Start', 640, 700, r1, g1, b1, 255);
  sdl_draw_text(renderer, font_id, 'Arrow Keys = Move   Space = Shoot', 584, 740, 180, 180, 180, 255);
end;

{ ---- SDL init / cleanup ---- }
procedure init_sdl;
var
  result: integer;
begin
  result := sdl_init();
  if result <> 0 then
  begin
    writeln('failed to init SDL');
    halt(EXIT_FAILURE);
  end;

  window_id := sdl_create_window('Space Shooter', 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if window_id = -1 then
  begin
    writeln('Could not create window');
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
  if renderer = -1 then
  begin
    writeln('Could not create renderer');
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  target_id := sdl_create_render_target(renderer, SCALE_W, SCALE_H);
  if target_id = -1 then
  begin
    writeln('Could not create render target');
    sdl_destroy_renderer(renderer);
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  sdl_set_draw_color(renderer, 0, 0, 0, 255);

  if sdl_init_text() <> 1 then
  begin
    writeln('Font subsystem failed to load');
    halt(EXIT_FAILURE);
  end;

  font_id := sdl_load_font(fontfile, 14);
  if font_id = -1 then
  begin
    writeln('Failed to open: ', fontfile);
    halt(EXIT_FAILURE);
  end;

  logo_tex := sdl_load_texture(renderer, logofile);
  if logo_tex = -1 then
    writeln('Warning: could not load ', logofile);

  spacelogo_tex := sdl_load_texture(renderer, spacelogofile);
  if spacelogo_tex = -1 then
    writeln('Warning: could not load ', spacelogofile);

  ship_tex := sdl_load_texture_color_key(renderer, shipfile);
  if ship_tex = -1 then
    writeln('Warning: could not load ', shipfile);

  asteroid_tex := sdl_load_texture_color_key(renderer, asteroidfile);
  if asteroid_tex = -1 then
    writeln('Warning: could not load ', asteroidfile);

  saucer_tex := sdl_load_texture_color_key(renderer, saucerfile);
  if saucer_tex = -1 then
    writeln('Warning: could not load ', saucerfile);

  particle_tex := sdl_load_texture_color_key(renderer, particlefile);
  if particle_tex = -1 then
    writeln('Warning: could not load ', particlefile);
end;

procedure cleanup;
begin
  if ship_tex <> -1 then sdl_destroy_texture(ship_tex);
  if asteroid_tex <> -1 then sdl_destroy_texture(asteroid_tex);
  if saucer_tex <> -1 then sdl_destroy_texture(saucer_tex);
  if particle_tex <> -1 then sdl_destroy_texture(particle_tex);
  if logo_tex <> -1 then sdl_destroy_texture(logo_tex);
  if spacelogo_tex <> -1 then sdl_destroy_texture(spacelogo_tex);
  sdl_destroy_render_target(target_id);
  sdl_destroy_renderer(renderer);
  sdl_destroy_window(window_id);
  sdl_quit_text();
  sdl_quit();
end;

{ ---- main loop ---- }
var
  prev_time, cur_time: integer;

begin
  init_sdl;
  seed_random;
  game_state := STATE_INTRO;
  lives := 5;
  score := 0;
  clear_all_entities;
  init_stars;

  prev_time := sdl_get_ticks();
  running := true;

  while running = true do
  begin
    { poll events }
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();

      if event_type_result = SDL_QUIT then
        running := false;

      if event_type_result = SDL_KEYDOWN then
      begin
        key := sdl_get_key_code();
        if key = SDLK_ESCAPE then
          running := false;

        if key = SDLK_RETURN then
        begin
          if game_state = STATE_INTRO then
            reset_game
          else if game_state = STATE_DIED then
            reset_round
          else if game_state = STATE_GAMEOVER then
            reset_game;
        end;
      end;
    end;

    { timed update at ~60fps }
    cur_time := sdl_get_ticks();
    if cur_time - prev_time >= 15 then
    begin
      prev_time := cur_time;
      if game_state = STATE_PLAYING then
        update_game;
    end;

    { render }
    sdl_set_render_target(renderer, target_id);
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);

    if game_state = STATE_INTRO then
      render_intro
    else
      render_game;

    sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    sdl_delay(16);
  end;

  cleanup;
end.
