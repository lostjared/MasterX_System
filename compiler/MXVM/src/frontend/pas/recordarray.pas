program recordtest;
type
point = record
x, y: integer;
end;
var
points: array [0..10] of point;
i: integer;
begin

for i := 0 to 10 do
begin
points[i].x := i;
points[i].y := 7;
end;
writeln(points[0].x);
writeln(points[5].y);
end.