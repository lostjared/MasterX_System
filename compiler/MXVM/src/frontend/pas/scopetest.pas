program ScopeTest;

{
  This program is designed to test the scoping rules of the validator.
  It covers global, local, nested, parameter, and record field scopes.
}

const
  GlobalConst = 100;

type
  { Test using the same field names in different record types. }
  TPerson = record
    name: string;
    id: integer;
  end;

  TLocation = record
    city: string;
    id: integer; { 'id' is also in TPerson, testing separate field scopes. }
  end;

var
  globalVar: integer;
  person1: TPerson;
  location1: TLocation;

{ Test a forward-declared procedure. }
{ procedure ForwardDeclaredProc(p: integer); forward; }

procedure TestNestedScopes;
var
  globalVar: real;

  procedure InnerProc(globalVar: boolean);
  var
    innerVar: integer;
  begin
    innerVar := 30;
    if globalVar then innerVar := 40;
    innerVar := GlobalConst;
  end;

begin
  globalVar := 10.5;
  InnerProc(true);
end;

{ Implementation of the forward-declared procedure. }
procedure ForwardDeclaredProc(p: integer);
var
  localVar: integer;
begin
  { This procedure uses the GLOBAL 'integer' globalVar, as it's not shadowed here. }
  localVar := p + globalVar;
end;

procedure TestRecordScopes;
var
  person2: TPerson;
  location2: TLocation;
begin
  { This procedure tests that record fields do not clash between different record types. }
  person2.name := 'Alice';
  person2.id := 123;

  location2.city := 'Paris';
  location2.id := 456; { This should not conflict with person2.id. }
end;

{ Main program block }
begin
  { 1. Test global scope access. }
  globalVar := GlobalConst;

  { 2. Test procedure calls and nested scopes. }
  TestNestedScopes;

  { 3. Test forward declaration call. }
  ForwardDeclaredProc(5);

  { 4. Test record field scopes. }
  TestRecordScopes;

  { 5. Test direct access to global record variables. }
  person1.id := 789;
  location1.id := 987;


  { --- TESTS THAT SHOULD FAIL VALIDATION (commented out) --- }

  { ERROR: Redeclaration of 'globalVar' in the same (global) scope. }
  { var globalVar: integer; }

  { ERROR: Use of an undeclared identifier. }
  { undeclaredVar := 10; }

  { ERROR: Accessing a local variable from an outer scope. }
  { 'innerVar' only exists inside 'InnerProc'. }
  { globalVar := innerVar; }

end.
