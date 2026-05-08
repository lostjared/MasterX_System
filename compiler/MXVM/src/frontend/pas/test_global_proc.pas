program TestGlobalProc;

var
  counter: integer;

procedure increment;
begin
  counter := counter + 1;
end;

begin
  counter := 0;
  writeln('before: counter=', counter);
  increment;
  writeln('after increment: counter=', counter);
  increment;
  writeln('after increment: counter=', counter);
  increment;
  writeln('after increment: counter=', counter);
end.
