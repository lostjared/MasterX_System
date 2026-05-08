program TestFeatures;

{ Test packed keyword - should be accepted and ignored }
type
  PackedRec = packed record
    x: integer;
    y: integer;
  end;
  PackedArr = packed array[1..5] of integer;
  Digits = set of integer;

var
  s: set of integer;
  s2: Digits;
  i: integer;
  pr: PackedRec;
  pa: PackedArr;

begin
  { Test packed record }
  pr.x := 10;
  pr.y := 20;
  writeln('Packed record: x=', pr.x, ' y=', pr.y);

  { Test packed array }
  pa[1] := 100;
  pa[2] := 200;
  writeln('Packed array: [1]=', pa[1], ' [2]=', pa[2]);

  { Test set literal with in operator }
  if 3 in [1, 2, 3, 4, 5] then
    writeln('3 is in [1,2,3,4,5]');

  if 7 in [1, 2, 3] then
    writeln('ERROR: 7 should not be in [1,2,3]')
  else
    writeln('7 is NOT in [1,2,3] - correct');

  { Test set variable assignment }
  s := [1, 3, 5, 7, 9];

  { Test in operator with set variable }
  if 3 in s then
    writeln('3 is in s - correct');

  if 4 in s then
    writeln('ERROR: 4 should not be in s')
  else
    writeln('4 is NOT in s - correct');

  { Test include and exclude }
  include(s, 4);
  if 4 in s then
    writeln('After include(s,4): 4 is in s - correct');

  exclude(s, 3);
  if 3 in s then
    writeln('ERROR: 3 should not be in s after exclude')
  else
    writeln('After exclude(s,3): 3 is NOT in s - correct');

  { Test set union }
  s2 := s + [10, 20, 30];
  if 10 in s2 then
    writeln('10 is in s2 after union - correct');
  if 1 in s2 then
    writeln('1 is in s2 after union - correct');

  { Test set intersection }
  s2 := s * [1, 4, 5, 100];
  if 1 in s2 then
    writeln('1 is in intersection - correct');
  if 4 in s2 then
    writeln('4 is in intersection - correct');
  if 9 in s2 then
    writeln('ERROR: 9 should not be in intersection')
  else
    writeln('9 is NOT in intersection - correct');

  { Test set difference }
  s2 := s - [1, 5];
  if 1 in s2 then
    writeln('ERROR: 1 should not be in difference')
  else
    writeln('1 is NOT in difference - correct');
  if 4 in s2 then
    writeln('4 is in difference - correct');

  writeln('All set tests completed.');
end.
