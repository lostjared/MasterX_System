program Fibonacci;
var
  n: integer;
  a, b, temp: integer;
  i: integer;
  code: integer;
begin
  a := 0;
  b := 1;
  i := 2;
  write('enter how many: ');
  readln(n);
  write(a, ' ');
  if n > 1 then
    write(b, ' ');

  while i < n do
  begin
    temp := a + b;
    a := b;
    b := temp;
    write(b, ' ');
    i := i + 1;
  end;
  writeln(' done ');
end.
