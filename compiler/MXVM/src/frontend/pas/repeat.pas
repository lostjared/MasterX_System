program wrepeat;
var 
i: integer;
begin
i := 10;
repeat
writeln('hey: ', i);
i := i - 1;
until i = 0;
end.
