program AddrTest;
var
  x: integer;
  p: ^integer;
begin
  x := 42;
  p := @x;
  writeln(p^);
  p^ := 100;
  writeln(x);
end.
