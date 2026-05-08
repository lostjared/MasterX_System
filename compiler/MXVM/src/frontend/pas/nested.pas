program testapp;

procedure test();
	procedure sub();
	begin
		writeln(' world');
	end;
begin
	writeln('hello ');
	sub();
end;

begin
	test();
end.
