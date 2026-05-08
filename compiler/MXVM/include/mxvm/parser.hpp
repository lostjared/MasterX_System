/**
 * @file parser.hpp
 * @brief Parser that tokenizes and parses MXVM source into programs, handles modules/objects, and dispatches code generation
 * @author Jared Bruni
 */
#ifndef _PARSER_H_X
#define _PARSER_H_X

#include "mxvm/instruct.hpp"
#include "mxvm/valid.hpp"
#include "scanner/scanner.hpp"
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

namespace mxvm {
    class ProgramNode;
    class SectionNode;
    class InstructionNode;
    class VariableNode;
    class CommentNode;
    class LabelNode;
    class Program;
    class ModuleNode;
    class ObjectNode;
    struct Variable;
    struct Operand;

    extern bool debug_mode;     ///< enable verbose debug output during parsing
    extern bool instruct_mode;   ///< enable instruction trace mode
    extern bool html_mode;       ///< enable HTML debug output

    /** @brief Execution mode: interpretation or native compilation */
    enum class Mode {
        MODE_INTERPRET,
        MODE_COMPILE
    };

    /** @brief Target platform for native code generation */
    enum class Platform {
        LINUX,
        DARWIN,
        WINX64
    };

    class ModuleParser;

    /**
     * @brief Main MXVM parser — tokenizes source, builds AST, and generates intermediate code
     *
     * Handles the full pipeline from source text to a populated Program:
     * scanning tokens, parsing sections (data/code/module/object),
     * resolving modules and objects, and dispatching to the code generator.
     */
    class Parser {

      public:
        friend class ModuleParser;
        /**
         * @brief Construct a parser from MXVM source text
         * @param source The MXVM source code to parse
         */
        explicit Parser(const std::string &source);
        /** @brief Copy constructor */
        Parser(const Parser &other)
            : source_file(other.source_file),
              scanner(other.scanner),
              validator(other.validator),
              parser_mode(other.parser_mode),
              module_path(other.module_path),
              object_path(other.object_path),
              include_path(other.include_path),
              object_mode(other.object_mode),
              object_name(other.object_name),
              platform{other.platform} {}

        /** @brief Copy assignment */
        Parser &operator=(const Parser &other) {
            if (this != &other) {
                source_file = other.source_file;
                scanner = other.scanner;
                validator = other.validator;
                parser_mode = other.parser_mode;
                module_path = other.module_path;
                object_path = other.object_path;
                include_path = other.include_path;
                object_mode = other.object_mode;
                object_name = other.object_name;
                platform = other.platform;
            }
            return *this;
        }

        /** @brief Tokenize the source and return the token count */
        uint64_t scan();

        /** @brief Parse all tokens into the internal AST */
        void parse();
        /** @brief Access a token by position */
        auto operator[](size_t pos);

        /**
         * @brief Parse a top-level program or object node
         * @param index Token index (advanced past consumed tokens)
         * @return Parsed ProgramNode
         */
        std::unique_ptr<ProgramNode> parseProgramOrObject(uint64_t &index);
        /**
         * @brief Build the full AST from tokens
         * @return Root ProgramNode of the AST
         */
        std::unique_ptr<ProgramNode> parseAST();
        /**
         * @brief Walk the AST and populate a Program with instructions and variables
         * @param m Execution mode (interpret or compile)
         * @param program Output program to populate
         * @return true on success
         */
        bool generateProgramCode(const Mode &m, std::unique_ptr<Program> &program);
        /** @brief Generate debug HTML output for the program */
        bool generateDebugHTML(std::ostream &out, std::unique_ptr<Program> &program);
        /** @brief Generate a native assembly file for an object */
        void generateObjectAssemblyFile(std::unique_ptr<Program> &objProgram);
        /** @brief Register external functions from an object into the main program */
        void registerObjectExterns(std::unique_ptr<Program> &mainProgram, const std::unique_ptr<Program> &objProgram);
        std::string source_file;
        scan::Scanner scanner;
        Validator validator;
        Mode parser_mode = Mode::MODE_INTERPRET;
        std::string module_path = ".";     ///< search path for .mxvm module files
        std::string object_path = ".";     ///< search path for .mxvm object files
        std::string include_path = "/usr/local/include/mxvm/modules"; ///< system module include path
        bool object_mode = false;           ///< true when parsing an object definition
        std::string object_name;
        Platform platform;

      private:
        std::unique_ptr<SectionNode> parseSection(uint64_t &index);
        std::unique_ptr<VariableNode> parseDataVariable(uint64_t &index);
        std::unique_ptr<InstructionNode> parseCodeInstruction(uint64_t &index);
        std::unique_ptr<CommentNode> parseComment(uint64_t &index);
        std::unique_ptr<LabelNode> parseLabel(uint64_t &index);
        std::unique_ptr<ModuleNode> parseModule(uint64_t &index);
        std::unique_ptr<ObjectNode> parseObject(uint64_t &index);

        void processDataSection(SectionNode *sectionNode, std::unique_ptr<Program> &program);
        void processCodeSection(SectionNode *sectionNode, std::unique_ptr<Program> &program);
        void processModuleSection(SectionNode *sectionNode, std::unique_ptr<Program> &program);
        void processObjectSection(SectionNode *sectionNode, std::unique_ptr<Program> &program);
        void processModuleFile(const std::string &src, std::unique_ptr<Program> &program);
        void processObjectFile(const std::string &src, std::unique_ptr<Program> &program);
        void setVariableValue(Variable &var, VarType type, const std::string &value, size_t buf_size = 0);
        void setDefaultVariableValue(Variable &var, VarType type);
        void resolveLabelReference(Operand &operand, const std::unordered_map<std::string, size_t> &labelMap);
        void collectObjectNames(std::vector<std::pair<std::string, std::string>> &names, const std::unique_ptr<Program> &program);
        void printObjectHTML(std::ostream &out, const std::unique_ptr<Program> &objPtr);
    };

    /** @brief Describes an external function imported from a module or object */
    struct ExternalFunction {
        std::string name;   ///< function symbol name
        std::string mod;    ///< module or object name
        bool module;        ///< true if from a module, false if from an object
        /** @brief Equality comparison by name and module */
        bool operator==(const ExternalFunction &f) {
            return (name == f.name && mod == f.mod);
        }

        VarType ret_type = VarType::VAR_INTEGER;  ///< return type of the external function
        bool ret_owned = false;                    ///< true if the return value is an owned pointer
    };

    /**
     * @brief Parser for MXVM module definition files (.mxvm)
     *
     * Reads a module's function declarations and registers them
     * as external functions in the calling program.
     */
    class ModuleParser {
      public:
        /**
         * @brief Construct a module parser
         * @param m Execution mode
         * @param mod_name Module name
         * @param source Module source text
         */
        ModuleParser(const Mode &m, const std::string &mod_name, const std::string &source);

        /** @brief Copy constructor */
        ModuleParser(const ModuleParser &other)
            : functions(other.functions),
              mod_name(other.mod_name),
              scanner(other.scanner),
              index(other.index),
              token(other.token ? new scan::TToken(*other.token) : nullptr) {}

        /** @brief Copy assignment */
        ModuleParser &operator=(const ModuleParser &other) {
            if (this != &other) {
                functions = other.functions;
                mod_name = other.mod_name;
                scanner = other.scanner;
                index = other.index;
                if (token)
                    delete token;
                token = other.token ? new scan::TToken(*other.token) : nullptr;
            }
            return *this;
        }

        /** @brief Tokenize the module source */
        uint64_t scan();
        /** @brief Parse the module declarations */
        bool parse();
        /** @brief Access a token by position */
        scan::TToken operator[](size_t pos);
        /** @brief Access a token by position (bounds-checked) */
        scan::TToken at(size_t pos);
        /**
         * @brief Register the module's external functions in a Program
         * @param m Execution mode
         * @param mod_id Module identifier
         * @param mod_name Module display name
         * @param program Target program
         * @return true on success
         */
        bool generateProgramCode(const Mode &m, const std::string &mod_id, const std::string &mod_name, std::unique_ptr<Program> &program);
        /** @brief Advance to the next token */
        bool next();
        /** @brief Check if the current token value matches and advance */
        bool match(const std::string &s);
        /** @brief Check if the current token type matches and advance */
        bool match(const types::TokenType &t);
        /** @brief Require the current token value to match, or throw */
        void require(const std::string &s);
        /** @brief Require the current token type to match, or throw */
        void require(const types::TokenType &t);

      protected:
        std::vector<ExternalFunction> functions; ///< parsed external function declarations
        std::string mod_name;                    ///< module name
        scan::Scanner scanner;                   ///< underlying scanner
        uint64_t index = 0;                      ///< current token index
        scan::TToken *token = nullptr;           ///< current token pointer
        Mode parser_mode;                        ///< interpret or compile
    };
} // namespace mxvm

#endif