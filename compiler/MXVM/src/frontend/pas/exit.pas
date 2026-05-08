program TestProg;
procedure xval;
begin
writeln('early exit');
exit;
writeln('should not get here');
end;
begin
xval;
end.
