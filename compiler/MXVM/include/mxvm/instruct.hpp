/**
 * @file instruct.hpp
 * @brief Instruction set enum, operand/instruction structs, variable types, and Variable/Variable_Value definitions
 * @author Jared Bruni
 */
#ifndef __INSTRUCT_HPP_X_
#define __INSTRUCT_HPP_X_

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief MXVM instruction set opcodes
 *
 * Enumerates all supported virtual machine instructions including
 * data movement, arithmetic, logic, control flow, stack operations,
 * memory management, I/O, and type conversion.
 */
enum Inc {
    NULL_INC = 0,
    MOV,
    LOAD,
    STORE,
    ADD,
    SUB,
    MUL,
    DIV,
    OR,
    AND,
    XOR,
    NOT,
    MOD,
    CMP,
    JMP,
    JE,
    JNE,
    JL,
    JLE,
    JG,
    JGE,
    JZ,
    JNZ,
    JA,
    JB,
    PRINT,
    EXIT,
    ALLOC,
    FREE,
    GETLINE,
    PUSH,
    POP,
    STACK_LOAD,
    STACK_STORE,
    STACK_SUB,
    CALL,
    RET,
    STRING_PRINT,
    DONE,
    TO_INT,
    TO_FLOAT,
    INVOKE,
    RETURN,
    NEG,
    FCMP,
    JAE,
    JBE,
    JC,
    JNC,
    JP,
    JNP,
    JO,
    JNO,
    JS,
    JNS,
    LEA,
    REALLOC         ///< Reallocate a dynamic memory block: realloc dest, elemSize, count
};

/** @brief String representations of Inc opcodes, indexed by enum value */
inline std::vector<std::string> IncType{
    "NULL",         // NULL_INC = 0
    "mov",          // MOV = 1
    "load",         // LOAD = 2
    "store",        // STORE = 3
    "add",          // ADD = 4
    "sub",          // SUB = 5
    "mul",          // MUL = 6
    "div",          // DIV = 7
    "or",           // OR = 8
    "and",          // AND = 9
    "xor",          // XOR = 10
    "not",          // NOT = 11
    "mod",          // MOD = 12
    "cmp",          // CMP = 13
    "jmp",          // JMP = 14
    "je",           // JE = 15
    "jne",          // JNE = 16
    "jl",           // JL = 17
    "jle",          // JLE = 18
    "jg",           // JG = 19
    "jge",          // JGE = 20
    "jz",           // JZ = 21
    "jnz",          // JNZ = 22
    "ja",           // JA = 23
    "jb",           // JB = 24
    "print",        // PRINT = 25
    "exit",         // EXIT = 26
    "alloc",        // ALLOC = 27
    "free",         // FREE = 28
    "getline",      // GETLINE = 29
    "push",         // PUSH = 30
    "pop",          // POP = 31
    "stack_load",   // STACK_LOAD = 32
    "stack_store",  // STACK_STORE = 33
    "stack_sub",    // STACK_SUB = 34
    "call",         // CALL = 35
    "ret",          // RET = 36
    "string_print", // STRING_PRINT = 37
    "done",         // DONE = 38
    "to_int",       // TO_INT = 39
    "to_float",     // TO_FLOAT = 40
    "invoke",       // INVOKE = 41
    "return",       // RETURN = 42
    "neg",          // NEG = 43
    "fcmp",         // FCMP = 44 (floating-point compare)
    "jae",          // JAE = 45 (jump if above or equal, unsigned)
    "jbe",          // JBE = 46 (jump if below or equal, unsigned)
    "jc",           // JC = 47 (jump if carry)
    "jnc",          // JNC = 48 (jump if no carry)
    "jp",           // JP = 49 (jump if parity)
    "jnp",          // JNP = 50 (jump if no parity)
    "jo",           // JO = 51 (jump if overflow)
    "jno",          // JNO = 52 (jump if no overflow)
    "js",           // JS = 53 (jump if sign)
    "jns",          // JNS = 54 (jump if no sign)
    "lea",          // LEA = 55 (load effective address)
    "realloc"       // REALLOC = 56 (reallocate memory)
};

/**
 * @brief Stream insertion operator for Inc opcodes
 * @param out Output stream
 * @param i Opcode to print
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &out, const enum Inc &i);

namespace mxvm {

    /** @brief Discriminates whether an operand is a compile-time constant or a variable reference */
    enum class OperandType {
        OP_CONSTANT,
        OP_VARIABLE
    };

    /** @brief A single instruction operand (constant value or variable reference) */
    struct Operand {
        std::string label;      ///< label target for jump/call operands
        std::string op;         ///< textual operand value
        int op_value = 0;       ///< numeric operand value
        OperandType type;
        std::string object;     ///< owning object name for member access
    };

    /** @brief A complete MXVM instruction with opcode, operands, and optional label */
    struct Instruction {
        Inc instruction;        ///< the opcode
        Operand op1, op2, op3;  ///< up to three fixed operands
        std::vector<Operand> vop; ///< variable-length operand list (for print, invoke, etc.)
        std::string label = ""; ///< label at this instruction address
        /** @brief Convert this instruction to its assembly text representation
         * @return Formatted string with opcode, operands, and label
         */
        std::string toString() const;
    };

    /** @brief MXVM variable type discriminator */
    enum class VarType {
        VAR_NULL = 0,
        VAR_INTEGER,
        VAR_FLOAT,
        VAR_STRING,
        VAR_POINTER,
        VAR_LABEL,
        VAR_ARRAY,
        VAR_EXTERN,
        VAR_BYTE
    };
    /** @brief MXVM language keywords */
    enum class Keywords {
        KEY_PROGRAM,
        KEY_OBJECT,
        KEY_MODULE,
        KEY_DATA,
        KEY_CODE,
        KEY_SECTION
    };
    /** @brief String representations of Keywords enum values */
    extern std::vector<std::string> keywords;
    // const char *keywords[] = {"program", "object", "module", "data" , "code", "section" };

    /**
     * @brief Runtime value storage for a variable
     *
     * Uses a union for int/float/pointer storage. Tracks pointer ownership
     * and allocation state for memory management.
     */
    struct Variable_Value {
        std::string str_value;
        std::string label_value;
        union {
            int64_t int_value;
            double float_value;
            void *ptr_value;
        };
        uint64_t ptr_size = 0;      ///< size of each element in allocated memory
        uint64_t ptr_count = 0;     ///< number of elements allocated
        VarType type;
        uint64_t buffer_size;       ///< declared buffer size
        bool owns = false;          ///< true if this value owns the allocated memory
        bool released = false;      ///< true if the owned memory has been freed

        /** @brief Copy constructor — deep-copies all fields, dispatching on type for the union member
         * @param other Source value to copy from
         */
        Variable_Value(const Variable_Value &other)
            : str_value(other.str_value),
              label_value(other.label_value),
              ptr_size(other.ptr_size),
              ptr_count(other.ptr_count),
              type(other.type),
              buffer_size(other.buffer_size),
              owns(other.owns),
              released(other.released) {
            switch (type) {
            case VarType::VAR_INTEGER:
            case VarType::VAR_BYTE:
                int_value = other.int_value;
                break;
            case VarType::VAR_FLOAT:
                float_value = other.float_value;
                break;
            case VarType::VAR_POINTER:
                ptr_value = other.ptr_value;
                break;
            default:
                int_value = other.int_value;
                break;
            }
        }

        /** @brief Copy-assignment operator — self-check then deep copy, dispatching on type
         * @param other Source value to copy from
         * @return Reference to this
         */
        Variable_Value &operator=(const Variable_Value &other) {
            if (this != &other) {
                str_value = other.str_value;
                label_value = other.label_value;
                ptr_size = other.ptr_size;
                ptr_count = other.ptr_count;
                type = other.type;
                buffer_size = other.buffer_size;
                owns = other.owns;
                released = other.released;
                switch (type) {
                case VarType::VAR_INTEGER:
                case VarType::VAR_BYTE:
                    int_value = other.int_value;
                    break;
                case VarType::VAR_FLOAT:
                    float_value = other.float_value;
                    break;
                case VarType::VAR_POINTER:
                    ptr_value = other.ptr_value;
                    break;
                default:
                    int_value = other.int_value;
                    break;
                }
            }
            return *this;
        }

        /** @brief Default constructor — initialises to VAR_NULL with zeroed fields */
        Variable_Value() : int_value(0), ptr_size(0), ptr_count(0), type(VarType::VAR_NULL), buffer_size(0), owns(false), released(false) {}
    };

    /** @brief A named variable with type, value, and optional object association */
    struct Variable {
        VarType type;
        std::string var_name;
        Variable_Value var_value;
        bool is_global = false;
        std::string obj_name;
        /** @brief Default constructor — VAR_NULL type with empty name and value */
        Variable() : type(VarType::VAR_NULL), var_name(), var_value() {}

        /** @brief Construct a variable with the given name, type, and initial value
         * @param name Variable identifier
         * @param vtype Variable type discriminator
         * @param value Initial runtime value
         */
        Variable(const std::string &name, const VarType &vtype, const Variable_Value &value)
            : type(vtype), var_name(name), var_value(value), is_global(false) {}

        /** @brief Copy constructor
         * @param v Source variable to copy
         */
        Variable(const Variable &v)
            : type(v.type), var_name(v.var_name), var_value(v.var_value), is_global(v.is_global), obj_name(v.obj_name) {}

        /** @brief Configure this variable as an external pointer reference
         * @param name External variable name
         * @param value Pointer to the external data
         */
        void setExtern(std::string name, void *value) {
            type = VarType::VAR_EXTERN;
            var_value.type = VarType::VAR_EXTERN;
            var_value.ptr_value = value;
            var_value.ptr_count = 0;
            var_value.ptr_size = 0;
            var_value.owns = false;
            var_name = name;
        }

        /** @brief Copy-assignment operator
         * @param v Source variable to copy
         * @return Reference to this
         */
        Variable &operator=(const Variable &v) {
            if (this != &v) {
                type = v.type;
                var_name = v.var_name;
                var_value = v.var_value;
                is_global = v.is_global;
                obj_name = v.obj_name;
            }
            return *this;
        }
        /** @brief Convert this variable to a human-readable string
         * @return Formatted string describing the variable name, type, and value
         */
        std::string toString() const;
    };
} // namespace mxvm

/**
 * @brief Stream insertion operator for Instruction
 * @param out Output stream
 * @param inc Instruction to format and print
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &out, const mxvm::Instruction &inc);

/**
 * @brief Stream insertion operator for VarType
 * @param out Output stream
 * @param type Variable type to print
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &out, const mxvm::VarType &type);

/**
 * @brief Stream insertion operator for Keywords
 * @param out Output stream
 * @param key Keyword to print
 * @return Reference to the output stream
 */
std::ostream &operator<<(std::ostream &out, const mxvm::Keywords &key);
#endif
