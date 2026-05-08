/**
 * @file icode.hpp
 * @brief Intermediate code representation, execution engine, and native x64 code generation (SysV + Win64)
 * @author Jared Bruni
 */
#ifndef ICODE_HPP_
#define ICODE_HPP_

#include "mxvm/instruct.hpp"
#include "mxvm/parser.hpp"
#include "scanner/exception.hpp"
#include <functional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace mxvm {

    /** @brief Stack element: integer, pointer, double, or string */
    using StackValue = std::variant<int64_t, void *, double, std::string>;

    /** @brief Runtime operand stack for the MXVM interpreter */
    class Stack {
      public:
        Stack() = default;

        /** @brief Push a value onto the stack
         * @param value Integer or pointer value to push
         */
        void push(const StackValue &value) {
            data.push_back(value);
        }

        /** @brief Pop and return the top value
         * @return The top stack value
         * @throws mx::Exception on stack underflow
         */
        [[nodiscard]] StackValue pop() {
            if (data.empty())
                throw mx::Exception("Stack underflow");
            StackValue val = data.back();
            data.pop_back();
            return val;
        }

        /** @brief Check whether the stack is empty */
        [[nodiscard]] bool empty() const { return data.empty(); }

        /** @brief Return the number of elements on the stack */
        [[nodiscard]] size_t size() const { return data.size(); }

        /** @brief Random access into the stack
         * @param index Zero-based index from the bottom
         * @return Reference to the stack element
         * @throws mx::Exception if index is out of bounds
         */
        StackValue &operator[](size_t index) {
            if (index >= data.size())
                throw mx::Exception("Stack index out of bounds: " + std::to_string(index));
            return data[index];
        }

      private:
        std::vector<StackValue> data;
    };

    class Program;

    /** @brief Callback type for runtime-registered native functions */
    using runtime_call = std::function<void(Program *program, std::vector<Operand> &operands)>;

    /**
     * @brief Wraps a dynamically loaded native function from a shared library module
     *
     * Manages dlopen handles and function pointers for external module calls.
     * Shared library handles are cached in a static map.
     */
    class RuntimeFunction {
      public:
        /** @brief Default constructor — null function pointer and handle */
        RuntimeFunction() : func(nullptr), handle(nullptr) {}

        /** @brief Copy constructor */
        RuntimeFunction(const RuntimeFunction &r) : func(r.func), handle(r.handle), mod_name(r.mod_name), fname(r.fname) {}

        /** @brief Copy-assignment operator */
        RuntimeFunction &operator=(const RuntimeFunction &r) {
            func = r.func;
            handle = r.handle;
            mod_name = r.mod_name;
            fname = r.fname;
            return *this;
        }
        ~RuntimeFunction() = default;

        /** @brief Load a function symbol from a shared library
         * @param mod Module/library name (without extension)
         * @param name Function symbol name to resolve
         */
        RuntimeFunction(const std::string &mod, const std::string &name);

        /** @brief Invoke the loaded function
         * @param program Pointer to the running Program for variable access
         * @param operands Instruction operands passed to the native function
         */
        void call(Program *program, std::vector<Operand> &operands);
        void *func = nullptr;   ///< resolved function pointer
        void *handle = nullptr;  ///< dlopen handle
        std::string mod_name;    ///< module name
        std::string fname;       ///< function symbol name
        static std::unordered_map<std::string, void *> handles; ///< cached library handles
    };

    /**
     * @brief Base class for instruction storage, variables, labels, and external function registrations
     *
     * Provides the shared data structures used by both interpreted and compiled programs:
     * instruction list, variable table, label map, external function bindings, and
     * object references.
     */
    class Base {
      public:
        Base() = default;
        /** @brief Move constructor */
        Base(Base &&other) noexcept;
        ~Base() = default;
        /** @brief Move-assignment operator */
        Base &operator=(Base &&other) noexcept;

        /** @brief Set the root program base pointer (used for global variable lookup)
         * @param b Pointer to the main program's Base
         */
        void setMainBase(Base *b) { Base::base = b; }

        /** @brief Append an instruction to the instruction stream
         * @param i Instruction to add
         */
        void add_instruction(const Instruction &i);

        /** @brief Register a label at the given instruction address
         * @param name Label identifier
         * @param address Instruction index the label points to
         * @param f true if the label marks a function entry
         */
        void add_label(const std::string &name, uint64_t address, bool f);

        /** @brief Declare a local variable in the symbol table
         * @param name Variable name
         * @param v Variable definition
         */
        void add_variable(const std::string &name, const Variable &v);

        /** @brief Declare a global variable scoped to an object
         * @param objname Object that owns the variable
         * @param name Variable name
         * @param v Variable definition
         */
        void add_global(const std::string &objname, const std::string &name, const Variable &v);

        /** @brief Register an external function import
         * @param mod Module name
         * @param name Function symbol name
         * @param module true if the import is a module-level extern
         */
        void add_extern(const std::string &mod, const std::string &name, bool module);

        /** @brief Register and resolve a runtime-loaded external function
         * @param mod_name Module display name
         * @param mod Shared library path
         * @param func_name Native symbol name
         * @param name MXVM-visible function name
         */
        void add_runtime_extern(const std::string &mod_name, const std::string &mod, const std::string &func_name, const std::string &name);

        /** @brief Record a source filename for debug/error reporting
         * @param fname Filename to add
         */
        void add_filename(const std::string &fname);

        /** @brief Track a heap-allocated variable for cleanup
         * @param name Variable name
         * @param v Variable (with allocation metadata)
         */
        void add_allocated(const std::string &name, Variable &v);

        /** @brief Register a child object program
         * @param name Object identifier
         * @param prog Pointer to the object's Program
         */
        void add_object(const std::string &name, Program *prog);
        std::string name;
        std::vector<Instruction> inc;                                ///< instruction stream
        std::unordered_map<std::string, Variable> vars;             ///< variable symbol table
        std::unordered_map<std::string, std::pair<uint64_t, bool>> labels; ///< label -> (address, is_function)
        std::vector<ExternalFunction> external;                      ///< declared external function imports
        std::unordered_map<std::string, RuntimeFunction> external_functions; ///< resolved runtime function bindings
        static Base *base;                                           ///< pointer to the main program base
        static std::string root_name;
        static std::vector<std::string> filenames;
        static std::unordered_map<std::string, Variable> allocated;  ///< heap-allocated variables
        static std::unordered_map<std::string, Program *> object_map; ///< object name -> Program mapping
        std::string assembly_code;                                   ///< generated assembly output for objects
    };

    /**
     * @brief Complete MXVM program: instruction interpreter and native x64 code generator
     *
     * Extends Base with execution state (program counter, flags, stack) for interpretation,
     * and a full set of code generation methods targeting x86-64 assembly
     * (System V ABI for Linux/macOS, Win64 ABI for Windows).
     */
    class Program : public Base {
      public:
        friend class Parser;
        /** @brief Default constructor — initializes execution state (pc, flags, etc.) */
        Program();
        /** @brief Destructor — frees any heap-allocated variables */
        ~Program();

        /** @brief Move constructor — transfers all execution state and child objects */
        Program(Program &&other) noexcept
            : Base(std::move(other)),
              filename(std::move(other.filename)),
              objects(std::move(other.objects)),
              object(other.object),
              pc(other.pc),
              running(other.running),
              exitCode(other.exitCode),
              zero_flag(other.zero_flag),
              less_flag(other.less_flag),
              greater_flag(other.greater_flag),
              carry_flag(other.carry_flag),
              xmm_offset(other.xmm_offset),
              args(std::move(other.args)),
              main_function(other.main_function),
              object_external(other.object_external),
              stack(std::move(other.stack)),
              result(std::move(other.result)),
              parent(other.parent),
              platform(other.platform) {
            other.pc = 0;
            other.running = false;
            other.exitCode = 0;
            for (auto &var_pair : vars) {
                var_pair.second.obj_name = name;
            }
        }

        /** @brief Move-assignment operator */
        Program &operator=(Program &&other) noexcept {
            if (this != &other) {
                Base::operator=(std::move(other));
                filename = std::move(other.filename);
                objects = std::move(other.objects);
                object = other.object;
                pc = other.pc;
                running = other.running;
                exitCode = other.exitCode;
                zero_flag = other.zero_flag;
                less_flag = other.less_flag;
                greater_flag = other.greater_flag;
                carry_flag = other.carry_flag;
                xmm_offset = other.xmm_offset;
                args = std::move(other.args);
                main_function = other.main_function;
                object_external = other.object_external;
                stack = std::move(other.stack);
                result = std::move(other.result);
                parent = other.parent;
                other.pc = 0;
                other.running = false;
                other.exitCode = 0;
                for (auto &var_pair : vars) {
                    var_pair.second.obj_name = name;
                }
                platform = other.platform;
            }
            return *this;
        }

        /** @brief Register standard library runtime functions */
        void add_standard();

        /** @brief Halt the interpreter loop */
        void stop();

        /**
         * @brief Execute the program via the interpreter
         * @return Exit code
         */
        int exec();
        /** @brief Print all instructions and data to an output stream
         * @param out Destination stream
         */
        void print(std::ostream &out);

        /** @brief Generate native data section (variable declarations) into the output stream
         * @param out Destination stream
         */
        void post(std::ostream &out);

        /** @brief Get the program's exit code after execution
         * @return Exit code (0 on success)
         */
        int getExitCode() const { return exitCode; }
        std::string name;
        std::string filename;
        /** @brief Mangle a symbol name with the platform prefix (e.g. _name for Darwin)
         * @param name Original symbol name
         * @return Platform-adjusted symbol name
         */
        std::string getPlatformSymbolName(const std::string &name);
        /**
         * @brief Generate native x64 assembly for the entire program
         * @param platform Target platform (Linux, Darwin, WinX64)
         * @param obj true if generating as an object (no _start/main)
         * @param out Output stream for the assembly
         */
        void generateCode(const Platform &platform, bool obj, std::ostream &out);
        /** @brief Escape newline characters in a string literal for assembly output
         * @param text Raw string
         * @return Escaped string
         */
        static std::string escapeNewLines(const std::string &text);

        /** @brief Dump all variable values to a stream for debugging
         * @param out Destination stream
         */
        void memoryDump(std::ostream &out);

        /** @brief Set the command-line arguments available to the program
         * @param argv Argument vector
         */
        void setArgs(const std::vector<std::string> &argv);

        /** @brief Mark this program as an object (no main entry point)
         * @param obj true for object mode, false for stand-alone program
         */
        void setObject(bool obj);
        std::vector<std::unique_ptr<Program>> objects;
        bool object = false;
        /** @brief Validate all variable and label names against the Validator
         * @param v Validator reference
         * @return true if all names pass validation
         */
        bool validateNames(Validator &v);

        /** @brief Flatten child object programs into the root instruction stream
         * @param program Root program to merge into
         */
        void flatten(Program *program);

        /** @brief Rewrite object-qualified operand references in a single instruction
         * @param root Root program owning the merged instruction stream
         * @param i Instruction to rewrite (modified in place)
         */
        void flatten_inc(Program *root, Instruction &i);

        /** @brief Copy a label from a child object into the root label map with an address offset
         * @param root Root program
         * @param offset Instruction index offset
         * @param label Label name
         * @param func true if the label is a function entry
         */
        void flatten_label(Program *root, int64_t offset, const std::string &label, bool func);

        /** @brief Copy an external function binding from a child object into the root
         * @param root Root program
         * @param e External function name
         * @param r Resolved RuntimeFunction binding
         */
        void flatten_external(Program *root, const std::string &e, RuntimeFunction &r);

        /** @brief Mangle a variable name with its owning object prefix
         * @param var Raw variable name
         * @return Object-qualified name (e.g. "Object.var")
         */
        std::string getMangledName(const std::string &var);

        /** @brief Mangle an operand's variable reference with its object prefix
         * @param op Operand containing the variable reference
         * @return Object-qualified name
         */
        std::string getMangledName(const Operand &op);

        /** @brief Look up a child object program by name
         * @param name Object identifier
         * @return Pointer to the Program, or nullptr if not found
         */
        Program *getObjectByName(const std::string &name);

        /** @brief Check whether a label exists and is marked as a function entry
         * @param label Label name to check
         * @return true if the label is a valid function
         */
        bool isFunctionValid(const std::string &label);

      private:
        size_t pc;             ///< program counter
        bool running;          ///< interpreter running flag
        int exitCode = 0;
        bool zero_flag = false;    ///< comparison zero flag
        bool less_flag = false;    ///< comparison less-than flag
        bool greater_flag = false; ///< comparison greater-than flag
        bool carry_flag = false;   ///< comparison carry flag
        int xmm_offset = 0;        ///< current XMM register allocation offset
        std::vector<std::string> args;
        bool main_function;    ///< true if this program has a main entry point
        bool object_external = false;

        /** @brief Tracks type of last comparison for conditional jumps */
        enum LastCmpType {
            CMP_NONE,
            CMP_INTEGER,
            CMP_FLOAT
        };
        LastCmpType last_cmp_type = CMP_NONE;
        bool last_call_returns_owned_ptr = false; ///< tracks ownership of last call's return value

      public:
        /** @name System V ABI (Linux/macOS) x86-64 code generation */
        /** @{ */
        /** @brief Generate a System V ABI function call with operands as arguments
         * @param out Assembly output stream
         * @param name Callee function name
         * @param op Operand list to pass as arguments
         */
        void generateFunctionCall(std::ostream &out, const std::string &name, std::vector<Operand> &op);

        /** @brief Generate a dynamic invoke call (runtime-resolved external function)
         * @param out Assembly output stream
         * @param op Operand list: first is the function name, rest are arguments
         */
        void generateInvokeCall(std::ostream &out, std::vector<Operand> &op);

        /** @brief Dispatch a single instruction to its System V code generator
         * @param out Assembly output stream
         * @param i Instruction to generate code for
         */
        void generateInstruction(std::ostream &out, const Instruction &i);

        /** @brief Load a variable or constant into a numbered register
         * @param out Assembly output stream
         * @param reg Register index (0-based)
         * @param op Operand to load
         * @return 1 if the operand was a constant, 0 if variable
         */
        int generateLoadVar(std::ostream &out, int reg, const Operand &op);

        /** @brief Load a variable or constant into a named register
         * @param out Assembly output stream
         * @param type Expected variable type
         * @param reg Target register name (e.g. "rax")
         * @param op Operand to load
         * @return 1 if the operand was a constant, 0 if variable
         */
        int generateLoadVar(std::ostream &out, VarType type, std::string reg, const Operand &op);

        /** @brief Get the register name for a given index and variable type
         * @param index Register index
         * @param type Variable type (determines integer vs. XMM register)
         * @return Register name string
         */
        std::string getRegisterByIndex(int index, VarType type);
        void gen_print(std::ostream &out, const Instruction &i);
        void gen_arth(std::ostream &out, std::string arth, const Instruction &i);
        void gen_exit(std::ostream &out, const Instruction &i);
        void gen_mov(std::ostream &out, const Instruction &i);
        void gen_jmp(std::ostream &out, const Instruction &i);
        void gen_cmp(std::ostream &out, const Instruction &i);
        void gen_alloc(std::ostream &out, const Instruction &i);
        /** @brief Generate System V x86-64 assembly for the REALLOC instruction (dynamic array resize) */
        void gen_realloc(std::ostream &out, const Instruction &i);
        void gen_free(std::ostream &out, const Instruction &i);
        void gen_load(std::ostream &out, const Instruction &i);
        void gen_store(std::ostream &out, const Instruction &i);
        void gen_ret(std::ostream &out, const Instruction &i);
        void gen_call(std::ostream &out, const Instruction &i);
        void gen_done(std::ostream &out, const Instruction &i);
        void gen_bitop(std::ostream &out, const std::string &opc, const Instruction &i);
        void gen_not(std::ostream &out, const Instruction &i);
        void gen_push(std::ostream &out, const Instruction &i);
        void gen_pop(std::ostream &out, const Instruction &i);
        void gen_stack_load(std::ostream &out, const Instruction &i);
        void gen_stack_store(std::ostream &out, const Instruction &i);
        void gen_stack_sub(std::ostream &out, const Instruction &i);
        void gen_getline(std::ostream &out, const Instruction &i);
        void gen_to_int(std::ostream &out, const Instruction &i);
        void gen_to_float(std::ostream &out, const Instruction &i);
        void gen_invoke(std::ostream &out, const Instruction &i);
        void gen_return(std::ostream &out, const Instruction &i);
        void gen_neg(std::ostream &out, const Instruction &i);
        void gen_lea(std::ostream &out, const Instruction &i);
        void gen_fcmp(std::ostream &out, const Instruction &i);
        void gen_jae(std::ostream &out, const Instruction &i);
        void gen_jbe(std::ostream &out, const Instruction &i);
        void gen_jc(std::ostream &out, const Instruction &i);
        void gen_jnc(std::ostream &out, const Instruction &i);
        void gen_jp(std::ostream &out, const Instruction &i);
        void gen_jnp(std::ostream &out, const Instruction &i);
        void gen_jo(std::ostream &out, const Instruction &i);
        void gen_jno(std::ostream &out, const Instruction &i);
        void gen_js(std::ostream &out, const Instruction &i);
        void gen_jns(std::ostream &out, const Instruction &i);
        void gen_add(std::ostream &out, const Instruction &i);
        void gen_sub(std::ostream &out, const Instruction &i);
        void gen_mul(std::ostream &out, const Instruction &i);
        void gen_div(std::ostream &out, const Instruction &i);
        void gen_mod(std::ostream &out, const Instruction &i);

        /** @brief Peephole optimizer for generated assembly */
        /** @brief Apply peephole optimizations to generated assembly
         * @param code Raw assembly text
         * @param platform Target platform (affects optimization rules)
         * @return Optimized assembly text
         */
        std::string gen_optimize(const std::string &code, const Platform &platform);

        /** @brief Darwin-specific assembly optimizations
         * @param code Assembly text
         * @return Optimized assembly text
         */
        std::string optimize_darwin(const std::string &code);

        /** @brief Platform-independent peephole optimizations
         * @param code Assembly text
         * @return Optimized assembly text
         */
        std::string optimize_core(const std::string &code);
        /** @} */

        /** @name Win64 ABI x86-64 code generation */
        /** @{ */
        /** @brief Emit an integer-or-byte function argument load for Win64 ABI
         * @param out Assembly output stream
         * @param index Argument index (determines register)
         * @param dstReg Destination register name
         */
        void x64_emit_iob_func(std::ostream &out, int index, const std::string &dstReg);

        /** @brief Get the Win64 register name for a given index and variable type
         * @param index Register index
         * @param type Variable type (determines integer vs. XMM register)
         * @return Register name string
         */
        std::string x64_getRegisterByIndex(int index, VarType type);

        /** @brief Load a variable or constant into a numbered Win64 register
         * @param out Assembly output stream
         * @param r Register index
         * @param op Operand to load
         * @return 1 if the operand was a constant, 0 if variable
         */
        int x64_generateLoadVar(std::ostream &out, int r, const Operand &op);

        /** @brief Load a variable or constant into a named Win64 register
         * @param out Assembly output stream
         * @param type Expected variable type
         * @param reg Target register name
         * @param op Operand to load
         * @return 1 if the operand was a constant, 0 if variable
         */
        int x64_generateLoadVar(std::ostream &out, VarType type, std::string reg, const Operand &op);

        /** @brief Generate a Win64 ABI function call with operands as arguments
         * @param out Assembly output stream
         * @param name Callee function name
         * @param args Argument operand list
         */
        void x64_generateFunctionCall(std::ostream &out, const std::string &name, std::vector<Operand> &args);

        /** @brief Generate a Win64 dynamic invoke call
         * @param out Assembly output stream
         * @param op Operand list
         */
        void x64_generateInvokeCall(std::ostream &out, std::vector<Operand> &op);

        /** @brief Top-level Win64 code generation entry point
         * @param platform Target platform
         * @param obj true if generating as an object
         * @param out Assembly output stream
         */
        void x64_generateCode(const Platform &platform, bool obj, std::ostream &out);

        /** @brief Dispatch a single instruction to its Win64 code generator
         * @param out Assembly output stream
         * @param i Instruction to generate code for
         */
        void x64_generateInstruction(std::ostream &out, const Instruction &i);
        void x64_gen_done(std::ostream &out, const Instruction &i);
        void x64_gen_ret(std::ostream &out, const Instruction &i);
        void x64_gen_return(std::ostream &out, const Instruction &i);
        void x64_gen_neg(std::ostream &out, const Instruction &i);
        void x64_gen_invoke(std::ostream &out, const Instruction &i);
        void x64_gen_call(std::ostream &out, const Instruction &i);
        void x64_gen_alloc(std::ostream &out, const Instruction &i);
        /** @brief Generate Win64 x86-64 assembly for the REALLOC instruction (dynamic array resize) */
        void x64_gen_realloc(std::ostream &out, const Instruction &i);
        void x64_gen_free(std::ostream &out, const Instruction &i);
        void x64_gen_load(std::ostream &out, const Instruction &i);
        void x64_gen_store(std::ostream &out, const Instruction &i);
        void x64_gen_to_int(std::ostream &out, const Instruction &i);
        void x64_gen_to_float(std::ostream &out, const Instruction &i);
        void x64_gen_div(std::ostream &out, const Instruction &i);
        void x64_gen_mod(std::ostream &out, const Instruction &i);
        void x64_gen_getline(std::ostream &out, const Instruction &i);
        void x64_gen_bitop(std::ostream &out, const std::string &opc, const Instruction &i);
        void x64_gen_not(std::ostream &out, const Instruction &i);
        void x64_gen_push(std::ostream &out, const Instruction &i);
        void x64_gen_pop(std::ostream &out, const Instruction &i);
        void x64_gen_stack_load(std::ostream &out, const Instruction &i);
        void x64_gen_stack_store(std::ostream &out, const Instruction &i);
        void x64_gen_stack_sub(std::ostream &out, const Instruction &i);
        void x64_gen_print(std::ostream &out, const Instruction &i);
        void x64_gen_jmp(std::ostream &out, const Instruction &i);
        void x64_gen_cmp(std::ostream &out, const Instruction &i);
        void x64_gen_mov(std::ostream &out, const Instruction &i);
        void x64_gen_arth(std::ostream &out, std::string arth, const Instruction &i);
        void x64_gen_exit(std::ostream &out, const Instruction &i);
        void x64_gen_lea(std::ostream &out, const Instruction &i);
        void x64_gen_fcmp(std::ostream &out, const Instruction &i);
        void x64_gen_jae(std::ostream &out, const Instruction &i);
        void x64_gen_jbe(std::ostream &out, const Instruction &i);
        void x64_gen_jc(std::ostream &out, const Instruction &i);
        void x64_gen_jnc(std::ostream &out, const Instruction &i);
        void x64_gen_jp(std::ostream &out, const Instruction &i);
        void x64_gen_jnp(std::ostream &out, const Instruction &i);
        void x64_gen_jo(std::ostream &out, const Instruction &i);
        void x64_gen_jno(std::ostream &out, const Instruction &i);
        void x64_gen_js(std::ostream &out, const Instruction &i);
        void x64_gen_jns(std::ostream &out, const Instruction &i);

        std::unordered_map<std::string, std::string> x64_reg_vars;   ///< Win64 variable-to-register mapping
        std::vector<std::string> x64_reg_save_order;

        /** @brief Analyze variable usage and assign Win64 callee-saved registers
         * @param uses_std_module true if the program uses the std module (affects register choices)
         */
        void x64_analyzeRegAlloc(bool uses_std_module);

        /** @brief Write all register-allocated variables back to memory */
        void x64_emitFlushRegs(std::ostream &out);

        /** @brief Reload register-allocated variables from memory */
        void x64_emitReloadRegs(std::ostream &out);

        /** @brief Save callee-saved registers at function prologue */
        void x64_emitSaveRegs(std::ostream &out);

        /** @brief Restore callee-saved registers at function epilogue */
        void x64_emitRestoreRegs(std::ostream &out);

        /** @brief Store a register value into a variable (register-allocated or memory)
         * @param out Assembly output stream
         * @param srcReg Source register name
         * @param dest Destination operand
         */
        void x64_emitStoreVar(std::ostream &out, const std::string &srcReg, const Operand &dest);

        /** @brief Store an immediate value into a variable
         * @param out Assembly output stream
         * @param imm Immediate value string
         * @param dest Destination operand
         */
        void x64_emitStoreVarImm(std::ostream &out, const std::string &imm, const Operand &dest);

        /** @brief Load a variable into a destination register
         * @param out Assembly output stream
         * @param dstReg Destination register name
         * @param src Source operand
         */
        void x64_emitLoadVar(std::ostream &out, const std::string &dstReg, const Operand &src);
        /** @} */

        /** @name System V ABI register allocation */
        /** @{ */
        std::unordered_map<std::string, std::string> sysv_reg_vars;  ///< SysV variable-to-register mapping
        std::vector<std::string> sysv_reg_save_order;

        /** @brief Analyze variable usage and assign System V callee-saved registers
         * @param uses_std_module true if the program uses the std module
         */
        void sysv_analyzeRegAlloc(bool uses_std_module);

        /** @brief Write all register-allocated variables back to memory */
        void sysv_emitFlushRegs(std::ostream &out);

        /** @brief Reload register-allocated variables from memory */
        void sysv_emitReloadRegs(std::ostream &out);

        /** @brief Save callee-saved registers at function prologue */
        void sysv_emitSaveRegs(std::ostream &out);

        /** @brief Restore callee-saved registers at function epilogue */
        void sysv_emitRestoreRegs(std::ostream &out);

        /** @brief Store a register value into a variable (register-allocated or memory)
         * @param out Assembly output stream
         * @param srcReg Source register name
         * @param dest Destination operand
         */
        void sysv_emitStoreVar(std::ostream &out, const std::string &srcReg, const Operand &dest);

        /** @brief Store an immediate value into a variable
         * @param out Assembly output stream
         * @param imm Immediate value string
         * @param dest Destination operand
         */
        void sysv_emitStoreVarImm(std::ostream &out, const std::string &imm, const Operand &dest);

        /** @brief Load a variable into a destination register
         * @param out Assembly output stream
         * @param dstReg Destination register name
         * @param src Source operand
         */
        void sysv_emitLoadVar(std::ostream &out, const std::string &dstReg, const Operand &src);
        /** @} */

        /** @name Interpreter execution methods */
        /** @{ */
        void exec_mov(const Instruction &instr);
        void exec_add(const Instruction &instr);
        void exec_sub(const Instruction &instr);
        void exec_mul(const Instruction &instr);
        void exec_div(const Instruction &instr);
        void exec_cmp(const Instruction &instr);
        void exec_jmp(const Instruction &instr);
        void exec_load(const Instruction &instr);
        void exec_store(const Instruction &instr);
        void exec_or(const Instruction &instr);
        void exec_and(const Instruction &instr);
        void exec_xor(const Instruction &instr);
        void exec_not(const Instruction &instr);
        void exec_mod(const Instruction &instr);
        void exec_je(const Instruction &instr);
        void exec_jne(const Instruction &instr);
        void exec_jl(const Instruction &instr);
        void exec_jle(const Instruction &instr);
        void exec_jg(const Instruction &instr);
        void exec_jge(const Instruction &instr);
        void exec_jz(const Instruction &instr);
        void exec_jnz(const Instruction &instr);
        void exec_ja(const Instruction &instr);
        void exec_jb(const Instruction &instr);
        void exec_print(const Instruction &instr);
        void exec_string_print(const Instruction &instr);
        void exec_exit(const Instruction &instr);
        void exec_alloc(const Instruction &instr);
        void exec_free(const Instruction &instr);
        void exec_getline(const Instruction &instr);
        void exec_push(const Instruction &instr);
        void exec_pop(const Instruction &instr);
        void exec_stack_load(const Instruction &instr);
        void exec_stack_store(const Instruction &instr);
        void exec_stack_sub(const Instruction &instr);
        void exec_call(const Instruction &instr);
        void exec_ret(const Instruction &instr);
        void exec_done(const Instruction &instr);
        void exec_to_int(const Instruction &instr);
        void exec_to_float(const Instruction &instr);
        void exec_invoke(const Instruction &instr);
        void exec_return(const Instruction &instr);
        void exec_lea(const Instruction &instr);
        void exec_neg(const Instruction &instr);
        /**
         * @brief Execute the REALLOC instruction in the interpreter
         *
         * Resizes the memory block pointed to by the destination variable.
         * Operands: dest (pointer var), elemSize, count.
         * Zero-initialises any newly allocated bytes beyond the old allocation.
         * A count of zero frees the block.
         */
        void exec_realloc(const Instruction &instr);
        void exec_fcmp(const Instruction &instr);
        void exec_jae(const Instruction &instr);
        void exec_jbe(const Instruction &instr);
        void exec_jc(const Instruction &instr);
        void exec_jnc(const Instruction &instr);
        void exec_jp(const Instruction &instr);
        void exec_jnp(const Instruction &instr);
        void exec_jo(const Instruction &instr);
        void exec_jno(const Instruction &instr);
        void exec_js(const Instruction &instr);
        void exec_jns(const Instruction &instr);
        /** @} */

        /** @name Interpreter helper methods */
        /** @{ */
        /** @brief Look up a variable by name, searching local scope then global
         * @param name Variable identifier
         * @return Reference to the Variable
         * @throws mx::Exception if the variable is not found
         */
        Variable &getVariable(const std::string &name);

        /** @brief Check whether a variable exists in local or global scope
         * @param name Variable identifier
         * @return true if the variable is declared
         */
        bool isVariable(const std::string &name);

        /** @brief Set a variable's value from a string representation, coercing types
         * @param var Variable to modify
         * @param value String to parse
         */
        void setVariableFromString(Variable &var, const std::string &value);

        /** @brief Add two variables and store the result in dest
         * @param dest Destination variable
         * @param src1 First operand
         * @param src2 Second operand
         */
        void addVariables(Variable &dest, Variable &src1, Variable &src2);

        /** @brief Subtract src2 from src1 and store the result in dest */
        void subVariables(Variable &dest, Variable &src1, Variable &src2);

        /** @brief Multiply two variables and store the result in dest */
        void mulVariables(Variable &dest, Variable &src1, Variable &src2);

        /** @brief Divide src1 by src2 and store the result in dest */
        void divVariables(Variable &dest, Variable &src1, Variable &src2);

        /** @brief Format and optionally print a printf-style format string with variable arguments
         * @param format Format string with % specifiers
         * @param args Variables corresponding to format specifiers
         * @param output true to print, false to only return the string
         * @return The formatted string
         */
        std::string printFormatted(const std::string &format, const std::vector<Variable *> &args, bool output = true);

        /** @brief Set a variable's value from a constant string (integer, float, or string literal)
         * @param var Variable to modify
         * @param value Constant string to parse
         */
        void setVariableFromConstant(Variable &var, const std::string &value);

        /** @brief Create a temporary unnamed variable with the given type and value
         * @param type Variable type
         * @param value Initial value as string
         * @return A new Variable
         */
        Variable createTempVariable(VarType type, const std::string &value);

        /** @brief Check whether a string represents a compile-time constant (number or quoted string)
         * @param value String to test
         * @return true if it is a constant
         */
        bool isConstant(const std::string &value);

        /** @brief Resolve an Operand to its corresponding Variable
         * @param op Operand to resolve
         * @return The Variable (looked up or created as a temporary for constants)
         */
        Variable variableFromOperand(const Operand &op);
        /** @} */

        Stack stack;
        Operand result;              ///< return value operand from last invoke
        Program *parent = nullptr;   ///< parent program (for object programs)
        Platform platform;
        /** @brief Reserve stack space for a Win64 call frame including spill area
         * @param out Assembly output stream
         * @param spill_bytes Additional spill bytes needed
         * @return Total frame size allocated (for matching release)
         */
        size_t x64_reserve_call_area(std::ostream &out, size_t spill_bytes);

        /** @brief Release a Win64 call frame previously reserved
         * @param out Assembly output stream
         * @param frame Frame size to release
         */
        void x64_release_call_area(std::ostream &out, size_t frame);

        /** @brief Emit a direct stack pointer adjustment
         * @param out Assembly output stream
         * @param bytes Positive = allocate, negative = deallocate
         */
        void x64_direct_stack_adjust(std::ostream &out, int64_t bytes);
        bool last_call_returns_owned = false;
        VarType last_call_ret_type = VarType::VAR_INTEGER;
    };

    /** @brief Assert a condition, throwing an Exception with the given reason on failure
     * @param reason Error message
     * @param value Condition to assert (true = pass)
     */
    void except_assert(std::string reason, bool value);

    /** @brief Check whether a named function returns an owned (heap-allocated) pointer
     * @param funcName Function name to check
     * @return true if the function returns an owned pointer
     */
    bool isFunctionReturningOwnedPtr(const std::string &funcName);

} // namespace mxvm

#endif