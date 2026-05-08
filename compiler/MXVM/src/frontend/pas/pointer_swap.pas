program PointerSwap;
{ Swap two values using pointers }
var
  a: ^integer;
  b: ^integer;
  temp: integer;
begin
  new(a);
  new(b);
  a^ := 25;
  b^ := 75;
  writeln('before swap:');
  writeln(a^);
  writeln(b^);
  temp := a^;
  a^ := b^;
  b^ := temp;
  writeln('after swap:');
  writeln(a^);
  writeln(b^);
  dispose(a);
  dispose(b);
end.
