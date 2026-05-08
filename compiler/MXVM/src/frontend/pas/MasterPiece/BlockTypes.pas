{ BlockTypes - Block type constants and shared definitions for MasterPiece }
unit BlockTypes;

interface

const
  BLOCK_BLACK  = 0;
  BLOCK_YELLOW = 1;
  BLOCK_ORANGE = 2;
  BLOCK_LTBLUE = 3;
  BLOCK_DBLUE  = 4;
  BLOCK_PURPLE = 5;
  BLOCK_PINK   = 6;
  BLOCK_GRAY   = 7;
  BLOCK_RED    = 8;
  BLOCK_GREEN  = 9;
  BLOCK_CLEAR  = 10;

  GRID_COLS = 8;
  GRID_ROWS = 17;
  GRID_SIZE = 136;

  CELL_W = 32;
  CELL_H = 16;
  BOARD_OX = 185;
  BOARD_OY = 95;

  NEXT_OX = 510;
  NEXT_OY = 200;

  WIN_W = 640;
  WIN_H = 480;
  TARGET_W = 1440;
  TARGET_H = 1080;

  NUM_COLORS = 9;
  FRAME_DELAY = 8;
  INITIAL_SPEED = 80;
  MIN_SPEED = 6;
  SPEED_DECREASE = 4;
  LINES_PER_LEVEL = 10;
  POINTS_PER_LINE = 6;
  FLASH_DURATION = 50;

  SCREEN_INTRO      = 0;
  SCREEN_START      = 1;
  SCREEN_GAME       = 2;
  SCREEN_OPTIONS    = 3;
  SCREEN_CREDITS    = 4;
  SCREEN_HIGHSCORES = 5;

  SDL_QUIT_EVENT = 256;
  SDL_KEYDOWN    = 768;
  SDLK_ESCAPE    = 27;
  SDLK_RETURN    = 13;
  SDLK_LEFT      = 1073741904;
  SDLK_RIGHT     = 1073741903;
  SDLK_DOWN      = 1073741905;
  SDLK_UP        = 1073741906;
  SDLK_SPACE     = 32;
  SDLK_a         = 97;
  SDLK_s         = 115;
  SDLK_p         = 112;
  SDLK_A_UPPER   = 65;
  SDLK_S_UPPER   = 83;
  SDLK_P_UPPER   = 80;

  NUM_HIGHSCORES = 8;
  FONT_FILE = 'img/font.ttf';
  FONT_SIZE = 14;

implementation

end.
