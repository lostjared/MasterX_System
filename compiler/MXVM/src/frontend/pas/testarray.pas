program TestArray;

type
  TInner = record
    a: integer;
    b: array[1..3] of real;
  end;

  TOuter = record
    id: integer;
    name: string;
    inner: TInner;
    grid: array[0..1] of array[2..4] of integer;  { 2 x 3 ints }
  end;

  TMat = array[1..2] of array[-1..1] of real;     { 2 x 3 reals }

  TBook = record
    title: string;
    ratings: array[1..5] of integer;
  end;

var
  rec: TOuter;
  arrRec: array[10..12] of TOuter;  { <-- declared here, visible everywhere below }
  mat: TMat;
  books: array[1..2] of TBook;
  i, j: integer;
  total: integer;
  sumR: real;

procedure InitOuter(var o: TOuter);
begin
  o.id := 42;
  o.name := 'outer!';
  o.inner.a := 7;
  o.inner.b[1] := 1.25;
  o.inner.b[2] := 2.5;
  o.inner.b[3] := 3.75;
  o.grid[0][2] := 10;   { lower bound 2..4 }
  o.grid[1][4] := 20;
end;

function SumGrid(var o: TOuter): integer;
var s: integer;
begin
  s := 0;
  for i := 0 to 1 do
    for j := 2 to 4 do
      s := s + o.grid[i][j];
  SumGrid := s;
end;

function DotFirstRow(m: TMat): real;
begin
  DotFirstRow := m[1][-1] + m[1][0] + m[1][1];
end;

procedure BumpBook(var b: TBook);
begin
  b.ratings[5] := b.ratings[5] + 1;
end;

begin
  { Initialize a standalone record }
  InitOuter(rec);
  rec.grid[0][3] := 11;
  rec.grid[1][2] := 12;

  { Initialize an array of records }
  InitOuter(arrRec[10]);
  InitOuter(arrRec[11]);
  InitOuter(arrRec[12]);

  { Modify nested fields inside array-of-records }
  arrRec[12].inner.b[2] := 9.5;
  arrRec[10].grid[1][3] := 33;

  { Work with a record containing a nested array-of-arrays }
  total := SumGrid(rec);

  { Work with a typedef’d array-of-arrays of reals }
  mat[1][-1] := 1.0; mat[1][0] := 2.0; mat[1][1] := 3.0;
  mat[2][-1] := 4.0; mat[2][0] := 5.0; mat[2][1] := 6.0;
  sumR := DotFirstRow(mat);

  { Records with array fields of simple types }
  books[1].title := 'Pascal';
  books[1].ratings[5] := 8;
  BumpBook(books[1]);

  writeln('rec.id=', rec.id);
  writeln('rec.inner.a=', rec.inner.a);
  writeln('rec.inner.b[1]=', rec.inner.b[1]);
  writeln('rec.grid[0][2]=', rec.grid[0][2]);
  writeln('SumGrid(rec)=', total);

  writeln('arrRec[12].inner.b[2]=', arrRec[12].inner.b[2]);
  writeln('arrRec[10].grid[1][3]=', arrRec[10].grid[1][3]);

  writeln('DotFirstRow(mat)=', sumR);
  writeln('books[1].title=', books[1].title);
  writeln('books[1].ratings[5]=', books[1].ratings[5]);
end.

