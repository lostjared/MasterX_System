{ GridUnit - Grid and game logic for PuzzleDrop }
unit GridUnit;

interface

uses BlockTypes, PieceUnit;

var
  grid_blocks: array[0..439] of integer;
  grid_clear: array[0..439] of integer;
  grid_flash: array[0..439] of integer;
  grid_original: array[0..439] of integer;
  game_level: integer;
  game_lines: integer;
  game_over: integer;

function GridIndex(x, y: integer): integer;
function GridGet(x, y: integer): integer;
procedure GridSet(x, y, val: integer);
function GridGetClear(x, y: integer): integer;
procedure GridSetClear(x, y, val: integer);
function GridGetFlash(x, y: integer): integer;
procedure GridSetFlash(x, y, val: integer);
procedure GridClearAll;
function GridInBounds(x, y: integer): integer;
function CheckPieceAt(px0, py0, px1, py1, px2, py2: integer): integer;
function CheckPieceDelta(dx, dy: integer): integer;
procedure SetPieceOnGrid;
procedure GridKeyLeft;
procedure GridKeyRight;
procedure GridKeyDown;
procedure GridKeyShiftUp;
procedure GridKeyShiftDown;
procedure GridKeyRotateLeft;
procedure GridKeyRotateRight;
function CheckBlock(x, y, btype: integer): integer;
procedure ClearBlock(x, y, expected: integer);
procedure AddScore;
function ProcBlocks: integer;
function ProcMoveDown: integer;
function IsGameOver: integer;
function GetLevel: integer;
function GetLines: integer;
function BlockName(b: integer): string;
procedure LogMatch(dir: string; x1, y1, x2, y2, x3, y3: integer);

implementation

function GridIndex(x, y: integer): integer;
begin
  GridIndex := y * GRID_W + x;
end;

function GridInBounds(x, y: integer): integer;
begin
  if (x >= 0) and (x < GRID_W) and (y >= 0) and (y < GRID_H) then
    GridInBounds := 1
  else
    GridInBounds := 0;
end;

function GridGet(x, y: integer): integer;
begin
  if GridInBounds(x, y) = 1 then
    GridGet := grid_blocks[GridIndex(x, y)]
  else
    GridGet := -1;
end;

procedure GridSet(x, y, val: integer);
begin
  if GridInBounds(x, y) = 1 then
    grid_blocks[GridIndex(x, y)] := val;
end;

function GridGetClear(x, y: integer): integer;
begin
  if GridInBounds(x, y) = 1 then
    GridGetClear := grid_clear[GridIndex(x, y)]
  else
    GridGetClear := 0;
end;

procedure GridSetClear(x, y, val: integer);
begin
  if GridInBounds(x, y) = 1 then
    grid_clear[GridIndex(x, y)] := val;
end;

function GridGetFlash(x, y: integer): integer;
begin
  if GridInBounds(x, y) = 1 then
    GridGetFlash := grid_flash[GridIndex(x, y)]
  else
    GridGetFlash := 0;
end;

procedure GridSetFlash(x, y, val: integer);
begin
  if GridInBounds(x, y) = 1 then
    grid_flash[GridIndex(x, y)] := val;
end;

procedure GridClearAll;
var
  i: integer;
begin
  for i := 0 to GRID_SIZE - 1 do
  begin
    grid_blocks[i] := BLOCK_NULL;
    grid_clear[i] := 0;
    grid_flash[i] := 0;
    grid_original[i] := BLOCK_NULL;
  end;
  game_level := 1;
  game_lines := 0;
  game_over := 0;
  PieceUnit.PieceNew(GRID_W div 2, 0);
end;

function CheckPieceAt(px0, py0, px1, py1, px2, py2: integer): integer;
var
  b: integer;
begin
  CheckPieceAt := 1;
  b := GridGet(px0, py0);
  if (b = -1) or ((b <> BLOCK_NULL) and (b <> BLOCK_CLEAR)) then
  begin
    CheckPieceAt := 0;
    exit;
  end;
  b := GridGet(px1, py1);
  if (b = -1) or ((b <> BLOCK_NULL) and (b <> BLOCK_CLEAR)) then
  begin
    CheckPieceAt := 0;
    exit;
  end;
  b := GridGet(px2, py2);
  if (b = -1) or ((b <> BLOCK_NULL) and (b <> BLOCK_CLEAR)) then
  begin
    CheckPieceAt := 0;
  end;
end;

function CheckPieceDelta(dx, dy: integer): integer;
begin
  CheckPieceDelta := CheckPieceAt(
    PieceUnit.piece_x[0] + dx, PieceUnit.piece_y[0] + dy,
    PieceUnit.piece_x[1] + dx, PieceUnit.piece_y[1] + dy,
    PieceUnit.piece_x[2] + dx, PieceUnit.piece_y[2] + dy);
end;

procedure SetPieceOnGrid;
var
  i, px, py: integer;
begin
  for i := 0 to 2 do
  begin
    px := PieceUnit.piece_x[i];
    py := PieceUnit.piece_y[i];
    if GridInBounds(px, py) = 1 then
    begin
      GridSet(px, py, PieceUnit.piece_type[i]);
      GridSetClear(px, py, 0);
      GridSetFlash(px, py, 0);
    end;
    if py = 0 then
      game_over := 1;
  end;
end;

procedure GridKeyLeft;
begin
  if CheckPieceDelta(-1, 0) = 1 then
    PieceUnit.PieceMoveLeft;
end;

procedure GridKeyRight;
begin
  if CheckPieceDelta(1, 0) = 1 then
    PieceUnit.PieceMoveRight;
end;

procedure GridKeyDown;
var
  cd: integer;
begin
  cd := CheckPieceDelta(0, 1);

  if cd = 1 then
    PieceUnit.PieceMoveDown
  else
  begin
    SetPieceOnGrid;
    PieceUnit.PieceNew(GRID_W div 2, 0);
  end;
end;

procedure GridKeyShiftUp;
begin
  PieceUnit.PieceShiftUp;
end;

procedure GridKeyShiftDown;
begin
  PieceUnit.PieceShiftDown;
end;

procedure GridKeyRotateLeft;
var
  save_x0, save_y0, save_x1, save_y1, save_x2, save_y2, save_pos: integer;
begin
  save_x0 := PieceUnit.piece_x[0]; save_y0 := PieceUnit.piece_y[0];
  save_x1 := PieceUnit.piece_x[1]; save_y1 := PieceUnit.piece_y[1];
  save_x2 := PieceUnit.piece_x[2]; save_y2 := PieceUnit.piece_y[2];
  save_pos := PieceUnit.piece_position;
  PieceUnit.PieceRotateLeft;
  if CheckPieceDelta(0, 0) = 0 then
  begin
    PieceUnit.piece_x[0] := save_x0; PieceUnit.piece_y[0] := save_y0;
    PieceUnit.piece_x[1] := save_x1; PieceUnit.piece_y[1] := save_y1;
    PieceUnit.piece_x[2] := save_x2; PieceUnit.piece_y[2] := save_y2;
    PieceUnit.piece_position := save_pos;
  end;
end;

procedure GridKeyRotateRight;
var
  save_x0, save_y0, save_x1, save_y1, save_x2, save_y2, save_pos: integer;
begin
  save_x0 := PieceUnit.piece_x[0]; save_y0 := PieceUnit.piece_y[0];
  save_x1 := PieceUnit.piece_x[1]; save_y1 := PieceUnit.piece_y[1];
  save_x2 := PieceUnit.piece_x[2]; save_y2 := PieceUnit.piece_y[2];
  save_pos := PieceUnit.piece_position;
  PieceUnit.PieceRotateRight;
  if CheckPieceDelta(0, 0) = 0 then
  begin
    PieceUnit.piece_x[0] := save_x0; PieceUnit.piece_y[0] := save_y0;
    PieceUnit.piece_x[1] := save_x1; PieceUnit.piece_y[1] := save_y1;
    PieceUnit.piece_x[2] := save_x2; PieceUnit.piece_y[2] := save_y2;
    PieceUnit.piece_position := save_pos;
  end;
end;

function CheckBlock(x, y, btype: integer): integer;
var
  b: integer;
begin
  b := GridGet(x, y);
  if (b = btype) or (b = BLOCK_MATCH) then
    CheckBlock := 1
  else
    CheckBlock := 0;
end;

function BlockName(b: integer): string;
begin
  if b = BLOCK_NULL then BlockName := 'NULL'
  else if b = BLOCK_CLEAR then BlockName := 'CLEAR'
  else if b = RED1 then BlockName := 'RED1'
  else if b = RED2 then BlockName := 'RED2'
  else if b = RED3 then BlockName := 'RED3'
  else if b = GREEN1 then BlockName := 'GREEN1'
  else if b = GREEN2 then BlockName := 'GREEN2'
  else if b = GREEN3 then BlockName := 'GREEN3'
  else if b = BLUE1 then BlockName := 'BLUE1'
  else if b = BLUE2 then BlockName := 'BLUE2'
  else if b = BLUE3 then BlockName := 'BLUE3'
  else if b = BLOCK_MATCH then BlockName := 'WILD'
  else BlockName := '?' + inttostr(b);
end;

procedure LogMatch(dir: string; x1, y1, x2, y2, x3, y3: integer);
begin
  writeln('MATCH [', dir, '] (',
    x1, ',', y1, ')=', BlockName(GridGet(x1, y1)), ' (',
    x2, ',', y2, ')=', BlockName(GridGet(x2, y2)), ' (',
    x3, ',', y3, ')=', BlockName(GridGet(x3, y3)));
end;

procedure ClearBlock(x, y, expected: integer);
var
  idx: integer;
begin
  idx := GridIndex(x, y);
  grid_original[idx] := expected;
  GridSet(x, y, BLOCK_CLEAR);
  GridSetClear(x, y, 1);
  GridSetFlash(x, y, 0);
end;

procedure AddScore;
begin
  game_lines := game_lines + 1;
  if (game_lines mod 6) = 0 then
  begin
    if game_level < 7 then
      game_level := game_level + 1;
  end;
end;

function ProcBlocks: integer;
var
  x, y: integer;
begin
  ProcBlocks := 0;
  for x := 0 to GRID_W - 1 do
  begin
    for y := 0 to GRID_H - 1 do
    begin
      { Vertical matches (column of 3) }
      if (CheckBlock(x, y, RED1) = 1) and (CheckBlock(x, y+1, RED2) = 1) and (CheckBlock(x, y+2, RED3) = 1) then
      begin LogMatch('V-R123', x, y, x, y+1, x, y+2); AddScore; ClearBlock(x, y, RED1); ClearBlock(x, y+1, RED2); ClearBlock(x, y+2, RED3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, RED3) = 1) and (CheckBlock(x, y+1, RED2) = 1) and (CheckBlock(x, y+2, RED1) = 1) then
      begin LogMatch('V-R321', x, y, x, y+1, x, y+2); AddScore; ClearBlock(x, y, RED3); ClearBlock(x, y+1, RED2); ClearBlock(x, y+2, RED1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN1) = 1) and (CheckBlock(x, y+1, GREEN2) = 1) and (CheckBlock(x, y+2, GREEN3) = 1) then
      begin LogMatch('V-G123', x, y, x, y+1, x, y+2); AddScore; ClearBlock(x, y, GREEN1); ClearBlock(x, y+1, GREEN2); ClearBlock(x, y+2, GREEN3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN3) = 1) and (CheckBlock(x, y+1, GREEN2) = 1) and (CheckBlock(x, y+2, GREEN1) = 1) then
      begin LogMatch('V-G321', x, y, x, y+1, x, y+2); AddScore; ClearBlock(x, y, GREEN3); ClearBlock(x, y+1, GREEN2); ClearBlock(x, y+2, GREEN1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE1) = 1) and (CheckBlock(x, y+1, BLUE2) = 1) and (CheckBlock(x, y+2, BLUE3) = 1) then
      begin LogMatch('V-B123', x, y, x, y+1, x, y+2); AddScore; ClearBlock(x, y, BLUE1); ClearBlock(x, y+1, BLUE2); ClearBlock(x, y+2, BLUE3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE3) = 1) and (CheckBlock(x, y+1, BLUE2) = 1) and (CheckBlock(x, y+2, BLUE1) = 1) then
      begin LogMatch('V-B321', x, y, x, y+1, x, y+2); AddScore; ClearBlock(x, y, BLUE3); ClearBlock(x, y+1, BLUE2); ClearBlock(x, y+2, BLUE1); ProcBlocks := 1; exit; end;

      { Horizontal matches (row of 3) }
      if (CheckBlock(x, y, RED1) = 1) and (CheckBlock(x+1, y, RED2) = 1) and (CheckBlock(x+2, y, RED3) = 1) then
      begin LogMatch('H-R123', x, y, x+1, y, x+2, y); AddScore; ClearBlock(x, y, RED1); ClearBlock(x+1, y, RED2); ClearBlock(x+2, y, RED3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, RED3) = 1) and (CheckBlock(x+1, y, RED2) = 1) and (CheckBlock(x+2, y, RED1) = 1) then
      begin LogMatch('H-R321', x, y, x+1, y, x+2, y); AddScore; ClearBlock(x, y, RED3); ClearBlock(x+1, y, RED2); ClearBlock(x+2, y, RED1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN1) = 1) and (CheckBlock(x+1, y, GREEN2) = 1) and (CheckBlock(x+2, y, GREEN3) = 1) then
      begin LogMatch('H-G123', x, y, x+1, y, x+2, y); AddScore; ClearBlock(x, y, GREEN1); ClearBlock(x+1, y, GREEN2); ClearBlock(x+2, y, GREEN3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN3) = 1) and (CheckBlock(x+1, y, GREEN2) = 1) and (CheckBlock(x+2, y, GREEN1) = 1) then
      begin LogMatch('H-G321', x, y, x+1, y, x+2, y); AddScore; ClearBlock(x, y, GREEN3); ClearBlock(x+1, y, GREEN2); ClearBlock(x+2, y, GREEN1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE1) = 1) and (CheckBlock(x+1, y, BLUE2) = 1) and (CheckBlock(x+2, y, BLUE3) = 1) then
      begin LogMatch('H-B123', x, y, x+1, y, x+2, y); AddScore; ClearBlock(x, y, BLUE1); ClearBlock(x+1, y, BLUE2); ClearBlock(x+2, y, BLUE3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE3) = 1) and (CheckBlock(x+1, y, BLUE2) = 1) and (CheckBlock(x+2, y, BLUE1) = 1) then
      begin LogMatch('H-B321', x, y, x+1, y, x+2, y); AddScore; ClearBlock(x, y, BLUE3); ClearBlock(x+1, y, BLUE2); ClearBlock(x+2, y, BLUE1); ProcBlocks := 1; exit; end;

      { Diagonal down-right matches }
      if (CheckBlock(x, y, RED1) = 1) and (CheckBlock(x+1, y+1, RED2) = 1) and (CheckBlock(x+2, y+2, RED3) = 1) then
      begin LogMatch('DR-R123', x, y, x+1, y+1, x+2, y+2); AddScore; ClearBlock(x, y, RED1); ClearBlock(x+1, y+1, RED2); ClearBlock(x+2, y+2, RED3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, RED3) = 1) and (CheckBlock(x+1, y+1, RED2) = 1) and (CheckBlock(x+2, y+2, RED1) = 1) then
      begin LogMatch('DR-R321', x, y, x+1, y+1, x+2, y+2); AddScore; ClearBlock(x, y, RED3); ClearBlock(x+1, y+1, RED2); ClearBlock(x+2, y+2, RED1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN1) = 1) and (CheckBlock(x+1, y+1, GREEN2) = 1) and (CheckBlock(x+2, y+2, GREEN3) = 1) then
      begin LogMatch('DR-G123', x, y, x+1, y+1, x+2, y+2); AddScore; ClearBlock(x, y, GREEN1); ClearBlock(x+1, y+1, GREEN2); ClearBlock(x+2, y+2, GREEN3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN3) = 1) and (CheckBlock(x+1, y+1, GREEN2) = 1) and (CheckBlock(x+2, y+2, GREEN1) = 1) then
      begin LogMatch('DR-G321', x, y, x+1, y+1, x+2, y+2); AddScore; ClearBlock(x, y, GREEN3); ClearBlock(x+1, y+1, GREEN2); ClearBlock(x+2, y+2, GREEN1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE1) = 1) and (CheckBlock(x+1, y+1, BLUE2) = 1) and (CheckBlock(x+2, y+2, BLUE3) = 1) then
      begin LogMatch('DR-B123', x, y, x+1, y+1, x+2, y+2); AddScore; ClearBlock(x, y, BLUE1); ClearBlock(x+1, y+1, BLUE2); ClearBlock(x+2, y+2, BLUE3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE3) = 1) and (CheckBlock(x+1, y+1, BLUE2) = 1) and (CheckBlock(x+2, y+2, BLUE1) = 1) then
      begin LogMatch('DR-B321', x, y, x+1, y+1, x+2, y+2); AddScore; ClearBlock(x, y, BLUE3); ClearBlock(x+1, y+1, BLUE2); ClearBlock(x+2, y+2, BLUE1); ProcBlocks := 1; exit; end;

      { Diagonal up-right matches }
      if (CheckBlock(x, y, RED1) = 1) and (CheckBlock(x+1, y-1, RED2) = 1) and (CheckBlock(x+2, y-2, RED3) = 1) then
      begin LogMatch('UR-R123', x, y, x+1, y-1, x+2, y-2); AddScore; ClearBlock(x, y, RED1); ClearBlock(x+1, y-1, RED2); ClearBlock(x+2, y-2, RED3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, RED3) = 1) and (CheckBlock(x+1, y-1, RED2) = 1) and (CheckBlock(x+2, y-2, RED1) = 1) then
      begin LogMatch('UR-R321', x, y, x+1, y-1, x+2, y-2); AddScore; ClearBlock(x, y, RED3); ClearBlock(x+1, y-1, RED2); ClearBlock(x+2, y-2, RED1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN1) = 1) and (CheckBlock(x+1, y-1, GREEN2) = 1) and (CheckBlock(x+2, y-2, GREEN3) = 1) then
      begin LogMatch('UR-G123', x, y, x+1, y-1, x+2, y-2); AddScore; ClearBlock(x, y, GREEN1); ClearBlock(x+1, y-1, GREEN2); ClearBlock(x+2, y-2, GREEN3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, GREEN3) = 1) and (CheckBlock(x+1, y-1, GREEN2) = 1) and (CheckBlock(x+2, y-2, GREEN1) = 1) then
      begin LogMatch('UR-G321', x, y, x+1, y-1, x+2, y-2); AddScore; ClearBlock(x, y, GREEN3); ClearBlock(x+1, y-1, GREEN2); ClearBlock(x+2, y-2, GREEN1); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE1) = 1) and (CheckBlock(x+1, y-1, BLUE2) = 1) and (CheckBlock(x+2, y-2, BLUE3) = 1) then
      begin LogMatch('UR-B123', x, y, x+1, y-1, x+2, y-2); AddScore; ClearBlock(x, y, BLUE1); ClearBlock(x+1, y-1, BLUE2); ClearBlock(x+2, y-2, BLUE3); ProcBlocks := 1; exit; end;
      if (CheckBlock(x, y, BLUE3) = 1) and (CheckBlock(x+1, y-1, BLUE2) = 1) and (CheckBlock(x+2, y-2, BLUE1) = 1) then
      begin LogMatch('UR-B321', x, y, x+1, y-1, x+2, y-2); AddScore; ClearBlock(x, y, BLUE3); ClearBlock(x+1, y-1, BLUE2); ClearBlock(x+2, y-2, BLUE1); ProcBlocks := 1; exit; end;
    end;
  end;
end;

function ProcMoveDown: integer;
var
  x, y, b1, b2, idx, cv: integer;
  did_update: integer;
begin
  ProcMoveDown := 0;
  for x := 0 to GRID_W - 1 do
  begin
    for y := 0 to GRID_H - 2 do
    begin
      b1 := GridGet(x, y);
      b2 := GridGet(x, y + 1);
      if (b1 <> BLOCK_NULL) and (b1 <> BLOCK_CLEAR) and (b1 <> -1) and (b2 = BLOCK_NULL) then
      begin
        GridSet(x, y + 1, b1);
        GridSetClear(x, y + 1, 0);
        GridSetFlash(x, y + 1, 0);
        GridSet(x, y, BLOCK_NULL);
        GridSetClear(x, y, 0);
        GridSetFlash(x, y, 0);
        ProcMoveDown := 1;
        exit;
      end;
    end;
  end;
  did_update := 0;
  for x := 0 to GRID_W - 1 do
  begin
    for y := 0 to GRID_H - 1 do
    begin
      if GridGet(x, y) = BLOCK_CLEAR then
      begin
        idx := GridIndex(x, y);
        cv := grid_clear[idx] + 1;
        grid_clear[idx] := cv;
        grid_flash[idx] := grid_flash[idx] + 1;
        if cv > 50 then
        begin
          GridSet(x, y, BLOCK_NULL);
          GridSetClear(x, y, 0);
          GridSetFlash(x, y, 0);
        end;
        did_update := 1;
      end;
    end;
  end;
  ProcMoveDown := did_update;
end;

function IsGameOver: integer;
begin
  IsGameOver := game_over;
end;

function GetLevel: integer;
begin
  GetLevel := game_level;
end;

function GetLines: integer;
begin
  GetLines := game_lines;
end;

end.
