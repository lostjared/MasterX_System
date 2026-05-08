/**
 * @file valid.cpp
 * @brief MXVM instruction operand validator and variable/label checker
 * @author Jared Bruni
 */
#include "mxvm/valid.hpp"
#include "mxvm/instruct.hpp"
#include "scanner/exception.hpp"
#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace mxvm {

    static inline bool isImmediate(const OpKind k) {
        return k == OpKind::Num || k == OpKind::Hex || k == OpKind::Str;
    }

    static bool has_semicolon(const std::string &s) {
        bool in_str = false, escaped = false;
        for (size_t i = 0; i < s.size(); ++i) {
            char c = s[i];
            if (!in_str) {
                if (c == '#')
                    break;
                if (c == '/' && i + 1 < s.size() && s[i + 1] == '/')
                    break;
                if (c == '"') {
                    in_str = true;
                    continue;
                }
                if (c == ';')
                    return true;
            } else {
                if (escaped) {
                    escaped = false;
                    continue;
                }
                if (c == '\\') {
                    escaped = true;
                    continue;
                }
                if (c == '"') {
                    in_str = false;
                    continue;
                }
            }
        }
        return false;
    }

    void Validator::collect_objects(std::unordered_set<std::string> &objects, size_t start_index, size_t end_index) {
        bool in_object_section = false;
        int brace_depth = 0;

        for (size_t i = start_index; i <= end_index && i < scanner.size(); ++i) {
            const auto &tok = scanner[i];

            if (!in_object_section) {
                // Look for "section object {"
                if (tok.getTokenType() == types::TokenType::TT_ID &&
                    tok.getTokenValue() == "section" &&
                    i + 2 < scanner.size() &&
                    scanner[i + 1].getTokenValue() == "object" &&
                    scanner[i + 2].getTokenValue() == "{") {
                    in_object_section = true;
                    brace_depth = 1;
                    i += 2; // Skip "object" and "{"
                    continue;
                }
            } else {
                // Inside object section
                if (tok.getTokenValue() == "{") {
                    ++brace_depth;
                } else if (tok.getTokenValue() == "}") {
                    if (brace_depth <= 0) {
                        throw mx::Exception("Syntax Error in '" + filename + "': unexpected '}' in object section");
                    }
                    --brace_depth;
                    if (brace_depth == 0) {
                        in_object_section = false;
                    }
                } else if (tok.getTokenType() == types::TokenType::TT_ID && brace_depth == 1) {
                    // This is an object name at the top level of the object section
                    objects.insert(tok.getTokenValue());
                }
            }
        }
    }

    static const std::unordered_map<std::string, OpSpec> kOpSpecs = {
        {"mov", {"mov", {OpKind::Id, OpKind::Any}}},
        {"load", {"load", {OpKind::Id, OpKind::Id, OpKind::Any}, VArity::None, 3, 4}},
        {"store", {"store", {OpKind::Any, OpKind::Id, OpKind::Any}, VArity::None, 3, 4}},
        {"add", {"add", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"sub", {"sub", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"mul", {"mul", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"div", {"div", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"or", {"or", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"and", {"and", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"xor", {"xor", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"not", {"not", {OpKind::Id}}},
        {"neg", {"neg", {OpKind::Id}}},
        {"mod", {"mod", {OpKind::Id, OpKind::Any}, VArity::AnyTail, 2, 3}},
        {"cmp", {"cmp", {OpKind::Any, OpKind::Any}}},
        {"fcmp", {"fcmp", {OpKind::Any, OpKind::Any}}},
        {"jmp", {"jmp", {OpKind::Label}}},
        {"je", {"je", {OpKind::Label}}},
        {"jne", {"jne", {OpKind::Label}}},
        {"jl", {"jl", {OpKind::Label}}},
        {"jle", {"jle", {OpKind::Label}}},
        {"jg", {"jg", {OpKind::Label}}},
        {"jge", {"jge", {OpKind::Label}}},
        {"jz", {"jz", {OpKind::Label}}},
        {"jnz", {"jnz", {OpKind::Label}}},
        {"ja", {"ja", {OpKind::Label}}},
        {"jb", {"jb", {OpKind::Label}}},
        {"print", {"print", {OpKind::Any}, VArity::AnyTail, 1, -1}},
        {"string_print", {"string_print", {OpKind::Any}}},
        {"exit", {"exit", {}, VArity::AnyTail, 0, 1}},
        {"alloc", {"alloc", {OpKind::Id, OpKind::Any, OpKind::Any}}},
        {"realloc", {"realloc", {OpKind::Id, OpKind::Any, OpKind::Any}}},
        {"free", {"free", {OpKind::Id}}},
        {"lea", {"lea", {OpKind::Id, OpKind::Id}}},
        {"getline", {"getline", {OpKind::Id}}},
        {"push", {"push", {OpKind::Any}}},
        {"pop", {"pop", {OpKind::Id}}},
        {"stack_load", {"stack_load", {OpKind::Id, OpKind::Any}}},
        {"stack_store", {"stack_store", {OpKind::Any, OpKind::Any}}},
        {"stack_sub", {"stack_sub", {OpKind::Any}}},
        {"call", {"call", {OpKind::Label}}},
        {"ret", {"ret", {}}},
        {"done", {"done", {}}},
        {"to_int", {"to_int", {OpKind::Id, OpKind::Any}}},
        {"to_float", {"to_float", {OpKind::Id, OpKind::Any}}},
        {"invoke", {"invoke", {OpKind::Id}, VArity::ArgsTail}},
        {"return", {"return", {OpKind::Id}}}};

    static bool isIdLike(OpKind k) { return k == OpKind::Id || k == OpKind::Member; }

    ParsedOp Validator::parseOperand() {
        if (!token)
            throw mx::Exception("Syntax Error in '" + filename + "': Unexpected EOF parsing operand");
        ParsedOp n;
        if (match(types::TokenType::TT_ID)) {
            std::string a = token->getTokenValue();
            const scan::TToken *at = token;
            next();
            if (match(".")) {
                next();
                require(types::TokenType::TT_ID);
                a += "." + token->getTokenValue();
                n.kind = OpKind::Member;
                n.text = a;
                n.at = at;
                next();
                return n;
            }
            n.kind = OpKind::Id;
            n.text = a;
            n.at = at;
            return n;
        }
        if (match("-") && peekIs(types::TokenType::TT_NUM)) {
            next();
        }
        if (match(types::TokenType::TT_NUM)) {
            std::string num_str = token->getTokenValue();
            if (num_str.find('.') != std::string::npos) {
                throw mx::Exception("Syntax Error in '" + filename + "': Floating point constants must be declared as variables, not used directly in instructions at line " + std::to_string(token->getLine()));
            }
            n.kind = OpKind::Num;
            n.text = token->getTokenValue();
            n.at = token;
            next();
            return n;
        }
        if (match(types::TokenType::TT_HEX)) {
            n.kind = OpKind::Hex;
            n.text = token->getTokenValue();
            n.at = token;
            next();
            return n;
        }
        if (match(types::TokenType::TT_STR)) {
            n.kind = OpKind::Str;
            n.text = token->getTokenValue();
            n.at = token;
            next();
            return n;
        }
        throw mx::Exception("Syntax Error in '" + filename + "': invalid operand '" + token->getTokenValue() + "' at line " + std::to_string(token->getLine()));
    }

    std::vector<ParsedOp> Validator::parseOperandList() {
        std::vector<ParsedOp> out;
        while (token && !match("}")) {
            size_t operand_line = token->getLine();
            out.push_back(parseOperand());
            if (match(",")) {
                next();
                if (token && token->getLine() != operand_line) {
                    throw mx::Exception("Syntax Error in file '" + filename + "': Operands must be on the same line");
                }
                continue;
            }

            if (token && token->getLine() == operand_line &&
                !token->getTokenValue().empty()) {
                throw mx::Exception("Syntax Error in file '" + filename + "': Multiple items or missing comma between operands on line " +
                                    std::to_string(operand_line));
            }
            break;
        }
        return out;
    }

    void Validator::collect_labels(std::unordered_map<std::string, std::string> &labels) {
        for (size_t i = 0; i < scanner.size(); ++i) {
            const auto &tok = scanner[i];
            if (tok.getTokenType() == types::TokenType::TT_ID) {
                if (i + 1 < scanner.size() && scanner[i + 1].getTokenValue() == ":") {
                    labels[tok.getTokenValue()] = tok.getTokenValue();
                }
            }
        }
    }

    void Validator::validateAgainstSpec(
        const std::string &op,
        const std::vector<ParsedOp> &ops,
        const std::unordered_map<std::string, Variable> &vars,
        const std::unordered_map<std::string, std::string> &labels,
        const std::unordered_set<std::string> &objects,
        std::vector<UseVar> &usedVarsRef,
        std::vector<UseLabel> &usedLabelsRef) {
        auto it = kOpSpecs.find(op);
        if (it == kOpSpecs.end()) {
            throw mx::Exception("Syntax Error in '" + filename + "': Unknown instruction '" + op + "'");
        }
        const OpSpec &spec = it->second;

        int minArgs = (spec.minArgs >= 0) ? spec.minArgs : (int)spec.fixed.size();
        int maxArgs = (spec.maxArgs >= 0) ? spec.maxArgs : (spec.varPolicy == VArity::None ? (int)spec.fixed.size() : INT32_MAX);

        if ((int)ops.size() < minArgs || (int)ops.size() > maxArgs) {
            throw mx::Exception("Syntax Error in '" + filename + "': '" + op + "' expects " + std::to_string(minArgs) + ((maxArgs == INT32_MAX) ? "..inf" : ".." + std::to_string(maxArgs)) + " operands; found " + std::to_string(ops.size()));
        }

        auto kindOk = [&](OpKind want, OpKind got) {
            if (want == OpKind::Any)
                return true;
            if (want == OpKind::Label)
                return got == OpKind::Id || got == OpKind::Member || got == OpKind::Label;
            if (want == OpKind::Id)
                return got == OpKind::Id || got == OpKind::Member;
            return want == got;
        };

        for (size_t i = 0; i < spec.fixed.size() && i < ops.size(); ++i) {
            if (!kindOk(spec.fixed[i], ops[i].kind)) {
                throw mx::Exception("Syntax Error in '" + filename + "': '" + op + "' operand " + std::to_string((int)i + 1) + " has wrong kind");
            }
        }

        if (spec.varPolicy == VArity::ArgsTail) {
            for (size_t i = spec.fixed.size(); i < ops.size(); ++i) {
                if (!(isImmediate(ops[i].kind) || isIdLike(ops[i].kind))) {
                    throw mx::Exception("Syntax Error in '" + filename + "': '" + op + "' extra args must be Id/Imm");
                }
            }
        }

        auto pushVar = [&](const ParsedOp &p) {
            if (isIdLike(p.kind)) {
                if (p.kind == OpKind::Id && p.text.find('.') == std::string::npos) {
                    if (!vars.count(p.text)) {
                        std::string msg = "Syntax Error in '" + filename + "': Undefined variable '" + p.text + "'";
                        if (p.at) {
                            msg += " at line " + std::to_string(p.at->getLine());
                        }
                        throw mx::Exception(msg);
                    }
                }
                // NEW: Validate object references
                else if (p.kind == OpKind::Member && p.text.find('.') != std::string::npos) {
                    std::string objectName = p.text.substr(0, p.text.find('.'));
                    if (!objects.count(objectName)) {
                        std::string msg = "Syntax Error in '" + filename + "': Undefined object '" + objectName + "'";
                        if (p.at) {
                            msg += " at line " + std::to_string(p.at->getLine());
                        }
                        throw mx::Exception(msg);
                    }
                }
                usedVarsRef.push_back({p.text, p.at});
            }
        };
        auto pushLabel = [&](const ParsedOp &p) {
            if (p.text.find('.') == std::string::npos) {
                usedLabelsRef.push_back({p.text, p.at});
            }
        };

        if (op == "jmp" || op == "je" || op == "jne" || op == "jl" || op == "jle" || op == "jg" || op == "jge" || op == "jz" || op == "jnz" || op == "ja" || op == "jb") {
            if (!ops.empty())
                pushLabel(ops[0]);
        }
        if (op == "call") {
            if (!ops.empty())
                pushLabel(ops[0]);
        }

        if (op == "mov" || op == "pop" || op == "stack_load" || op == "alloc" || op == "getline" ||
            op == "return" || op == "not" || op == "neg" || op == "to_int" || op == "to_float") {
            if (!ops.empty() && isIdLike(ops[0].kind))
                pushVar(ops[0]);
        }

        if (op == "add" || op == "sub" || op == "mul" || op == "div" || op == "or" || op == "and" ||
            op == "xor" || op == "mod" || op == "cmp") {
            if (ops.size() >= 1 && isIdLike(ops[0].kind))
                pushVar(ops[0]);
            if (ops.size() >= 2 && isIdLike(ops[1].kind))
                pushVar(ops[1]);
            if (ops.size() >= 3 && isIdLike(ops[2].kind))
                pushVar(ops[2]);
        }

        if (op == "load") {
            if (ops.size() >= 1 && isIdLike(ops[0].kind))
                pushVar(ops[0]);
            if (ops.size() >= 2 && isIdLike(ops[1].kind))
                pushVar(ops[1]);
            if (ops.size() >= 3 && isIdLike(ops[2].kind))
                pushVar(ops[2]);
        }

        if (op == "store") {
            if (ops.size() >= 1 && isIdLike(ops[0].kind))
                pushVar(ops[0]);
            if (ops.size() >= 2 && isIdLike(ops[1].kind))
                pushVar(ops[1]);
            if (ops.size() >= 3 && isIdLike(ops[2].kind))
                pushVar(ops[2]);
        }

        if (op == "free") {
            if (!ops.empty() && isIdLike(ops[0].kind))
                pushVar(ops[0]);
        }

        if (op == "invoke") {
            for (size_t i = 1; i < ops.size(); ++i) {
                if (isIdLike(ops[i].kind))
                    pushVar(ops[i]);
            }
        }
        if (op == "print") {
            for (size_t i = 0; i < ops.size(); ++i) {
                if (isIdLike(ops[i].kind))
                    pushVar(ops[i]);
            }
        }
    }

    bool Validator::validate(const std::string &name) {
        filename = name;
        scanner.scan();
        next();

        while (token) {
            std::unordered_map<std::string, std::string> labels;
            std::unordered_set<std::string> objects;
            std::vector<UseVar> usedVars;
            std::vector<UseLabel> usedLabels;

            size_t block_start = index - 1;
            size_t block_end = block_start;

            int brace_count = 0;
            bool found_opening = false;
            for (size_t i = block_start; i < scanner.size(); ++i) {
                if (scanner[i].getTokenValue() == "{") {
                    found_opening = true;
                    brace_count++;
                } else if (scanner[i].getTokenValue() == "}") {
                    brace_count--;
                    if (found_opening && brace_count == 0) {
                        block_end = i;
                        break;
                    }
                }
            }

            collect_labels(labels);
            collect_objects(objects, block_start, block_end);

            std::vector<std::string> lines;
            {
                std::istringstream code_info(source);
                std::string l;
                while (std::getline(code_info, l)) {
                    lines.push_back(l);
                }
            }

            std::vector<std::pair<int, int>> code_ranges;
            {
                bool seen_section = false;
                bool section_is_code = false;
                bool in_code = false;
                int brace_depth = 0;
                int range_start = -1;
                for (size_t i = 0; i < scanner.size(); ++i) {
                    const auto &t = scanner[i];
                    if (!in_code) {
                        if (t.getTokenType() == types::TokenType::TT_ID && t.getTokenValue() == "section") {
                            seen_section = true;
                            section_is_code = false;
                            continue;
                        }
                        if (seen_section && t.getTokenType() == types::TokenType::TT_ID) {
                            section_is_code = (t.getTokenValue() == "code");
                            continue;
                        }
                        if (section_is_code && t.getTokenValue() == "{") {
                            in_code = true;
                            brace_depth = 1;
                            range_start = t.getLine();
                            seen_section = false;
                            section_is_code = false;
                            continue;
                        }
                    } else {
                        if (t.getTokenValue() == "{") {
                            ++brace_depth;
                        } else if (t.getTokenValue() == "}") {
                            --brace_depth;
                            if (brace_depth == 0) {
                                int range_end = t.getLine();
                                code_ranges.emplace_back(range_start, range_end);
                                in_code = false;
                            }
                        }
                    }
                }
            }

            for (const auto &pr : code_ranges) {
                const int start = std::max(1, pr.first);
                const int end = pr.second;
                for (int line = start; line <= end; ++line) {
                    const int idx = line - 1;
                    if (idx < 0 || idx >= static_cast<int>(lines.size()))
                        continue;
                    const std::string &text = lines[idx];
                    if (has_semicolon(text)) {
                        throw mx::Exception(
                            "Syntax Error in file '" + filename +
                            "': Semicolons are not allowed in code section at line " +
                            std::to_string(line) + ": '" + text + "'");
                    }
                }
            }

            std::unordered_map<int, int> line_instruction_count;
            auto is_in_code = [&](int line) -> bool {
                for (const auto &pr : code_ranges) {
                    if (line >= pr.first && line <= pr.second)
                        return true;
                }
                return false;
            };
            for (size_t i = 0; i < scanner.size(); ++i) {
                const auto &tok = scanner[i];
                const int line = tok.getLine();
                if (!is_in_code(line))
                    continue;
                if (tok.getTokenType() != types::TokenType::TT_ID)
                    continue;

                if (i + 1 < scanner.size() && scanner[i + 1].getTokenValue() == ":") {
                    continue;
                }
                if (tok.getTokenValue() == "function") {
                    continue;
                }
                if (std::find(IncType.begin(), IncType.end(), tok.getTokenValue()) != IncType.end()) {
                    line_instruction_count[line]++;
                }
            }
            for (const auto &kv : line_instruction_count) {
                const int line_num = kv.first;
                const int count = kv.second;
                if (count > 1) {
                    const int idx = line_num - 1;
                    const std::string line_text =
                        (idx >= 0 && idx < static_cast<int>(lines.size())) ? lines[idx] : std::string("<unknown>");
                    throw mx::Exception("Syntax Error in file '" + filename +
                                        "': Multiple instructions on same line at line " +
                                        std::to_string(line_num) + ": '" + line_text + "'");
                }
            }

            auto skipSeparators = [&]() {};

            skipSeparators();
            if (!token)
                break;

            if (match("program")) {
                next();
            } else {
                require("object");
                next();
            }

            skipSeparators();
            require(types::TokenType::TT_ID);
            next();
            skipSeparators();
            require("{");
            next();

            std::unordered_map<std::string, Variable> vars;
            for (auto &n : {"stdout", "stdin", "stderr"}) {
                vars[n] = Variable();
                vars[n].var_name = n;
            }

            skipSeparators();
            while (token && !match("}")) {
                skipSeparators();
                require("section");
                next();
                skipSeparators();
                require(types::TokenType::TT_ID);
                std::string sectionName = token->getTokenValue();
                next();
                skipSeparators();
                require("{");
                next();

                if (sectionName == "module" || sectionName == "object") {
                    skipSeparators();
                    while (token && !match("}")) {
                        skipSeparators();
                        if (match(types::TokenType::TT_ID)) {
                            next();
                            skipSeparators();
                            if (match(",")) {
                                next();
                                continue;
                            }
                            continue;
                        }
                        break;
                    }
                    skipSeparators();
                    require("}");
                    next();
                } else if (sectionName == "data") {
                    skipSeparators();
                    while (token && !match("}")) {
                        skipSeparators();
                        if (match(types::TokenType::TT_ID) &&
                            (token->getTokenValue() == "int" ||
                             token->getTokenValue() == "string" ||
                             token->getTokenValue() == "float" ||
                             token->getTokenValue() == "ptr" ||
                             token->getTokenValue() == "byte" ||
                             token->getTokenValue() == "export")) {
                            if (token->getTokenValue() == "export")
                                next();

                            std::string vtype = token->getTokenValue();
                            next();
                            skipSeparators();

                            require(types::TokenType::TT_ID);
                            std::string vname = token->getTokenValue();
                            vars[vname].var_name = vname;
                            next();
                            skipSeparators();

                            if (match(",") && vtype == "string") {
                                next();
                                skipSeparators();
                                if (match(types::TokenType::TT_NUM) || match(types::TokenType::TT_HEX)) {
                                    next();
                                    skipSeparators();
                                    continue;
                                } else {
                                    throw mx::Exception("Syntax Error in file '" + filename + "': string buffer requires number on line " + std::to_string(token->getLine()));
                                }
                            }

                            require("=");
                            next();
                            skipSeparators();
                            if (match("-") && match(types::TokenType::TT_SYM)) {
                                next();
                                skipSeparators();
                            }

                            if (vtype == "byte") {
                                if (!(match(types::TokenType::TT_NUM) || match(types::TokenType::TT_HEX))) {
                                    throw mx::Exception("Syntax Error in file '" + filename + "': byte must be a valid byte value integer 0-255 on line " + std::to_string(token->getLine()));
                                }
                                int64_t value = std::stoll(token->getTokenValue(), nullptr, 0);
                                if (value < 0 || value > 0xFF) {
                                    throw mx::Exception("Syntax Error in file '" + filename + "': byte out of range 0-255 on line: " + std::to_string(token->getLine()));
                                }
                                next();
                                skipSeparators();
                            } else if (vtype == "string") {
                                require(types::TokenType::TT_STR);
                                next();
                                skipSeparators();
                            } else if (token->getTokenValue() == "null" ||
                                       match(types::TokenType::TT_NUM) ||
                                       match(types::TokenType::TT_HEX) ||
                                       match(types::TokenType::TT_STR)) {
                                next();
                                skipSeparators();
                            } else {
                                throw mx::Exception("Syntax Error in file '" + filename + "': Expected value for variable, found: " + token->getTokenValue() + " at line " + std::to_string(token->getLine()));
                            }
                        } else {
                            throw mx::Exception("Syntax Error in file '" + filename + "': Expected variable declaration, found: " + token->getTokenValue() + " at line " + std::to_string(token->getLine()));
                        }
                    }
                    skipSeparators();
                    require("}");
                    next();
                } else if (sectionName == "code") {
                    skipSeparators();

                    while (token && !match("}")) {
                        size_t old_index = index;
                        skipSeparators();

                        if (token && token->getTokenValue() == ";") {
                            throw mx::Exception("Syntax Error in file '" + filename +
                                                "': Semicolons not allowed in code section at line " +
                                                std::to_string(token->getLine()));
                        }

                        if (match(types::TokenType::TT_ID) && token->getTokenValue() == "function") {
                            next();
                            skipSeparators();
                            require(types::TokenType::TT_ID);
                            next();
                            skipSeparators();
                            require(":");
                            next();
                            continue;
                        }

                        if (match(types::TokenType::TT_ID) && peekIs(":")) {
                            next();
                            next();
                            continue;
                        }

                        if (match(types::TokenType::TT_ID)) {
                            std::string op = token->getTokenValue();
                            if (std::find(IncType.begin(), IncType.end(), op) == IncType.end()) {
                                throw mx::Exception("Syntax Error in file '" + filename + "': Unknown instruction '" + op + "' at line " + std::to_string(token->getLine()));
                            }
                            next();
                            skipSeparators();

                            if (op == "ret" || op == "done") {
                                std::vector<ParsedOp> emptyOps;
                                validateAgainstSpec(op, emptyOps, vars, labels, objects, usedVars, usedLabels); // Add objects
                                continue;
                            }

                            auto ops = parseOperandList();
                            validateAgainstSpec(op, ops, vars, labels, objects, usedVars, usedLabels); // Add objects
                            continue;
                        } else {
                            throw mx::Exception("Syntax Error in file '" + filename + "': Unexpected token '" + token->getTokenValue() + "' in code section at line " + std::to_string(token->getLine()));
                        }

                        if (old_index == index) {
                            if (!next())
                                break;
                        }
                    }

                    skipSeparators();
                    require("}");
                    next();
                } else {
                    throw mx::Exception("Syntax Error in file '" + filename + "': Unknown section: " + sectionName + " at line " + std::to_string(token->getLine()));
                }
                skipSeparators();
            }

            skipSeparators();
            require("}");
            next(); // Consume the closing brace of the object/program

            for (auto &u : usedLabels) {
                if (!labels.count(u.name)) {
                    throw mx::Exception("Syntax Error in '" + filename + "': Undefined label '" + u.name + "' at line " + std::to_string(u.at->getLine()));
                }
            }
            skipSeparators(); // Prepare for the next object/program or EOF
        }
        return true;
    }

    bool Validator::match(const std::string &m) {
        if (!token)
            return false;
        if (token->getTokenValue() != m)
            return false;
        return true;
    }

    void Validator::require(const std::string &r) {
        if (!token) {
            throw mx::Exception("Syntax Error in '" + filename + "': Required: " + r + " but reached end of file");
        }
        if (r != token->getTokenValue())
            throw mx::Exception(
                "Syntax Error in '" + filename + "': Required: " + r +
                " Found: " + token->getTokenValue() +
                " at line " + std::to_string(token->getLine()));
    }

    bool Validator::match(const types::TokenType &t) {
        if (!token || index >= scanner.size())
            return false;
        if (t != token->getTokenType())
            return false;
        return true;
    }

    void Validator::require(const types::TokenType &t) {
        if (!token || index >= scanner.size()) {
            throw mx::Exception("Syntax Error in '" + filename + "': unexpected EOF");
        }
        if (t != token->getTokenType())
            throw mx::Exception(
                "Syntax Error in '" + filename + "': Required: " + tokenTypeToString(t) +
                " instead found: " + token->getTokenValue() +
                ":" + tokenTypeToString(token->getTokenType()) +
                " at line " + std::to_string(token->getLine()));
    }

    bool Validator::next() {
        while (index < scanner.size() &&
               scanner[index].getTokenValue() == "\n" &&
               scanner[index].getTokenType() == types::TokenType::TT_SYM) {
            index++;
        }
        if (index < scanner.size()) {
            token = &scanner[index++];
            return true;
        }
        token = nullptr;
        return false;
    }

    bool Validator::peekIs(const std::string &s) {
        return index < scanner.size() && scanner[index].getTokenValue() == s;
    }

    bool Validator::peekIs(const types::TokenType &t) {
        return index < scanner.size() && scanner[index].getTokenType() == t;
    }

    std::string Validator::tokenTypeToString(types::TokenType t) {
        switch (t) {
        case types::TokenType::TT_ID:
            return "Identifier";
        case types::TokenType::TT_NUM:
            return "Number";
        case types::TokenType::TT_HEX:
            return "Hex";
        case types::TokenType::TT_STR:
            return "String";
        case types::TokenType::TT_SYM:
            return "Symbol";
        default:
            return "";
        }
    }

    Validator::Validator(const std::string &source_) : scanner(source_), source(source_) {
    }
} // namespace mxvm