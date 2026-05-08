program TestFileIO;

var
  f: file;
  line: string;

begin
  { Write to a file }
  assign(f, 'test_output.txt');
  rewrite(f);
  writeln(f, 'Hello from Pascal file I/O!');
  writeln(f, 'Line two');
  writeln(f, 'Line three');
  close(f);
  writeln('File written successfully.');

  { Read it back }
  assign(f, 'test_output.txt');
  reset(f);
  while not eof(f) do
  begin
    readln(f, line);
    writeln('Read: ', line);
  end;
  close(f);

  { Append to the file }
  assign(f, 'test_output.txt');
  append(f);
  writeln(f, 'Appended line');
  close(f);

  { Read again to verify append }
  assign(f, 'test_output.txt');
  reset(f);
  while not eof(f) do
  begin
    readln(f, line);
    writeln('After append: ', line);
  end;
  close(f);

  writeln('All file I/O tests completed.');
end.
