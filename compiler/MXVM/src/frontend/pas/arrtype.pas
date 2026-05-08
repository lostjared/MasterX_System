program RecordTest;
type
  Point = record
    x: integer;
    y: integer;
    test_val: array[0..10] of real;
  end;
var
  p: Point;
  z: integer;

procedure printPoint(pi: Point);
var
px: array[0..2] of Point;
i: integer;
begin
px[0].x := 100;
px[0].y := 200;
writeln(px[0].x, ':', px[0].y);
end;


begin
  p.x := 5;
  p.y := 10;
  z := p.x * p.y;
  printPoint(p);
  writeln('equals: ', z);
  writeln(0.5);
end.
