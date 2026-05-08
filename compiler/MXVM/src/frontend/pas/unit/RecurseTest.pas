program RecurseTest;

procedure Countdown(msg: string; n: integer);
begin
  if n > 0 then
  begin
    write(msg);
    write(n);
    writeln;
    Countdown(msg, n - 1);
  end;
end;

function Factorial(n: integer): integer;
begin
  if n <= 1 then
    Factorial := 1
  else
    Factorial := n * Factorial(n - 1);
end;

var
  i: integer;
begin
  Countdown('Count: ', 5);
  writeln;
  for i := 1 to 10 do
  begin
    write('Factorial(');
    write(i);
    write(') = ');
    writeln(Factorial(i));
  end;
end.
