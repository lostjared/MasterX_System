/**
 * @file validator.cpp
 * @brief Pascal semantic validator — scope tracking, type checking, and declaration verification
 * @author Jared Bruni
 */
#include "validator.hpp"
#include "scanner/exception.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <unordered_set>

namespace mxx {

    TPValidator::TPValidator(const std::string &source_) : scanner(source_), source(source_) {}

    bool TPValidator::validate(const std::string &name) {
        filename = name;
        scanner.scan();

        { // strip { } comments
            auto &toks = scanner.getTokens();
            for (size_t i = 0; i < toks.size();) {
                if (toks[i].getTokenValue() == "{") {
                    size_t start = i;
                    ++i;
                    while (i < toks.size() && toks[i].getTokenValue() != "}") {
                        ++i;
                    }
                    if (i < toks.size()) {
                        ++i;
                    }
                    toks.erase(toks.begin() + static_cast<int64_t>(start),
                               toks.begin() + static_cast<int64_t>(i));
                    i = start;
                } else {
                    ++i;
                }
            }
        }

        index = 0;
        token = nullptr;
        scopeStack.clear();
        declaredProcs.clear();
        declaredFuncs.clear();
        importedUnits.clear();
        pushScope();
        next();
        if (isKW("unit"))
            parseUnit();
        else
            parseProgram();
        if (token)
            failHere("Unexpected tokens after end of program");
        popScope();
        return true;
    }

    /**
     * @brief Case-insensitive string equality comparison
     * @param a First string
     * @param b Second string
     * @return true if @p a and @p b are equal ignoring ASCII case
     */
    static bool ciEquals(const std::string &a, const std::string &b) {
        if (a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i)
            if (std::tolower(static_cast<unsigned char>(a[i])) !=
                std::tolower(static_cast<unsigned char>(b[i])))
                return false;
        return true;
    }

    bool TPValidator::match(const std::string &s) const {
        return token && ciEquals(token->getTokenValue(), s);
    }

    bool TPValidator::match(types::TokenType t) const {
        return token && token->getTokenType() == t;
    }

    bool TPValidator::next() {
        while (index < scanner.size() &&
               scanner[index].getTokenType() == types::TokenType::TT_SYM &&
               scanner[index].getTokenValue() == "\n") {
            ++index;
        }
        if (index < scanner.size()) {
            token = &scanner[index++];
            return true;
        }
        token = nullptr;
        return false;
    }

    bool TPValidator::peekIs(const std::string &s) {
        return index < scanner.size() && ciEquals(scanner[index].getTokenValue(), s);
    }

    void TPValidator::require(const std::string &s) {
        if (!match(s)) {
            if (token)
                failAt(token, "Required: " + s + " Found: " + found());
            fail("Required: " + s + " Found: EOF");
        }
    }

    void TPValidator::require(types::TokenType t) {
        if (!match(t)) {
            if (token)
                failAt(token, "Required: " + tokenTypeToString(t) + " Found: " + found());
            fail("Required: " + tokenTypeToString(t) + " Found: EOF");
        }
    }

    void TPValidator::requireKW(const std::string &k) {
        if (!isKW(k)) {
            if (token)
                failAt(token, "Required: " + k + " Found: " + found());
            fail("Required: " + k + " Found: EOF");
        }
    }

    void TPValidator::fail(const std::string &msg) {
        throw mx::Exception("Syntax Error in '" + filename + "': " + msg);
    }

    void TPValidator::failHere(const std::string &msg) {
        if (token)
            throw mx::Exception("Syntax Error in '" + filename + "': " + msg + " at line " + std::to_string(token->getLine()));
        throw mx::Exception("Syntax Error in '" + filename + "': " + msg);
    }

    void TPValidator::failAt(const scan::TToken *at, const std::string &msg) {
        if (at)
            throw mx::Exception("Syntax Error in '" + filename + "': " + msg + " at line " + std::to_string(at->getLine()));
        throw mx::Exception("Syntax Error in '" + filename + "': " + msg);
    }

    std::string TPValidator::tokenTypeToString(types::TokenType t) {
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
            return "Unknown";
        }
    }

    std::string TPValidator::lower(std::string s) {
        for (auto &c : s)
            c = (char)std::tolower((unsigned char)c);
        return s;
    }

    std::string TPValidator::found() const {
        if (!token)
            return "EOF";
        return token->getTokenValue() + ":" + tokenTypeToString(token->getTokenType());
    }

    bool TPValidator::isKW(const std::string &k) const {
        if (!token || !isPascalKeyword(k) || token->getTokenType() != types::TokenType::TT_ID)
            return false;
        return lower(token->getTokenValue()) == k;
    }

    void TPValidator::pushScope() {
        scopeStack.emplace_back();
    }

    void TPValidator::popScope() {
        if (!scopeStack.empty()) {
            scopeStack.pop_back();
        }
    }

    void TPValidator::declareConst(const std::string &name, const scan::TToken *at) {
        auto key = lower(name);
        if (scopeStack.back().consts.count(key)) {
            failAt(at, "Redeclaration of constant '" + name + "' in the same scope");
        }
        scopeStack.back().consts.insert(key);
    }

    bool TPValidator::isBuiltinConst(const std::string &name) const {
        std::string key = lower(name);
        return key == "true" || key == "false" || key == "nil";
    }

    void TPValidator::checkVar(const std::string &name, const scan::TToken *at) {
        auto key = lower(name);

        if (key == "result" && inFunctionDepth > 0)
            return;

        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if (it->vars.count(key))
                return;
        }

        if (withDepth > 0)
            return;

        if (!declaredFuncs.count(key) && !declaredProcs.count(key) && !isBuiltinConst(name) && !isPascalKeyword(key) && !importedUnits.count(key)) {
            failAt(at, "Use of undeclared identifier '" + name + "'");
        }
    }

    void TPValidator::checkVarOrConst(const std::string &name, const scan::TToken *at) {
        auto key = lower(name);

        if (key == "result" && inFunctionDepth > 0)
            return;

        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if (it->vars.count(key) || it->consts.count(key))
                return;
        }

        if (withDepth > 0)
            return;

        if (!declaredFuncs.count(key) && !declaredProcs.count(key) && !isBuiltinConst(name) && !isPascalKeyword(key) && !importedUnits.count(key)) {
            failAt(at, "Use of undeclared identifier '" + name + "'");
        }
    }

    void TPValidator::checkConstOnly(const std::string &name, const scan::TToken *at) {
        auto key = lower(name);
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if (it->consts.count(key))
                return;
        }
        if (!isBuiltinConst(name)) {
            failAt(at, "Constant expression requires constant '" + name + "'");
        }
    }

    void TPValidator::parseProgram() {
        requireKW("program");
        next();
        require(types::TokenType::TT_ID);
        next();
        require(";");
        next();
        if (isKW("uses"))
            parseUses();
        parseBlock();
        require(".");
        next();
    }

    void TPValidator::parseUnit() {
        requireKW("unit");
        next();
        require(types::TokenType::TT_ID);
        next();
        require(";");
        next();

        // interface section
        requireKW("interface");
        next();
        if (isKW("uses"))
            parseUses();
        parseInterfaceSection();

        // implementation section
        requireKW("implementation");
        next();
        if (isKW("uses"))
            parseUses();

        // parse implementation declarations (const, type, var, procedure, function)
        while (isKW("const") || isKW("type") || isKW("var") || isKW("procedure") || isKW("function")) {
            if (isKW("const"))
                parseConstSection();
            else if (isKW("type"))
                parseTypeSection();
            else if (isKW("var"))
                parseVarSection();
            else if (isKW("procedure") || isKW("function"))
                parseSubprogram();
        }

        requireKW("end");
        next();
        require(".");
        next();
    }

    void TPValidator::parseInterfaceSection() {
        while (isKW("procedure") || isKW("function") || isKW("type") || isKW("const") || isKW("var")) {
            if (isKW("procedure")) {
                next();
                require(types::TokenType::TT_ID);
                declaredProcs.insert(lower(token->getTokenValue()));
                next();
                pushScope();
                if (match("("))
                    parseFormalParams();
                require(";");
                next();
                popScope();
            } else if (isKW("function")) {
                next();
                require(types::TokenType::TT_ID);
                declaredFuncs.insert(lower(token->getTokenValue()));
                next();
                pushScope();
                if (match("("))
                    parseFormalParams();
                require(":");
                next();
                parseType("");
                require(";");
                next();
                popScope();
            } else if (isKW("type")) {
                parseTypeSection();
            } else if (isKW("const")) {
                parseConstSection();
            } else if (isKW("var")) {
                parseVarSection();
            }
        }
    }

    void TPValidator::parseUses() {
        requireKW("uses");
        next();
        require(types::TokenType::TT_ID);
        std::vector<std::string> usedUnits;
        usedUnits.push_back(token->getTokenValue());
        next();
        while (match(",")) {
            next();
            require(types::TokenType::TT_ID);
            usedUnits.push_back(token->getTokenValue());
            next();
        }
        require(";");
        next();

        // Import interface declarations from used units
        static const std::unordered_set<std::string> nativeModules = {"io", "std", "string", "sdl", "strlib"};
        std::string inputDir;
        {
            auto pos = filename.find_last_of("/\\");
            inputDir = (pos != std::string::npos) ? filename.substr(0, pos + 1) : "./";
        }
        for (const auto &unit : usedUnits) {
            importedUnits.insert(lower(unit));
            if (nativeModules.count(unit))
                continue;
            // Try to find the unit's .pas file
            std::string unitFile = inputDir + unit + ".pas";
            std::ifstream uf(unitFile);
            if (!uf.is_open()) {
                std::string lowerUnit = unit;
                std::transform(lowerUnit.begin(), lowerUnit.end(), lowerUnit.begin(), ::tolower);
                unitFile = inputDir + lowerUnit + ".pas";
                uf.open(unitFile);
            }
            if (!uf.is_open())
                continue;
            std::ostringstream buf;
            buf << uf.rdbuf();
            uf.close();
            try {
                // Use a scanner to extract interface declarations
                scan::Scanner unitScanner(buf.str());
                unitScanner.scan();
                auto &toks = unitScanner.getTokens();
                // Strip brace comments
                for (size_t i = 0; i < toks.size();) {
                    if (toks[i].getTokenValue() == "{") {
                        size_t start = i;
                        ++i;
                        while (i < toks.size() && toks[i].getTokenValue() != "}") ++i;
                        if (i < toks.size()) ++i;
                        toks.erase(toks.begin() + static_cast<int64_t>(start),
                                   toks.begin() + static_cast<int64_t>(i));
                        i = start;
                    } else {
                        ++i;
                    }
                }
                // Find 'interface' keyword, then collect procedure/function names until 'implementation'
                size_t ti = 0;
                auto tlower = [](const std::string &s) {
                    std::string r = s;
                    std::transform(r.begin(), r.end(), r.begin(), ::tolower);
                    return r;
                };
                // Helper to skip newline tokens in the raw token stream
                auto skipNL = [&]() {
                    while (ti < toks.size() && toks[ti].getTokenType() == types::TokenType::TT_SYM
                           && toks[ti].getTokenValue() == "\n") ++ti;
                };
                // Skip to 'interface'
                while (ti < toks.size() && tlower(toks[ti].getTokenValue()) != "interface") ++ti;
                if (ti < toks.size()) ++ti; // move past 'interface'
                skipNL();
                // Skip optional uses in interface
                if (ti < toks.size() && tlower(toks[ti].getTokenValue()) == "uses") {
                    while (ti < toks.size() && toks[ti].getTokenValue() != ";") ++ti;
                    if (ti < toks.size()) ++ti;
                    skipNL();
                }
                // Collect procedure/function/const/var/type names until 'implementation'
                while (ti < toks.size() && tlower(toks[ti].getTokenValue()) != "implementation") {
                    skipNL();
                    if (ti >= toks.size() || tlower(toks[ti].getTokenValue()) == "implementation") break;
                    std::string kw = tlower(toks[ti].getTokenValue());
                    if (kw == "procedure" && ti + 1 < toks.size()) {
                        ++ti; skipNL();
                        declaredProcs.insert(lower(toks[ti].getTokenValue()));
                        while (ti < toks.size() && toks[ti].getTokenValue() != ";") ++ti;
                        if (ti < toks.size()) ++ti;
                        skipNL();
                    } else if (kw == "function" && ti + 1 < toks.size()) {
                        ++ti; skipNL();
                        declaredFuncs.insert(lower(toks[ti].getTokenValue()));
                        while (ti < toks.size() && toks[ti].getTokenValue() != ";") ++ti;
                        if (ti < toks.size()) ++ti;
                        skipNL();
                    } else if (kw == "const") {
                        ++ti; skipNL(); // skip 'const'
                        // Collect constant names: name = expr ;
                        while (ti < toks.size() && tlower(toks[ti].getTokenValue()) != "implementation"
                               && toks[ti].getTokenType() == types::TokenType::TT_ID
                               && tlower(toks[ti].getTokenValue()) != "var"
                               && tlower(toks[ti].getTokenValue()) != "type"
                               && tlower(toks[ti].getTokenValue()) != "procedure"
                               && tlower(toks[ti].getTokenValue()) != "function") {
                            scopeStack.back().consts.insert(lower(toks[ti].getTokenValue()));
                            // Skip past = expr ;
                            while (ti < toks.size() && toks[ti].getTokenValue() != ";") ++ti;
                            if (ti < toks.size()) ++ti;
                            skipNL();
                        }
                    } else if (kw == "var") {
                        ++ti; skipNL(); // skip 'var'
                        // Collect variable names: name [, name]* : type ;
                        while (ti < toks.size() && tlower(toks[ti].getTokenValue()) != "implementation"
                               && toks[ti].getTokenType() == types::TokenType::TT_ID
                               && tlower(toks[ti].getTokenValue()) != "const"
                               && tlower(toks[ti].getTokenValue()) != "type"
                               && tlower(toks[ti].getTokenValue()) != "procedure"
                               && tlower(toks[ti].getTokenValue()) != "function") {
                            scopeStack.back().vars.insert(lower(toks[ti].getTokenValue()));
                            ++ti;
                            while (ti < toks.size() && toks[ti].getTokenValue() == ",") {
                                ++ti; // skip ','
                                if (ti < toks.size()) {
                                    scopeStack.back().vars.insert(lower(toks[ti].getTokenValue()));
                                    ++ti;
                                }
                            }
                            // Skip past : type ;
                            while (ti < toks.size() && toks[ti].getTokenValue() != ";") ++ti;
                            if (ti < toks.size()) ++ti;
                            skipNL();
                        }
                    } else if (kw == "type") {
                        ++ti; skipNL(); // skip 'type'
                        // Collect type names: name = typedecl ;
                        while (ti < toks.size() && tlower(toks[ti].getTokenValue()) != "implementation"
                               && toks[ti].getTokenType() == types::TokenType::TT_ID
                               && tlower(toks[ti].getTokenValue()) != "const"
                               && tlower(toks[ti].getTokenValue()) != "var"
                               && tlower(toks[ti].getTokenValue()) != "procedure"
                               && tlower(toks[ti].getTokenValue()) != "function") {
                            scopeStack.back().types.insert(lower(toks[ti].getTokenValue()));
                            // Skip past = typedecl ;
                            while (ti < toks.size() && toks[ti].getTokenValue() != ";") ++ti;
                            if (ti < toks.size()) ++ti;
                            skipNL();
                        }
                    } else {
                        ++ti; skipNL();
                    }
                }
            } catch (...) {
                // Unit parsing failed; skip importing
            }
        }
    }

    void TPValidator::parseBlock() {
        if (isKW("label"))
            parseLabelSection();
        while (isKW("const") || isKW("type") || isKW("var") || isKW("procedure") || isKW("function") || isKW("label")) {
            if (isKW("label"))
                parseLabelSection();
            else if (isKW("const"))
                parseConstSection();
            else if (isKW("type"))
                parseTypeSection();
            else if (isKW("var"))
                parseVarSection();
            else if (isKW("procedure") || isKW("function"))
                parseSubprogram();
        }
        parseCompoundStatement();
    }

    void TPValidator::parseLabelSection() {
        requireKW("label");
        next();
        require(types::TokenType::TT_NUM);
        next();
        while (match(",")) {
            next();
            require(types::TokenType::TT_NUM);
            next();
        }
        require(";");
        next();
    }

    void TPValidator::parseConstSection() {
        requireKW("const");
        next();
        while (match(types::TokenType::TT_ID)) {
            if (isKW("type") || isKW("var") || isKW("label") ||
                isKW("begin") || isKW("procedure") || isKW("function") ||
                isKW("implementation") || isKW("end") || isKW("interface")) {
                break;
            }
            const auto *nameTok = token;
            std::string name = token->getTokenValue();
            next();
            require("=");
            next();
            parseConstExpr({";"}, true);
            declareConst(name, nameTok);
            require(";");
            next();
        }
    }

    void TPValidator::parseTypeSection() {
        requireKW("type");
        next();

        while (match(types::TokenType::TT_ID)) {
            if (isKW("procedure") || isKW("function") || isKW("var") || isKW("begin") || isKW("const") ||
                isKW("implementation") || isKW("end") || isKW("interface")) {
                break;
            }

            const auto *nameTok = token;
            std::string name = token->getTokenValue();
            next();
            require("=");
            next();

            parseType(name);

            declareType(name, nameTok);
            require(";");
            next();
        }
    }

    void TPValidator::parseVarSection() {
        requireKW("var");
        next();
        while (match(types::TokenType::TT_ID)) {
            if (isKW("procedure") || isKW("function") || isKW("begin") ||
                isKW("type") || isKW("const") || isKW("label") ||
                isKW("implementation") || isKW("end") || isKW("interface")) {
                break;
            }
            declareVar(token->getTokenValue(), token);
            next();
            while (match(",")) {
                next();
                require(types::TokenType::TT_ID);
                declareVar(token->getTokenValue(), token);
                next();
            }
            require(":");
            next();
            parseType("");
            if (match("=")) {
                next();
                parseConstExpr({";"}, false);
            }
            require(";");
            next();
        }
    }
    void TPValidator::parseSubprogram() {
        bool isProc = isKW("procedure");
        if (isProc) {
            requireKW("procedure");
            next();
            require(types::TokenType::TT_ID);
            declaredProcs.insert(lower(token->getTokenValue()));
            next();
        } else {
            requireKW("function");
            next();
            require(types::TokenType::TT_ID);
            declaredFuncs.insert(lower(token->getTokenValue()));
            next();
        }

        pushScope();

        if (match("("))
            parseFormalParams();

        if (isProc) {
            require(";");
            next();
        } else {
            require(":");
            next();
            parseType("");
            require(";");
            next();
        }

        if (isKW("forward")) {
            next();
            require(";");
            next();
            popScope();
            return;
        }

        if (!isProc)
            ++inFunctionDepth;
        parseBlock();
        if (!isProc)
            --inFunctionDepth;
        require(";");
        next();
        popScope();
    }

    void TPValidator::parseFormalParams() {
        require("(");
        next();
        if (match(")")) {
            next();
            return;
        }
        while (true) {
            bool byRef = isKW("var");
            if (byRef)
                next();
            parseIdentList();
            require(":");
            next();
            parseType("");
            if (match(";")) {
                next();
                continue;
            }
            require(")");
            next();
            break;
        }
    }

    void TPValidator::parseIdentList() {
        require(types::TokenType::TT_ID);
        declareVar(token->getTokenValue(), token);
        next();
        while (match(",")) {
            next();
            require(types::TokenType::TT_ID);
            declareVar(token->getTokenValue(), token);
            next();
        }
    }

    void TPValidator::parseType(const std::string &typeName) {
        if (isKW("packed")) {
            next(); // consume 'packed' modifier (accepted but has no effect)
        }
        if (isKW("array")) {
            next();
            // Dynamic array: array of <type> (no bounds)
            if (isKW("of")) {
                next();
                parseType("");
                return;
            }
            // Static array: array[lo..hi] of <type>
            require("[");
            next();
            parseSubrange();
            while (match(",")) {
                next();
                parseSubrange();
            }
            require("]");
            next();
            requireKW("of");
            next();
            parseType("");
            return;
        }
        if (isKW("record")) {
            next();
            pushRecordFieldScope(typeName);
            while (!isKW("end") && !isKW("case")) {
                parseFieldIdentList();
                require(":");
                next();
                parseType("");
                require(";");
                next();
            }
            if (isKW("case")) {
                next();
                require(types::TokenType::TT_ID);
                next();
                require(":");
                next();
                parseType("");
                requireKW("of");
                next();
                while (!isKW("end")) {
                    parseConstExpr({":", ","}, false);
                    while (match(",")) {
                        next();
                        parseConstExpr({":", ","}, false);
                    }
                    require(":");
                    next();
                    require("(");
                    next();
                    while (!match(")")) {
                        require(types::TokenType::TT_ID);
                        next();
                        while (match(",")) {
                            next();
                            require(types::TokenType::TT_ID);
                            next();
                        }
                        require(":");
                        next();
                        parseType("");
                        if (match(";"))
                            next();
                    }
                    require(")");
                    next();
                    if (match(";"))
                        next();
                }
            }
            requireKW("end");
            next();
            popRecordFieldScope();
            return;
        }
        if (isKW("set")) {
            next();
            requireKW("of");
            next();
            parseTypeName();
            return;
        }
        if (isKW("file")) {
            next();
            if (isKW("of")) {
                next();
                parseTypeName();
            }
            return;
        }
        if (match("(")) {
            next();
            while (match(types::TokenType::TT_ID)) {
                declareConst(token->getTokenValue(), token);
                next();
                if (match(","))
                    next();
            }
            require(")");
            next();
            return;
        }
        if (match("^")) {
            next();
            parseTypeName();
            return;
        }
        if (isKW("string")) {
            next();
            if (match("[")) {
                next();
                parseConstExpr({"]"}, true);
                require("]");
                next();
            }
            return;
        }
        if (isBuiltinType()) {
            next();
            return;
        }
        parseTypeName();
    }

    void TPValidator::parseTypeName() {
        require(types::TokenType::TT_ID);
        std::string name = token->getTokenValue();
        checkType(name, token);
        next();
    }

    void TPValidator::parseSubrange() {
        parseConstExpr({"..", "]", ","}, true);
        if (match("..")) {
            next();
            parseConstExpr({"]", ","}, true);
        }
    }

    void TPValidator::parseConstant() {
        if (match("-")) {
            next();
            parseConstSimple();
            return;
        }
        parseConstSimple();
    }

    void TPValidator::parseConstSimple() {
        if (match(types::TokenType::TT_NUM) || match(types::TokenType::TT_HEX) || match(types::TokenType::TT_STR)) {
            next();
            return;
        }
        if (isKW("true") || isKW("false") || isKW("nil")) {
            next();
            return;
        }
        require(types::TokenType::TT_ID);
        checkConstOnly(token->getTokenValue(), token);
        next();
    }

    void TPValidator::parseCompoundStatement() {
        requireKW("begin");
        next();

        if (isKW("end")) {
            requireKW("end");
            next();
            return;
        }

        while (!isKW("end")) {
            if (token == nullptr) {
                fail("Unexpected end of file inside compound statement");
            }

            parseStatement();

            if (match(";")) {
                next();

                if (isKW("end")) {
                    break;
                }
            } else if (isKW("end")) {

                break;
            } else {

                failHere("Expected ';' or 'end'");
            }
        }

        requireKW("end");
        next();
    }

    void TPValidator::parseStatement() {
        if (!token)
            failHere("Unexpected EOF in statement");
        if (isKW("begin")) {
            parseCompoundStatement();
            return;
        }
        if (isKW("if")) {
            parseIf();
            return;
        }
        if (isKW("while")) {
            parseWhile();
            return;
        }
        if (isKW("repeat")) {
            parseRepeat();
            return;
        }
        if (isKW("for")) {
            parseFor();
            return;
        }
        if (isKW("case")) {
            parseCase();
            return;
        }
        if (isKW("with")) {
            parseWith();
            return;
        }
        if (isKW("goto")) {
            parseGoto();
            return;
        }
        if (match(types::TokenType::TT_NUM)) {
            // Label prefix: 100: statement
            next();
            if (match(":")) {
                next();
                parseStatement();
                return;
            }
            failHere("Statement cannot start with number (expected ':' for label)");
        }
        if (match(types::TokenType::TT_ID)) {
            parseSimpleOrCallOrAssign();
            return;
        }
        if (match(";") || match(")"))
            return;
        if (isKW("end"))
            return;
        failHere("Invalid statement start");
    }

    void TPValidator::parseIf() {
        requireKW("if");
        next();
        parseExprStop({"then"});
        requireKW("then");
        next();
        parseStatement();
        if (isKW("else")) {
            next();
            parseStatement();
        }
    }

    void TPValidator::parseWhile() {
        requireKW("while");
        next();
        parseExprStop({"do"});
        requireKW("do");
        next();
        parseStatement();
    }

    void TPValidator::parseRepeat() {
        requireKW("repeat");
        next();
        while (true) {
            if (isKW("until")) {
                break;
            }
            parseStatement();
            if (match(";")) {
                next();
                if (isKW("until")) {
                    break;
                }
            } else if (isKW("until")) {
                break;
            } else {
                failHere("Expected ';' or 'until'");
            }
        }
        requireKW("until");
        next();
        parseExprStop({";", "end"});
    }

    void TPValidator::parseFor() {
        requireKW("for");
        next();
        require(types::TokenType::TT_ID);
        checkVar(token->getTokenValue(), token);
        next();
        require(":=");
        next();
        parseExprStop({"to", "downto"});
        if (isKW("to"))
            next();
        else {
            requireKW("downto");
            next();
        }
        parseExprStop({"do"});
        requireKW("do");
        next();
        parseStatement();
    }

    void TPValidator::parseCase() {
        requireKW("case");
        next();
        parseExprStop({"of"});
        requireKW("of");
        next();
        while (!isKW("end")) {
            parseCaseLabelList();
            require(":");
            next();
            parseStatement();
            if (match(";"))
                next();
            if (isKW("else")) {
                next();
                parseStatement();
                if (match(";"))
                    next();
                break;
            }
            if (isKW("end"))
                break;
        }
        requireKW("end");
        next();
    }

    void TPValidator::parseCaseLabelList() {
        parseCaseLabel();
        while (match(",")) {
            next();
            parseCaseLabel();
        }
    }

    void TPValidator::parseCaseLabel() {
        if (match("-"))
            next();
        if (match(types::TokenType::TT_NUM) || match(types::TokenType::TT_STR) || match(types::TokenType::TT_ID)) {
            const auto *at = token;
            next();
            if (match("..")) {
                next();
                if (match("-"))
                    next();
                if (match(types::TokenType::TT_NUM) || match(types::TokenType::TT_STR) || match(types::TokenType::TT_ID)) {
                    next();
                    return;
                }
                failAt(at, "Invalid case label range");
            }
            return;
        }
        failHere("Invalid case label");
    }

    void TPValidator::parseWith() {
        requireKW("with");
        next();
        parseDesignator();
        while (match(",")) {
            next();
            parseDesignator();
        }
        requireKW("do");
        next();
        ++withDepth;
        parseStatement();
        --withDepth;
    }

    void TPValidator::parseGoto() {
        requireKW("goto");
        next();
        require(types::TokenType::TT_NUM);
        next();
    }

    void TPValidator::parseSimpleOrCallOrAssign() {
        if (match(types::TokenType::TT_ID) && peekIs("(")) {
            next();
            parseActualParams();
            return;
        }
        parseDesignator();
        if (match(":=")) {
            next();
            parseExprStop({";", "end", "else", ")", "]", "until", "of", "do", "then"});
            return;
        }
        if (match("(")) {
            parseActualParams();
            return;
        }
    }

    void TPValidator::parseDesignator() {
        require(types::TokenType::TT_ID);
        checkVar(token->getTokenValue(), token);
        next();
        while (true) {
            if (match(".")) {
                next();
                require(types::TokenType::TT_ID);
                next();
                continue;
            }
            if (match("[")) {
                next();
                parseExprStop({"]"});
                while (match(",")) {
                    next();
                    parseExprStop({"]"});
                }
                require("]");
                next();
                continue;
            }
            if (match("^")) {
                next();
                continue;
            }
            break;
        }
    }

    void TPValidator::parseActualParams() {
        require("(");
        next();
        if (match(")")) {
            next();
            return;
        }
        while (true) {
            if (isKW("var"))
                next();
            parseExprStop({")", ";", ","});
            if (match(",")) {
                next();
                continue;
            }
            require(")");
            next();
            break;
        }
    }

    void TPValidator::parseExprStop(const std::unordered_set<std::string> &stops) {
        int paren = 0, bracket = 0;
        bool expectOperand = true;
        while (token) {
            if (paren == 0 && bracket == 0 && token->getTokenType() == types::TokenType::TT_ID) {
                if (stops.count(lower(token->getTokenValue())))
                    return;
            }
            if (paren == 0 && bracket == 0 && token->getTokenType() == types::TokenType::TT_SYM) {
                if (stops.count(token->getTokenValue()))
                    return;
            }

            if (match("(")) {
                ++paren;
                next();
                expectOperand = true;
                continue;
            }
            if (match(")")) {
                if (paren <= 0)
                    break;
                --paren;
                next();
                expectOperand = false;
                continue;
            }
            if (match("[")) {
                ++bracket;
                next();
                expectOperand = true;
                continue;
            }
            if (match("]")) {
                if (bracket <= 0)
                    break;
                --bracket;
                next();
                expectOperand = false;
                continue;
            }

            if (expectOperand) {
                if (match("+") || match("-") || isKW("not") || match("@")) {
                    next();
                    continue;
                }
                if (match(types::TokenType::TT_NUM) || match(types::TokenType::TT_HEX) || match(types::TokenType::TT_STR)) {
                    next();
                    expectOperand = false;
                    continue;
                }
                if (match(types::TokenType::TT_ID)) {
                    const auto *at = token;
                    std::string name = token->getTokenValue();
                    next();
                    if (match("(")) {
                        parseActualParams();
                        expectOperand = false;

                    } else {
                        checkVarOrConst(name, at);
                        expectOperand = false;
                    }

                    while (true) {
                        if (match("^")) {
                            next();
                            continue;
                        }
                        if (match(".")) {
                            next();
                            require(types::TokenType::TT_ID);
                            next();
                            continue;
                        }
                        if (match("[")) {
                            next();
                            parseExprStop({"]"});
                            while (match(",")) {
                                next();
                                parseExprStop({"]"});
                            }
                            require("]");
                            next();
                            continue;
                        }
                        if (match("(")) {
                            parseActualParams();
                            continue;
                        }
                        break;
                    }
                    continue;
                }
                failHere("Invalid expression");
            } else {
                if (bracket > 0 && match(",")) {
                    next();
                    expectOperand = true;
                    continue;
                }
                if (isRelOp() || isAddOp() || isMulOp() || isSetOp()) {
                    next();
                    expectOperand = true;
                    continue;
                }
                return;
            }
        }
    }

    void TPValidator::parseConstExpr(const std::unordered_set<std::string> &stops, bool constOnly) {
        int paren = 0;
        bool expectOperand = true;
        while (token) {
            if (paren == 0) {
                if (token->getTokenType() == types::TokenType::TT_ID && stops.count(lower(token->getTokenValue())))
                    return;
                if (token->getTokenType() == types::TokenType::TT_SYM && stops.count(token->getTokenValue()))
                    return;
            }
            if (match("(")) {
                ++paren;
                next();
                expectOperand = true;
                continue;
            }
            if (match(")")) {
                if (paren <= 0)
                    break;
                --paren;
                next();
                expectOperand = false;
                continue;
            }
            if (expectOperand) {
                if (match("+") || match("-") || isKW("not")) {
                    next();
                    continue;
                }
                if (match(types::TokenType::TT_NUM) || match(types::TokenType::TT_HEX) || match(types::TokenType::TT_STR)) {
                    next();
                    expectOperand = false;
                    continue;
                }
                if (match(types::TokenType::TT_ID)) {
                    const auto *at = token;
                    if (constOnly)
                        checkConstOnly(token->getTokenValue(), token);
                    next();
                    if (match("("))
                        failAt(at, "Function call not allowed in constant expression");
                    expectOperand = false;
                    continue;
                }
                failHere("Invalid constant expression");
            } else {
                if (match("+") || match("-") || match("*") || match("/") ||
                    isKW("div") || isKW("mod") || isKW("and") || isKW("or") || isKW("xor") ||
                    match("<") || match("<=") || match(">") || match(">=") || match("=") || match("<>")) {
                    next();
                    expectOperand = true;
                    continue;
                }
                return;
            }
        }
    }

    bool TPValidator::isRelOp() const { return isKW("in") || match("=") || match("<>") || match("<") || match("<=") || match(">") || match(">="); }
    bool TPValidator::isAddOp() const { return match("+") || match("-") || isKW("or") || isKW("xor"); }
    bool TPValidator::isMulOp() const { return match("*") || match("/") || isKW("div") || isKW("mod") || isKW("and") || isKW("shl") || isKW("shr"); }
    bool TPValidator::isSetOp() const { return isKW("union") || isKW("exclude") || isKW("symdiff"); }

    bool TPValidator::isBuiltinType() const {
        return isKW("integer") || isKW("real") || isKW("boolean") || isKW("char") ||
               isKW("byte") || isKW("word") || isKW("longint") || isKW("shortint") ||
               isKW("smallint") || isKW("cardinal") || isKW("string") || isKW("text") ||
               isKW("pointer");
    }

    void TPValidator::checkType(const std::string &name, const scan::TToken *at) {
        auto key = lower(name);
        for (auto it = scopeStack.rbegin(); it != scopeStack.rend(); ++it) {
            if (it->types.count(key))
                return;
        }
        if (!isBuiltinType()) {
            failAt(at, "Unknown type identifier '" + name + "'");
        }
    }

    mxx::Scope *TPValidator::currentScope() {
        if (scopeStack.empty())
            return nullptr;
        return &scopeStack.back();
    }

    const mxx::Scope *TPValidator::currentScope() const {
        if (scopeStack.empty())
            return nullptr;
        return &scopeStack.back();
    }

    static const std::unordered_set<std::string> pascal_keywords = {
        "program", "unit", "interface", "implementation",
        "uses", "var", "const", "type", "procedure", "function", "begin", "end",
        "if", "then", "else", "while", "do", "for", "to", "downto", "repeat", "until",
        "case", "of", "with", "goto", "label", "exit", "break", "continue",
        "nil", "new", "dispose", "setlength", "high", "low",
        "writeln", "write", "readln", "read", "seed_random", "rand_number",

        "div", "mod", "and", "or", "not", "in",
        "integer", "real", "boolean", "char", "byte", "word", "longint", "shortint",
        "smallint", "cardinal", "string", "text", "double", "single", "extended",
        "comp", "currency", "ptr", "pointer", "array", "record", "set",
        "packed", "file", "text",
        "assign", "reset", "rewrite", "append", "close", "eof", "eoln",
        "include", "exclude"};

    bool TPValidator::isPascalKeyword(const std::string &s) const {
        return pascal_keywords.count(lower(s));
    }

    bool TPValidator::isVarDeclaredHere(const std::string &name) const {
        return currentScope() && currentScope()->vars.count(lower(name));
    }
    bool TPValidator::isTypeDeclaredHere(const std::string &name) const {
        return currentScope() && currentScope()->types.count(lower(name));
    }
    bool TPValidator::isFuncDeclaredHere(const std::string &name) const {
        return currentScope() && currentScope()->funcs.count(lower(name));
    }
    bool TPValidator::isProcDeclaredHere(const std::string &name) const {
        return currentScope() && currentScope()->procs.count(lower(name));
    }
    bool TPValidator::isParamDeclaredHere(const std::string &name) const {
        return currentScope() && currentScope()->params.count(lower(name));
    }

    void TPValidator::declareVar(const std::string &name, const scan::TToken *at) {
        std::string key = lower(name);
        if (isVarDeclaredHere(key) || isParamDeclaredHere(key) ||
            isTypeDeclaredHere(key) || isFuncDeclaredHere(key) || isProcDeclaredHere(key)) {
            failAt(at, "Redeclaration of variable '" + name + "' in this scope");
        }
        currentScope()->vars.insert(key);
    }

    void TPValidator::declareFunc(const std::string &name, const scan::TToken *at) {
        std::string key = lower(name);
        if (isFuncDeclaredHere(key) || isVarDeclaredHere(key) ||
            isTypeDeclaredHere(key) || isProcDeclaredHere(key) || isParamDeclaredHere(key)) {
            failAt(at, "Redeclaration of function '" + name + "' in this scope");
        }
        currentScope()->funcs.insert(key);
    }

    void TPValidator::declareProc(const std::string &name, const scan::TToken *at) {
        std::string key = lower(name);
        if (isProcDeclaredHere(key) || isVarDeclaredHere(key) ||
            isTypeDeclaredHere(key) || isFuncDeclaredHere(key) || isParamDeclaredHere(key)) {
            failAt(at, "Redeclaration of procedure '" + name + "' in this scope");
        }
        currentScope()->procs.insert(key);
    }

    void TPValidator::declareParam(const std::string &name, const scan::TToken *at) {
        std::string key = lower(name);
        if (isParamDeclaredHere(key) || isVarDeclaredHere(key) ||
            isTypeDeclaredHere(key) || isFuncDeclaredHere(key) || isProcDeclaredHere(key)) {
            failAt(at, "Redeclaration of parameter '" + name + "' in this scope");
        }
        currentScope()->params.insert(key);
    }

    void TPValidator::declareType(const std::string &name, const scan::TToken *at) {
        auto key = lower(name);
        if (isTypeDeclaredHere(key) || isVarDeclaredHere(key) ||
            isFuncDeclaredHere(key) || isProcDeclaredHere(key) || isParamDeclaredHere(key)) {
            failAt(at, "Redeclaration of type '" + name + "' in this scope");
        }
        currentScope()->types.insert(key);
    }

    void TPValidator::pushRecordFieldScope(const std::string &recordTypeName) {
        currentRecordTypeName = recordTypeName;

        if (recordFieldScopesByType.find(recordTypeName) == recordFieldScopesByType.end()) {
            recordFieldScopesByType[recordTypeName] = std::unordered_set<std::string>();
        }
    }

    void TPValidator::popRecordFieldScope() {
        currentRecordTypeName.clear();
    }

    bool TPValidator::inRecordFieldScope() const {
        return !currentRecordTypeName.empty();
    }

    void TPValidator::declareRecordField(const std::string &name, const scan::TToken *at) {
        if (!inRecordFieldScope()) {
            declareVar(name, at);
            return;
        }

        std::string key = lower(name);
        auto &fields = recordFieldScopesByType[currentRecordTypeName];

        if (fields.count(key)) {
            failAt(at, "Redeclaration of field '" + name + "' in this record");
        }

        fields.insert(key);
    }

    void TPValidator::parseFieldIdentList() {
        require(types::TokenType::TT_ID);
        declareRecordField(token->getTokenValue(), token);
        next();
        while (match(",")) {
            next();
            require(types::TokenType::TT_ID);
            declareRecordField(token->getTokenValue(), token);
            next();
        }
    }

} // namespace mxx
