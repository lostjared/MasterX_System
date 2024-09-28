# ETL (Easy Toy Language) Compiler

This compiler translates a simple language to GNU Assembly Language (AT&T Syntax) and then uses your existing compiler toolchain
to compile the assembly language to machine language. The target architecture is x86_64 (amd64).


# `etl` - Bash Script for Compiling `.e` Files with ETL

This script automates the process of compiling `.e` files using the `ETL_C` tool and assembling them into an executable. It also supports object files and linker flags.

## Usage

```bash
etl output_executable file1.e [file2.e ... | file3.o ...] [-- linker_flags] [-d] -[q]
```

### Parameters:
- `output_executable`: The name of the resulting executable.
- `file1.e`, `file2.e`, etc.: Files with the `.e` extension to be compiled using `ETL_C`.
- `file3.o`, `file4.o`, etc.: Pre-compiled object files to be linked.
- `--`: Specifies the start of linker flags (optional).
- `linker_flags`: Custom linker flags for the final linking step (optional).
- `-d`: Enables debug mode for `ETL_C` (optional).
- `-q`: Enables quiet mode (Don't output IR code to stdout)

### Example:
```bash
etl my_program file1.e file2.e -- -lmylib -L/usr/local/lib
```

This example compiles `file1.e` and `file2.e`, and links them with the `mylib` library and any additional flags provided.

## Steps Performed by the Script

1. **Preprocessing (`*.e` files):**
   - Each `.e` file is preprocessed with `cc` (C preprocessor).
   - The preprocessed output is saved as a `.i` file.
   - If preprocessing fails, the script exits with an error.

2. **ETL Conversion:**
   - The `.i` file is passed to `ETL_C`, which produces a `.s` (assembly) file.
   - If the conversion fails, the script exits with an error.

3. **Assembling:**
   - The `.s` file is assembled into an `.o` object file using the assembler (`as`).
   - If assembling fails, the script exits with an error.

4. **Linking:**
   - All `.o` files, including those generated and any passed as arguments, are linked to produce the executable.
   - Custom linker flags provided after `--` are passed to the linker.
   - If linking fails, the script exits with an error.

## Debug Mode

To enable debug mode for the ETL conversion, add the `-d` flag when running the script.

Example:
```bash
etl my_program file1.e -d
```

## Output

The script will generate an executable with the specified name (`output_executable`). All intermediate `.o` files are handled automatically by the script.
