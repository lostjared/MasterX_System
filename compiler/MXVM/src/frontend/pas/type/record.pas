program Test;

type
	value = record
	x: integer;
	y: integer;
	z: array[0..10] of integer;
	end;

	zval = record
	xx: integer;
	yy: integer;
	meal: value;
	end;
var
px: value;
py: zval;
begin
px.x := 5;
px.y := 1;
px.z[0] := 5 * 5 + px.x;
writeln(px.x, px.y);
writeln(px.z[0]);
py.meal.x := 5;
writeln(py.meal.x);
end.
