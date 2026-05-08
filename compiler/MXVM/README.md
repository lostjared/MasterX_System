# MXVM -- Virtual Machine, Compiler & Code Generator

**MXVM** is a custom virtual machine and compiler suite written in modern C++20.
It provides a RISC-style bytecode language with an interpreter, a Pascal-subset
compiler frontend, and native x86-64 code generators targeting Linux (System V ABI),
macOS (Darwin), and Windows (Win64).

> **Experimental / educational project -- not intended for production use.**

> **[Download PDF Reference Manual](https://lostsidedead.biz/MXVM/doc/MXVM_Reference.pdf)**

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

---

## Building

```bash
git clone https://github.com/lostjared/MXVM.git
cd MXVM && mkdir build && cd build
cmake .. && make -j$(nproc)
```

Requires: C++20 compiler, CMake >= 3.10.
Optional: SDL2 + SDL2_ttf (for the SDL module), Emscripten (WebAssembly target).

## Running

> **Note:** The `.mxvm` extension is optional — if you omit it, `mxvmc` will append `.mxvm` automatically (e.g. `mxvmc program` is equivalent to `mxvmc program.mxvm`).

| Mode | Command |
|------|---------|
| **Interpret** | `mxvmc program.mxvm --path /usr/local/lib` |
| **Compile -> Assembly** | `mxvmc program.mxvm --path /usr/local/lib --action translate` |
| **Compile -> Executable** | `mxvmc program.mxvm --path /usr/local/lib --action compile` |

---

## MXVM Bytecode Language

Every `.mxvm` file is wrapped in a `program` block with **data**, **module**, and **code** sections:

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

### Data Types

| Type | Size | Description |
|------|------|-------------|
| `int` | 64-bit | Signed integer |
| `float` | 64-bit | Double-precision floating point |
| `string` | varies | NUL-terminated string (optional max-length: `string name, 256`) |
| `ptr` | 64-bit | Opaque pointer (for `alloc`/module use) |
| `byte` | 8-bit | Unsigned byte |
| `label` | -- | Code address (for indirect jumps) |

### Key Instructions

| Category | Instructions |
|----------|-------------|
| **Arithmetic** | `mov`, `add`, `sub`, `mul`, `div`, `mod`, `neg`, `or`, `and`, `xor`, `not` |
| **Branching** | `cmp`, `jmp`, `je`, `jne`, `jl`, `jle`, `jg`, `jge`, `jz`, `jnz`, `ja`, `jb` |
| **Memory** | `load`, `store`, `lea`, `alloc`, `realloc`, `free` |
| **Stack** | `push`, `pop`, `stack_load`, `stack_store`, `stack_sub` |
| **I/O** | `print`, `string_print`, `getline` |
| **Calls** | `call`, `ret`, `invoke`, `return`, `done`, `exit` |
| **Conversion** | `to_int`, `to_float` |

All instructions use **destination-first operand order** (RISC convention).

### Object System

Separately compiled units (`object` blocks) can be linked into a main program
and referenced with dot notation: `call Utils.print_line`.

---

## Pascal Frontend

MXVM includes a Pascal-subset compiler that parses Pascal source, builds a typed AST,
validates semantics, and emits MXVM bytecode.

### Supported Features

- **Case-insensitive** keywords and identifiers
- **Types**: `integer`, `real`, `boolean`, `char`, `string`, typed pointers (`^Type`),
  fixed arrays (`array[lo..hi] of T`), dynamic arrays (`array of T`), records
  (including variant records), enumerated types, sets (`set of T`), and file types
- **Control flow**: `if/then/else`, `while/do`, `for/to/downto`, `repeat/until`,
  `case`, `break`, `continue`, `exit`, `goto`/`label`
- **Procedures & functions** with by-value and `var` (by-reference) parameters,
  nesting, and recursion
- **Result variable**: functions can return values via `result := ...` (Delphi style)
  or by assigning to the function name
- **With statement**: shorthand access to record fields (`with rec do begin ... end`)
- **Sets**: `set of T` with `in`, `+` (union), `-` (difference), `*` (intersection),
  `include`, `exclude`
- **File I/O**: Pascal-style `file` type with `assign`, `reset`, `rewrite`, `append`,
  `close`, `eof`, file-parameterized `writeln`/`readln`
- **Packed**: `packed record` and `packed array` accepted (no layout effect)
- **Enumerated types**: `type Color = (Red, Green, Blue);` with `ord`, `succ`, `pred`, `inc`, `dec`
- **Variant records**: `case tag: type of` inside records (union semantics)
- **Units** (separately compiled modules with `interface` / `implementation` sections)
- **Qualified names** for cross-unit calls (`UnitName.Routine`); variables and constants
  from other units must always be qualified
- **Operators**: arithmetic, relational, logical, string concatenation, pointer
  dereference (`^`) and address-of (`@`)
- **Dynamic arrays**: `SetLength`, `Length`, `High`, `Low`
- **Pointers**: `new`, `dispose`, `@` (address-of)
- **Doubled-quote strings**: `'It''s'` produces `It's` (standard Pascal convention)
- **Built-in routines**: I/O (`write`, `writeln`, `readln`), math (`abs`, `sqrt`,
  `sin`, `cos`, `pow`, ...), string ops (`length`, `pos`, `copy`, `insert`, `delete`),
  memory (`malloc`, `calloc`, `free`, `memcpy`), system (`halt`, `system`, `rand`)

### Example

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

### Compiling Pascal with `mxx`

| Mode | Command | Description |
|------|---------|-------------|
| **Legacy** | `mxx source.pas output.mxvm` | Compile a single file to a named output |
| **Explicit I/O** | `mxx -i source.pas -o output.mxvm` | Explicit input/output flags; `-o` is optional — when omitted, the output filename is derived from the `program` or `unit` name declared inside the source file (e.g. `program Test;` → `Test.mxvm`) |
| **Batch** | `mxx -c source1.pas source2.pas ...` | Compile multiple files; each output filename is derived from the declared `program`/`unit` name inside the file |

```bash
mxx MathUtils.pas MathUtils.mxvm    # legacy: explicit output name
mxx TestMain.pas TestMain.mxvm      # legacy: explicit output name
mxx -i TestMain.pas -o TestMain.mxvm  # explicit I/O flags
mxx -i TestMain.pas                   # -o omitted: reads 'program TestMain;' -> TestMain.mxvm
mxx -c MathUtils.pas TestMain.pas   # batch: reads declared names -> MathUtils.mxvm, TestMain.mxvm
mxvmc TestMain.mxvm -x .            # run with object path
```

Functions and procedures from another unit can be called with qualified
names (`MathUtils.Add(3, 4)`) or unqualified (`Add(3, 4)`).  Variables
and constants from other units must always be qualified
(`MathUtils.counter`, `MathUtils.MAX_VALUE`).

---

## Runtime Modules

External functions are called with `invoke` (bytecode) or normal call syntax (Pascal).

| Module | Description | Key Functions |
|--------|-------------|---------------|
| **io** | File I/O, RNG | `fopen`, `fclose`, `fread`, `fwrite`, `fseek`, `fprintf`, `feof`, `fgets`, `fputs`, `rand_number`, `seed_random` |
| **std** | Math, memory, system | `abs`, `sqrt`, `sin`, `cos`, `pow`, `malloc`, `free`, `memcpy`, `atoi`, `system`, `argc`, `argv` |
| **string** | String manipulation | `strlen`, `strcmp`, `strncpy`, `strncat`, `snprintf`, `strfind`, `substr` |
| **sdl** | SDL2/SDL2_ttf graphics | Window, renderer, textures, events, audio, text rendering |

---

## Code Generation

### Native x86-64 Targets

| Platform | ABI | Notes |
|----------|-----|-------|
| **Linux** | System V | Default target. `rdi`, `rsi`, `rdx`, `rcx`, `r8`, `r9` for args. |
| **macOS** | System V (Darwin) | Underscore-prefixed symbols, PIC relocations. |
| **Windows** | Win64 | `rcx`, `rdx`, `r8`, `r9` + shadow space. |

### Peephole Optimiser

Post-generation pass applies: redundant load elimination, dead store removal,
strength reduction (multiply/divide by powers of 2 -> shifts), and redundant
move elimination.

---

## Project Structure

```
MXVM/
├── CMakeLists.txt        # Build rules
├── LICENSE               # Apache-2.0
├── include/mxvm/         # Public headers
├── src/                  # VM + codegen implementation
├── modules/              # Built-in extension modules (io, std, string, sdl)
├── mxvm_src/             # Example .mxvm programs
└── docs/                 # HTML documentation
```

---

## Contributing

1. Fork the repo
2. Create a branch (`git checkout -b feature/foo`)
3. Commit your changes (`git commit -am "Add feature"`)
4. Push (`git push origin feature/foo`)
5. Open a Pull Request
