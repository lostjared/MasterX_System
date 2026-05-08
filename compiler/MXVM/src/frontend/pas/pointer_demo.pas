program PointerDemo;
{ Comprehensive pointer demo: new, dispose, nil, @, ^, type aliases }

type
  PInt = ^integer;

var
  { heap-allocated pointers }
  hp: PInt;
  hp2: ^integer;

  { stack variables and address-of pointers }
  score: integer;
  bonus: integer;
  total: integer;
  sp: PInt;

  { nil checking }
  maybe: ^integer;

  { loop / accumulator }
  acc: ^integer;
  i: integer;

begin
  writeln('=== Pointer Demo ===');

  { --- Part 1: Heap allocation with new/dispose --- }
  writeln('--- heap pointers ---');
  new(hp);
  new(hp2);
  hp^ := 42;
  hp2^ := 58;
  total := hp^ + hp2^;
  writeln(hp^);
  writeln(hp2^);
  writeln(total);
  dispose(hp);
  dispose(hp2);

  { --- Part 2: Address-of operator @ --- }
  writeln('--- address-of @ ---');
  score := 90;
  bonus := 10;
  sp := @score;
  writeln(sp^);

  { modify the original variable through the pointer }
  sp^ := sp^ + bonus;
  writeln(score);

  { re-point to a different variable }
  sp := @bonus;
  sp^ := 25;
  writeln(bonus);

  { --- Part 3: Pointer type alias --- }
  writeln('--- type alias PInt ---');
  new(hp);
  hp^ := 256;
  total := hp^ * 2;
  writeln(hp^);
  writeln(total);
  dispose(hp);

  { --- Part 4: nil assignment and re-use --- }
  writeln('--- nil and reuse ---');
  maybe := nil;
  new(maybe);
  maybe^ := 999;
  writeln(maybe^);
  dispose(maybe);
  maybe := nil;

  { --- Part 5: Accumulator loop through a pointer --- }
  writeln('--- accumulator loop ---');
  new(acc);
  acc^ := 0;
  for i := 1 to 10 do
  begin
    acc^ := acc^ + i;
  end;
  writeln(acc^);
  dispose(acc);

  { --- Part 6: Swap via address-of --- }
  writeln('--- swap via @ ---');
  score := 11;
  bonus := 22;
  writeln(score);
  writeln(bonus);
  sp := @score;
  total := sp^;
  sp^ := bonus;
  sp := @bonus;
  sp^ := total;
  writeln(score);
  writeln(bonus);

  writeln('=== done ===');
end.
