# MXVM -- Virtual Machine, Compiler & Code Generator {#mainpage}

**MXVM** is a custom virtual machine and compiler suite written in modern C++20.
It provides a RISC-style bytecode language with an interpreter, a Pascal-subset
compiler frontend, and native x86-64 code generators targeting Linux (System V ABI),
macOS (Darwin), and Windows (Win64).

> **Experimental / educational project -- not intended for production use.**

> **[Download PDF Reference Manual](https://lostsidedead.biz/MXVM/doc/MXVM_Reference.pdf)**

> **[View Examples Online](https://lostsidedead.biz/MXVM/code/)**

---

## Architecture Overview

| Layer | Component | Description |
|-------|-----------|-------------|
| **Frontend** | Scanner / Lexer | Tokenises `.mxvm` and `.pas` source files |
| **Frontend** | Parser / AST | Builds a typed abstract syntax tree |
| **Frontend** | Validator | Scope-based semantic analysis and type checking |
| **Middle** | Code Generator | Emits MXVM bytecode from the AST |
| **Backend** | Interpreter | Stack-based execution engine for bytecode |
| **Backend** | x86-64 Codegen | Native assembly output (System V, Win64, Darwin) |
| **Backend** | Peephole Optimizer | Post-generation assembly cleanup passes |
| **Runtime** | Module System | Dynamically loaded C shared libraries (`io`, `std`, `string`, `sdl`) and Pascal unit linking |
| **Tools** | mxvm-html | Converts `.mxvm` and `.pas` source files to syntax-highlighted HTML (Borland Delphi colour scheme) |

@dot
digraph pipeline {
    rankdir=TB;
    node [shape=box, style=filled, fillcolor="#E8F0FE", fontname="Helvetica", fontsize=12];
    edge [color="#555555"];

    source [label=".pas / .mxvm source", shape=note, fillcolor="#FFF9C4"];
    scanner [label="Scanner"];
    parser [label="Parser"];
    validator [label="Validator"];
    codegen [label="CodeGen (IR)", fillcolor="#D4EDDA"];
    interp [label="Interpreter", fillcolor="#F8D7DA"];
    sysv [label="SysV x64", fillcolor="#F8D7DA"];
    win64 [label="Win64 x64", fillcolor="#F8D7DA"];

    source -> scanner;
    scanner -> parser;
    parser -> validator;
    validator -> codegen;
    codegen -> interp;
    codegen -> sysv;
    codegen -> win64;
}
@enddot

---

## Building

```bash
git clone https://github.com/lostjared/MXVM.git
cd MXVM && mkdir build && cd build
cmake .. && make -j$(nproc)
```

Requires: C++20 compiler, CMake >= 3.10.\n
Optional: SDL2 + SDL2_ttf (for the SDL module), Emscripten (WebAssembly target).

---

## Running

> **Note:** The `.mxvm` extension is optional — if you omit it, `mxvmc` will append `.mxvm` automatically (e.g. `mxvmc program` is equivalent to `mxvmc program.mxvm`).

| Mode | Command |
|------|---------|
| **Interpret** | `mxvmc program.mxvm --path /usr/local/lib` |
| **Compile -> Assembly** | `mxvmc program.mxvm --path /usr/local/lib --action translate` |
| **Compile -> Executable** | `mxvmc program.mxvm --path /usr/local/lib --action compile` |

### `mxx` Pascal Compiler Arguments

| Mode | Command | Description |
|------|---------|-------------|
| **Legacy** | `mxx source.pas output.mxvm` | Compile a single file to a named output |
| **Explicit I/O** | `mxx -i source.pas -o output.mxvm` | Explicit input/output flags; `-o` is optional — when omitted, the output filename is derived from the `program` or `unit` name declared inside the source file (e.g. `program Test;` → `Test.mxvm`) |
| **Batch** | `mxx -c source1.pas source2.pas ...` | Compile multiple files; each output filename is derived from the declared `program`/`unit` name inside the file |

```bash
# Legacy positional arguments (explicit output name)
mxx program.pas program.mxvm

# Explicit input/output flags
mxx -i program.pas -o program.mxvm
mxx -i program.pas        # reads 'program MyApp;' inside -> MyApp.mxvm

# Batch compile: output names come from the declared program/unit names
mxx -c unit1.pas unit2.pas main.pas
# e.g. 'unit MathUtils;' -> MathUtils.mxvm, 'program TestMain;' -> TestMain.mxvm
```

---

# mxvm-html — Syntax Highlighter {#mxvm_html}

`mxvm-html` converts `.mxvm` bytecode and `.pas` Pascal source files to
self-contained, syntax-highlighted HTML pages using a Borland Delphi 7
colour scheme.  The generated pages embed all CSS inline and work without a
web server.  An optional external stylesheet can override the defaults.

**Installed location:** `/usr/local/bin/mxvm-html`

## Usage

| Mode | Command |
|------|---------|
| **Single file** | `mxvm-html -i <file> [-o <output.html>] [-s <stylesheet.css>]` |
| **Batch** | `mxvm-html -c <file1> [file2 ...]` |
| **Help** | `mxvm-html -h` |

### Options

| Flag | Argument | Description |
|------|----------|-------------|
| `-i` | `<file>` | Input file (`.mxvm` or `.pas`) |
| `-o` | `<file.html>` | Output HTML file.  Defaults to auto-named (see below). |
| `-s` | `<stylesheet.css>` | External CSS href embedded in the `<link>` tag.  Defaults to `mxvm-highlight.css`. |
| `-c` | `<file1> [file2 …]` | Batch mode — convert multiple files in one invocation. |
| `-h` | — | Print usage and exit. |

### Auto-naming

When `-o` is omitted, the output name is derived from the first top-level
declaration in the source file:

| Keyword | Example | Output |
|---------|---------|--------|
| `program` | `program HelloWorld;` | `HelloWorld.html` |
| `unit` | `unit MathUtils;` | `MathUtils.html` |
| `object` | `object Stack;` | `Stack.html` |

If no declaration is found the output falls back to `<input>.html`.

## Examples

```bash
# Highlight a single Pascal source file
mxvm-html -i hello.pas -o hello.html

# Auto-name from 'program Hello;' declaration
mxvm-html -i hello.pas          # -> Hello.html

# Full pipeline: compile, then highlight both source and bytecode
mxx -i program.pas -o program.mxvm
mxvm-html -i program.pas  -o program.pas.html
mxvm-html -i program.mxvm -o program.mxvm.html

# Batch-convert a directory of Pascal files
mxvm-html -c examples/*.pas

# Use a custom stylesheet
mxvm-html -i hello.pas -o hello.html -s ../../mxvm-highlight.css
```

## CSS Customisation

Each output page contains an inline `<style>` block (Borland Delphi defaults)
followed by a `<link>` to an external stylesheet.  Rules in the external file
take precedence via normal CSS cascade.  The relevant span classes are:

| Class | Applies to |
|-------|-----------|
| `.kw` | Reserved keywords / instructions |
| `.typ` | Built-in types |
| `.bi` | Built-in routines and constants (Pascal only) |
| `.str` | String and character literals |
| `.num` | Numeric literals |
| `.com` | Comments |
| `.lbl` | Label declarations (`.mxvm` only) |
| `.op` | Operators / punctuation (`.mxvm` only) |
| `.id` | All other identifiers |

See [docs/mxvm-html.html](docs/mxvm-html.html) for the full reference and a
dark-theme CSS example.

---

# MXVM Bytecode Language Reference {#bytecode}

## Program Structure

Every `.mxvm` file is wrapped in a `program` (or `object`) block containing
three sections: **data**, **module**, and **code**.

```
program MyApp {

  section data {
    int    counter = 0
    float  pi      = 3.14159
    string msg     = "Hello, MXVM!\n"
    ptr    buffer  = null
    byte   flags   = 0
  }

  section module { io, string, std }

  section code {
  start:
    print msg
    add counter, counter, 1
    cmp counter, 10
    jl  start
    done
  }
}
```

### Data Section -- Variable Types

| Type | Size | Description |
|------|------|-------------|
| `int` | 64-bit | Signed integer |
| `float` | 64-bit | Double-precision floating point |
| `string` | varies | NUL-terminated string with optional max-length: `string name, 256` |
| `ptr` | 64-bit | Opaque pointer (for `alloc`/module use) |
| `byte` | 8-bit | Unsigned byte |
| `label` | -- | Code address (for indirect jumps) |

- Variables are declared with an optional initialiser: `int x = 42`.
- All string literals must live in the data section -- inline string constants
  are **not** allowed in the code section.
- Hex immediates are supported in code: `mov x, 0xFF`.

### Module Section

```
section module { io, string, std }
```

Lists the C shared-library modules whose functions may be called via `invoke`.
See the @ref modules "Module Reference" below.

### Code Section

The code section is a flat sequence of labeled instructions.

- **One instruction per line.**
- **Destination-first operand order** (RISC convention): `add dest, src1, src2`.
- Labels end with `:` and may be marked as functions with `function`:
  ```
  my_func: function
      ; ... body ...
      ret
  ```

---

## Instruction Set {#instructions}

### Arithmetic & Logic

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `mov` | `dest, src` | `dest <- src` |
| `add` | `dest, a, b` | `dest <- a + b` (2-operand form: `dest += a`) |
| `sub` | `dest, a, b` | `dest <- a - b` |
| `mul` | `dest, a, b` | `dest <- a * b` |
| `div` | `dest, a, b` | `dest <- a / b` (integer division for int operands) |
| `mod` | `dest, a, b` | `dest <- a mod b` |
| `neg` | `dest, a` | `dest <- -a` |
| `or`  | `dest, a, b` | Bitwise OR |
| `and` | `dest, a, b` | Bitwise AND |
| `xor` | `dest, a, b` | Bitwise XOR |
| `not` | `dest, a` | Bitwise NOT |

### Comparison & Branching

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `cmp` | `a, b` | Set flags from `a - b` |
| `fcmp` | `a, b` | Floating-point compare, set flags |
| `jmp` | `label` | Unconditional jump |
| `je` / `jne` | `label` | Jump if equal / not equal |
| `jl` / `jle` | `label` | Jump if less / less-or-equal (signed) |
| `jg` / `jge` | `label` | Jump if greater / greater-or-equal (signed) |
| `jz` / `jnz` | `label` | Jump if zero flag set / clear |
| `ja` / `jb` | `label` | Jump if above / below (unsigned) |
| `jae` / `jbe` | `label` | Jump if above-or-equal / below-or-equal (unsigned) |
| `jc` / `jnc` | `label` | Jump if carry / no carry |
| `jp` / `jnp` | `label` | Jump if parity / no parity |
| `jo` / `jno` | `label` | Jump if overflow / no overflow |
| `js` / `jns` | `label` | Jump if sign / no sign |

### Memory & Pointers

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `load` | `dest, ptr, index, size` | `dest <- *(ptr + index * size)` |
| `store` | `src, ptr, index, size` | `*(ptr + index * size) <- src` |
| `lea` | `dest, base, offset` | Load effective address |
| `alloc` | `ptr, elem_size, count` | `ptr <- calloc(count, elem_size)` |
| `realloc` | `ptr, elem_size, count` | Resize `ptr` to hold `count` elements of `elem_size` bytes; zero-fills new space |
| `free` | `ptr` | Release allocated memory |

### Stack Operations

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `push` | `src` | Push value onto the VM stack |
| `pop` | `dest` | Pop top of stack into `dest` |
| `stack_load` | `dest, offset` | Read stack slot at `offset` |
| `stack_store` | `src, offset` | Write to stack slot at `offset` |
| `stack_sub` | `amount` | Reserve stack space |

### Control Flow & Calls

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `call` | `label` | Call an internal function (or `Object.Function`) |
| `ret` | -- | Return from function |
| `invoke` | `func, args...` | Call an external module function (C ABI) |
| `return` | `var` | Capture the return value of the last `invoke` |
| `done` | -- | Normal program termination |
| `exit` | `code` | Terminate with exit code |

### I/O

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `print` | `fmt, args...` | Printf-style formatted output |
| `string_print` | `ptr` | Print raw string at pointer |
| `getline` | `ptr` | Read a line from stdin into buffer |

### Type Conversion

| Instruction | Operands | Effect |
|-------------|----------|--------|
| `to_int` | `dest, ptr` | Parse string -> integer |
| `to_float` | `dest, ptr` | Parse string -> float |

### Calling Convention -- `invoke` / `return`

External functions loaded from modules are called with `invoke`.
The return value is captured into a variable with the `return` pseudo-instruction
immediately following the `invoke`:

```
invoke fopen, filename, mode
return file_handle

invoke strlen, my_string
return len
```

---

## Object System {#objects}

MXVM supports **objects** -- separately compiled units that can be
linked into a main program.

```
object Utils {
  section data { ... }
  section code {
    print_line: function
        print fmt
        ret
  }
}
```

The main program references object functions with dot notation:

```
call Utils.print_line
```

Objects can be **inline** (defined in the same file) or loaded from separate files
via `--object-path`.

---

# MXVM Pascal Frontend {#pascal}

MXVM includes a **Pascal-subset compiler** that parses Pascal source code,
builds a typed AST, validates semantics with scope-based analysis, and emits
MXVM bytecode.

## Supported Language Features

### Case Insensitivity

Like standard Pascal, all keywords, built-in routine names, and type names
are **case-insensitive**.  `BEGIN`, `Begin`, and `begin` are equivalent.
User-defined identifiers (variable names, function names, unit names) are
also resolved case-insensitively:

```pascal
PROGRAM MixedCase;
VAR x: INTEGER;
BEGIN
  x := 42;
  WriteLn('x = ', x);
  WRITELN('works too');
END.
```

### Program Structure

```pascal
program HelloWorld;
uses io, std, strlib;

const
  MAX = 100;

type
  PInt = ^integer;
  Point = record
    x: integer;
    y: integer;
  end;
  IntArray = array[0..9] of integer;

var
  i: integer;
  name: string;
  flag: boolean;
  pt: Point;
  nums: IntArray;
  p: PInt;

procedure Greet(msg: string);
begin
  writeln(msg);
end;

function Add(a, b: integer): integer;
begin
  Add := a + b;
end;

begin
  name := 'MXVM Pascal';
  writeln('Hello from ', name);
  writeln('Sum = ', Add(3, 4));
  Greet('Welcome!');
end.
```

### Uses Clause

The `uses` clause imports runtime modules and/or separately compiled units.
It must appear immediately after the `program` (or `unit`) declaration:

```pascal
program MyApp;
uses std, io, strlib, MathUtils;
```

Built-in runtime modules: `std`, `io`, `strlib`, `sdl`.  Any other name is
treated as a reference to a separately compiled Pascal **unit** (see @ref units
below).  If no `uses` clause is present, modules are auto-detected from
builtin function usage.

### Units (Separately Compiled Modules) {#units}

Pascal units allow you to split a project into separately compiled modules.
A unit has an **interface** section (exported declarations visible to importers)
and an **implementation** section (private code).

#### Unit Structure

```pascal
unit MathUtils;

interface

function Add(a, b: integer): integer;
function Multiply(a, b: integer): integer;
procedure PrintResult(x: integer);

implementation

uses io;

function Add(a, b: integer): integer;
begin
  Add := a + b;
end;

function Multiply(a, b: integer): integer;
begin
  Multiply := a * b;
end;

procedure PrintResult(x: integer);
begin
  writeln(x);
end;

end.
```

#### Using a Unit

A program imports a unit by listing it in the `uses` clause alongside any
runtime modules:

```pascal
program TestMain;
uses io, MathUtils;

var
  result: integer;

begin
  result := MathUtils.Add(3, 4);
  writeln('3 + 4 = ', result);
  result := MathUtils.Multiply(5, 6);
  writeln('5 * 6 = ', result);
  MathUtils.PrintResult(99);
end.
```

#### Qualified Names {#qualified-names}

When calling functions or procedures from another unit, you may use
**qualified names** (`UnitName.RoutineName`) to make the origin explicit:

```pascal
result := MathUtils.Add(3, 4);   { qualified function call }
MathUtils.PrintResult(result);   { qualified procedure call }
MathUtils.PrintResult;           { zero-arg qualified call (parens optional) }
```

Unqualified calls are also accepted -- the compiler resolves the name from
the `uses` list:

```pascal
result := Add(3, 4);             { unqualified -- still valid }
PrintResult(result);             { also valid }
```

**Variables** and **constants** from another unit **must** always be qualified:

```pascal
writeln(MathUtils.counter);      { variable -- qualification required }
writeln(MathUtils.MAX_VALUE);    { constant -- qualification required }
```

| Element | Qualified | Unqualified |
|---------|-----------|-------------|
| **Variables** | Required | Not allowed |
| **Constants** | Required | Not allowed |
| **Functions / Procedures** | Allowed | Allowed |

#### Compiling and Linking

Each unit is compiled to a separate `.mxvm` object file.  The main program
is compiled separately and references the unit via `section object`.  At
runtime the VM loads and links unit objects automatically.

```bash
# 1. Compile the unit
mxx MathUtils.pas MathUtils.mxvm

# 2. Compile the main program (unit .pas must be in the same directory)
mxx TestMain.pas TestMain.mxvm

# 3. Run — tell the VM where to find object files
mxvmc TestMain.mxvm -x .
```

The `-x` (or `--object-path`) flag tells the VM where to search for `.mxvm`
object files referenced by `section object` declarations.  The default is `.`
(current directory).

#### How It Works

| Step | Detail |
|------|--------|
| **Unit compilation** | The Pascal compiler parses the unit and emits an MXVM `object` block (instead of `program`). The interface forward declarations register function signatures; the implementation section generates the actual function bodies. |
| **Program compilation** | When compiling a program that `uses` a non-native module name, the compiler locates the unit's `.pas` file, parses its interface to learn function/procedure signatures, and emits qualified cross-object references (e.g. `call MathUtils.FUNC_Add`, `mov MathUtils.rbx, 3`). |
| **VM linking** | The VM's `section object { MathUtils }` triggers loading of `MathUtils.mxvm` from the object path. Object labels and variables are merged into the execution environment using qualified names (`MathUtils.FUNC_Add`, `MathUtils.rax`, etc.). |

#### Interface Section

The interface section may contain:
- Procedure and function **forward declarations** (signature only, no body)
- Type declarations
- Constant declarations
- Variable declarations

#### Implementation Section

The implementation section may contain:
- An optional additional `uses` clause
- Full procedure and function definitions (with bodies)
- Additional type, constant, and variable declarations

### Data Types

| Type | Description |
|------|-------------|
| `integer` | 64-bit signed integer |
| `real` | 64-bit double-precision float |
| `boolean` | Stored as integer (0 or 1); `true` / `false` literals |
| `char` | Stored as integer (character ordinal value) |
| `string` | Heap-managed, pointer-based string |
| `^Type` | Typed pointer (e.g. `^integer`, `^Point`) |
| `array[lo..hi] of T` | Fixed-size array with arbitrary integer bounds |
| `array of T` | Dynamic array (resized at runtime with `SetLength`) |
| `record ... end` | Named record (struct) with typed fields; variant parts supported (`case tag: type of`) |
| `(id, id, ...)` | Enumerated type -- constants mapped to integers starting from 0 |
| `set of T` | Set of ordinal values (0..255); supports `in`, `+`, `-`, `*`, `include`, `exclude` |
| `file` / `file of T` | File handle for Pascal-style file I/O (`assign`, `reset`, `rewrite`, `append`, `close`, `eof`) |

### Constants

```pascal
const
  MAX     = 100;
  PI      = 3.14159;
  GREETING = 'Hello';
  ENABLED = true;
```

Built-in constants: `true`, `false`, `maxint`, `nil`.

### Type Declarations

```pascal
type
  Counter   = integer;         { type alias }
  PInt      = ^integer;        { pointer type }
  IntArray  = array[1..10] of integer;
  Matrix    = array[0..3] of array[0..3] of real;  { multi-dimensional }
  Point = record
    x: integer;
    y: integer;
  end;

  { Enumerated types }
  Color     = (Red, Green, Blue, Yellow);
  Direction = (North, East, South, West);

  { Variant records }
  Shape = record
    x, y: integer;
    case tag: integer of
      0: (radius: integer);
      1: (width, height: integer);
  end;
```

### Variable Declarations

```pascal
var
  a, b, c: integer;           { multiple identifiers }
  name: string;
  pi: real;
  items: array[0..99] of integer;
  data: array of real;         { dynamic array }
  pt: Point;
  p: ^integer;
```

Variables may include initialisers: `var x: integer = 42;`

### Operators

| Category | Operators |
|----------|-----------|
| **Arithmetic** | `+`  `-`  `*`  `/`  `div`  `mod` |
| **Relational** | `=`  `<>`  `<`  `<=`  `>`  `>=` |
| **Logical** | `and`  `or`  `not` |
| **String** | `+` (concatenation) |
| **Pointer** | `^` (dereference, postfix), `@` (address-of, prefix) |
| **Unary** | `+`  `-`  `not` |

### Control Flow

```pascal
{ If-Then-Else }
if x > 0 then
  writeln('positive')
else
  writeln('non-positive');

{ While }
while i < 10 do
begin
  writeln(i);
  i := i + 1;
end;

{ For }
for i := 1 to 10 do
  writeln(i);

for i := 10 downto 1 do
  writeln(i);

{ Repeat-Until }
repeat
  readln(x);
until x = 0;

{ Case }
case ch of
  1, 2, 3: writeln('small');
  4..6:    writeln('medium');
else
  writeln('other');
end;

{ Flow control }
break;       { exit loop }
continue;    { next iteration }
exit;        { exit current scope }
```

### Procedures & Functions

```pascal
procedure Swap(var a, b: integer);
var tmp: integer;
begin
  tmp := a;
  a := b;
  b := tmp;
end;

function Factorial(n: integer): integer;
begin
  if n <= 1 then
    Factorial := 1
  else
    Factorial := n * Factorial(n - 1);
end;
```

- **Parameters**: by value (default) or by reference (`var` keyword).
- **Return values**: assigned by writing to the **function name** (`Factorial := ...`)
  or by assigning to the special `result` variable (Delphi/FreePascal style).
- **Nested** procedures and functions are supported.
- **Recursion** is fully supported.

#### Result Variable

Inside any function body, the identifier `result` is an implicit alias for the
function return value.  Both styles may be used interchangeably:

```pascal
function Max(a, b: integer): integer;
begin
  if a > b then
    result := a
  else
    result := b;
end;

function Clamp(val, lo, hi: integer): integer;
begin
  result := val;
  if result < lo then result := lo;
  if result > hi then result := hi;
end;
```

### Records

```pascal
type
  Point = record
    x: integer;
    y: integer;
  end;

var pt: Point;
begin
  pt.x := 10;
  pt.y := 20;
  writeln(pt.x, ', ', pt.y);
end.
```

Records may contain fields of any type including arrays, strings,
pointers, and other record types.

#### Variant Records

Records may include a **variant part** after the fixed fields.  All variant
arms share the same memory region (union semantics), and a tag field selects
which arm is active:

```pascal
type
  Shape = record
    name: string;
    x, y: integer;
    case tag: integer of
      0: (radius: integer);               { circle }
      1: (width, height: integer);         { rectangle }
      2: (base, side1, side2: integer);    { triangle }
  end;

var s: Shape;
begin
  s.tag := 1;
  s.width := 8;
  s.height := 6;
  writeln('Area = ', s.width * s.height);
end.
```

### Enumerated Types

Enumerated types define an ordered set of named constants.  Each value is
assigned a sequential integer starting from 0:

```pascal
type
  Color = (Red, Green, Blue, Yellow);
  Season = (Spring, Summer, Autumn, Winter);

var c: integer;
begin
  c := Red;            { c = 0 }
  c := Blue;           { c = 2 }
  writeln(ord(Green));  { 1 }
  writeln(succ(Red));   { 1 }
  writeln(pred(Blue));  { 1 }
  inc(c);               { c = 3 }
  dec(c);               { c = 2 }
end.
```

| Routine | Description |
|---------|-------------|
| `ord(x)` | Return the ordinal (integer) value of an enum constant |
| `succ(x)` | Return the next value (ordinal + 1) |
| `pred(x)` | Return the previous value (ordinal - 1) |
| `inc(x)` / `inc(x, n)` | Increment variable by 1 or by `n` |
| `dec(x)` / `dec(x, n)` | Decrement variable by 1 or by `n` |

### With Statement

The `with` statement provides shorthand access to record fields without
repeating the record variable name:

```pascal
type
  Point = record x, y: integer; end;

var pt: Point;
begin
  with pt do
  begin
    x := 10;
    y := 20;
    writeln('(', x, ', ', y, ')');
  end;
end.
```

### Goto and Labels

Labels must be declared in a `label` section and may be numeric or named.
`goto` performs an unconditional jump to the labelled statement:

```pascal
label 100, 200;
var i: integer;
begin
  i := 0;
100:
  i := i + 1;
  if i > 5 then goto 200;
  write(i, ' ');
  goto 100;
200:
  writeln;
end.
```

### In Operator (Set Membership)

The `in` operator tests whether a value is a member of a set literal or
a set variable:

```pascal
if x in [1, 3, 5, 7, 9] then
  writeln('odd')
else
  writeln('even');

var s: set of integer;
s := [2, 3, 5, 7, 11, 13, 17, 19];
for n := 1 to 20 do
  if n in s then
    writeln(n, ' is prime');
```

### Sets

Set variables hold collections of ordinal values in the range 0..255.
Declare with `set of <type>`:

```pascal
type
  Digits = set of integer;

var
  s, t, u: set of integer;
begin
  s := [1, 3, 5, 7, 9];
  t := [2, 4, 6, 8, 10];

  { Set operations }
  u := s + t;          { union }
  u := s * [1, 2, 3];  { intersection }
  u := s - [1, 3];     { difference }

  { Membership test }
  if 3 in s then writeln('yes');

  { Add / remove elements }
  include(s, 4);       { add 4 to s }
  exclude(s, 3);       { remove 3 from s }
end.
```

| Operation | Syntax | Description |
|-----------|--------|-------------|
| Union | `s + t` | Elements in either set |
| Intersection | `s * t` | Elements in both sets |
| Difference | `s - t` | Elements in `s` but not `t` |
| Membership | `x in s` | Test if `x` is in set `s` |
| Include | `include(s, x)` | Add element `x` to set `s` |
| Exclude | `exclude(s, x)` | Remove element `x` from set `s` |

### File I/O

Pascal-style file I/O is supported using the `file` type with standard
procedures:

```pascal
var
  f: file;
  line: string;
begin
  assign(f, 'output.txt');
  rewrite(f);
  writeln(f, 'Hello, file!');
  close(f);

  assign(f, 'output.txt');
  reset(f);
  while not eof(f) do
  begin
    readln(f, line);
    writeln(line);
  end;
  close(f);
end.
```

| Procedure | Description |
|-----------|-------------|
| `assign(f, name)` | Associate file variable with a filename |
| `reset(f)` | Open file for reading |
| `rewrite(f)` | Create/truncate file for writing |
| `append(f)` | Open file for appending |
| `close(f)` | Close the file |
| `eof(f)` | Returns `true` at end of file |
| `writeln(f, ...)` | Write to the file (instead of stdout) |
| `readln(f, var)` | Read a line from the file |

### Doubled-Quote Strings

To include a single-quote character inside a string literal, double it
(standard Pascal convention):

```pascal
writeln('It''s a test');      { prints: It's a test }
writeln('She said ''hi''');   { prints: She said 'hi' }
```

### Arrays

```pascal
var
  nums: array[0..9] of integer;
  grid: array[0..3] of array[0..3] of real;
begin
  nums[0] := 42;
  grid[1][2] := 3.14;
end.
```

- Arbitrary integer bounds (not restricted to 0-based).
- Multi-dimensional arrays via nested `array[...] of array[...]`.
- Runtime bounds checking is enabled by default.

### Dynamic Arrays

Dynamic arrays have no compile-time bounds.  They are declared with `array of <type>`
and resized at runtime with `SetLength`:

```pascal
var
  data: array of integer;
begin
  SetLength(data, 10);           { allocate 10 elements (0..9) }
  data[0] := 42;
  writeln(Length(data));          { 10 }
  writeln(High(data));           { 9 }
  writeln(Low(data));            { 0 — always 0 for dynamic arrays }

  SetLength(data, 20);           { grow to 20; existing data preserved }
end.
```

| Routine | Description |
|---------|-------------|
| `SetLength(arr, n)` | Resize `arr` to `n` elements.  Existing data is preserved; new slots are zero-initialised.  Passing 0 frees the block. |
| `Length(arr)` | Number of elements (runtime for dynamic, compile-time for static). |
| `High(arr)` | Highest valid index (`Length(arr) - 1` for dynamic arrays, upper bound for static). |
| `Low(arr)` | Lowest valid index (always 0 for dynamic arrays, lower bound for static). |

- Dynamic arrays are always **0-based**.
- Memory is managed via the `realloc` VM instruction; the compiler emits
  a companion `_dynlen` variable that tracks the runtime length.
- Bounds checking applies at runtime using the companion length variable.

### Pointers

```pascal
type PInt = ^integer;
var
  p: PInt;
  x: integer;
begin
  new(p);          { allocate }
  p^ := 42;        { dereference and assign }
  x := p^;         { dereference and read }
  dispose(p);      { free }

  p := @x;          { address-of }
end.
```

### String Operations

```pascal
var
  s: string;
  n: integer;
begin
  s := 'Hello' + ' World';       { concatenation }
  n := length(s);                 { length }
  writeln(copy(s, 1, 5));         { substring }
  writeln(pos('World', s));       { search }
  writeln(inttostr(42));          { int -> string }
  writeln(strtoint('123'));       { string -> int }
  end.
```

### Built-in Procedures & Functions

#### I/O

| Routine | Description |
|---------|-------------|
| `write(args...)` | Print values (no trailing newline) |
| `writeln(args...)` | Print values with trailing newline |
| `readln(var)` | Read a line from stdin; auto-converts to int/float |

#### Math

`abs`, `sqrt`, `pow`, `sin`, `cos`, `tan`, `asin`, `acos`, `atan`,
`sinh`, `cosh`, `tanh`, `exp`, `log`, `log10`, `log2`, `exp2`,
`fmod`, `ceil`, `floor`, `fabs`, `round`, `trunc`, `hypot`, `atan2`

#### Conversion

`float()`, `float_to_int()`, `int_to_float()`, `atoi()`, `atof()`

#### Character Classification

`toupper()`, `tolower()`, `isalpha()`, `isdigit()`, `isspace()`

#### String Operations

`length()`, `pos()`, `copy()`, `insert()`, `delete()`, `inttostr()`, `strtoint()`

#### Memory

`new(ptr)`, `dispose(ptr)`, `malloc()`, `calloc()`, `free()`,
`memcpy()`, `memcmp()`, `memmove()`, `memset()`

#### Arrays

| Routine | Description |
|---------|-------------|
| `SetLength(arr, n)` | Resize a dynamic array to `n` elements |
| `Length(arr)` | Number of elements in a static or dynamic array |
| `High(arr)` | Highest valid index |
| `Low(arr)` | Lowest valid index |

#### System

`halt(code)`, `system(cmd)`, `rand()`, `srand()`,
`seed_random()`, `rand_number(max)`, `argc()`, `argv(index)`

---

## Limitations & Known Quirks {#limitations}

The Pascal frontend implements a practical subset of standard Pascal.
The following limitations apply:

| Area | Limitation |
|------|------------|
| **Limited module support** | The `uses` clause imports runtime modules (`io`, `std`, `strlib`, `sdl`) and separately compiled Pascal units.  Units must be compiled individually and linked via the VM object-path mechanism.  There is no automatic dependency resolution or build ordering. |
| **`packed`** | Accepted and parsed but has no effect on memory layout. |
| **`forward`** | Parsed and validated but codegen depends on declaration order. |
| **Operator precedence** | Follows standard Pascal precedence: `not` > `* / div mod and` > `+ - or` > relational. |

---

# Runtime Module Reference {#modules}

External functions are called from bytecode with `invoke func, args...` and
from Pascal with regular function-call syntax. Return values are captured
with `return var` (bytecode) or assigned normally (Pascal).

## Module: io {#mod_io}

File I/O, random numbers.

| Function | Params | Returns | Description |
|----------|--------|---------|-------------|
| `fopen` | 2 | POINTER | Open a file (filename, mode). Returns `FILE*` handle. |
| `fclose` | 1 | INTEGER | Close a file handle. |
| `fread` | 4 | INTEGER | Read from file into buffer (dst, size, count, fh). |
| `fwrite` | 4 | INTEGER | Write buffer to file (src, size, count, fh). |
| `fseek` | 3 | INTEGER | Seek within file (fh, offset, whence). |
| `fsize` | 1 | INTEGER | Get file size in bytes. |
| `fprintf` | >= 2 | -- | Formatted write to file (fh, fmt, ...). |
| `feof` | 1 | INTEGER | Test end-of-file on a file handle. Returns 1 at EOF, 0 otherwise. |
| `fgets` | 1 | STRING | Read one line from file (strips trailing newline). |
| `fputs` | 2 | -- | Write a string to a file (str, fh). |
| `rand_number` | 1 | INTEGER | Random integer in [0, max). |
| `seed_random` | 0 | INTEGER | Seed RNG with current time. |

## Module: std {#mod_std}

Standard library -- math, memory, string conversion, system calls.

### Math Functions

| Function | Params | Returns |
|----------|--------|---------|
| `abs` | 1 | INTEGER |
| `fabs` | 1 | FLOAT |
| `sqrt`, `exp`, `exp2`, `log`, `log10`, `log2` | 1 | FLOAT |
| `sin`, `cos`, `tan`, `asin`, `acos`, `atan` | 1 | FLOAT |
| `sinh`, `cosh`, `tanh` | 1 | FLOAT |
| `ceil`, `floor`, `round`, `trunc` | 1 | FLOAT |
| `pow`, `fmod`, `atan2`, `hypot` | 2 | FLOAT |

### Memory Functions

| Function | Params | Returns | Description |
|----------|--------|---------|-------------|
| `malloc` | 1 | POINTER | Allocate bytes. |
| `calloc` | 2 | POINTER | Allocate zeroed memory (count, size). |
| `free` | 1 | -- | Release pointer. |
| `memcpy` | 3 | POINTER | Copy memory (dest, src, n). |
| `memmove` | 3 | POINTER | Move memory (dest, src, n). |
| `memset` | 3 | POINTER | Fill memory (dest, byte, n). |
| `memcmp` | 3 | INTEGER | Compare memory (a, b, n). |

### Conversion & Classification

| Function | Params | Returns | Description |
|----------|--------|---------|-------------|
| `atoi` | 1 | INTEGER | String -> int. |
| `atof` | 1 | FLOAT | String -> float. |
| `toupper`, `tolower` | 1 | INTEGER | Case conversion. |
| `isalpha`, `isdigit`, `isspace` | 1 | INTEGER | Character classification. |

### System

| Function | Params | Returns | Description |
|----------|--------|---------|-------------|
| `system` | 1 | INTEGER | Execute shell command. |
| `exit` | 1 | -- | Terminate program. |
| `rand` | 0 | INTEGER | Random integer. |
| `srand` | 1 | -- | Seed random. |
| `argc` | 0 | INTEGER | Argument count. |
| `argv` | 1 | POINTER | Argument string by index. |

## Module: string {#mod_string}

String manipulation.

| Function | Params | Returns | Description |
|----------|--------|---------|-------------|
| `strlen` | 1 | INTEGER | String length. |
| `strcmp` | 2 | INTEGER | Compare two strings. Returns <0, 0, >0. |
| `strncpy` | 3 | INTEGER | Copy up to n characters. |
| `strncat` | 3 | INTEGER | Append up to n characters. |
| `snprintf` | >= 4 | INTEGER | Printf into buffer (dest, size, fmt, ...). |
| `strfind` | 3 | INTEGER | Find substring (haystack, needle, start). Returns index or -1. |
| `substr` | 5 | INTEGER | Extract substring (dest, maxsize, src, pos, len). |
| `strat` | 2 | INTEGER | Character code at index. |

## Module: sdl {#mod_sdl}

SDL2 / SDL2_ttf bindings for graphics, events, audio, and text rendering.

### Core & Window

| Function | Description |
|----------|-------------|
| `init` | Initialise SDL2 subsystems. |
| `quit` | Shut down SDL2. |
| `create_window` | Create a window (title, x, y, w, h, flags). |
| `destroy_window` | Destroy a window. |
| `set_window_title` | Set window title text. |
| `set_window_position` | Reposition a window. |
| `get_window_size` | Query window dimensions. |
| `set_window_fullscreen` | Toggle fullscreen mode. |
| `set_window_icon` | Set window icon from image file. |

### Renderer & Drawing

| Function | Description |
|----------|-------------|
| `create_renderer` | Create a hardware renderer. |
| `destroy_renderer` | Destroy a renderer. |
| `set_draw_color` | Set RGBA draw colour. |
| `clear` | Clear with current draw colour. |
| `present` | Flip back buffer to screen. |
| `draw_point` | Draw a single pixel. |
| `draw_line` | Draw a line. |
| `draw_rect` | Draw a rectangle outline. |
| `fill_rect` | Draw a filled rectangle. |

### Textures & Surfaces

| Function | Description |
|----------|-------------|
| `create_texture` | Create a blank texture. |
| `destroy_texture` | Destroy a texture. |
| `load_texture` | Load texture from image file. |
| `render_texture` | Render src rect -> dst rect. |
| `update_texture` | Upload pixel data to texture. |
| `lock_texture` / `unlock_texture` | Direct pixel access. |
| `create_rgb_surface` / `free_surface` | Surface management. |
| `blit_surface` | Blit surface to surface. |
| `create_render_target` / `set_render_target` / `destroy_render_target` | Off-screen render targets. |
| `present_scaled` / `present_stretched` | Present render target with scaling. |

### Events & Input

| Function | Description |
|----------|-------------|
| `poll_event` | Poll for pending event. |
| `get_event_type` | Event type of last polled event. |
| `get_key_code` | Key code from keyboard event. |
| `get_mouse_x` / `get_mouse_y` | Mouse coordinates. |
| `get_mouse_button` | Mouse button from event. |
| `get_mouse_state` | Current mouse position + buttons. |
| `get_relative_mouse_state` | Relative mouse delta. |
| `is_key_pressed` | Check if a scancode is pressed. |
| `get_keyboard_state` | Full keyboard state array. |
| `set_clipboard_text` / `get_clipboard_text` | Clipboard access. |
| `show_cursor` | Show or hide cursor. |

### Timing

| Function | Description |
|----------|-------------|
| `get_ticks` | Milliseconds since SDL init. |
| `delay` | Sleep for N milliseconds. |

### Audio

| Function | Description |
|----------|-------------|
| `open_audio` | Open audio device (freq, format, channels, samples). |
| `close_audio` | Close audio device. |
| `pause_audio` | Pause or resume playback. |
| `load_wav` | Load WAV file. |
| `free_wav` | Free loaded WAV buffer. |
| `queue_audio` | Queue audio data for playback. |
| `get_queued_audio_size` | Bytes of queued audio. |
| `clear_queued_audio` | Flush audio queue. |

### Text Rendering (SDL_ttf)

| Function | Description |
|----------|-------------|
| `init_text` | Initialise TTF subsystem. |
| `quit_text` | Shut down TTF. |
| `load_font` | Load a TrueType font (path, pt_size). |
| `draw_text` | Render text (renderer, font, text, x, y, r, g, b, a). |

---

# Code Generation & Optimisation {#codegen}

## Native x86-64 Targets

| Platform | ABI | Entry Point | Notes |
|----------|-----|-------------|-------|
| **Linux** | System V | `_start` / `main` | Default target. Uses `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` for args. |
| **macOS** | System V (Darwin) | `_main` | Underscore-prefixed symbols. PIC relocations. |
| **Windows** | Win64 | `main` | Uses `rcx`, `rdx`, `r8`, `r9` + shadow space. |

## Register Allocation

The code generator performs a register allocation pass that maps
frequently-used variables to callee-saved registers:

- **System V**: `r12`-`r15`, `rbx` (integer); `xmm6`-`xmm15` (float).
- **Win64**: `r12`-`r15`, `rbx`, `rdi`, `rsi` (integer); extended XMM set.

Variables not assigned to registers are kept in memory (BSS/data section)
and accessed via RIP-relative addressing.

## Peephole Optimiser

A post-generation pass (`gen_optimize`) applies pattern-based rewrites:

- **Redundant load elimination**: removes `mov reg, [var]` immediately
  followed by `mov [var], reg` (and vice versa).
- **Dead store removal**: eliminates stores to variables that are
  immediately overwritten.
- **Strength reduction**: replaces multiply/divide by powers of 2
  with shift instructions.
- **Redundant move elimination**: removes `mov rax, rax` and similar
  no-op register-to-register moves.

---

# Examples {#examples}

## MXVM Bytecode -- Hello World

```
program HelloWorld {
  section data {
    string msg = "Hello, World!\n"
  }
  section code {
    print msg
    done
  }
}
```

## MXVM Bytecode -- Fibonacci

```
program Fibonacci {
  section data {
    int a = 0
    int b = 1
    int temp = 0
    int count = 0
    string fmt = "%d\n"
  }
  section code {
  loop:
    print fmt, a
    mov temp, b
    add b, a, b
    mov a, temp
    add count, count, 1
    cmp count, 20
    jl loop
    done
  }
}
```

## Pascal -- Recursive Factorial

```pascal
program FactorialDemo;

function Factorial(n: integer): integer;
begin
  if n <= 1 then
    Factorial := 1
  else
    Factorial := n * Factorial(n - 1);
end;

var i: integer;
begin
  for i := 1 to 12 do
    writeln(i, '! = ', Factorial(i));
end.
```

## Pascal -- Records & Pointers

```pascal
program RecordDemo;

type
  Point = record
    x: integer;
    y: integer;
  end;

var
  p: ^Point;

begin
  new(p);
  p^.x := 10;
  p^.y := 20;
  writeln('Point: (', p^.x, ', ', p^.y, ')');
  dispose(p);
end.
```

## Pascal -- Dynamic Arrays

```pascal
program DynArrayDemo;

var
  arr: array of integer;
  i: integer;

begin
  SetLength(arr, 5);
  for i := 0 to High(arr) do
    arr[i] := (i + 1) * 10;

  writeln('Length = ', Length(arr));
  write('Elements:');
  for i := Low(arr) to High(arr) do
    write(' ', arr[i]);
  writeln;

  { Grow the array — existing data is preserved }
  SetLength(arr, 8);
  arr[5] := 60;
  arr[6] := 70;
  arr[7] := 80;

  writeln('After grow, Length = ', Length(arr));
end.
```

Output:
```
Length =  5
Elements: 10  20  30  40  50
After grow, Length =  8
```

## Pascal -- Extended Features Demo

Demonstrates enumerated types, variant records, the `result` variable,
`with` statement, `goto`/`label`, `in` operator, and doubled-quote strings:

```pascal
program FeaturesDemo;

type
  Season = (Spring, Summer, Autumn, Winter);

  Shape = record
    name: string;
    x, y: integer;
    case tag: integer of
      0: (radius: integer);
      1: (width, height: integer);
  end;

var
  s: integer;
  shape1: Shape;

label 100, 200;

function SeasonName(s: integer): string;
begin
  case s of
    0: result := 'Spring';
    1: result := 'Summer';
    2: result := 'Autumn';
    3: result := 'Winter';
  else
    result := 'Unknown';
  end;
end;

function Max(a, b: integer): integer;
begin
  if a > b then result := a else result := b;
end;

begin
  { Doubled-quote strings }
  writeln('It''s a beautiful day!');

  { Enumerated types }
  for s := Spring to Winter do
    writeln(SeasonName(s));
  writeln('succ(Spring) = ', succ(Spring));
  s := East;
  inc(s, 2);

  { In operator }
  for s := 1 to 10 do
    if s in [2, 3, 5, 7] then
      writeln(s, ' is prime');

  { With statement }
  with shape1 do
  begin
    name := 'Circle';
    x := 10;
    y := 20;
    tag := 0;
    radius := 5;
  end;
  writeln(shape1.name, ' r=', shape1.radius);

  { Variant record: switch to rectangle }
  shape1.tag := 1;
  shape1.width := 8;
  shape1.height := 6;

  { Result variable }
  writeln('Max(42, 17) = ', Max(42, 17));

  { Goto / Label }
  s := 0;
100:
  s := s + 1;
  if s > 5 then goto 200;
  write(s, ' ');
  goto 100;
200:
  writeln;
end.
```

[Full source](https://github.com/lostjared/MXVM/blob/main/src/frontend/pas/features_demo.pas)

## Pascal -- Set Operations {#example_sets}

Demonstrates set literals, membership testing, union/intersection/difference,
and the `include`/`exclude` procedures:

```pascal
program SetDemo;

uses io;

var
  primes, evens, both: set of integer;
  i: integer;

begin
  primes := [2, 3, 5, 7, 11, 13, 17, 19];
  evens  := [2, 4, 6, 8, 10, 12, 14, 16, 18, 20];

  { Membership test }
  for i := 1 to 20 do
    if i in primes then
      write(i, ' ');
  writeln;

  { Intersection: even primes }
  both := primes * evens;
  for i := 1 to 20 do
    if i in both then
      write(i, ' ');
  writeln;

  { Union }
  both := primes + evens;
  write('Union count: ');
  i := 0;
  for i := 1 to 20 do
    if i in both then
      write(i, ' ');
  writeln;

  { Difference: odd primes }
  both := primes - evens;
  for i := 1 to 20 do
    if i in both then
      write(i, ' ');
  writeln;

  { Include / Exclude }
  include(primes, 23);
  exclude(primes, 2);
  if 23 in primes then writeln('23 is now prime');
  if not (2 in primes) then writeln('2 removed');
end.
```

Output:
```
2 3 5 7 11 13 17 19
2
Union count: 2 3 4 5 6 7 8 10 11 12 13 14 16 17 18 19 20
3 5 7 11 13 17 19
23 is now prime
2 removed
```

## Pascal -- File I/O {#example_fileio}

Demonstrates text file creation, writing, reading line-by-line, and appending:

```pascal
program FileDemo;

uses io;

var
  f: file;
  line: string;
  count: integer;

begin
  { Write a file }
  assign(f, 'greeting.txt');
  rewrite(f);
  writeln(f, 'Hello from MXVM!');
  writeln(f, 'Line two');
  writeln(f, 'Line three');
  close(f);

  { Read it back }
  assign(f, 'greeting.txt');
  reset(f);
  count := 0;
  while not eof(f) do
  begin
    readln(f, line);
    count := count + 1;
    writeln('[', count, '] ', line);
  end;
  close(f);

  { Append another line }
  assign(f, 'greeting.txt');
  append(f);
  writeln(f, 'Appended line');
  close(f);

  writeln('Done -- wrote ', count, ' lines, then appended one more.');
end.
```

Output:
```
[1] Hello from MXVM!
[2] Line two
[3] Line three
Done -- wrote 3 lines, then appended one more.
```

## Pascal -- Packed Types {#example_packed}

The `packed` keyword is accepted on `array` and `record` declarations for
source compatibility with standard Pascal. It has no effect on the internal
memory layout:

```pascal
program PackedDemo;

uses io;

type
  PackedPoint = packed record
    x, y: integer;
  end;

var
  points: packed array[1..5] of integer;
  p: PackedPoint;
  i: integer;

begin
  for i := 1 to 5 do
    points[i] := i * 10;

  write('Packed array:');
  for i := 1 to 5 do
    write(' ', points[i]);
  writeln;

  p.x := 42;
  p.y := 99;
  writeln('Packed record: x=', p.x, ' y=', p.y);
end.
```

Output:
```
Packed array: 10 20 30 40 50
Packed record: x=42 y=99
```

## Pascal -- Units (Separate Compilation)

**MathUtils.pas** (unit):
```pascal
unit MathUtils;

interface

function Add(a, b: integer): integer;
function Multiply(a, b: integer): integer;
procedure PrintResult(x: integer);

implementation

uses io;

function Add(a, b: integer): integer;
begin
  Add := a + b;
end;

function Multiply(a, b: integer): integer;
begin
  Multiply := a * b;
end;

procedure PrintResult(x: integer);
begin
  writeln(x);
end;

end.
```

**TestMain.pas** (program using the unit):
```pascal
program TestMain;
uses io, MathUtils;

var
  result: integer;

begin
  result := Add(3, 4);
  writeln('3 + 4 = ', result);
  result := Multiply(5, 6);
  writeln('5 * 6 = ', result);
  PrintResult(99);
end.
```

Compile and run:
```bash
mxx MathUtils.pas MathUtils.mxvm
mxx TestMain.pas TestMain.mxvm
mxvmc TestMain.mxvm -x .
```

Output:
```
3 + 4 =  7
5 * 6 =  30
99
```

## Pascal -- SDL2 Knight's Tour (Full Game) {#knights_tour}

A Knight's Tour game demonstrating Warnsdorff's heuristic with backtracking,
flat-array board representation, selection sort, SDL2 textures with custom RGB
color-key transparency, scancode polling with auto-repeat, and the full
compile-to-native pipeline.

The player watches the knight traverse every square on the board exactly once.
Press **Space** (tap or hold) to advance the knight, **Return** to generate a
new tour from a random starting position, or click **left mouse** / **right
mouse** for move / reset.

```pascal
program KnightsTour;

const
  BOARD_SIZE = 8;
  TOTAL_MOVES = 65;
  START_X = 140;
  START_Y = 40;
  CELL_SIZE = 85;
  CELL_DRAW_SIZE = 80;
  KNIGHT_SIZE = 60;

  WIN_W = 960;
  WIN_H = 720;

  SDL_QUIT_EVENT = 256;
  SDL_KEYDOWN = 768;
  SDL_MOUSEBUTTONDOWN = 1025;
  SDLK_ESCAPE = 27;
  SDLK_RETURN = 13;
  SDL_SCANCODE_SPACE = 44;

  STATE_INTRO = 0;
  STATE_PLAYING = 1;
  MAX_SEQ = 64;

var
  window_id, renderer_id: integer;
  font_id, knight_tex, logo_tex: integer;
  running: boolean;
  game_state: integer;

  { board as flat array: board[row * 8 + col] }
  board: array[0..63] of integer;
  seq_row: array[0..63] of integer;
  seq_col: array[0..63] of integer;
  seq_count: integer;

  knight_row, knight_col: integer;
  moves: integer;
  tour_over: boolean;

  { L-shaped move offsets }
  horizontal: array[0..7] of integer;
  vertical: array[0..7] of integer;

  { scratch arrays for Warnsdorff sort }
  sort_deg: array[0..7] of integer;
  sort_row: array[0..7] of integer;
  sort_col: array[0..7] of integer;
  intro_start: integer;

{ ... helper procedures: init_move_offsets, clear_board, get_board,
  set_board, is_valid_move, get_degree, sort_moves ... }

function solve_tour(pos_row, pos_col, move_count: integer): integer;
var
  i, nr, nc, n: integer;
begin
  if move_count = TOTAL_MOVES then
  begin
    solve_tour := 1;
    exit;
  end;

  { collect and sort valid moves by Warnsdorff degree }
  n := 0;
  for i := 0 to 7 do
  begin
    nr := pos_row + vertical[i];
    nc := pos_col + horizontal[i];
    if is_valid_move(nr, nc) = 1 then
    begin
      sort_deg[n] := get_degree(nr, nc);
      sort_row[n] := nr;
      sort_col[n] := nc;
      n := n + 1;
    end;
  end;
  sort_moves(n);

  { try each move with backtracking }
  i := 0;
  while i < n do
  begin
    set_board(sort_row[i], sort_col[i], move_count);
    seq_row[seq_count] := sort_row[i];
    seq_col[seq_count] := sort_col[i];
    seq_count := seq_count + 1;

    if solve_tour(sort_row[i], sort_col[i], move_count + 1) = 1 then
    begin
      solve_tour := 1;
      exit;
    end
    else
    begin
      set_board(sort_row[i], sort_col[i], 0);
      seq_count := seq_count - 1;
    end;
    i := i + 1;
  end;
  solve_tour := 0;
end;

procedure next_move;
var
  nr, nc: integer;
begin
  if moves >= 64 then exit;
  if seq_count <= 0 then exit;
  if tour_over then exit;
  nr := seq_row[moves];
  nc := seq_col[moves];
  set_board(knight_row, knight_col, -1);
  knight_row := nr;
  knight_col := nc;
  moves := moves + 1;
  set_board(knight_row, knight_col, moves);
  if moves >= 64 then
    tour_over := true;
end;

{ ... draw_board, draw_knight, reset_tour ... }

begin
  sdl_init();
  window_id := sdl_create_window('Knights Tour', 100, 100, WIN_W, WIN_H, 0);
  renderer_id := sdl_create_renderer(window_id, -1, 0);
  sdl_init_text();
  font_id := sdl_load_font('data/font.ttf', 14);
  knight_tex := sdl_load_texture_color_key_rgb(renderer_id,
                    'data/knight.bmp', 255, 255, 255);
  logo_tex := sdl_load_texture(renderer_id, 'data/logo.bmp');
  seed_random;
  init_move_offsets;
  game_state := STATE_INTRO;
  running := true;

  while running do
  begin
    { poll SDL events, handle keys/mouse }
    { space bar: first press immediate, hold repeats every 150 ms }
    { render board, knight, and HUD text }
    sdl_present(renderer_id);
    sdl_delay(16 - (sdl_get_ticks() - frame_start));
  end;

  sdl_quit();
end.
```

**Building & running:**

```bash
mxx knight.pas knight.mxvm               # compile Pascal -> bytecode
mxvmc knight.mxvm                         # interpret
mxvmc knight.mxvm --action translate      # emit x86-64 assembly
LDFLAGS="-lSDL2 -lSDL2_ttf" mxvmc knight.mxvm --action compile        # compile to native executable
```

[Online Source Code](https://github.com/lostjared/MXVM/tree/main/src/frontend/pas/knights_tour)


---

## Pascal -- SDL2 Space Shooter (Full Game)

A complete space-shooter game demonstrating records, arrays, SDL2 textures,
collision detection, and the full compile-to-native pipeline.

```pascal
program SpaceShooter;
const
  SDL_WINDOW_SHOWN = 4;
  SDL_RENDERER_ACCELLERATED = 2;
  SDL_QUIT = 256;
  SDL_KEYDOWN = 768;
  SDL_KEYUP = 769;
  SDLK_ESCAPE = 27;
  SDLK_RETURN = 13;
  SDLK_SPACE = 32;

  WIDTH = 1440;
  HEIGHT = 1080;
  SCALE_W = 1440;
  SCALE_H = 1080;

  EXIT_FAILURE = 1;
  fontfile = 'data/font.ttf';
  logofile = 'data/logo.bmp';
  spacelogofile = 'data/spacelogo.bmp';
  shipfile = 'data/ship.bmp';
  asteroidfile = 'data/asteroid.bmp';
  saucerfile = 'data/saucer.bmp';
  particlefile = 'data/particle.bmp';

  SDL_SCANCODE_LEFT = 80;
  SDL_SCANCODE_RIGHT = 79;
  SDL_SCANCODE_UP = 82;
  SDL_SCANCODE_DOWN = 81;
  SDL_SCANCODE_SPACE = 44;

  PI = 3.14159265358979323846;

  MAX_PROJECTILES = 30;
  MAX_ENEMIES = 20;
  MAX_STARS = 100;
  MAX_EXPLOSIONS = 15;
  MAX_CIRCULAR = 8;

  EXPTYPE_RED = 0;
  EXPTYPE_BLUE = 1;

  { game states }
  STATE_INTRO = 0;
  STATE_PLAYING = 1;
  STATE_DIED = 2;
  STATE_GAMEOVER = 3;

type
  Star = record
    x, y, speed: integer;
  end;

  Projectile = record
    x, y, w, h: integer;
    active: boolean;
  end;

  Enemy = record
    x, y, w, h: integer;
    active: boolean;
  end;

  CircularEnemy = record
    x, y: integer;
    angle: real;
    direction: integer;
    active: boolean;
  end;

  Explosion = record
    x, y: integer;
    duration: integer;
    cr, cg, cb: integer;
    die: boolean;
    active: boolean;
  end;

var
  running: boolean;
  window_id, renderer, target_id: integer;
  font_id: integer;
  logo_tex, spacelogo_tex: integer;
  ship_tex, asteroid_tex, saucer_tex, particle_tex: integer;
  event_type_result: integer;
  key: integer;

  game_state: integer;

  { player }
  player_x, player_y, player_w, player_h: integer;
  player_die: boolean;

  { game data }
  lives, score: integer;
  regular_enemies_destroyed: integer;
  last_shot_time: integer;
  last_spawn_time: integer;

  { intro }
  intro_alpha: integer;
  intro_fading: boolean;
  intro_timer: integer;

  { arrays - use active flag, no shifting }
  stars: array[0..MAX_STARS - 1] of Star;
  projectiles: array[0..MAX_PROJECTILES - 1] of Projectile;
  enemies: array[0..MAX_ENEMIES - 1] of Enemy;
  circular: array[0..MAX_CIRCULAR - 1] of CircularEnemy;
  circular_cx: array[0..MAX_CIRCULAR - 1] of integer;
  explosions: array[0..MAX_EXPLOSIONS - 1] of Explosion;

{ ---- collision detection ---- }
function aabb(ax, ay, aw, ah, bx, by, bw, bh: integer): boolean;
begin
  aabb := not ((ax + aw <= bx) or (bx + bw <= ax) or (ay + ah <= by) or (by + bh <= ay));
end;

{ ---- explosion helpers ---- }
procedure add_explosion(ex, ey, etype: integer; d: boolean);
var
  i: integer;
begin
  i := 0;
  while i < MAX_EXPLOSIONS do
  begin
    if explosions[i].active = false then
    begin
      explosions[i].x := ex;
      explosions[i].y := ey;
      explosions[i].duration := 60;
      explosions[i].die := d;
      explosions[i].active := true;
      if etype = EXPTYPE_RED then
      begin
        explosions[i].cr := 255;
        explosions[i].cg := 0;
        explosions[i].cb := 0;
      end
      else
      begin
        explosions[i].cr := 0;
        explosions[i].cg := 0;
        explosions[i].cb := 255;
      end;
      exit;
    end;
    i := i + 1;
  end;
end;

procedure subtract_life;
begin
  lives := lives - 1;
  if lives <= 0 then
    game_state := STATE_GAMEOVER
  else
    game_state := STATE_DIED;
  player_die := false;
end;

procedure update_explosions;
var
  i: integer;
begin
  for i := 0 to MAX_EXPLOSIONS - 1 do
  begin
    if explosions[i].active then
    begin
      explosions[i].duration := explosions[i].duration - 1;
      if explosions[i].duration <= 0 then
      begin
        explosions[i].active := false;
        if explosions[i].die then
          subtract_life;
      end;
    end;
  end;
end;

procedure draw_explosions;
var
  i, k, dx, dy: integer;
begin
  for i := 0 to MAX_EXPLOSIONS - 1 do
  begin
    if explosions[i].active then
    begin
      if explosions[i].die then
      begin
        { large player death explosion }
        for k := 0 to 39 do
        begin
          dx := (rand() mod 160) - 80;
          dy := (rand() mod 160) - 80;
          sdl_set_draw_color(renderer, 255, 200 + (rand() mod 56), rand() mod 80, 255);
          sdl_draw_line(renderer, explosions[i].x, explosions[i].y,
                        explosions[i].x + dx, explosions[i].y + dy);
        end;
        { white-hot core }
        sdl_set_draw_color(renderer, 255, 255, 200, 255);
        for k := 0 to 14 do
        begin
          dx := (rand() mod 40) - 20;
          dy := (rand() mod 40) - 20;
          sdl_draw_line(renderer, explosions[i].x + dx, explosions[i].y + dy,
                        explosions[i].x + (rand() mod 20) - 10, explosions[i].y + (rand() mod 20) - 10);
        end;
      end
      else
      begin
        { normal enemy explosion }
        sdl_set_draw_color(renderer, explosions[i].cr, explosions[i].cg, explosions[i].cb, 255);
        for k := 0 to 19 do
        begin
          dx := (rand() mod 60) - 30;
          dy := (rand() mod 60) - 30;
          sdl_draw_line(renderer, explosions[i].x, explosions[i].y,
                        explosions[i].x + dx, explosions[i].y + dy);
        end;
      end;
    end;
  end;
end;

{ ---- drawing helpers ---- }
procedure draw_gradient_triangle(x1, y1, y3: integer);
var
  i, r, g, height: integer;
begin
  if player_die then exit;
  height := y3 - y1;
  if height <= 0 then exit;
  for i := 0 to height - 1 do
  begin
    r := 255 - (255 * i div height);
    g := 255 * i div height;
    sdl_set_draw_color(renderer, r, g, 255, 255);
    sdl_draw_line(renderer, x1 - i, y1 + i, x1 + i, y1 + i);
  end;
end;

procedure draw_gradient_diamond(cx, cy, hw, hh: integer);
var
  i, r, b, g2: integer;
begin
  for i := 0 to hh - 1 do
  begin
    r := 255 - (255 * i div hh);
    b := 255 * i div hh;
    sdl_set_draw_color(renderer, r, 255, b, 255);
    sdl_draw_line(renderer, cx - i, cy - i, cx + i, cy - i);
  end;
  for i := 0 to hh - 1 do
  begin
    g2 := 255 - (255 * i div hh);
    b := 255 * i div hh;
    sdl_set_draw_color(renderer, 255, g2, b, 255);
    sdl_draw_line(renderer, cx - i, cy + i, cx + i, cy + i);
  end;
end;

procedure draw_gradient_circle(cx, cy, radius: integer);
var
  i, w, color: integer;
begin
  for i := 0 to radius - 1 do
  begin
    w := radius - i;
    color := 255 * w div radius;
    sdl_set_draw_color(renderer, color, 0, color, 255);
    sdl_draw_line(renderer, cx - w, cy - i, cx + w, cy - i);
    sdl_draw_line(renderer, cx - w, cy + i, cx + w, cy + i);
  end;
end;

{ ---- find free slot helpers ---- }
function find_free_projectile: integer;
var
  i: integer;
begin
  find_free_projectile := -1;
  for i := 0 to MAX_PROJECTILES - 1 do
    if projectiles[i].active = false then
    begin
      find_free_projectile := i;
      exit;
    end;
end;

function find_free_enemy: integer;
var
  i: integer;
begin
  find_free_enemy := -1;
  for i := 0 to MAX_ENEMIES - 1 do
    if enemies[i].active = false then
    begin
      find_free_enemy := i;
      exit;
    end;
end;

function find_free_circular: integer;
var
  i: integer;
begin
  find_free_circular := -1;
  for i := 0 to MAX_CIRCULAR - 1 do
    if circular[i].active = false then
    begin
      find_free_circular := i;
      exit;
    end;
end;

{ ---- init / reset ---- }
procedure init_stars;
var
  i: integer;
begin
  for i := 0 to MAX_STARS - 1 do
  begin
    stars[i].x := rand() mod SCALE_W;
    stars[i].y := rand() mod SCALE_H;
    stars[i].speed := 1 + (rand() mod 3);
  end;
end;

procedure clear_all_entities;
var
  i: integer;
begin
  for i := 0 to MAX_PROJECTILES - 1 do
    projectiles[i].active := false;
  for i := 0 to MAX_ENEMIES - 1 do
    enemies[i].active := false;
  for i := 0 to MAX_CIRCULAR - 1 do
    circular[i].active := false;
  for i := 0 to MAX_EXPLOSIONS - 1 do
    explosions[i].active := false;
end;

procedure reset_game;
begin
  player_x := (SCALE_W div 2) - 24;
  player_y := SCALE_H - 80;
  player_w := 48;
  player_h := 48;
  player_die := false;
  clear_all_entities;
  lives := 5;
  score := 0;
  regular_enemies_destroyed := 0;
  last_shot_time := 0;
  last_spawn_time := sdl_get_ticks();
  init_stars;
  game_state := STATE_PLAYING;
end;

procedure reset_round;
begin
  clear_all_entities;
  player_die := false;
  player_x := (SCALE_W div 2) - 24;
  player_y := SCALE_H - 80;
  game_state := STATE_PLAYING;
end;

{ ---- game update ---- }
procedure update_game;
var
  dx, dy: integer;
  ticks: integer;
  i, ei, ci: integer;
  slot: integer;
  crect_x, crect_y: integer;
  new_cx: real;
begin
  if (game_state <> STATE_PLAYING) then exit;

  update_explosions;

  { player movement via keyboard state }
  dx := 0;
  dy := 0;
  if sdl_is_key_pressed(SDL_SCANCODE_LEFT) <> 0 then dx := dx - 5;
  if sdl_is_key_pressed(SDL_SCANCODE_RIGHT) <> 0 then dx := dx + 5;
  if sdl_is_key_pressed(SDL_SCANCODE_UP) <> 0 then dy := dy - 5;
  if sdl_is_key_pressed(SDL_SCANCODE_DOWN) <> 0 then dy := dy + 5;

  player_x := player_x + dx;
  player_y := player_y + dy;

  if player_x < 0 then player_x := 0;
  if player_x > SCALE_W - player_w then player_x := SCALE_W - player_w;
  if player_y < 0 then player_y := 0;
  if player_y > SCALE_H - player_h then player_y := SCALE_H - player_h;

  { shooting }
  ticks := sdl_get_ticks();
  if (sdl_is_key_pressed(SDL_SCANCODE_SPACE) <> 0) and (ticks - last_shot_time > 300) then
  begin
    slot := find_free_projectile();
    if slot <> -1 then
    begin
      projectiles[slot].x := player_x + (player_w div 2) - 8;
      projectiles[slot].y := player_y - 30;
      projectiles[slot].w := 16;
      projectiles[slot].h := 24;
      projectiles[slot].active := true;
      last_shot_time := ticks;
    end;
  end;

  { update projectiles and check collisions }
  for i := 0 to MAX_PROJECTILES - 1 do
  begin
    if projectiles[i].active then
    begin
      projectiles[i].y := projectiles[i].y - 10;

      { off screen }
      if projectiles[i].y < -24 then
      begin
        projectiles[i].active := false;
      end
      else
      begin
        { check against regular enemies }
        for ei := 0 to MAX_ENEMIES - 1 do
        begin
          if enemies[ei].active and projectiles[i].active then
          begin
            if aabb(projectiles[i].x, projectiles[i].y, projectiles[i].w, projectiles[i].h,
                    enemies[ei].x, enemies[ei].y, enemies[ei].w, enemies[ei].h) then
            begin
              add_explosion(enemies[ei].x + enemies[ei].w div 2,
                            enemies[ei].y + enemies[ei].h div 2, EXPTYPE_RED, false);
              enemies[ei].active := false;
              projectiles[i].active := false;
              score := score + 5;
              regular_enemies_destroyed := regular_enemies_destroyed + 1;
              { spawn circular enemy every 5 regular kills }
              if (regular_enemies_destroyed mod 5) = 0 then
              begin
                slot := find_free_circular();
                if slot <> -1 then
                begin
                  circular_cx[slot] := 100 + (rand() mod (SCALE_W - 200));
                  circular[slot].x := circular_cx[slot];
                  circular[slot].y := -128;
                  circular[slot].angle := 0.0;
                  circular[slot].direction := 1;
                  circular[slot].active := true;
                end;
              end;
            end;
          end;
        end;

        { check against circular enemies }
        for ci := 0 to MAX_CIRCULAR - 1 do
        begin
          if circular[ci].active and projectiles[i].active then
          begin
            crect_x := circular[ci].x - 64;
            crect_y := circular[ci].y - 64;
            if aabb(projectiles[i].x, projectiles[i].y, projectiles[i].w, projectiles[i].h,
                    crect_x, crect_y, 128, 128) then
            begin
              add_explosion(circular[ci].x, circular[ci].y, EXPTYPE_BLUE, false);
              circular[ci].active := false;
              projectiles[i].active := false;
              score := score + 10;
            end;
          end;
        end;
      end;
    end;
  end;

  { update regular enemies }
  for ei := 0 to MAX_ENEMIES - 1 do
  begin
    if enemies[ei].active then
    begin
      enemies[ei].y := enemies[ei].y + 2;
      if enemies[ei].y > SCALE_H then
      begin
        add_explosion(enemies[ei].x + enemies[ei].w div 2, SCALE_H - 20, EXPTYPE_RED, true);
        enemies[ei].active := false;
      end
      else if aabb(player_x, player_y, player_w, player_h,
                   enemies[ei].x, enemies[ei].y, enemies[ei].w, enemies[ei].h) then
      begin
        add_explosion(player_x + player_w div 2, player_y + player_h div 2, EXPTYPE_RED, true);
        enemies[ei].active := false;
        player_die := true;
      end;
    end;
  end;

  { update circular enemies }
  for ci := 0 to MAX_CIRCULAR - 1 do
  begin
    if circular[ci].active then
    begin
      circular[ci].angle := circular[ci].angle + 0.05;
      new_cx := circular_cx[ci] + 100.0 * cos(circular[ci].angle);
      circular[ci].x := trunc(new_cx);
      circular[ci].y := circular[ci].y + circular[ci].direction * 2;
      if circular[ci].y > SCALE_H then circular[ci].direction := -1;
      if circular[ci].y < 0 then circular[ci].direction := 1;

      crect_x := circular[ci].x - 64;
      crect_y := circular[ci].y - 64;
      if aabb(player_x, player_y, player_w, player_h,
              crect_x, crect_y, 128, 128) then
      begin
        add_explosion(player_x + player_w div 2, player_y + player_h div 2, EXPTYPE_RED, true);
        circular[ci].active := false;
        player_die := true;
      end;
    end;
  end;

  { update stars }
  for i := 0 to MAX_STARS - 1 do
  begin
    stars[i].y := stars[i].y + stars[i].speed;
    if stars[i].y > SCALE_H then
    begin
      stars[i].y := 0;
      stars[i].x := rand() mod SCALE_W;
    end;
  end;

  { spawn enemies }
  if ticks - last_spawn_time > 1000 then
  begin
    slot := find_free_enemy();
    if slot <> -1 then
    begin
      enemies[slot].x := rand() mod (SCALE_W - 64);
      enemies[slot].y := -64;
      enemies[slot].w := 64;
      enemies[slot].h := 64;
      enemies[slot].active := true;
    end;
    last_spawn_time := ticks;
  end;
end;

{ ---- rendering ---- }
procedure render_game;
var
  i: integer;
  brightness: integer;
  r1, g1, b1: integer;
begin
  { draw explosions }
  draw_explosions;

  { draw stars }
  for i := 0 to MAX_STARS - 1 do
  begin
    brightness := 200 + 55 * (stars[i].speed - 1);
    if brightness > 255 then brightness := 255;
    sdl_set_draw_color(renderer, brightness, brightness, brightness, 255);
    sdl_draw_point(renderer, stars[i].x, stars[i].y);
  end;

  if game_state = STATE_PLAYING then
  begin
    { draw player ship }
    if player_die = false then
    begin
      if ship_tex <> -1 then
        sdl_render_texture(renderer, ship_tex, -1, -1, -1, -1,
                           player_x - 8, player_y - 8, 64, 64)
      else
        draw_gradient_triangle(player_x + player_w div 2, player_y,
                               player_y + player_h);
    end;

    { draw projectiles }
    for i := 0 to MAX_PROJECTILES - 1 do
      if projectiles[i].active then
      begin
        if particle_tex <> -1 then
          sdl_render_texture(renderer, particle_tex, -1, -1, -1, -1,
                             projectiles[i].x, projectiles[i].y,
                             projectiles[i].w, projectiles[i].h)
        else
        begin
          sdl_set_draw_color(renderer, 255, 255, 255, 255);
          sdl_fill_rect(renderer, projectiles[i].x, projectiles[i].y,
                        projectiles[i].w, projectiles[i].h);
        end;
      end;

    { draw regular enemies }
    for i := 0 to MAX_ENEMIES - 1 do
      if enemies[i].active then
      begin
        if asteroid_tex <> -1 then
          sdl_render_texture(renderer, asteroid_tex, -1, -1, -1, -1,
                             enemies[i].x, enemies[i].y,
                             enemies[i].w, enemies[i].h)
        else
          draw_gradient_diamond(enemies[i].x + enemies[i].w div 2,
                                enemies[i].y + enemies[i].h div 2,
                                enemies[i].w div 2, enemies[i].h div 2);
      end;

    { draw circular enemies }
    for i := 0 to MAX_CIRCULAR - 1 do
      if circular[i].active then
      begin
        if saucer_tex <> -1 then
          sdl_render_texture(renderer, saucer_tex, -1, -1, -1, -1,
                             circular[i].x - 64, circular[i].y - 64, 128, 128)
        else
          draw_gradient_circle(circular[i].x, circular[i].y, 64);
      end;
  end;

  { draw HUD }
  sdl_draw_text(renderer, font_id, 'Lives: ' + inttostr(lives), 10, 10, 255, 255, 255, 255);
  sdl_draw_text(renderer, font_id, 'Score: ' + inttostr(score), 10, 30, 255, 255, 255, 255);

  { death message }
  if game_state = STATE_DIED then
  begin
    if spacelogo_tex <> -1 then
      sdl_render_texture(renderer, spacelogo_tex, -1, -1, -1, -1, 0, 0, SCALE_W, SCALE_H);
    r1 := rand() mod 255;
    g1 := rand() mod 255;
    b1 := rand() mod 255;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_fill_rect(renderer, 470, 530, 500, 40);
    sdl_draw_text(renderer, font_id, 'You Died! Press ENTER to continue', 490, 538, r1, g1, b1, 255);
  end;

  { game over message }
  if game_state = STATE_GAMEOVER then
  begin
    if spacelogo_tex <> -1 then
      sdl_render_texture(renderer, spacelogo_tex, -1, -1, -1, -1, 0, 0, SCALE_W, SCALE_H);
    r1 := rand() mod 255;
    g1 := rand() mod 255;
    b1 := rand() mod 255;
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_fill_rect(renderer, 470, 530, 500, 40);
    sdl_draw_text(renderer, font_id, 'Game Over! Press ENTER to restart', 490, 538, r1, g1, b1, 255);
  end;
end;

{ ---- intro screen ---- }
procedure render_intro;
var
  r1, g1, b1: integer;
begin
  { draw background bitmap }
  if spacelogo_tex <> -1 then
    sdl_render_texture(renderer, spacelogo_tex, -1, -1, -1, -1, 0, 0, 1440, 1080)
  else
  begin
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);
  end;

  { draw logo bitmap }
  if logo_tex <> -1 then
    sdl_render_texture(renderer, logo_tex, -1, -1, -1, -1, 0, 0, SCALE_W, SCALE_H);

  r1 := rand() mod 255;
  g1 := rand() mod 255;
  b1 := rand() mod 255;

  sdl_draw_text(renderer, font_id, 'Press ENTER to Start', 640, 700, r1, g1, b1, 255);
  sdl_draw_text(renderer, font_id, 'Arrow Keys = Move   Space = Shoot', 584, 740, 180, 180, 180, 255);
end;

{ ---- SDL init / cleanup ---- }
procedure init_sdl;
var
  result: integer;
begin
  result := sdl_init();
  if result <> 0 then
  begin
    writeln('failed to init SDL');
    halt(EXIT_FAILURE);
  end;

  window_id := sdl_create_window('Space Shooter', 100, 100, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
  if window_id = -1 then
  begin
    writeln('Could not create window');
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  renderer := sdl_create_renderer(window_id, -1, SDL_RENDERER_ACCELLERATED);
  if renderer = -1 then
  begin
    writeln('Could not create renderer');
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  target_id := sdl_create_render_target(renderer, SCALE_W, SCALE_H);
  if target_id = -1 then
  begin
    writeln('Could not create render target');
    sdl_destroy_renderer(renderer);
    sdl_destroy_window(window_id);
    sdl_quit();
    halt(EXIT_FAILURE);
  end;

  sdl_set_draw_color(renderer, 0, 0, 0, 255);

  if sdl_init_text() <> 1 then
  begin
    writeln('Font subsystem failed to load');
    halt(EXIT_FAILURE);
  end;

  font_id := sdl_load_font(fontfile, 14);
  if font_id = -1 then
  begin
    writeln('Failed to open: ', fontfile);
    halt(EXIT_FAILURE);
  end;

  { load background textures without color key }
  logo_tex := sdl_load_texture(renderer, logofile);
  if logo_tex = -1 then
    writeln('Warning: could not load ', logofile);

  spacelogo_tex := sdl_load_texture(renderer, spacelogofile);
  if spacelogo_tex = -1 then
    writeln('Warning: could not load ', spacelogofile);

  { load sprite textures with black color key for transparency }
  ship_tex := sdl_load_texture_color_key(renderer, shipfile);
  if ship_tex = -1 then
    writeln('Warning: could not load ', shipfile);

  asteroid_tex := sdl_load_texture_color_key(renderer, asteroidfile);
  if asteroid_tex = -1 then
    writeln('Warning: could not load ', asteroidfile);

  saucer_tex := sdl_load_texture_color_key(renderer, saucerfile);
  if saucer_tex = -1 then
    writeln('Warning: could not load ', saucerfile);

  particle_tex := sdl_load_texture_color_key(renderer, particlefile);
  if particle_tex = -1 then
    writeln('Warning: could not load ', particlefile);
end;

procedure cleanup;
begin
  if ship_tex <> -1 then sdl_destroy_texture(ship_tex);
  if asteroid_tex <> -1 then sdl_destroy_texture(asteroid_tex);
  if saucer_tex <> -1 then sdl_destroy_texture(saucer_tex);
  if particle_tex <> -1 then sdl_destroy_texture(particle_tex);
  if logo_tex <> -1 then sdl_destroy_texture(logo_tex);
  if spacelogo_tex <> -1 then sdl_destroy_texture(spacelogo_tex);
  sdl_destroy_render_target(target_id);
  sdl_destroy_renderer(renderer);
  sdl_destroy_window(window_id);
  sdl_quit_text();
  sdl_quit();
end;

{ ---- main loop ---- }
var
  prev_time, cur_time: integer;

begin
  init_sdl;
  seed_random;
  game_state := STATE_INTRO;
  lives := 5;
  score := 0;
  clear_all_entities;
  init_stars;

  prev_time := sdl_get_ticks();
  running := true;

  while running = true do
  begin
    { poll events }
    while sdl_poll_event() <> 0 do
    begin
      event_type_result := sdl_get_event_type();

      if event_type_result = SDL_QUIT then
        running := false;

      if event_type_result = SDL_KEYDOWN then
      begin
        key := sdl_get_key_code();
        if key = SDLK_ESCAPE then
          running := false;

        if key = SDLK_RETURN then
        begin
          if game_state = STATE_INTRO then
            reset_game
          else if game_state = STATE_DIED then
            reset_round
          else if game_state = STATE_GAMEOVER then
            reset_game;
        end;
      end;
    end;

    { timed update at ~60fps }
    cur_time := sdl_get_ticks();
    if cur_time - prev_time >= 15 then
    begin
      prev_time := cur_time;
      if game_state = STATE_PLAYING then
        update_game;
    end;

    { render }
    sdl_set_render_target(renderer, target_id);
    sdl_set_draw_color(renderer, 0, 0, 0, 255);
    sdl_clear(renderer);

    if game_state = STATE_INTRO then
      render_intro
    else
      render_game;

    sdl_present_scaled(renderer, target_id, WIDTH, HEIGHT);
    sdl_delay(16);
  end;

  cleanup;
end.

```

Compile and run:
```bash
mxx space.pas space.mxvm              # Pascal -> MXVM bytecode
mxvmc space.mxvm                      # run in interpreter
```

Compile to native x86-64 binary:
```bash
LDFLAGS="-lSDL2 -lSDL2_ttf" mxvmc space.mxvm --action compile
./SpaceShooter                        # run native binary
```
[Online Source Code](https://github.com/lostjared/MXVM/tree/main/src/frontend/pas/spaceshooter)


---

# Project Layout {#layout}

```
MXVM/
|---- include/mxvm/          Core VM headers (parser, icode, instruct, etc.)
|---- src/                    Implementation sources
|   |---- frontend/           Pascal parser, validator, AST, codegen
|   |   `---- include/        Frontend-specific headers
|   |---- html_gen/           mxvm-html tool (html.cpp, CMakeLists.txt)
|   |---- scanner/            Tokeniser / lexer
|   |   `---- include/scanner/ Scanner headers
|   |---- vm/                 Interpreter, argument parsing
|   |   `---- include/        VM headers (argz.hpp)
|   `---- webasm/             WebAssembly target (experimental)
|---- modules/                Runtime C modules
|   |---- io/                 File I/O, random numbers
|   |---- std/                Math, memory, conversion, system
|   |---- string/             String manipulation
|   `---- sdl/                SDL2 + SDL_ttf bindings
|---- docs/                   HTML reference pages (index, standard, sdl, mxvm-html, examples/)
|---- mxvm_src/               Example .mxvm programs
|---- CMakeLists.txt          Top-level build script
`---- Doxyfile                Doxygen configuration
```

---

 [source repository](https://github.com/lostjared/MXVM) for the latest code.*
