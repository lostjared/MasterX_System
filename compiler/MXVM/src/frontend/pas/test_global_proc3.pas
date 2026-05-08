program TestGlobalProc3;

var
  moves: integer;
  knight_row: integer;
  seq_row: array[0..63] of integer;
  seq_count: integer;

procedure next_move;
var
  nr: integer;
begin
  if moves >= 65 then exit;

  if seq_count > 0 then
  begin
    nr := seq_row[moves];
    knight_row := nr;
    moves := moves + 1;
  end;
end;

var
  i: integer;

begin
  moves := 1;
  seq_count := 64;
  knight_row := 0;
  for i := 0 to 63 do
    seq_row[i] := i;

  writeln('before: moves=', moves);
  next_move;
  writeln('after 1: moves=', moves);
  next_move;
  writeln('after 2: moves=', moves);
end.
