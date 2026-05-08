program PointerTypeAlias;
{ Using type aliases for pointer types }
type
  PInt = ^integer;
var
  p: PInt;
  q: PInt;
  sum: integer;
begin
  new(p);
  new(q);
  p^ := 123;
  q^ := 456;
  sum := p^ + q^;
  writeln(p^);
  writeln(q^);
  writeln(sum);
  dispose(p);
  dispose(q);
end.
