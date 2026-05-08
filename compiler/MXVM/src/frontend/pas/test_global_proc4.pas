program TestGlobalProc4;

var
  moves: integer;
  knight_row, knight_col: integer;
  seq_row: array[0..63] of integer;
  seq_col: array[0..63] of integer;
  seq_count: integer;

procedure next_move;
var
  nr, nc: integer;
begin
  nr := seq_row[moves];
  nc := seq_col[moves];
  knight_row := nr;
  knight_col := nc;
  moves := moves + 1;
end;

var
  i: integer;

begin
  moves := 1;
  seq_count := 64;
  knight_row := 0;
  knight_col := 0;
  for i := 0 to 63 do
  begin
    seq_row[i] := i div 8;
    seq_col[i] := i mod 8;
  end;

  writeln('before: moves=', moves, ' row=', knight_row, ' col=', knight_col);
  next_move;
  writeln('after 1: moves=', moves, ' row=', knight_row, ' col=', knight_col);
  next_move;
  writeln('after 2: moves=', moves, ' row=', knight_row, ' col=', knight_col);
end.
