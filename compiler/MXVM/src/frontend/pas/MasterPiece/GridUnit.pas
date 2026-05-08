{ GridUnit - Game board grid and match logic for MasterPiece }
unit GridUnit;

interface

uses BlockTypes, PieceUnit;

var
  grid: array[0..135] of integer;
  grid_clear: array[0..135] of integer;
  grid_flash: array[0..135] of integer;
  grid_original: array[0..135] of integer;
  score: integer;
  lines: integer;
  speed: integer;
  line_count: integer;
  game_over: integer;
  drop_timer: integer;
  lines_per_speed: integer;
  clearing_active: integer;

function GridIndex(col, row: integer): integer;
function GridGet(col, row: integer): integer;
procedure GridSet(col, row, val: integer);
function GridInBounds(col, row: integer): integer;
function GridGetClear(col, row: integer): integer;
function GridGetFlash(col, row: integer): integer;
function GridGetOriginal(col, row: integer): integer;
procedure GridClearAll;
procedure ClearBlock(col, row, original: integer);
procedure PlacePiece;
function CanMoveLeft: integer;
function CanMoveRight: integer;
function CanMoveDown: integer;
function CanRotate: integer;
function CheckGameOver: integer;
procedure DropPiece;
procedure HardDrop;
function CheckAndClearMatches: integer;
procedure ProcessGravity;
procedure ProcessFlash;
function IsClearing: integer;
procedure AddLine;
procedure ResetGame;
procedure UpdateGame;

implementation

function GridIndex(col, row: integer): integer;
begin
  GridIndex := row * GRID_COLS + col;
end;

function GridInBounds(col, row: integer): integer;
begin
  if (col >= 0) and (col < GRID_COLS) and (row >= 0) and (row < GRID_ROWS) then
    GridInBounds := 1
  else
    GridInBounds := 0;
end;

function GridGet(col, row: integer): integer;
begin
  if GridInBounds(col, row) = 1 then
    GridGet := grid[GridIndex(col, row)]
  else
    GridGet := -1;
end;

procedure GridSet(col, row, val: integer);
begin
  if GridInBounds(col, row) = 1 then
    grid[GridIndex(col, row)] := val;
end;

function GridGetClear(col, row: integer): integer;
begin
  if GridInBounds(col, row) = 1 then
    GridGetClear := grid_clear[GridIndex(col, row)]
  else
    GridGetClear := 0;
end;

function GridGetFlash(col, row: integer): integer;
begin
  if GridInBounds(col, row) = 1 then
    GridGetFlash := grid_flash[GridIndex(col, row)]
  else
    GridGetFlash := 0;
end;

function GridGetOriginal(col, row: integer): integer;
begin
  if GridInBounds(col, row) = 1 then
    GridGetOriginal := grid_original[GridIndex(col, row)]
  else
    GridGetOriginal := 0;
end;

procedure GridClearAll;
var
  i: integer;
begin
  for i := 0 to GRID_SIZE - 1 do
  begin
    grid[i] := BLOCK_BLACK;
    grid_clear[i] := 0;
    grid_flash[i] := 0;
    grid_original[i] := 0;
  end;
  clearing_active := 0;
end;

procedure ClearBlock(col, row, original: integer);
var
  idx: integer;
begin
  if GridInBounds(col, row) = 1 then
  begin
    idx := GridIndex(col, row);
    grid_original[idx] := original;
    grid[idx] := BLOCK_CLEAR;
    grid_clear[idx] := 1;
    grid_flash[idx] := 0;
    clearing_active := 1;
  end;
end;

procedure PlacePiece;
begin
  if PieceUnit.piece_y <= 0 then
  begin
    game_over := 1;
    exit;
  end;
  GridSet(PieceUnit.piece_x1, PieceUnit.piece_y1, PieceUnit.piece_c1);
  GridSet(PieceUnit.piece_x2, PieceUnit.piece_y2, PieceUnit.piece_c2);
  GridSet(PieceUnit.piece_x3, PieceUnit.piece_y3, PieceUnit.piece_c3);
  PieceUnit.PieceSpawn;
end;

function CanMoveLeft: integer;
begin
  CanMoveLeft := 1;
  if (PieceUnit.piece_x1 <= 0) or (PieceUnit.piece_x2 <= 0) or (PieceUnit.piece_x3 <= 0) then
  begin
    CanMoveLeft := 0;
    exit;
  end;
  if GridGet(PieceUnit.piece_x1 - 1, PieceUnit.piece_y1) <> BLOCK_BLACK then
    CanMoveLeft := 0
  else if GridGet(PieceUnit.piece_x2 - 1, PieceUnit.piece_y2) <> BLOCK_BLACK then
    CanMoveLeft := 0
  else if GridGet(PieceUnit.piece_x3 - 1, PieceUnit.piece_y3) <> BLOCK_BLACK then
    CanMoveLeft := 0;
end;

function CanMoveRight: integer;
begin
  CanMoveRight := 1;
  if (PieceUnit.piece_x1 >= GRID_COLS - 1) or (PieceUnit.piece_x2 >= GRID_COLS - 1) or (PieceUnit.piece_x3 >= GRID_COLS - 1) then
  begin
    CanMoveRight := 0;
    exit;
  end;
  if GridGet(PieceUnit.piece_x1 + 1, PieceUnit.piece_y1) <> BLOCK_BLACK then
    CanMoveRight := 0
  else if GridGet(PieceUnit.piece_x2 + 1, PieceUnit.piece_y2) <> BLOCK_BLACK then
    CanMoveRight := 0
  else if GridGet(PieceUnit.piece_x3 + 1, PieceUnit.piece_y3) <> BLOCK_BLACK then
    CanMoveRight := 0;
end;

function CanMoveDown: integer;
begin
  CanMoveDown := 1;
  if (PieceUnit.piece_y1 + 1 > GRID_ROWS - 1) or (PieceUnit.piece_y2 + 1 > GRID_ROWS - 1) or (PieceUnit.piece_y3 + 1 > GRID_ROWS - 1) then
  begin
    CanMoveDown := 0;
    exit;
  end;
  if GridGet(PieceUnit.piece_x1, PieceUnit.piece_y1 + 1) <> BLOCK_BLACK then
  begin
    CanMoveDown := 0;
    exit;
  end;
  if GridGet(PieceUnit.piece_x2, PieceUnit.piece_y2 + 1) <> BLOCK_BLACK then
  begin
    CanMoveDown := 0;
    exit;
  end;
  if GridGet(PieceUnit.piece_x3, PieceUnit.piece_y3 + 1) <> BLOCK_BLACK then
    CanMoveDown := 0;
end;

function CanRotate: integer;
var
  nx2, ny2, nx3, ny3: integer;
begin
  CanRotate := 1;
  if PieceUnit.piece_shape = 0 then
  begin
    { Rotating to horizontal: block2 goes to (x+1, y), block3 to (x+2, y) }
    nx2 := PieceUnit.piece_x + 1;
    ny2 := PieceUnit.piece_y;
    nx3 := PieceUnit.piece_x + 2;
    ny3 := PieceUnit.piece_y;
    if (nx3 >= GRID_COLS) then
    begin
      CanRotate := 0;
      exit;
    end;
    if GridGet(nx2, ny2) <> BLOCK_BLACK then
      CanRotate := 0
    else if GridGet(nx3, ny3) <> BLOCK_BLACK then
      CanRotate := 0;
  end
  else
  begin
    { Rotating to vertical: block2 goes to (x, y+1), block3 to (x, y+2) }
    nx2 := PieceUnit.piece_x;
    ny2 := PieceUnit.piece_y + 1;
    nx3 := PieceUnit.piece_x;
    ny3 := PieceUnit.piece_y + 2;
    if (ny3 >= GRID_ROWS) then
    begin
      CanRotate := 0;
      exit;
    end;
    if GridGet(nx2, ny2) <> BLOCK_BLACK then
      CanRotate := 0
    else if GridGet(nx3, ny3) <> BLOCK_BLACK then
      CanRotate := 0;
  end;
end;

function CheckGameOver: integer;
begin
  if GridGet(PieceUnit.piece_x1, PieceUnit.piece_y1) <> BLOCK_BLACK then
    CheckGameOver := 1
  else
    CheckGameOver := 0;
end;

procedure AddLine;
begin
  lines := lines + 1;
  score := score + POINTS_PER_LINE;
  line_count := line_count + 1;
  if line_count >= lines_per_speed then
  begin
    line_count := 0;
    speed := speed - SPEED_DECREASE;
    if speed < MIN_SPEED then
      speed := MIN_SPEED;
  end;
end;

{ Check for 3-in-a-row horizontally }
function CheckHorizontal: integer;
var
  r, c, cur: integer;
  found: integer;
begin
  found := 0;
  for r := 0 to GRID_ROWS - 1 do
  begin
    for c := 0 to GRID_COLS - 3 do
    begin
      cur := GridGet(c, r);
      if (cur <> BLOCK_BLACK) and (cur <> BLOCK_CLEAR) then
      begin
        if (GridGet(c + 1, r) = cur) and (GridGet(c + 2, r) = cur) then
        begin
          ClearBlock(c, r, cur);
          ClearBlock(c + 1, r, cur);
          ClearBlock(c + 2, r, cur);
          AddLine;
          found := 1;
        end;
      end;
    end;
  end;
  CheckHorizontal := found;
end;

{ Check for 3-in-a-row vertically }
function CheckVertical: integer;
var
  r, c, cur: integer;
  found: integer;
begin
  found := 0;
  for c := 0 to GRID_COLS - 1 do
  begin
    for r := 0 to GRID_ROWS - 3 do
    begin
      cur := GridGet(c, r);
      if (cur <> BLOCK_BLACK) and (cur <> BLOCK_CLEAR) then
      begin
        if (GridGet(c, r + 1) = cur) and (GridGet(c, r + 2) = cur) then
        begin
          ClearBlock(c, r, cur);
          ClearBlock(c, r + 1, cur);
          ClearBlock(c, r + 2, cur);
          AddLine;
          found := 1;
        end;
      end;
    end;
  end;
  CheckVertical := found;
end;

{ Check for 3-in-a-row diagonal (down-right and down-left) }
function CheckDiagonal: integer;
var
  r, c, cur: integer;
  found: integer;
begin
  found := 0;
  { Down-right diagonal }
  for r := 0 to GRID_ROWS - 3 do
  begin
    for c := 0 to GRID_COLS - 3 do
    begin
      cur := GridGet(c, r);
      if (cur <> BLOCK_BLACK) and (cur <> BLOCK_CLEAR) then
      begin
        if (GridGet(c + 1, r + 1) = cur) and (GridGet(c + 2, r + 2) = cur) then
        begin
          ClearBlock(c, r, cur);
          ClearBlock(c + 1, r + 1, cur);
          ClearBlock(c + 2, r + 2, cur);
          AddLine;
          found := 1;
        end;
      end;
    end;
  end;
  { Down-left diagonal }
  for r := 0 to GRID_ROWS - 3 do
  begin
    for c := 2 to GRID_COLS - 1 do
    begin
      cur := GridGet(c, r);
      if (cur <> BLOCK_BLACK) and (cur <> BLOCK_CLEAR) then
      begin
        if (GridGet(c - 1, r + 1) = cur) and (GridGet(c - 2, r + 2) = cur) then
        begin
          ClearBlock(c, r, cur);
          ClearBlock(c - 1, r + 1, cur);
          ClearBlock(c - 2, r + 2, cur);
          AddLine;
          found := 1;
        end;
      end;
    end;
  end;
  { Up-right diagonal }
  for r := 2 to GRID_ROWS - 1 do
  begin
    for c := 0 to GRID_COLS - 3 do
    begin
      cur := GridGet(c, r);
      if (cur <> BLOCK_BLACK) and (cur <> BLOCK_CLEAR) then
      begin
        if (GridGet(c + 1, r - 1) = cur) and (GridGet(c + 2, r - 2) = cur) then
        begin
          ClearBlock(c, r, cur);
          ClearBlock(c + 1, r - 1, cur);
          ClearBlock(c + 2, r - 2, cur);
          AddLine;
          found := 1;
        end;
      end;
    end;
  end;
  { Up-left diagonal }
  for r := 2 to GRID_ROWS - 1 do
  begin
    for c := 2 to GRID_COLS - 1 do
    begin
      cur := GridGet(c, r);
      if (cur <> BLOCK_BLACK) and (cur <> BLOCK_CLEAR) then
      begin
        if (GridGet(c - 1, r - 1) = cur) and (GridGet(c - 2, r - 2) = cur) then
        begin
          ClearBlock(c, r, cur);
          ClearBlock(c - 1, r - 1, cur);
          ClearBlock(c - 2, r - 2, cur);
          AddLine;
          found := 1;
        end;
      end;
    end;
  end;
  CheckDiagonal := found;
end;

function CheckAndClearMatches: integer;
var
  found: integer;
begin
  found := 0;
  if CheckHorizontal() = 1 then
    found := 1;
  if CheckVertical() = 1 then
    found := 1;
  if CheckDiagonal() = 1 then
    found := 1;
  CheckAndClearMatches := found;
end;

{ Drop floating blocks down after matches are cleared }
procedure ProcessGravity;
var
  c, r, val: integer;
begin
  for c := 0 to GRID_COLS - 1 do
  begin
    for r := GRID_ROWS - 2 downto 0 do
    begin
      val := GridGet(c, r);
      if (val <> BLOCK_BLACK) and (val <> BLOCK_CLEAR) and (GridGet(c, r + 1) = BLOCK_BLACK) then
      begin
        GridSet(c, r + 1, val);
        GridSet(c, r, BLOCK_BLACK);
      end;
    end;
  end;
end;

procedure ProcessFlash;
var
  c, r, idx, cv: integer;
begin
  clearing_active := 0;
  for r := 0 to GRID_ROWS - 1 do
  begin
    for c := 0 to GRID_COLS - 1 do
    begin
      if GridGet(c, r) = BLOCK_CLEAR then
      begin
        idx := GridIndex(c, r);
        cv := grid_clear[idx] + 1;
        grid_clear[idx] := cv;
        grid_flash[idx] := grid_flash[idx] + 1;
        if cv > FLASH_DURATION then
        begin
          grid[idx] := BLOCK_BLACK;
          grid_clear[idx] := 0;
          grid_flash[idx] := 0;
          grid_original[idx] := 0;
        end
        else
          clearing_active := 1;
      end;
    end;
  end;
end;

function IsClearing: integer;
begin
  IsClearing := clearing_active;
end;

procedure DropPiece;
begin
  if CanMoveDown() = 1 then
    PieceUnit.PieceMoveDown
  else
    PlacePiece;
end;

procedure HardDrop;
begin
  while CanMoveDown() = 1 do
    PieceUnit.PieceMoveDown;
  PlacePiece;
end;

procedure ResetGame;
begin
  GridClearAll;
  score := 0;
  lines := 0;
  speed := INITIAL_SPEED;
  line_count := 0;
  lines_per_speed := LINES_PER_LEVEL;
  game_over := 0;
  drop_timer := 0;
  PieceUnit.PieceInit;
end;

procedure UpdateGame;
var
  matched: integer;
begin
  if game_over = 1 then
    exit;
  { Process any active flash animations }
  ProcessFlash;
  { Only drop and match when nothing is flashing }
  if IsClearing() = 0 then
  begin
    ProcessGravity;
    matched := CheckAndClearMatches();
    if matched = 0 then
    begin
      drop_timer := drop_timer + 1;
      if drop_timer >= speed then
      begin
        drop_timer := 0;
        DropPiece;
      end;
    end;
  end;
end;

end.
