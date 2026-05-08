program PointerFull;
{ Test pointers in records, arrays, functions, and procedures }

type
  PInt = ^integer;
  TNode = record
    value: integer;
    next: ^integer;
  end;

var
  { pointer parameters and return values }
  a, b, temp: integer;
  i: integer;
  p: ^integer;
  result: integer;

  { record with pointer field }
  node: TNode;

  { array of pointers }
  ptrs: array[1..3] of ^integer;

{ procedure with pointer parameter }
procedure SetValue(ptr: ^integer; val: integer);
begin
  ptr^ := val;
end;

{ function returning a pointer }
function MakePtr: ^integer;
var
  q: ^integer;
begin
  new(q);
  q^ := 777;
  MakePtr := q;
end;

{ procedure that swaps via pointer params }
procedure SwapPtr(x: ^integer; y: ^integer);
var
  t: integer;
begin
  t := x^;
  x^ := y^;
  y^ := t;
end;

begin
  writeln('=== Pointer Full Test ===');

  { Test 1: Pointer parameter }
  writeln('--- procedure with ^integer param ---');
  new(p);
  SetValue(p, 42);
  writeln(p^);
  dispose(p);

  { Test 2: Pointer return type }
  writeln('--- function returning ^integer ---');
  p := MakePtr();
  writeln(p^);
  dispose(p);

  { Test 3: Swap via pointer params }
  writeln('--- swap via pointer params ---');
  a := 11;
  b := 22;
  writeln(a);
  writeln(b);
  SwapPtr(@a, @b);
  writeln(a);
  writeln(b);

  { Test 4: Record with pointer field }
  writeln('--- record with ^integer field ---');
  node.value := 55;
  new(node.next);
  node.next^ := 66;
  writeln(node.value);
  writeln(node.next^);
  dispose(node.next);

  { Test 5: Array of pointers }
  writeln('--- array of ^integer ---');
  for i := 1 to 3 do
  begin
    new(ptrs[i]);
    ptrs[i]^ := i * 100;
  end;
  for i := 1 to 3 do
    writeln(ptrs[i]^);
  for i := 1 to 3 do
    dispose(ptrs[i]);

  writeln('=== done ===');
end.
