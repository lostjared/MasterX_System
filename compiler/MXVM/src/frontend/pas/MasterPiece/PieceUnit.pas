{ PieceUnit - Falling piece management for MasterPiece }
unit PieceUnit;

interface

uses BlockTypes;

var
  piece_x: integer;
  piece_y: integer;
  piece_c1: integer;
  piece_c2: integer;
  piece_c3: integer;
  next_c1: integer;
  next_c2: integer;
  next_c3: integer;
  piece_x1: integer;
  piece_y1: integer;
  piece_x2: integer;
  piece_y2: integer;
  piece_x3: integer;
  piece_y3: integer;
  piece_shape: integer;

procedure PieceInit;
procedure PieceSpawn;
procedure PieceShiftUp;
procedure PieceShiftDown;
procedure PieceMoveLeft;
procedure PieceMoveRight;
procedure PieceMoveDown;
procedure PieceRotate;
procedure PieceUpdatePositions;

implementation

function RandColor: integer;
begin
  RandColor := (rand() mod NUM_COLORS) + 1;
end;

procedure PieceInit;
begin
  next_c1 := RandColor();
  next_c2 := RandColor();
  next_c3 := RandColor();
  while (next_c1 = next_c2) and (next_c2 = next_c3) do
    next_c3 := RandColor();
  PieceSpawn;
end;

procedure PieceSpawn;
begin
  piece_x := 3;
  piece_y := 0;
  piece_shape := 0;
  piece_c1 := next_c1;
  piece_c2 := next_c2;
  piece_c3 := next_c3;
  next_c1 := RandColor();
  next_c2 := RandColor();
  next_c3 := RandColor();
  while (next_c1 = next_c2) and (next_c2 = next_c3) do
    next_c3 := RandColor();
  PieceUpdatePositions;
end;

procedure PieceUpdatePositions;
begin
  if piece_shape = 0 then
  begin
    piece_x1 := piece_x;
    piece_y1 := piece_y;
    piece_x2 := piece_x;
    piece_y2 := piece_y + 1;
    piece_x3 := piece_x;
    piece_y3 := piece_y + 2;
  end
  else
  begin
    piece_x1 := piece_x;
    piece_y1 := piece_y;
    piece_x2 := piece_x + 1;
    piece_y2 := piece_y;
    piece_x3 := piece_x + 2;
    piece_y3 := piece_y;
  end;
end;

procedure PieceRotate;
begin
  if piece_shape = 0 then
    piece_shape := 1
  else
    piece_shape := 0;
  PieceUpdatePositions;
end;

procedure PieceShiftUp;
var
  tmp: integer;
begin
  tmp := piece_c1;
  piece_c1 := piece_c3;
  piece_c3 := piece_c2;
  piece_c2 := tmp;
end;

procedure PieceShiftDown;
var
  tmp: integer;
begin
  tmp := piece_c1;
  piece_c1 := piece_c2;
  piece_c2 := piece_c3;
  piece_c3 := tmp;
end;

procedure PieceMoveLeft;
begin
  if piece_x > 0 then
  begin
    piece_x := piece_x - 1;
    PieceUpdatePositions;
  end;
end;

procedure PieceMoveRight;
begin
  if piece_x3 < GRID_COLS - 1 then
  begin
    piece_x := piece_x + 1;
    PieceUpdatePositions;
  end;
end;

procedure PieceMoveDown;
begin
  if piece_y3 < GRID_ROWS - 1 then
  begin
    piece_y := piece_y + 1;
    PieceUpdatePositions;
  end;
end;

end.
