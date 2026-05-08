{ features_demo.pas — comprehensive demonstration of extended Pascal features
  
  Features shown:
    1. Enumerated types with ord, succ, pred, inc, dec
    2. Variant records (case ... of inside record)
    3. Result variable (Delphi-style function return)
    4. With statement (record field shorthand)
    5. Goto / Label (unconditional jumps)
    6. In operator (set membership)
    7. Doubled-quote strings ('' inside string literals)
}
program FeaturesDemo;

{ ---------------------------------------------------------------------------
  1. Enumerated types
  2. Variant records
  --------------------------------------------------------------------------- }
type
  Season = (Spring, Summer, Autumn, Winter);
  Direction = (North, East, South, West);

  Shape = record
    name: string;
    x, y: integer;
    case tag: integer of
      0: (radius: integer);
      1: (width, height: integer);
      2: (base, side1, side2: integer);
  end;

  { A record with mixed fixed + variant fields }
  Employee = record
    id: integer;
    ename: string;
    case kind: integer of
      0: (hourlyRate: integer; hoursWorked: integer);
      1: (annualSalary: integer);
  end;

{ ---------------------------------------------------------------------------
  3. Result variable — Delphi-style function returns
  --------------------------------------------------------------------------- }
function SeasonName(s: integer): string;
begin
  case s of
    0: result := 'Spring';
    1: result := 'Summer';
    2: result := 'Autumn';
    3: result := 'Winter';
  else
    result := 'Unknown';
  end;
end;

function DirectionDegrees(d: integer): integer;
begin
  result := d * 90;
end;

function ShapeArea(var s: Shape): integer;
begin
  case s.tag of
    0: result := 3 * s.radius * s.radius;  { approximate pi*r^2 as 3*r^2 }
    1: result := s.width * s.height;
    2: result := (s.base * s.side1) div 2;  { approximate }
  else
    result := 0;
  end;
end;

function Max(a, b: integer): integer;
begin
  if a > b then
    result := a
  else
    result := b;
end;

function Clamp(val, lo, hi: integer): integer;
begin
  result := val;
  if result < lo then
    result := lo;
  if result > hi then
    result := hi;
end;

{ ---------------------------------------------------------------------------
  4. With statement
  --------------------------------------------------------------------------- }
procedure InitShape(var s: Shape; nx, ny: integer; shapeName: string);
begin
  with s do
  begin
    name := shapeName;
    x := nx;
    y := ny;
  end;
end;

procedure PrintEmployee(var e: Employee);
begin
  { Note: access string field directly to work around a known issue
    with reading string fields via 'with' in variant records }
  write('Employee #');
  with e do
  begin
    writeln(id, ': ', e.ename);
    if kind = 0 then
      writeln('  Hourly: $', hourlyRate, '/hr x ', hoursWorked, ' hrs = $', hourlyRate * hoursWorked)
    else
      writeln('  Salaried: $', annualSalary, '/year');
  end;
end;

{ ---------------------------------------------------------------------------
  Main program variables
  --------------------------------------------------------------------------- }
var
  s: integer;
  d: integer;
  shape1, shape2, shape3: Shape;
  emp1, emp2: Employee;
  i, n: integer;
  count: integer;

label 100, 200;

begin
  writeln('=== MXVM Extended Pascal Features Demo ===');
  writeln;

  { ---- 7. Doubled-quote strings ---- }
  writeln('--- Doubled-Quote Strings ---');
  writeln('It''s a beautiful day!');
  writeln('She said, ''Hello, World!''');
  writeln('Can''t stop, won''t stop.');
  writeln;

  { ---- 1. Enumerated types ---- }
  writeln('--- Enumerated Types ---');
  for s := Spring to Winter do
    writeln('Season ', s, ' = ', SeasonName(s));

  writeln('ord(Summer) = ', ord(Summer));
  writeln('succ(Spring) = ', succ(Spring));
  writeln('pred(Winter) = ', pred(Winter));

  s := North;
  inc(s);
  writeln('North + inc = ', s, ' (East)');
  inc(s, 2);
  writeln('East + inc(2) = ', s, ' (West)');
  dec(s);
  writeln('West - dec = ', s, ' (South)');
  writeln('Direction degrees: N=', DirectionDegrees(North),
          ' E=', DirectionDegrees(East),
          ' S=', DirectionDegrees(South),
          ' W=', DirectionDegrees(West));
  writeln;

  { ---- 6. In operator ---- }
  writeln('--- In Operator (Set Membership) ---');
  writeln('Warm seasons:');
  for s := Spring to Winter do
  begin
    if s in [Spring, Summer] then
      writeln('  ', SeasonName(s), ' is warm')
    else
      writeln('  ', SeasonName(s), ' is cold');
  end;

  writeln('Classifying 1-20:');
  for n := 1 to 20 do
  begin
    if n in [2, 3, 5, 7, 11, 13, 17, 19] then
      write(n, ':prime ')
    else
      write(n, ':composite ');
  end;
  writeln;
  writeln;

  { ---- 2. Variant records ---- }
  writeln('--- Variant Records ---');
  InitShape(shape1, 10, 20, 'Circle');
  shape1.tag := 0;
  shape1.radius := 5;
  writeln(shape1.name, ' at (', shape1.x, ',', shape1.y,
          ') radius=', shape1.radius,
          ' area~', ShapeArea(shape1));

  InitShape(shape2, 30, 40, 'Rectangle');
  shape2.tag := 1;
  shape2.width := 8;
  shape2.height := 6;
  writeln(shape2.name, ' at (', shape2.x, ',', shape2.y,
          ') ', shape2.width, 'x', shape2.height,
          ' area=', ShapeArea(shape2));

  InitShape(shape3, 50, 60, 'Triangle');
  shape3.tag := 2;
  shape3.base := 10;
  shape3.side1 := 7;
  shape3.side2 := 8;
  writeln(shape3.name, ' at (', shape3.x, ',', shape3.y,
          ') base=', shape3.base,
          ' area~', ShapeArea(shape3));
  writeln;

  { ---- 4. With statement + variant records ---- }
  writeln('--- With Statement + Variant Records ---');
  emp1.id := 101;
  emp1.ename := 'Alice';
  emp1.kind := 0;
  emp1.hourlyRate := 25;
  emp1.hoursWorked := 160;
  PrintEmployee(emp1);

  emp2.id := 102;
  emp2.ename := 'Bob';
  emp2.kind := 1;
  emp2.annualSalary := 75000;
  PrintEmployee(emp2);
  writeln;

  { ---- 3. Result variable ---- }
  writeln('--- Result Variable ---');
  writeln('Max(42, 17) = ', Max(42, 17));
  writeln('Max(3, 99) = ', Max(3, 99));
  writeln('Clamp(150, 0, 100) = ', Clamp(150, 0, 100));
  writeln('Clamp(-5, 0, 100) = ', Clamp(-5, 0, 100));
  writeln('Clamp(50, 0, 100) = ', Clamp(50, 0, 100));
  writeln;

  { ---- 5. Goto / Label ---- }
  writeln('--- Goto / Label ---');
  count := 0;

100:
  count := count + 1;
  if count > 5 then
    goto 200;
  write(count, ' ');
  goto 100;

200:
  writeln;
  writeln('Counted to 5 using goto');
  writeln;

  writeln('=== All features demonstrated successfully ===');
end.
