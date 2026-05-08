program PointerLoop;
{ Using pointers as accumulators in a loop }
var
  acc: ^integer;
  i: integer;
begin
  new(acc);
  acc^ := 0;
  for i := 1 to 10 do
  begin
    acc^ := acc^ + i;
  end;
  writeln(acc^);
  dispose(acc);
end.
