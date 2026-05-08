program PointerArith;
{ Pointer dereferencing with arithmetic expressions }
var
  x: ^integer;
  y: ^integer;
  result: integer;
begin
  new(x);
  new(y);
  x^ := 10;
  y^ := 3;

  result := x^ * y^;
  writeln(result);

  result := x^ - y^;
  writeln(result);

  result := x^ div y^;
  writeln(result);

  result := x^ mod y^;
  writeln(result);

  x^ := x^ + 1;
  writeln(x^);

  dispose(x);
  dispose(y);
end.
