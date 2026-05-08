program TestArray;
var
    grid: array[0..4] of array[0..4] of integer;
    i, z: integer;
begin
    for z := 0 to 3 do
    begin
        for i := 0 to 3 do
        begin
            grid[i][z] := i * z + 1;
            writeln('value is: ', grid[i][z]);
        end;
    end;
end.

