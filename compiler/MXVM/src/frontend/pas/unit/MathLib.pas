{ MathLib - A math utility unit demonstrating functions and procedures }
unit MathLib;

interface

function Add(a, b: integer): integer;
function Subtract(a, b: integer): integer;
function Multiply(a, b: integer): integer;
function Divide(a, b: integer): integer;
function Power(base, exp: integer): integer;
function Factorial(n: integer): integer;
function Abs(x: integer): integer;
function Max(a, b: integer): integer;
function Min(a, b: integer): integer;
procedure Swap(a, b: integer);
function TestReturn: integer;

implementation

function TestReturn: integer;
begin
TestReturn := 10 * 10;
end;

function Add(a, b: integer): integer;
begin
  Add := a + b;
end;

function Subtract(a, b: integer): integer;
begin
  Subtract := a - b;
end;

function Multiply(a, b: integer): integer;
begin
  Multiply := a * b;
end;

function Divide(a, b: integer): integer;
begin
  if b = 0 then
    Divide := 0
  else
    Divide := a div b;
end;

function Power(base, exp: integer): integer;
var
  result, i: integer;
begin
  result := 1;
  for i := 1 to exp do
    result := result * base;
  Power := result;
end;

function Factorial(n: integer): integer;
begin
  if n <= 1 then
    Factorial := 1
  else
    Factorial := n * Factorial(n - 1);
end;

function Abs(x: integer): integer;
begin
  if x < 0 then
    Abs := -x
  else
    Abs := x;
end;

function Max(a, b: integer): integer;
begin
  if a > b then
    Max := a
  else
    Max := b;
end;

function Min(a, b: integer): integer;
begin
  if a < b then
    Min := a
  else
    Min := b;
end;

procedure Swap(a, b: integer);
var
  temp: integer;
begin
  temp := a;
  a := b;
  b := temp;
end;

end.
