program testprog;

function testFunction(x,y,z: real): real;
begin
    testFunction := x * y * z;
end;

begin
writeln(testFunction(10.5, 20.2, 30.1));
end.