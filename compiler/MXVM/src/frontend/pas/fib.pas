program Fibonacci;
var
  n: integer;
  a, b, temp: integer;
  i: Integer;
  code: integer;
begin
  a := 0;
  b := 1;
  i := 2;
  n := 92;
  write('enter how many numbers to output: ');
  readln(n);
  writeln(a, ' ');
  if n > 1 then
    writeln(b, ' ');

 if (n < 1) or (n > 92) then
 begin
     writeln('out of range of 64-bit integer');
     halt(0);
 end;

  while i < n do
  begin
    temp := a + b;
    a := b;
    b := temp;
    writeln(b, ' ');
    i := i + 1;
  end;
  writeln(' done ');
end.
