program VariantEnumResult;

type
  Color = (Red, Green, Blue, Yellow);

  ShapeKind = (Circle, Rectangle);

  Shape = record
    x, y: integer;
    case kind: integer of
      0: (radius: integer);
      1: (width, height: integer);
  end;

var
  c: integer;
  s: Shape;

function DoubleIt(n: integer): integer;
begin
  result := n * 2;
end;

function ColorName(c: integer): string;
begin
  case c of
    0: result := 'Red';
    1: result := 'Green';
    2: result := 'Blue';
    3: result := 'Yellow';
  else
    result := 'Unknown';
  end;
end;

begin
  { Test enumerated types }
  c := Red;
  writeln('Red = ', c);
  c := Blue;
  writeln('Blue = ', c);
  writeln('Ord(Green) = ', ord(Green));
  writeln('Succ(Red) = ', succ(Red));
  writeln('Pred(Blue) = ', pred(Blue));

  { Test inc/dec }
  c := Red;
  inc(c);
  writeln('After inc(Red): ', c);
  dec(c);
  writeln('After dec back: ', c);

  { Test result variable }
  writeln('DoubleIt(21) = ', DoubleIt(21));
  writeln('ColorName(2) = ', ColorName(2));

  { Test variant record }
  s.x := 10;
  s.y := 20;
  s.kind := 0;
  s.radius := 5;
  writeln('Circle at (', s.x, ',', s.y, ') radius=', s.radius);

  s.kind := 1;
  s.width := 30;
  s.height := 40;
  writeln('Rect at (', s.x, ',', s.y, ') ', s.width, 'x', s.height);
end.
