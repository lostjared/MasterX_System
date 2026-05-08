program PointerBasic;
{ Basic pointer operations: new, dispose, dereference, nil }
var
  p: ^integer;
  x: integer;
begin
  new(p);
  p^ := 100;
  x := p^ + 50;
  writeln(p^);
  writeln(x);
  dispose(p);
  p := nil;
  writeln('pointer disposed and set to nil');
end.
