program LoopTest;
var
i : integer;
begin

for i := 0 to 20 do
begin
	if i < 10 then continue;
	writeln('for loop index: ', i);
end;

i := 0;

while i < 10 do 
begin

i := i  + 1;
if i > 5 then continue;
writeln('while index: ', i)

end;

i := 10;

repeat
i := i - 1;
writeln('repeat test: ', i);
if i = 5 then continue;
writeln(' no 5: ', i);
until i = 0;
end.
