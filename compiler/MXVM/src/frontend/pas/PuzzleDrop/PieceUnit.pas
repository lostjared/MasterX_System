{ PieceUnit - Piece management for PuzzleDrop }
unit PieceUnit;

interface

uses BlockTypes;

var
  piece_x: array[0..2] of integer;
  piece_y: array[0..2] of integer;
  piece_type: array[0..2] of integer;
  piece_position: integer;

procedure PieceNew(start_x, start_y: integer);
procedure PieceRandColors;
procedure PieceShiftUp;
procedure PieceShiftDown;
procedure PieceMoveLeft;
procedure PieceMoveRight;
procedure PieceMoveDown;
procedure PieceRotateLeft;
procedure PieceRotateRight;
function PieceGetPos: integer;

implementation

procedure PieceNew(start_x, start_y: integer);
begin
  piece_x[0] := start_x;
  piece_y[0] := start_y;
  piece_x[1] := start_x;
  piece_y[1] := start_y + 1;
  piece_x[2] := start_x;
  piece_y[2] := start_y + 2;
  piece_position := 0;
  PieceRandColors;
end;

procedure PieceRandColors;
begin
  piece_type[0] := FIRST_COLOR + (rand() mod (NUM_BLOCK_TYPES + 1));
  piece_type[1] := FIRST_COLOR + (rand() mod (NUM_BLOCK_TYPES + 1));
  piece_type[2] := FIRST_COLOR + (rand() mod (NUM_BLOCK_TYPES + 1));
end;

procedure PieceShiftDown;
var
  b0, b1, b2: integer;
begin
  b0 := piece_type[0];
  b1 := piece_type[1];
  b2 := piece_type[2];
  piece_type[0] := b2;
  piece_type[1] := b0;
  piece_type[2] := b1;
end;

procedure PieceShiftUp;
var
  b0, b1, b2: integer;
begin
  b0 := piece_type[0];
  b1 := piece_type[1];
  b2 := piece_type[2];
  piece_type[0] := b1;
  piece_type[1] := b2;
  piece_type[2] := b0;
end;

procedure PieceMoveLeft;
begin
  piece_x[0] := piece_x[0] - 1;
  piece_x[1] := piece_x[1] - 1;
  piece_x[2] := piece_x[2] - 1;
end;

procedure PieceMoveRight;
begin
  piece_x[0] := piece_x[0] + 1;
  piece_x[1] := piece_x[1] + 1;
  piece_x[2] := piece_x[2] + 1;
end;

procedure PieceMoveDown;
begin
  piece_y[0] := piece_y[0] + 1;
  piece_y[1] := piece_y[1] + 1;
  piece_y[2] := piece_y[2] + 1;
end;

procedure PieceRotateLeft;
begin
  if piece_position = 0 then
  begin
    piece_y[1] := piece_y[1] - 1;
    piece_x[1] := piece_x[1] - 1;
    piece_x[2] := piece_x[2] - 2;
    piece_y[2] := piece_y[2] - 2;
    piece_position := 1;
  end
  else if piece_position = 1 then
  begin
    piece_y[1] := piece_y[1] + 1;
    piece_x[1] := piece_x[1] + 1;
    piece_y[2] := piece_y[2] + 2;
    piece_x[2] := piece_x[2] + 2;
    piece_position := 0;
  end;
end;

procedure PieceRotateRight;
begin
  if piece_position = 0 then
  begin
    piece_x[1] := piece_x[1] + 1;
    piece_y[1] := piece_y[1] - 1;
    piece_x[2] := piece_x[2] + 2;
    piece_y[2] := piece_y[2] - 2;
    piece_position := 2;
  end
  else if piece_position = 2 then
  begin
    piece_x[1] := piece_x[1] - 1;
    piece_y[1] := piece_y[1] + 1;
    piece_x[2] := piece_x[2] - 2;
    piece_y[2] := piece_y[2] + 2;
    piece_position := 0;
  end;
end;

function PieceGetPos: integer;
begin
  PieceGetPos := piece_position;
end;

end.
