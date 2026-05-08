{ UnitDemo - Demonstrates using multiple separately compiled Pascal units }
program UnitDemo;
uses io, MathLib, StringUtils;

var
  a, b, result: integer;
  i: integer;

begin
  StringUtils.PrintBanner('  MXVM Unit Demo  ', 20);
  writeln;


  i := TestReturn;

  writeln('Test return', i);

  { Basic arithmetic }
  StringUtils.PrintHeader('Arithmetic');
  a := 42;
  b := 17;
  StringUtils.PrintLabeledInt('a', a);
  StringUtils.PrintLabeledInt('b', b);
  StringUtils.PrintLabeledInt('Add(a, b)', MathLib.Add(a, b));
  StringUtils.PrintLabeledInt('Subtract(a, b)', MathLib.Subtract(a, b));
  StringUtils.PrintLabeledInt('Multiply(a, b)', MathLib.Multiply(a, b));
  StringUtils.PrintLabeledInt('Divide(a, b)', MathLib.Divide(a, b));
  StringUtils.PrintSeparator;
  writeln;

  { Power and factorial }
  StringUtils.PrintHeader('Power & Factorial');
  StringUtils.PrintLabeledInt('2^10', MathLib.Power(2, 10));
  StringUtils.PrintLabeledInt('3^5', MathLib.Power(3, 5));
  for i := 1 to 10 do
    StringUtils.PrintLabeledInt('Factorial', MathLib.Factorial(i));
  StringUtils.PrintSeparator;
  writeln;

  { Min, Max, Abs }
  StringUtils.PrintHeader('Min / Max / Abs');
  StringUtils.PrintLabeledInt('Max(100, 200)', MathLib.Max(100, 200));
  StringUtils.PrintLabeledInt('Min(100, 200)', MathLib.Min(100, 200));
  StringUtils.PrintLabeledInt('Abs(-55)', MathLib.Abs(-55));
  StringUtils.PrintLabeledInt('Abs(55)', MathLib.Abs(55));
  StringUtils.PrintSeparator;
  writeln;

  { Division by zero safety }
  StringUtils.PrintHeader('Edge Cases');
  StringUtils.PrintLabeledInt('Divide(10, 0)', MathLib.Divide(10, 0));
  StringUtils.PrintLabeledInt('Factorial(0)', MathLib.Factorial(0));
  StringUtils.PrintLabeledInt('Power(5, 0)', MathLib.Power(5, 0));
  StringUtils.PrintSeparator;
  writeln;

  StringUtils.PrintBanner('  Demo Complete  ', 20);
end.
