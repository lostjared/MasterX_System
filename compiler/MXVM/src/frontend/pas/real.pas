program RealValue;
var
x: real;
i: integer;
begin

for i := 0 to 10 do
begin
x := 0.0;
x := x + 0.1;
end;
writeln('value of x: ', x);
end.
