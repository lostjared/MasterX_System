program TestRand;
var
  i, v: integer;
  neg_count, half_count: integer;
begin
  neg_count := 0;
  half_count := 0;
  for i := 0 to 99 do
  begin
    v := rand() mod 640;
    if v < 0 then
      neg_count := neg_count + 1;
    if v < 320 then
      half_count := half_count + 1;
  end;
  write('negative: ');
  writeln(neg_count);
  write('below 320: ');
  writeln(half_count);
  
  writeln('first 20 rand mod 640:');
  for i := 0 to 19 do
  begin
    v := rand() mod 640;
    write(v);
    write(' ');
  end;
  writeln('');
  writeln('done');
end.
