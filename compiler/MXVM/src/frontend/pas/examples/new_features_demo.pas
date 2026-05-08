program NewFeaturesDemo;

{ Comprehensive demo of sets, file I/O, and packed types }

type
  Weekday = set of integer;
  PackedPoint = packed record
    x: integer;
    y: integer;
  end;

var
  vowels: set of integer;
  consonants: set of integer;
  letters: set of integer;
  primes: Weekday;
  evens: set of integer;
  odds: set of integer;
  result_set: set of integer;
  f: file;
  line: string;
  i: integer;
  ch: integer;
  count: integer;
  pa: packed array[1..10] of integer;
  pr: PackedPoint;

begin
  writeln('=== MXVM New Features Demo ===');
  writeln;

  { ---------------------------------------------------------------- }
  {  PART 1: Sets                                                     }
  { ---------------------------------------------------------------- }
  writeln('--- Part 1: Set Operations ---');

  { Build sets using literals }
  vowels := [1, 5, 9, 15, 21];       { a=1, e=5, i=9, o=15, u=21 }
  consonants := [2, 3, 4, 6, 7, 8, 10, 11, 12, 13, 14, 16, 17, 18, 19, 20, 22, 23, 24, 25, 26];
  primes := [2, 3, 5, 7, 11, 13, 17, 19, 23];
  evens := [2, 4, 6, 8, 10, 12, 14, 16, 18, 20];
  odds := [1, 3, 5, 7, 9, 11, 13, 15, 17, 19];

  { Membership test with in }
  write('Vowel positions: ');
  for i := 1 to 26 do
    if i in vowels then
      write(i, ' ');
  writeln;

  { Set union (+) }
  result_set := primes + evens;
  write('Primes or evens (1-20): ');
  for i := 1 to 20 do
    if i in result_set then
      write(i, ' ');
  writeln;

  { Set intersection (*) }
  result_set := primes * odds;
  write('Odd primes (1-20): ');
  for i := 1 to 20 do
    if i in result_set then
      write(i, ' ');
  writeln;

  { Set difference (-) }
  result_set := primes - odds;
  write('Even primes: ');
  for i := 1 to 20 do
    if i in result_set then
      write(i, ' ');
  writeln;

  { Include and exclude }
  writeln;
  writeln('Modifying sets with include/exclude:');
  write('  Before: 6 in primes? ');
  if 6 in primes then writeln('yes') else writeln('no');
  include(primes, 6);
  write('  After include(primes, 6): 6 in primes? ');
  if 6 in primes then writeln('yes') else writeln('no');
  exclude(primes, 6);
  write('  After exclude(primes, 6): 6 in primes? ');
  if 6 in primes then writeln('yes') else writeln('no');

  { Inline set literal with in }
  writeln;
  write('Fibonacci numbers up to 20: ');
  for i := 1 to 20 do
    if i in [1, 2, 3, 5, 8, 13] then
      write(i, ' ');
  writeln;

  { Combining operations }
  result_set := (primes * odds) + [2];
  write('All primes up to 23 (odd primes + 2): ');
  for i := 1 to 23 do
    if i in result_set then
      write(i, ' ');
  writeln;

  { ---------------------------------------------------------------- }
  {  PART 2: File I/O                                                 }
  { ---------------------------------------------------------------- }
  writeln;
  writeln('--- Part 2: File I/O ---');

  { Write a data file }
  assign(f, 'demo_data.txt');
  rewrite(f);
  writeln(f, 'Name: MXVM Pascal Compiler');
  writeln(f, 'Version: 2.0');
  writeln(f, 'Features: sets, files, packed');
  for i := 1 to 5 do
    writeln(f, 'Item ', i, ': value=', i * i);
  close(f);
  writeln('Wrote demo_data.txt');

  { Read it back }
  writeln('Contents of demo_data.txt:');
  assign(f, 'demo_data.txt');
  reset(f);
  count := 0;
  while not eof(f) do
  begin
    readln(f, line);
    if length(line) > 0 then
    begin
      count := count + 1;
      writeln('  [', count, '] ', line);
    end;
  end;
  close(f);
  writeln('Read ', count, ' lines.');

  { Append more data }
  assign(f, 'demo_data.txt');
  append(f);
  writeln(f, 'Appended: timestamp=2026-04-06');
  close(f);
  writeln('Appended one line.');

  { Verify the append }
  assign(f, 'demo_data.txt');
  reset(f);
  count := 0;
  while not eof(f) do
  begin
    readln(f, line);
    if length(line) > 0 then
      count := count + 1;
  end;
  close(f);
  writeln('File now has ', count, ' lines.');

  { Write a CSV-style file using set-driven logic }
  assign(f, 'primes.csv');
  rewrite(f);
  writeln(f, 'number,is_prime,is_even,is_odd');
  for i := 1 to 30 do
  begin
    write(f, i);
    if i in primes then write(f, ',yes') else write(f, ',no');
    if i in evens then write(f, ',yes') else write(f, ',no');
    if i in odds then write(f, ',yes') else write(f, ',no');
    writeln(f, '');
  end;
  close(f);
  writeln('Wrote primes.csv with 30 rows.');

  { ---------------------------------------------------------------- }
  {  PART 3: Packed types                                             }
  { ---------------------------------------------------------------- }
  writeln;
  writeln('--- Part 3: Packed Types ---');

  { packed is accepted but does not change memory layout }

  for i := 1 to 10 do
    pa[i] := i * 10;

  write('Packed array: ');
  for i := 1 to 10 do
    write(pa[i], ' ');
  writeln;

  pr.x := 42;
  pr.y := 99;
  writeln('Packed record: x=', pr.x, ' y=', pr.y);

  { ---------------------------------------------------------------- }
  {  Summary                                                          }
  { ---------------------------------------------------------------- }
  writeln;
  writeln('=== All demos completed successfully ===');
end.
