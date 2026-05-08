program LoopTest;
var
i : integer;
begin

for i := 0 to 20 do
begin
	if i > 10 then break;
	writeln(' index: ', i);
end;

i := 0;

while i < 10 do
begin
if i = 5 then break;
i := i + 1;
writeln('index while: ', i);
end;


i := 10;
repeat
	i := i - 1;
	if i < 3 then break; 
	writeln('before zero: ', i);
until i = 0;

end.
