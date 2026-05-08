program read_line; 
var
x: integer;
i: integer;
begin
	x := 0;
	write('how many times to loop? ');
	readln(x);
	for i := 0 to x do
	begin
		writeln('looping: ', i);
	end;
end.
