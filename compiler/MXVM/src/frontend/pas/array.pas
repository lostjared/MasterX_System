program ArrayExample;
var
  numbers: array[1..5] of real; 
  i: integer;

begin
  for i := 1 to 5 do
    numbers[i] := i * 10; 

  writeln('Array contents:');
  for i := 1 to 5 do
    writeln('numbers[', i, '] = ', numbers[i]);
end.
