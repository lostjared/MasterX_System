program DynArrayTest;

var
  arr: array of integer;
  i: integer;

begin
  WriteLn('Dynamic Array Test');
  WriteLn('------------------');

  { Set length to 5 }
  SetLength(arr, 5);
  WriteLn('Length after SetLength(5): ', Length(arr));

  { Fill the array }
  for i := 0 to High(arr) do
    arr[i] := (i + 1) * 10;

  { Print elements }
  Write('Elements: ');
  for i := 0 to High(arr) do
    Write(arr[i], ' ');
  WriteLn;

  { Grow the array }
  SetLength(arr, 8);
  WriteLn('Length after SetLength(8): ', Length(arr));

  { Fill new elements }
  arr[5] := 60;
  arr[6] := 70;
  arr[7] := 80;

  { Print all elements }
  Write('Elements: ');
  for i := 0 to High(arr) do
    Write(arr[i], ' ');
  WriteLn;

  { Check Low }
  WriteLn('Low: ', Low(arr));
  WriteLn('High: ', High(arr));

  WriteLn('Done.');
end.
