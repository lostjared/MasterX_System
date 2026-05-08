{ new_features.pas — demonstrates with, goto/label, in operator, and Pascal doubled-quote strings }
program NewFeatures;

type
  TPoint = record
    x: integer;
    y: integer;
  end;

  TPlayer = record
    name: integer;
    score: integer;
    level: integer;
  end;

var
  pt: TPoint;
  player: TPlayer;
  i: integer;
  ch: integer;
  found: boolean;

label 10, 20, 30;

{ --- with statement --------------------------------------------------------
  Access record fields directly inside a with block, no need to repeat the
  record variable name on every line.
}
procedure DemoWith;
var
  a: TPoint;
  b: TPoint;
begin
  writeln('--- with statement ---');

  { initialise using with }
  with a do
  begin
    x := 3;
    y := 7;
  end;

  with b do
  begin
    x := a.x * 2;
    y := a.y * 2;
  end;

  writeln('a: x=', a.x, ' y=', a.y);
  writeln('b: x=', b.x, ' y=', b.y);

  { nested with modifies fields in place }
  with a do
  begin
    x := x + b.x;
    y := y + b.y;
  end;
  writeln('a after add: x=', a.x, ' y=', a.y);
end;

{ --- in operator ------------------------------------------------------------
  Test whether a value belongs to a set literal [v1, v2, ...].
}
procedure DemoIn;
var
  n: integer;
begin
  writeln('--- in operator ---');

  n := 5;
  if n in [1, 3, 5, 7, 9] then
    writeln(n, ' is odd (in set)')
  else
    writeln(n, ' is NOT in the odd set');

  n := 4;
  if n in [1, 3, 5, 7, 9] then
    writeln(n, ' is odd (in set)')
  else
    writeln(n, ' is NOT in the odd set');

  { grades: A=1 B=2 C=3 D=4 F=5 }
  n := 3;
  if n in [1, 2, 3] then
    writeln('grade passes (A/B/C)')
  else
    writeln('grade fails (D/F)');

  { for loop: classify each number 1..15 }
  writeln('classifying 1..15:');
  for n := 1 to 15 do
  begin
    if n in [2, 3, 5, 7, 11, 13] then
      writeln(n, ' -> prime')
    else if n in [4, 6, 8, 9, 10, 12, 14, 15] then
      writeln(n, ' -> composite')
    else
      writeln(n, ' -> one');
  end;
end;

{ --- goto / label -----------------------------------------------------------
  Jump forward to skip a block of code, or backward to retry.
}
procedure DemoGoto;
var
  retry: integer;
begin
  writeln('--- goto / label ---');

  retry := 0;

  { label 10: top of retry loop }
  10: retry := retry + 1;
  writeln('attempt ', retry);
  if retry < 3 then
    goto 10;

  writeln('done after ', retry, ' attempts');

  { jump over a section }
  writeln('jumping over middle block...');
  goto 20;
  30: writeln('this line is never reached');
  20: writeln('landed at label 20, skipped label 30');
end;

{ --- Pascal doubled-quote strings -------------------------------------------
  Use '' inside a string literal to embed a single quote character.
}
procedure DemoStrings;
begin
  writeln('--- doubled-quote strings ---');
  writeln('it''s a beautiful day');
  writeln('she said ''hello world''');
  writeln('can''t stop, won''t stop');
  writeln('apostrophe test: o''clock');
end;

begin
  DemoWith;
  writeln('');
  DemoIn;
  writeln('');
  DemoGoto;
  writeln('');
  DemoStrings;
end.
