{ StringUtils - A string utility unit demonstrating IO and string operations }
unit StringUtils;

interface

procedure PrintHeader(title: string);
procedure PrintSeparator;
procedure PrintLabeledInt(lbl: string; value: integer);
procedure PrintLabeledReal(lbl: string; value: real);
procedure PrintBanner(msg: string; width: integer);
procedure PrintRepeat(ch: string; count: integer);

implementation

uses io;

procedure PrintHeader(title: string);
begin
  writeln('=== ', title, ' ===');
end;

procedure PrintSeparator;
begin
  writeln('------------------------');
end;

procedure PrintLabeledInt(lbl: string; value: integer);
begin
  writeln('  ', lbl, ': ', value);
end;

procedure PrintLabeledReal(lbl: string; value: real);
begin
  writeln('  ', lbl, ': ', value);
end;

procedure PrintBanner(msg: string; width: integer);
var
  i: integer;
begin
  for i := 1 to width do
    write('-');
  writeln;
  writeln(msg);
  for i := 1 to width do
    write('-');
  writeln;
end;

procedure PrintRepeat(ch: string; count: integer);
var
  i: integer;
begin
  for i := 1 to count do
    write(ch);
  writeln;
end;

end.
