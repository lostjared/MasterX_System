/**
 * @file parser.cpp
 * @brief MXVM parser — tokenization, AST construction, and intermediate code generation
 * @author Jared Bruni
 */
#include "mxvm/parser.hpp"
#include "mxvm/ast.hpp"
#include "mxvm/icode.hpp"
#include "scanner/exception.hpp"
#include "scanner/scanner.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <unordered_map>

namespace {
    std::string html_escape(const std::string &s) {
        std::string out;
        out.reserve(s.size() * 12 / 10 + 8);
        for (char c : s) {
            switch (c) {
            case '&':
                out += "&amp;";
                break;
            case '<':
                out += "&lt;";
                break;
            case '>':
                out += "&gt;";
                break;
            case '"':
                out += "&quot;";
                break;
            case '\'':
                out += "&#39;";
                break;
            default:
                out.push_back(c);
                break;
            }
        }
        return out;
    }
    std::string js_escape(const std::string &s) {
        std::string out;
        out.reserve(s.size() * 12 / 10 + 8);
        for (char c : s) {
            switch (c) {
            case '\\':
                out += "\\\\";
                break;
            case '\'':
                out += "\\'";
                break;
            case '"':
                out += "\\\"";
                break;
            case '\n':
                out += "\\n";
                break;
            case '\r':
                out += "\\r";
                break;
            case '\0':
                out += "\\0";
                break;
            case '`':
                out += "\\`";
                break;
            case '$':
                out += "\\$";
                break;
            case '/':
                out += "\\/";
                break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof(buf), "\\x%02x", static_cast<unsigned char>(c));
                    out += buf;
                } else {
                    out.push_back(c);
                }
                break;
            }
        }
        return html_escape(out);
    }
} // namespace

namespace mxvm {

    bool debug_mode = false;
    bool instruct_mode = false;
    bool html_mode = false;

    ModuleParser::ModuleParser(const Mode &mode, const std::string &m, const std::string &source) : mod_name(m), scanner(source), parser_mode(mode) {}

    uint64_t ModuleParser::scan() {
        scanner.scan();
        return scanner.size();
    }

    bool ModuleParser::parse() {
        next();
        require("module");
        next();
        require(types::TokenType::TT_ID);
        std::string mod_name = token->getTokenValue();
        next();
        require("{");
        while (!match("}")) {
            if (match(types::TokenType::TT_ID) && token->getTokenValue() == "extern") {
                next();
                require(types::TokenType::TT_ID);
                std::string func_name = token->getTokenValue();
                functions.push_back({func_name, mod_name});
                next();
                continue;
            }
            next();
        }
        return true;
    }

    scan::TToken ModuleParser::operator[](size_t pos) {
        return at(pos);
    }

    scan::TToken ModuleParser::at(size_t pos) {
        if (pos < scanner.size())
            return scanner[pos];

        throw scan::ScanExcept("Out of bounds");
        return scanner[0];
    }

    bool ModuleParser::generateProgramCode(const Mode &m, const std::string &mod_id, const std::string &mod_name1, std::unique_ptr<Program> &program) {
        for (auto &f : functions) {
            if (m == Mode::MODE_INTERPRET)
                program->add_runtime_extern(mod_name, mod_name1, "mxvm_" + mod_id + "_" + f.name, f.name);
            else
                program->add_extern(mod_name, f.name, true);
        }
        return true;
    }

    bool ModuleParser::next() {
        while (index < scanner.size() && scanner[index].getTokenType() != types::TokenType::TT_STR && scanner[index].getTokenValue() == "\n") {
            index++;
        }
        if (index < scanner.size()) {
            token = &scanner[index++];
            return true;
        }
        return false;
    }

    bool ModuleParser::match(const std::string &s) {
        return (token->getTokenValue() == s);
    }

    bool ModuleParser::match(const types::TokenType &t) {
        return (token->getTokenType() == t);
    }

    void ModuleParser::require(const std::string &s) {
        if (!match(s)) {
            throw mx::Exception("Syntax Error: Looking for: " + s + " on line " + std::to_string(token->getLine()));
        }
    }
    void ModuleParser::require(const types::TokenType &t) {
        if (!match(t)) {
            throw mx::Exception("Syntax Error: Looking for: " + std::to_string(static_cast<unsigned int>(t)) + " on line " + std::to_string(token->getLine()));
        }
    }

    Parser::Parser(const std::string &source) : source_file(source), scanner(source), validator(source) {
    }

    uint64_t Parser::scan() {
        try {
            scanner.scan();
        } catch (scan::ScanExcept &e) {
            std::cerr << "Scanner error: " << e.why() << "\n";
        }
        return scanner.size();
    }

    auto Parser::operator[](size_t pos) {
        if (pos < scanner.size()) {
            return scanner[pos];
        }
        throw mx::Exception("Error: scanner index out of bounds.\n");
        return scanner[0];
    }

    std::unique_ptr<ProgramNode> Parser::parseAST() {
        std::unique_ptr<ProgramNode> mainProgram = nullptr;
        std::vector<std::unique_ptr<ProgramNode>> allDeclarations;
        std::string name;
        for (uint64_t i = 0; i < scanner.size(); ++i) {
            auto token = this->operator[](i);
            std::string tokenValue = token.getTokenValue();

            if (i + 1 < scanner.size())
                name = this->operator[](i + 1).getTokenValue();

            if (tokenValue == "program" || tokenValue == "object") {

                auto declaration = parseProgramOrObject(i);
                if (declaration) {
                    if (tokenValue == "program") {
                        declaration->object = false;
                        declaration->name = name;
                        mainProgram = std::move(declaration);
                        object_name = name;
                    } else {
                        declaration->object = true;
                        declaration->name = name;
                        object_name = name;
                        allDeclarations.push_back(std::move(declaration));
                    }
                }
            }
        }

        if (!mainProgram) {
            mainProgram = std::make_unique<ProgramNode>();
            mainProgram->name = name;
            mainProgram->object = false;
            object_name = name;
        }

        for (auto &obj : allDeclarations) {
            mainProgram->addInlineObject(std::move(obj));
        }

        return mainProgram;
    }

    std::unique_ptr<ObjectNode> Parser::parseObject(uint64_t &index) {
        if (index >= scanner.size())
            return nullptr;
        std::string name = this->operator[](index).getTokenValue();
        index++;
        if (index < scanner.size()) {
            if (this->operator[](index).getTokenValue() == ",")
                index++;
        }
        return std::make_unique<ObjectNode>(name);
    }

    void Parser::processObjectSection(SectionNode *sectionNode, std::unique_ptr<Program> &program) {
        for (const auto &statement : sectionNode->statements) {
            auto objNode = dynamic_cast<ObjectNode *>(statement.get());
            if (objNode) {
                std::string &name = objNode->name;
                processObjectFile(name, program);
            }
        }
    }

    void Parser::processObjectFile(const std::string &src, std::unique_ptr<Program> &program) {
        static std::set<std::string> processing_files;
        if (processing_files.count(src)) {
            return;
        }
        processing_files.insert(src);
        std::fstream file;
        std::string path;
        if (object_path.ends_with("/"))
            path = object_path;
        else
            path = object_path + "/";

        file.open(path + src + ".mxvm", std::ios::in);
        if (!file.is_open()) {
            return;
        }
        std::ostringstream stream;
        stream << file.rdbuf();
        file.close();

        std::string full_path = path + src + ".mxvm";

        if (!std::filesystem::exists(full_path)) {
            throw mx::Exception("Object file not found: " + full_path);
        }

        mxvm::Validator file_validator(stream.str());
        try {
            if (!file_validator.validate(full_path)) {
                throw mx::Exception("Validation failed for: " + full_path);
            }
        } catch (mx::Exception &e) {
            throw;
        }

        std::unique_ptr<Parser> parser(new Parser(stream.str()));
        parser->module_path = module_path;
        parser->object_path = object_path;
        parser->object_mode = true;
        parser->parser_mode = parser_mode;
        parser->platform = platform;
        parser->scan();

        auto ast = parser->parseAST();
        if (!ast)
            return;

        for (const auto &inlineObj : ast->inlineObjects) {
            auto objProgram = std::make_unique<Program>();
            objProgram->name = inlineObj->name;
            objProgram->object = true;
            objProgram->object_external = true;
            objProgram->filename = path + src + ".mxvm";
            objProgram->platform = platform;
            for (const auto &section : inlineObj->sections) {
                auto sectionNode = dynamic_cast<SectionNode *>(section.get());
                if (!sectionNode)
                    continue;

                if (sectionNode->type == SectionNode::DATA) {
                    parser->processDataSection(sectionNode, objProgram);
                } else if (sectionNode->type == SectionNode::CODE) {
                    parser->processCodeSection(sectionNode, objProgram);
                } else if (sectionNode->type == SectionNode::MODULE) {
                    parser->processModuleSection(sectionNode, objProgram);
                } else if (sectionNode->type == SectionNode::OBJECT) {
                    parser->processObjectSection(sectionNode, objProgram);
                }
            }

            if (Program::base != nullptr && objProgram->object)
                Program::base->add_object(objProgram->name, objProgram.get());

            registerObjectExterns(program, objProgram);

            program->objects.push_back(std::move(objProgram));
        }
        processing_files.erase(src);
    }
    std::unique_ptr<SectionNode> Parser::parseSection(uint64_t &index) {
        index++;

        if (index >= scanner.size())
            return nullptr;

        auto sectionNameToken = this->operator[](index);
        std::string sectionName = sectionNameToken.getTokenValue();

        SectionNode::SectionType sectionType;
        if (sectionName == "data") {
            sectionType = SectionNode::DATA;
        } else if (sectionName == "code") {
            sectionType = SectionNode::CODE;
        } else if (sectionName == "module") {
            sectionType = SectionNode::MODULE;
        } else if (sectionName == "object") {
            sectionType = SectionNode::OBJECT;
        } else {
            return nullptr;
        }

        auto section = std::make_unique<SectionNode>(sectionType);
        index++;

        if (index < scanner.size() && this->operator[](index).getTokenValue() == "{") {
            index++;

            while (index < scanner.size()) {
                auto token = this->operator[](index);
                std::string value = token.getTokenValue();

                if (value == "}") {
                    index++;
                    break;
                }

                if (value == "\n" || value == " " || value == "\t") {
                    index++;
                    continue;
                }

                if (value == "//" || value.starts_with("//") || value == "#" || value.starts_with("#")) {
                    auto comment = parseComment(index);
                    if (comment) {
                        section->addStatement(std::move(comment));
                    }
                    continue;
                }

                if (sectionType == SectionNode::DATA) {
                    auto variable = parseDataVariable(index);
                    if (variable) {
                        section->addStatement(std::move(variable));
                    } else {
                        index++;
                    }
                } else if (sectionType == SectionNode::MODULE) {
                    auto module = parseModule(index);
                    if (module) {
                        section->addStatement(std::move(module));
                    } else {
                        index++;
                    }
                } else if (sectionType == SectionNode::OBJECT) {
                    auto obj = parseObject(index);
                    if (obj) {
                        section->addStatement(std::move(obj));
                    } else {
                        index++;
                    }
                } else if (sectionType == SectionNode::CODE) {
                    if (token.getTokenType() == types::TokenType::TT_ID &&
                        index + 1 < scanner.size() &&
                        this->operator[](index + 1).getTokenValue() == ":") {
                        auto label = parseLabel(index);
                        if (label) {
                            section->addStatement(std::move(label));
                        }
                    } else if (token.getTokenType() == types::TokenType::TT_ID && token.getTokenValue() == "function" && index + 2 < scanner.size() && this->operator[](index + 1).getTokenType() == types::TokenType::TT_ID && this->operator[](index + 2).getTokenValue() == ":") {
                        index++;
                        auto label = parseLabel(index);
                        if (label) {
                            label->function = true;
                            section->addStatement(std::move(label));
                            continue;
                        }
                    } else {
                        auto instruction = parseCodeInstruction(index);
                        if (instruction) {
                            section->addStatement(std::move(instruction));
                        } else {
                            index++;
                        }
                    }
                }
            }
        }

        return section;
    }

    std::unique_ptr<ModuleNode> Parser::parseModule(uint64_t &index) {
        if (index >= scanner.size())
            return nullptr;
        std::string name = this->operator[](index).getTokenValue();
        index++;
        if (index < scanner.size()) {
            if (this->operator[](index).getTokenValue() == ",")
                index++;
        }
        return std::make_unique<ModuleNode>(name);
    }

    std::unique_ptr<VariableNode> Parser::parseDataVariable(uint64_t &index) {
        if (index >= scanner.size())
            return nullptr;

        bool is_global = false;
        auto token = this->operator[](index);
        if (token.getTokenValue() == "export") {
            is_global = true;
            index++;
            if (index >= scanner.size())
                return nullptr;
            token = this->operator[](index);
        }

        std::string typeStr = token.getTokenValue();
        VarType varType;
        if (typeStr == "int")
            varType = VarType::VAR_INTEGER;
        else if (typeStr == "float")
            varType = VarType::VAR_FLOAT;
        else if (typeStr == "string")
            varType = VarType::VAR_STRING;
        else if (typeStr == "ptr")
            varType = VarType::VAR_POINTER;
        else if (typeStr == "array")
            varType = VarType::VAR_ARRAY;
        else if (typeStr == "byte")
            varType = VarType::VAR_BYTE;
        else
            return nullptr;

        index++;
        if (index >= scanner.size())
            return nullptr;
        auto nameToken = this->operator[](index);

        if (nameToken.getTokenType() != types::TokenType::TT_ID) {
            return nullptr;
        }
        std::string varName = nameToken.getTokenValue();
        index++;

        if (index < scanner.size() && this->operator[](index).getTokenValue() == "=") {
            index++;

            std::string add_op;
            if (index < scanner.size() && this->operator[](index).getTokenValue() == "-"
                && this->operator[](index).getTokenType() == types::TokenType::TT_SYM) {
                add_op = "-";
                index++;
            }
            if (index < scanner.size()) {
                auto valueToken = this->operator[](index);
                std::string value = valueToken.getTokenValue();
                types::TokenType tokenType = valueToken.getTokenType();
                switch (tokenType) {
                case types::TokenType::TT_STR:
                    if (value.starts_with("\"") && value.ends_with("\"")) {
                        value = value.substr(1, value.length() - 2);
                    }
                    index++;
                    {
                        auto node = std::make_unique<VariableNode>(varType, varName, value);
                        node->is_global = is_global;
                        return node;
                    }

                case types::TokenType::TT_NUM:
                    index++;
                    {
                        auto node = std::make_unique<VariableNode>(varType, varName, add_op + value);
                        node->is_global = is_global;
                        return node;
                    }

                case types::TokenType::TT_HEX:
                    index++;
                    {
                        auto node = std::make_unique<VariableNode>(varType, varName, value);
                        node->is_global = is_global;
                        return node;
                    }

                case types::TokenType::TT_ID:
                    index++;
                    {
                        auto node = std::make_unique<VariableNode>(varType, varName, value);
                        node->is_global = is_global;
                        return node;
                    }
                default:
                    index++;
                    {
                        auto node = std::make_unique<VariableNode>(varType, varName, value);
                        node->is_global = is_global;
                        return node;
                    }
                }
            }
        } else if (index < scanner.size() && this->operator[](index).getTokenValue() == "," && varType == VarType::VAR_STRING) {
            index++;
            size_t buf_size = 0;
            if (index < scanner.size()) {
                buf_size = std::stoll(this->operator[](index).getTokenValue(), nullptr, 0);
            }
            if (buf_size == 0) {
                throw mx::Exception("string buffer: " + varName + " requires valid size");
            }
            auto node = std::make_unique<VariableNode>(varType, varName, buf_size);
            node->is_global = is_global;
            return node;
        }

        auto node = std::make_unique<VariableNode>(varType, varName);
        node->is_global = is_global;
        return node;
    }

    std::unique_ptr<InstructionNode> Parser::parseCodeInstruction(uint64_t &index) {
        static std::unordered_map<std::string, Inc> instructionMap = {
            {"mov", MOV}, {"load", LOAD}, {"store", STORE}, {"add", ADD}, {"sub", SUB}, {"mul", MUL}, {"div", DIV}, {"or", OR}, {"and", AND}, {"xor", XOR}, {"not", NOT}, {"mod", MOD}, {"cmp", CMP}, {"fcmp", FCMP}, {"jmp", JMP}, {"je", JE}, {"jne", JNE}, {"jl", JL}, {"jle", JLE}, {"jg", JG}, {"jge", JGE}, {"jz", JZ}, {"jnz", JNZ}, {"ja", JA}, {"jb", JB}, {"jae", JAE}, {"jbe", JBE}, {"jc", JC}, {"jnc", JNC}, {"jp", JP}, {"jnp", JNP}, {"jo", JO}, {"jno", JNO}, {"js", JS}, {"jns", JNS}, {"print", PRINT}, {"exit", EXIT}, {"alloc", ALLOC}, {"free", FREE}, {"getline", GETLINE}, {"push", PUSH}, {"pop", POP}, {"stack_load", STACK_LOAD}, {"stack_store", STACK_STORE}, {"stack_sub", STACK_SUB}, {"call", CALL}, {"ret", RET}, {"string_print", STRING_PRINT}, {"done", DONE}, {"to_int", TO_INT}, {"to_float", TO_FLOAT}, {"invoke", INVOKE}, {"return", RETURN}, {"neg", NEG}, {"lea", LEA}, {"realloc", REALLOC}};

        if (index >= scanner.size())
            return nullptr;

        auto token = this->operator[](index);
        std::string tokenValue = token.getTokenValue();

        if (token.getTokenType() == types::TokenType::TT_ID) {
            if (index + 1 < scanner.size() && this->operator[](index + 1).getTokenValue() == ":") {
                return nullptr;
            }
        }

        auto instIt = instructionMap.find(tokenValue);
        if (instIt == instructionMap.end()) {
            throw mx::Exception("Error invalid instruction: " + tokenValue);
            return nullptr;
        }

        Inc instruction = instIt->second;
        index++;

        std::vector<Operand> operands;

        while (index < scanner.size()) {

            std::string add_value;
            if (index < scanner.size() && this->operator[](index).getTokenValue() == "-") {
                add_value = "-";
                index++;
            }
            auto operandToken = this->operator[](index);
            std::string value = operandToken.getTokenValue();
            types::TokenType tokenType = operandToken.getTokenType();

            if (value == "\n" || value == "//" || value.starts_with("//") ||
                value == "#" || value.starts_with("#") || value == "}") {
                break;
            }

            if (value == "," || value == " " || value == "\t") {
                index++;
                continue;
            }

            Operand operand;

            switch (tokenType) {
            case types::TokenType::TT_NUM:
                operand.op = add_value + value;
                operand.op_value = std::stoll(add_value + value);
                operand.type = OperandType::OP_CONSTANT;
                break;

            case types::TokenType::TT_HEX:
                operand.op = value;
                if (value.starts_with("0x") || value.starts_with("0X")) {
                    operand.op_value = std::stoll(value, nullptr, 16);
                } else {
                    operand.op_value = std::stoll(value, nullptr, 16);
                }
                operand.type = OperandType::OP_CONSTANT;
                break;

            case types::TokenType::TT_ID:

                if (index + 1 < scanner.size() && this->operator[](index + 1).getTokenValue() == ".") {
                    operand.object = value;
                    index += 2;
                    if (index < scanner.size())
                        value = this->operator[](index).getTokenValue();
                }

                if (operand.object.empty())
                    operand.op = value;
                else
                    operand.op = operand.object + "." + value;
                operand.label = value;
                operand.type = OperandType::OP_VARIABLE;
                break;

            case types::TokenType::TT_STR:
                operand.op = value;
                operand.type = OperandType::OP_CONSTANT;
                break;

            case types::TokenType::TT_SYM:
                if (value != ",") {
                    operand.op = value;
                }
                break;
            default:
                operand.op = value;
                break;
            }

            if (!operand.op.empty()) {
                operands.push_back(operand);
            }

            index++;
        }

        return std::make_unique<InstructionNode>(instruction, operands);
    }

    std::unique_ptr<CommentNode> Parser::parseComment(uint64_t &index) {
        std::string commentText;

        auto token = this->operator[](index);
        if (token.getTokenValue() == "//" || token.getTokenValue() == "#" ||
            token.getTokenValue().starts_with("//") || token.getTokenValue().starts_with("#")) {
            index++;
        }

        while (index < scanner.size()) {
            auto token = this->operator[](index);
            if (token.getTokenValue() == "\n") {
                break;
            }
            commentText += token.getTokenValue() + " ";
            index++;
        }

        return std::make_unique<CommentNode>(commentText);
    }

    std::unique_ptr<LabelNode> Parser::parseLabel(uint64_t &index) {
        if (index >= scanner.size())
            return nullptr;

        auto token = this->operator[](index);

        if (token.getTokenType() != types::TokenType::TT_ID) {
            return nullptr;
        }

        std::string labelName = token.getTokenValue();
        index++;

        if (index < scanner.size() && this->operator[](index).getTokenValue() == ":") {
            index++;
            return std::make_unique<LabelNode>(labelName);
        }

        return nullptr;
    }

    void Parser::parse() {
        auto ast = parseAST();
        if (ast) {
            std::cout << ast->toString() << std::endl;
        }
    }

    bool Parser::generateProgramCode(const Mode &mode, std::unique_ptr<Program> &program) {
        parser_mode = mode;
        try {
            if (!validator.validate(program->filename)) {
                return false;
            }
        } catch (mx::Exception &e) {
            std::cerr << e.what() << "\n";
            return false;
        }

        auto ast = parseAST();
        if (ast) {
            program->name = ast->name;
            if (!ast->root_name.empty()) {
                program->root_name = ast->root_name;
                program->name = ast->root_name;
                program->object = false;
            } else {
                program->object = true;
            }

            for (const auto &section : ast->sections) {
                auto sectionNode = dynamic_cast<SectionNode *>(section.get());
                if (!sectionNode)
                    continue;

                if (sectionNode->type == SectionNode::DATA) {
                    processDataSection(sectionNode, program);
                } else if (sectionNode->type == SectionNode::CODE) {
                    processCodeSection(sectionNode, program);
                } else if (sectionNode->type == SectionNode::MODULE) {
                    processModuleSection(sectionNode, program);
                } else if (sectionNode->type == SectionNode::OBJECT) {
                    processObjectSection(sectionNode, program);
                }
            }

            for (const auto &inlineObj : ast->inlineObjects) {
                auto objProgram = std::make_unique<Program>();
                objProgram->name = inlineObj->name;
                objProgram->object = true;
                objProgram->object_external = false;
                objProgram->filename = program->filename;
                objProgram->platform = platform;
                object_name = inlineObj->name;

                for (const auto &section : inlineObj->sections) {
                    auto sectionNode = dynamic_cast<SectionNode *>(section.get());
                    if (!sectionNode)
                        continue;

                    if (sectionNode->type == SectionNode::DATA) {
                        processDataSection(sectionNode, objProgram);
                    } else if (sectionNode->type == SectionNode::CODE) {
                        processCodeSection(sectionNode, objProgram);
                    } else if (sectionNode->type == SectionNode::MODULE) {
                        processModuleSection(sectionNode, objProgram);
                    } else if (sectionNode->type == SectionNode::OBJECT) {
                        processObjectSection(sectionNode, objProgram);
                    }
                }

                registerObjectExterns(program, objProgram);

                if (Program::base != nullptr && objProgram->object)
                    Program::base->add_object(objProgram->name, objProgram.get());

                program->objects.push_back(std::move(objProgram));
            }

            if (mode == Mode::MODE_COMPILE) {
                std::set<std::string> generated_objects;

                std::function<void(std::unique_ptr<Program> &)> compileAllObjects;
                compileAllObjects =
                    [&](std::unique_ptr<Program> &p) {
                        if (!p)
                            return;
                        for (auto &obj : p->objects) {
                            if (obj && generated_objects.find(obj->name) == generated_objects.end()) {
                                generateObjectAssemblyFile(obj);
                                generated_objects.insert(obj->name);
                                if (html_mode) {
                                    std::ofstream htmlFile(obj->name + ".html");
                                    if (htmlFile.is_open()) {
                                        generateDebugHTML(htmlFile, obj);
                                        std::cout << Col("MXVM: Generated ", mx::Color::BRIGHT_GREEN) << "Debug HTML for: " << obj->name << "\n";
                                    }
                                }
                            }
                            compileAllObjects(obj);
                        }
                    };
                compileAllObjects(program);
            }

            if (program->object == false) {
                if (!program->validateNames(validator)) {
                    throw mx::Exception("Could not validate variables/functions\n");
                }
            }
        }
        return true;
    }
    void Parser::collectObjectNames(std::vector<std::pair<std::string, std::string>> &names, const std::unique_ptr<Program> &program) {
        for (const auto &objPtr : program->objects) {
            if (!objPtr)
                continue;
            for (const auto &ext : objPtr->external) {
                if (std::find(names.begin(), names.end(), std::make_pair(ext.mod, ext.name)) == names.end())
                    names.push_back(std::make_pair(ext.mod, ext.name));
            }
            collectObjectNames(names, objPtr);
        }
    }

    void Parser::printObjectHTML(std::ostream &out, const std::unique_ptr<Program> &objPtr) {
        out << R"(<div class="object-section">
                <div class="object-title">Object: <span style="color: lime;"><strong>)"
            << html_escape(objPtr->name) << R"(</span></strong></div>)";
        out << R"(
                    <div class="stats" style="margin-bottom:30px;">
                        <div class="stat-card">
                            <span class="number">)"
            << objPtr->vars.size() << R"(</span>
                            <span class="label">Variables</span>
                        </div>
                        <div class="stat-card">
                            <span class="number">)"
            << objPtr->inc.size() << R"(</span>
                            <span class="label">Instructions</span>
                        </div>
                        <div class="stat-card">
                            <span class="number">)"
            << objPtr->labels.size() << R"(</span>
                            <span class="label">Labels</span>
                        </div>
                        <div class="stat-card">
                            <span class="number">)"
            << objPtr->objects.size() << R"(</span>
                            <span class="label">Objects</span>
                        </div>
                    </div>
                )";
        out << R"(
                <div class="section">
                    <div class="section-header"><span class="icon">&#x1F9EE;</span> Variables</div>
                    <div class="section-content">)";
        if (objPtr->vars.empty()) {
            out << R"(<div class="no-data">No variables defined</div>)";
        } else {
            out << R"(<div class="variables-grid">)";
            for (const auto &var : objPtr->vars) {
                out << R"(<div class="variable-card">
                                <div class="variable-name">)"
                    << html_escape(var.first) << R"(</div>
                                <div class="variable-type">)";
                switch (var.second.type) {
                case VarType::VAR_INTEGER:
                    out << "Integer";
                    break;
                case VarType::VAR_FLOAT:
                    out << "Float";
                    break;
                case VarType::VAR_STRING:
                    out << "String";
                    break;
                case VarType::VAR_POINTER:
                    out << "Pointer";
                    break;
                case VarType::VAR_LABEL:
                    out << "Label";
                    break;
                case VarType::VAR_EXTERN:
                    out << "External";
                    break;
                case VarType::VAR_ARRAY:
                    out << "Array";
                    break;
                case VarType::VAR_BYTE:
                    out << "Byte";
                    break;
                default:
                    out << "Unknown";
                    break;
                }
                out << R"(</div>
                                <div class="variable-value">)";
                switch (var.second.type) {
                case VarType::VAR_INTEGER:
                case VarType::VAR_BYTE:
                    out << var.second.var_value.int_value;
                    break;
                case VarType::VAR_FLOAT:
                    out << std::fixed << std::setprecision(6) << var.second.var_value.float_value;
                    break;
                case VarType::VAR_STRING:
                    out << "\"" << html_escape(Program::escapeNewLines(var.second.var_value.str_value)) << "\"";
                    break;
                case VarType::VAR_POINTER:
                case VarType::VAR_EXTERN:
                    if (var.second.var_value.ptr_value == nullptr)
                        out << "null";
                    else
                        out << var.second.var_value.ptr_value;
                    break;
                case VarType::VAR_LABEL:
                    out << var.second.var_value.label_value;
                    break;
                default:
                    out << var.second.var_value.int_value;
                    break;
                }
                out << R"(</div>
                                </div>)";
            }
            out << R"(</div>)";
        }
        out << R"(</div>
                </div>)";
        out << R"(<div class="section">
                    <div class="section-header"><span class="icon">&#x1F516;</span> Labels</div>
                    <div class="section-content">)";
        if (objPtr->labels.empty()) {
            out << R"(<div class="no-data">No labels defined</div>)";
        } else {
            out << R"(<table class="instructions-table">
                        <thead>
                            <tr>
                                <th>Name</th>
                                <th>Address</th>
                                <th>Function</th>
                            </tr>
                        </thead>
                        <tbody>)";
            for (const auto &label : objPtr->labels) {
                out << R"(<tr>
                            <td>)"
                    << html_escape(label.first) << R"(</td>
                            <td>)"
                    << "0x" << std::hex << label.second.first << std::dec << R"(</td>
                            <td>)"
                    << (label.second.second ? "true" : "false") << R"(</td>
                        </tr>)";
            }
            out << R"(</tbody>
                    </table>)";
        }
        out << R"(</div>
                </div>)";
        out << R"(
                <div class="section">
                    <div class="section-header"><span class="icon">&#9881;</span> Instructions</div>
                    <div class="section-content">)";
        if (objPtr->inc.empty()) {
            out << R"(<div class="no-data">No instructions defined</div>)";
        } else {
            out << R"(<table class="instructions-table">
                            <thead>
                                <tr>
                                    <th>#</th>
                                    <th>Opcode</th>
                                    <th>Instruction</th>
                                    <th>Operand 1</th>
                                    <th>Operand 2</th>
                                    <th>Operand 3</th>
                                    <th>Extra Operands</th>
                                </tr>
                            </thead>
                            <tbody>)";
            for (size_t i = 0; i < objPtr->inc.size(); ++i) {
                const auto &instr = objPtr->inc[i];
                out << "<tr>"
                    << "<td>0x" << std::hex << std::uppercase << i << std::dec << "</td>"
                    << "<td class=\"opcode\">0x" << std::hex << std::uppercase << static_cast<int>(instr.instruction) << std::dec << "</td>"
                    << "<td class=\"opcode-name\">" << html_escape(IncType[static_cast<int>(instr.instruction)]) << "</td>"
                    << "<td class=\"operand\">" << html_escape(instr.op1.op) << "</td>"
                    << "<td class=\"operand\">" << html_escape(instr.op2.op) << "</td>"
                    << "<td class=\"operand\">" << html_escape(instr.op3.op) << "</td>"
                    << "<td class=\"operand\">";
                for (size_t j = 0; j < instr.vop.size(); ++j) {
                    if (j > 0)
                        out << ", ";
                    out << html_escape(instr.vop[j].op);
                }
                out << "</td></tr>";
            }
            out << R"(</tbody>
                        </table>)";
        }
        out << R"(</div>
                </div>)";
        if (!objPtr->objects.empty()) {
            out << R"(<div class="section">
                    <div class="section-header"><span class="icon">&#x1F9E9;</span> Nested Objects</div>
                    <div class="section-content">)";
            for (const auto &nestedObj : objPtr->objects) {
                if (nestedObj) {
                    printObjectHTML(out, nestedObj);
                }
            }
            out << R"(</div>
                </div>)";
        }
        out << R"(<div class="section">
    <div class="section-header"><span class="icon">&#x1F4DC;</span> Compiled Assembly</div>
    <div class="section-content">
        <textarea id="asm-)"
            << html_escape(objPtr->name) << R"(" readonly style="width:100%;height:300px;background:#222;color:#e0e0e0;border-radius:8px;padding:16px;font-family:'Fira Mono', 'Consolas', 'Courier New', monospace;font-size:1rem;">)";
        out << html_escape(objPtr->assembly_code);
        out << R"(</textarea>
        <div style="margin-top:10px;">)";
        out << "<button class=\"btn\" onclick=\"copyAsm('asm-" << js_escape(objPtr->name) << "')\">Copy</button>";
        out << "<button class=\"btn\" onclick=\"downloadAsm('asm-" << js_escape(objPtr->name) << "', '" << js_escape(objPtr->name) << ".s')\">Download</button>";
        out << R"(</div>
    </div>
</div>
)";
    }

    bool Parser::generateDebugHTML(std::ostream &out, std::unique_ptr<Program> &program) {
        out << R"(<!DOCTYPE html>
                <html lang="en">
                <head>
                    <meta charset="UTF-8">
                    <meta name="viewport" content="width=device-width, initial-scale=1.0">
                    <title>MXVM Debug Report - )"
            << html_escape(program->name) << R"(</title>
                </head>
                <body>
                    <style>
                        * {
                            margin: 0;
                            padding: 0;
                            box-sizing: border-box;
                        }
                        body {
                            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
                            background: #000;
                            color: #e0e0e0;
                            line-height: 1.6;
                            min-height: 100vh;
                        }
                        .container {
                            width: 100%;
                            max-width: 100vw;
                            margin: 0 auto;
                            padding: 20px;
                            background: transparent;
                        }
                        .header {
                            background: #222;
                            border-radius: 15px;
                            padding: 30px;
                            margin-bottom: 30px;
                            box-shadow: 0 8px 32px rgba(183, 28, 28, 0.2);
                            border: 1px solid #b71c1c;
                        }
                        .header h1 {
                            color: #ff5252;
                            font-size: 2.5rem;
                            margin-bottom: 10px;
                            font-weight: 700;
                            text-shadow: 1px 1px 2px #000;
                        }
                        .header .subtitle {
                            color: #e57373;
                            font-size: 1.2rem;
                            margin-bottom: 20px;
                        }
                        .stats {
                            display: flex;
                            gap: 20px;
                            flex-wrap: wrap;
                        }
                        .stat-card {
                            background: #b71c1c;
                            color: #fff;
                            padding: 15px 25px;
                            border-radius: 10px;
                            text-align: center;
                            min-width: 150px;
                            box-shadow: 0 4px 15px rgba(183, 28, 28, 0.3);
                        }
                        .stat-card .number {
                            font-size: 2rem;
                            font-weight: bold;
                            display: block;
                        }
                        .stat-card .label {
                            font-size: 0.9rem;
                            opacity: 0.9;
                        }
                        .section {
                            background: #181818;
                            border-radius: 15px;
                            margin-bottom: 30px;
                            box-shadow: 0 8px 32px rgba(183, 28, 28, 0.1);
                            border: 1px solid #b71c1c;
                            overflow: hidden;
                        }
                        .section-header {
                            background: #b71c1c;
                            color: #fff;
                            padding: 20px 30px;
                            font-size: 1.4rem;
                            font-weight: 600;
                            display: flex;
                            align-items: center;
                            gap: 10px;
                        }
                        .section-content {
                            padding: 30px;
                        }
                        .variables-grid {
                            display: grid;
                            gap: 15px;
                            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                        }
                        .variable-card {
                            background: #222;
                            border: 1px solid #b71c1c;
                            border-radius: 10px;
                            padding: 20px;
                            color: #fff;
                            transition: transform 0.2s, box-shadow 0.2s;
                        }
                        .variable-card:hover {
                            transform: translateY(-2px);
                            box-shadow: 0 4px 20px rgba(183, 28, 28, 0.2);
                        }
                        .variable-name {
                            font-weight: 600;
                            font-size: 1.1rem;
                            color: #ff5252;
                            margin-bottom: 8px;
                        }
                        .variable-type {
                            display: inline-block;
                            background: #b71c1c;
                            color: #fff;
                            padding: 4px 12px;
                            border-radius: 20px;
                            font-size: 0.8rem;
                            font-weight: 500;
                            margin-bottom: 10px;
                        }
                        .variable-value {
                            font-family: 'Courier New', monospace;
                            background: #181818;
                            border: 1px solid #b71c1c;
                            border-radius: 5px;
                            padding: 10px;
                            color: #e0e0e0;
                            word-break: break-all;
                        }
                        .instructions-table {
                            width: 100%;
                            border-collapse: collapse;
                            background: #222;
                            border-radius: 10px;
                            overflow: hidden;
                            box-shadow: 0 2px 10px rgba(183, 28, 28, 0.1);
                        }
                        .instructions-table th {
                            background: #b71c1c;
                            color: #fff;
                            padding: 15px;
                            text-align: left;
                            font-weight: 600;
                        }
                        .instructions-table td {
                            padding: 12px 15px;
                            border-bottom: 1px solid #b71c1c;
                            color: #e0e0e0;
                        }
                        .instructions-table tr:nth-child(even) {
                            background: #181818;
                        }
                        .instructions-table tr:hover {
                            background: #2d2d2d;
                        }
                        .opcode {
                            font-weight: 600;
                            color: #ff5252;
                        }
                        .operand {
                            font-family: 'Courier New', monospace;
                            color: #e0e0e0;
                        }
                        .object-section {
                            margin-left: 20px;
                            border-left: 3px solid #b71c1c;
                            padding-left: 20px;
                        }
                        .object-title {
                            font-size: 1.2rem;
                            font-weight: 600;
                            color: #ff5252;
                            margin-bottom: 15px;
                            padding: 10px 15px;
                            background: #222;
                            border-radius: 8px;
                            border-left: 4px solid #b71c1c;
                        }
                        .btn {
                            background: #b71c1c;
                            color: white;
                            border: none;
                            padding: 8px 16px;
                            border-radius: 5px;
                            cursor: pointer;
                            margin-right: 10px;
                            font-weight: 600;
                            transition: background 0.2s;
                        }
                        .btn:hover {
                            background: #ff5252;
                        }
                        .icon {
                            width: 20px;
                            height: 20px;
                            display: inline-block;
                        }
                        .no-data {
                            text-align: center;
                            color: #e57373;
                            font-style: italic;
                            padding: 40px;
                        }
                        @media (max-width: 768px) {
                            .container {
                                padding: 10px;
                            }
                            .header h1 {
                                font-size: 2rem;
                            }
                            .stats {
                                justify-content: center;
                            }
                            .variables-grid {
                                grid-template-columns: 1fr;
                            }
                            .instructions-table {
                                font-size: 0.9rem;
                            }
                        }
                    </style>
                )";

        bool mainHasContent = !program->vars.empty() || !program->inc.empty() || !program->labels.empty();
        bool hasObjects = !program->objects.empty();

        out << R"(
                    <div class="container">
                        <div class="header">
                            <h1>MXVM Debug Report</h1>
                            <div class="subtitle">Program: <span style="color: blue;"><strong>)"
            << html_escape(program->name) << R"(</span></strong></div>
                            <div class="stats">
                                <div class="stat-card">
                                    <span class="number">)"
            << program->vars.size() << R"(</span>
                                    <span class="label">Variables</span>
                                </div>
                                <div class="stat-card">
                                    <span class="number">)"
            << program->inc.size() << R"(</span>
                                    <span class="label">Instructions</span>
                                </div>
                                <div class="stat-card">
                                    <span class="number">)"
            << program->objects.size() << R"(</span>
                                    <span class="label">Objects</span>
                                </div>
                                <div class="stat-card">
                                    <span class="number">)"
            << program->labels.size() << R"(</span>
                                    <span class="label">Labels</span>
                                </div>
                            </div>
                        </div>)";

        if (mainHasContent) {
            out << R"(<div class="section">
                        <div class="section-header">
                            <span class="icon">&#x1F9EE;</span>
                            Variables
                        </div>
                        <div class="section-content">)";

            if (program->vars.empty()) {
                out << R"(<div class="no-data">No variables defined</div>)";
            } else {
                out << R"(<div class="variables-grid">)";
                for (const auto &var : program->vars) {
                    out << R"(<div class="variable-card">
                                <div class="variable-name">)"
                        << html_escape(var.first) << R"(</div>
                                <div class="variable-type">)";

                    switch (var.second.type) {
                    case VarType::VAR_INTEGER:
                        out << "Integer";
                        break;
                    case VarType::VAR_FLOAT:
                        out << "Float";
                        break;
                    case VarType::VAR_STRING:
                        out << "String";
                        break;
                    case VarType::VAR_POINTER:
                        out << "Pointer";
                        break;
                    case VarType::VAR_LABEL:
                        out << "Label";
                        break;
                    case VarType::VAR_EXTERN:
                        out << "External";
                        break;
                    case VarType::VAR_ARRAY:
                        out << "Array";
                        break;
                    case VarType::VAR_BYTE:
                        out << "Byte";
                        break;
                    default:
                        out << "Unknown";
                        break;
                    }

                    out << R"(</div>
                                <div class="variable-value">)";

                    switch (var.second.type) {
                    case VarType::VAR_INTEGER:
                    case VarType::VAR_BYTE:
                        out << var.second.var_value.int_value;
                        break;
                    case VarType::VAR_FLOAT:
                        out << std::fixed << std::setprecision(6) << var.second.var_value.float_value;
                        break;
                    case VarType::VAR_STRING:
                        out << "\"" << html_escape(Program::escapeNewLines(var.second.var_value.str_value)) << "\"";
                        break;
                    case VarType::VAR_POINTER:
                    case VarType::VAR_EXTERN:
                        if (var.second.var_value.ptr_value == nullptr)
                            out << "null";
                        else
                            out << var.second.var_value.ptr_value;
                        break;
                    case VarType::VAR_LABEL:
                        out << var.second.var_value.label_value;
                        break;
                    default:
                        out << var.second.var_value.int_value;
                        break;
                    }

                    out << R"(</div>
                            </div>)";
                }
                out << R"(</div>)";
            }
            out << R"(</div>
                    </div>)";

            out << R"(<div class="section">
                        <div class="section-header"><span class="icon">&#x1F516;</span> Labels</div>
                        <div class="section-content">)";
            if (program->labels.empty()) {
                out << R"(<div class="no-data">No labels defined</div>)";
            } else {
                out << R"(<table class="instructions-table">
                            <thead>
                                <tr>
                                    <th>Name</th>
                                    <th>Function</th>
                                </tr>
                            </thead>
                            <tbody>)";
                for (const auto &label : program->labels) {
                    out << "<tr><td>" << html_escape(label.first) << "</td><td>0x" << std::hex << label.second.first << std::dec << R"(</td>
                                <td>)"
                        << (label.second.second ? "true" : "false") << R"(</td>
                            </tr>)";
                }
                out << R"(</tbody>
                        </table>)";
            }
            out << R"(</div>
                    </div>)";

            out << R"(<div class="section">
                        <div class="section-header">
                            <span class="icon">&#9881;</span>
                            Instructions
                        </div>
                        <div class="section-content">)";

            if (program->inc.empty()) {
                out << R"(<div class="no-data">No instructions defined</div>)";
            } else {
                out << R"(<table class="instructions-table">
                            <thead>
                                <tr>
                                    <th>#</th>
                                    <th>Opcode</th>
                                    <th>Instruction</th>
                                    <th>Operand 1</th>
                                    <th>Operand 2</th>
                                    <th>Operand 3</th>
                                    <th>Extra Operands</th>
                                </tr>
                            </thead>
                            <tbody>)";

                for (size_t i = 0; i < program->inc.size(); ++i) {
                    const auto &instr = program->inc[i];
                    out << "<tr>"
                        << "<td>0x" << std::hex << std::uppercase << i << std::dec << "</td>"
                        << "<td class=\"opcode\">0x" << std::hex << std::uppercase << static_cast<int>(instr.instruction) << std::dec << "</td>"
                        << "<td class=\"opcode-name\">" << html_escape(IncType[static_cast<int>(instr.instruction)]) << "</td>"
                        << "<td class=\"operand\">" << html_escape(instr.op1.op) << "</td>"
                        << "<td class=\"operand\">" << html_escape(instr.op2.op) << "</td>"
                        << "<td class=\"operand\">" << html_escape(instr.op3.op) << "</td>"
                        << "<td class=\"operand\">";
                    for (size_t j = 0; j < instr.vop.size(); ++j) {
                        if (j > 0)
                            out << ", ";
                        out << html_escape(instr.vop[j].op);
                    }
                    out << "</td></tr>";
                }
                out << R"(</tbody>
                        </table>)";
            }
            out << R"(<div class="section">
                        <div class="section-header"><span class="icon">&#x1F4DC;</span> Compiled Assembly</div>
                        <div class="section-content">
                            <textarea id="asm-main" readonly style="width:100%;height:300px;background:#222;color:#e0e0e0;border-radius:8px;padding:16px;font-family:'Fira Mono', 'Consolas', 'Courier New', monospace;font-size:1rem;">)";
            out << html_escape(program->assembly_code);
            out << R"(</textarea>
                            <div style="margin-top:10px;">)";
            out << "<button class=\"btn\" onclick=\"copyAsm('asm-main')\">Copy</button>";
            out << "<button class=\"btn\" onclick=\"downloadAsm('asm-main', '" << js_escape(program->name) << ".s')\">Download</button>";
            out << R"(</div>
                        </div>
                    </div>
                    )";
        }

        if (hasObjects) {
            out << R"(<div class="section">
                        <div class="section-header">
                            <span class="icon">&#x1F4E6;</span>
                            Objects
                        </div>
                        <div class="section-content">)";

            for (const auto &obj : program->objects) {
                if (obj) {
                    printObjectHTML(out, obj);
                }
            }

            out << R"(</div>
                    </div>)";
        }

        out << R"(
                    </div>)";
        out << R"(
                <script>
                function copyAsm(id) {
                    var textarea = document.getElementById(id);
                    textarea.select();
                    navigator.clipboard.writeText(textarea.value);
                }
                function downloadAsm(id, filename) {
                    var textarea = document.getElementById(id);
                    var blob = new Blob([textarea.value], {type: 'text/plain'});
                    var link = document.createElement('a');
                    link.href = window.URL.createObjectURL(blob);
                    link.download = filename;
                    document.body.appendChild(link);
                    link.click();
                    document.body.removeChild(link);
                }
                </script>
                </body>
                </html>
                )";
        return true;
    }

    void Parser::processDataSection(SectionNode *sectionNode, std::unique_ptr<Program> &program) {
        for (const auto &statement : sectionNode->statements) {
            auto variableNode = dynamic_cast<VariableNode *>(statement.get());
            if (variableNode) {
                Variable var;
                var.type = variableNode->type;
                var.var_name = program->name + "." + variableNode->name;
                if (variableNode->hasInitializer) {
                    setVariableValue(var, variableNode->type, variableNode->initialValue);
                } else {
                    setDefaultVariableValue(var, variableNode->type);
                }
                var.var_value.buffer_size = variableNode->buffer_size;
                var.is_global = variableNode->is_global;
                var.obj_name = program->name;
                program->add_variable(var.var_name, var);
            }
        }
    }

    void Parser::processModuleSection(SectionNode *sectionNode, std::unique_ptr<Program> &program) {
        for (const auto &statement : sectionNode->statements) {
            auto moduleNode = dynamic_cast<ModuleNode *>(statement.get());
            if (moduleNode) {
                std::string &name = moduleNode->name;
                processModuleFile(name, program);
            }
        }
    }

    void Parser::processModuleFile(const std::string &src, std::unique_ptr<Program> &program) {
        std::string module_name = "libmxvm_" + src;
        if (!module_path.ends_with("/"))
            module_path += "/";
        if (!include_path.ends_with("/"))
            include_path += "/";
        std::string shared_ext = ".so";
#ifdef _WIN32
        shared_ext = ".dll";
#elif defined(__APPLE__)
        shared_ext = ".dylib";
#endif

        std::string module_path_so = module_path + "modules/" + src + "/" + module_name + shared_ext;
#ifdef _WIN32
        // On Windows, DLLs may not have the "lib" prefix (e.g. mxvm_sdl.dll vs libmxvm_sdl.dll)
        if (!std::filesystem::exists(module_path_so)) {
            std::string alt = module_path + "modules/" + src + "/mxvm_" + src + shared_ext;
            if (std::filesystem::exists(alt))
                module_path_so = alt;
        }
#endif
        std::string msys2prefix;
#ifdef _WIN32
        const char *msys2env = std::getenv("MSYSTEM_PREFIX");
        if (msys2env != nullptr) {
            msys2prefix = msys2env;
            size_t pos = msys2prefix.find("/msys64/");
            if (pos != std::string::npos) {
                msys2prefix = msys2prefix.substr(0, pos + 8);
            } else {
                msys2prefix = "";
            }
        }
#endif
        std::string module_src = msys2prefix + include_path + src + "/" + src + ".mxvm";
        std::fstream file;
        file.open(module_src, std::ios::in);
        if (!file.is_open()) {
            // Fall back to looking in the module directory
            module_src = module_path + "modules/" + src + "/" + src + ".mxvm";
            file.open(module_src, std::ios::in);
        }
        if (!file.is_open()) {
            throw mx::Exception("Error could not find module source file: " + module_src);
        }
        std::ostringstream data;
        data << file.rdbuf();
        ModuleParser mod_parser(this->parser_mode, src, data.str());
        if (mod_parser.scan() > 0) {
            if (mod_parser.parse() && mod_parser.generateProgramCode(this->parser_mode, src, module_path_so, program)) {
                return;
            } else {
                throw mx::Exception("Error parsing module file.\n");
            }
        }
    }

    void Parser::processCodeSection(SectionNode *sectionNode, std::unique_ptr<Program> &program) {
        std::unordered_map<std::string, size_t> labelMap;

        size_t instructionIndex = 0;
        for (const auto &statement : sectionNode->statements) {
            if (auto labelNode = dynamic_cast<LabelNode *>(statement.get())) {
                labelMap[labelNode->name] = instructionIndex;
                program->add_label(labelNode->name, instructionIndex, labelNode->function);
            } else if (dynamic_cast<InstructionNode *>(statement.get())) {
                instructionIndex++;
            }
        }
        for (const auto &statement : sectionNode->statements) {
            auto instructionNode = dynamic_cast<InstructionNode *>(statement.get());
            if (instructionNode) {
                Instruction instr;
                instr.instruction = instructionNode->instruction;
                if (instructionNode->operands.size() > 0) {
                    instr.op1 = instructionNode->operands[0];
                    resolveLabelReference(instr.op1, labelMap);
                }
                if (instructionNode->operands.size() > 1) {
                    instr.op2 = instructionNode->operands[1];
                    resolveLabelReference(instr.op2, labelMap);
                }
                if (instructionNode->operands.size() > 2) {
                    instr.op3 = instructionNode->operands[2];
                    resolveLabelReference(instr.op3, labelMap);
                }
                if (instructionNode->operands.size() > 3) {
                    for (size_t i = 3; i < instructionNode->operands.size(); ++i) {
                        Operand extraOp = instructionNode->operands[i];
                        resolveLabelReference(extraOp, labelMap);
                        instr.vop.push_back(extraOp);
                    }
                }
                program->add_instruction(instr);
            }
        }
    }

    void Parser::setVariableValue(Variable &var, VarType type, const std::string &value, size_t buf_size) {
        switch (type) {
        case VarType::VAR_BYTE:
        case VarType::VAR_INTEGER:
            if (value.starts_with("0x") || value.starts_with("0X")) {
                var.var_value.int_value = std::stoull(value, nullptr, 16);
            } else {
                var.var_value.int_value = std::stoull(value);
            }
            var.var_value.type = type;
            break;

        case VarType::VAR_FLOAT:
            var.var_value.float_value = std::stod(value);
            var.var_value.type = VarType::VAR_FLOAT;
            break;
        case VarType::VAR_STRING:
            var.var_value.str_value = value;
            var.var_value.type = VarType::VAR_STRING;
            var.var_value.buffer_size = buf_size;
            break;

        case VarType::VAR_POINTER:
            var.var_value.ptr_value = nullptr;
            if (value == "null" || value == "0") {
                var.var_value.ptr_value = nullptr;
                var.var_value.str_value = "null";
            }
            var.var_value.type = VarType::VAR_POINTER;
            break;

        case VarType::VAR_LABEL:
            var.var_value.label_value = value;
            var.var_value.type = VarType::VAR_LABEL;
            break;

        default:
            setDefaultVariableValue(var, type);
            break;
        }
    }

    void Parser::setDefaultVariableValue(Variable &var, VarType type) {
        switch (type) {
        case VarType::VAR_INTEGER:
            var.var_value.int_value = 0;
            var.var_value.type = VarType::VAR_INTEGER;
            break;

        case VarType::VAR_FLOAT:
            var.var_value.float_value = 0.0;
            var.var_value.type = VarType::VAR_FLOAT;
            break;

        case VarType::VAR_STRING:
            var.var_value.str_value = "";
            var.var_value.type = VarType::VAR_STRING;
            break;

        case VarType::VAR_POINTER:
            var.var_value.ptr_value = nullptr;
            var.var_value.type = VarType::VAR_POINTER;
            break;

        case VarType::VAR_LABEL:
            var.var_value.label_value = "";
            var.var_value.type = VarType::VAR_LABEL;
            break;

        default:
            var.var_value.type = VarType::VAR_NULL;
            break;
        }
    }

    void Parser::resolveLabelReference(Operand &operand, const std::unordered_map<std::string, size_t> &labelMap) {
        if (!operand.label.empty()) {
            auto it = labelMap.find(operand.label);
            if (it != labelMap.end()) {
                operand.op_value = static_cast<int>(it->second);
            }
        }
    }

    std::unique_ptr<ProgramNode> Parser::parseProgramOrObject(uint64_t &i) {
        auto program = std::make_unique<ProgramNode>();

        std::string tokenValue = this->operator[](i).getTokenValue();
        program->object = (tokenValue == "object");
        i++;

        if (i < scanner.size()) {
            auto nameToken = this->operator[](i);
            program->name = nameToken.getTokenValue();
            if (program->object == false)
                program->root_name = nameToken.getTokenValue();
            i++;
        }

        if (i < scanner.size() && this->operator[](i).getTokenValue() == "{") {
            i++;

            while (i < scanner.size()) {
                auto innerToken = this->operator[](i);
                std::string innerValue = innerToken.getTokenValue();

                if (innerValue == "}") {
                    i++;
                    break;
                }

                if (innerValue == "section") {
                    auto section = parseSection(i);
                    if (section) {
                        program->addSection(std::move(section));
                    }
                    continue;
                }
                i++;
            }
        }

        return program;
    }

    void Parser::generateObjectAssemblyFile(std::unique_ptr<Program> &objProgram) {
        std::string objectFileName = objProgram->name + ".s";
        std::ofstream objectFile(objectFileName);
        if (!objectFile.is_open()) {
            throw mx::Exception("Could not create object assembly file: " + objectFileName);
        }

        std::ostringstream code_v;
        objProgram->generateCode(platform, objProgram->object, code_v);
        objProgram->assembly_code = code_v.str();
        std::string opt_code = objProgram->gen_optimize(objProgram->assembly_code, platform);
        objProgram->assembly_code = opt_code;
        objectFile << opt_code;
        objectFile.close();
        if (debug_mode) {
            std::cout << "Generated object assembly file: " << objectFileName << std::endl;
        }
        if (Program::base != nullptr)
            Program::base->add_filename(objectFileName);
    }

    void Parser::registerObjectExterns(std::unique_ptr<Program> &mainProgram,
                                       const std::unique_ptr<Program> &objProgram) {
        for (const auto &[labelName, labelInfo] : objProgram->labels) {
            if (labelInfo.second && Program::base != nullptr)
                Program::base->add_extern(objProgram->name, labelName, false);
        }

        for (const auto &ext : objProgram->external) {
            if (Program::base != nullptr)
                Program::base->add_extern(ext.mod, ext.name, true);
        }

        for (const auto &child : objProgram->objects) {
            if (child)
                registerObjectExterns(mainProgram, child);
        }
    }
} // namespace mxvm