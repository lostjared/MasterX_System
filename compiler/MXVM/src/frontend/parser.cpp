/**
 * @file parser.cpp
 * @brief Pascal recursive-descent parser implementation
 * @author Jared Bruni
 */
#include "parser.hpp"
#include <algorithm>
#include <cctype>

namespace pascal {

    void PascalParser::removeBraceComments() {
        auto &toks = scanner.getTokens();
        for (size_t i = 0; i < toks.size();) {
            if (toks[i].getTokenValue() == "{") {
                size_t start = i;
                ++i;
                while (i < toks.size() && toks[i].getTokenValue() != "}") {
                    ++i;
                }
                if (i < toks.size()) {
                    ++i; // skip closing }
                }
                toks.erase(toks.begin() + static_cast<int64_t>(start),
                           toks.begin() + static_cast<int64_t>(i));
                i = start;
            } else {
                ++i;
            }
        }
    }

    void PascalParser::error(const std::string &message) {
        throw ParseException("Parse error: " + message + (token ? " at '" + token->getTokenValue() + "'" : " at end of input"));
    }

    void PascalParser::expectToken(const std::string &expected) {
        if (!peekIs(expected))
            error("Expected '" + expected + "'" + " on Line: " + std::to_string(token->getLine()));
    }

    void PascalParser::expectToken(types::TokenType expected) {
        if (!peekIs(expected))
            error("Expected " + tokenTypeToString(expected));
    }

    std::unique_ptr<ProgramNode> PascalParser::parseProgram() {
        expectToken("program");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string programName = token->getTokenValue();
        next();
        expectToken(";");
        next();
        std::vector<std::string> usesList;
        if (peekIs("uses")) {
            next();
            expectToken(types::TokenType::TT_ID);
            usesList.push_back(token->getTokenValue());
            next();
            while (peekIs(",")) {
                next();
                expectToken(types::TokenType::TT_ID);
                usesList.push_back(token->getTokenValue());
                next();
            }
            expectToken(";");
            next();
        }
        auto block = parseBlock();
        expectToken(".");
        auto programNode = std::make_unique<ProgramNode>(programName, std::move(block));
        programNode->uses = std::move(usesList);
        programNode->setLineNumber(lineNum);
        return programNode;
    }

    bool PascalParser::match(const std::string &s) {
        if (peekIs(s)) {
            next();
            return true;
        }
        return false;
    }

    bool PascalParser::isUnitSource() const {
        if (!token)
            return false;
        std::string val = token->getTokenValue();
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return val == "unit";
    }

    std::unique_ptr<UnitNode> PascalParser::parseUnit() {
        expectToken("unit");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string unitName = token->getTokenValue();
        next();
        expectToken(";");
        next();

        auto unitNode = std::make_unique<UnitNode>(unitName);
        unitNode->setLineNumber(lineNum);

        // interface section
        expectToken("interface");
        next();

        // optional uses clause in interface
        std::vector<std::string> usesList;
        if (peekIs("uses")) {
            next();
            expectToken(types::TokenType::TT_ID);
            usesList.push_back(token->getTokenValue());
            next();
            while (peekIs(",")) {
                next();
                expectToken(types::TokenType::TT_ID);
                usesList.push_back(token->getTokenValue());
                next();
            }
            expectToken(";");
            next();
        }
        unitNode->uses = std::move(usesList);

        // parse interface declarations (forward proc/func signatures)
        unitNode->interfaceDecls = parseInterfaceDeclarations();

        // implementation section
        expectToken("implementation");
        next();

        // optional uses clause in implementation (additional modules)
        if (peekIs("uses")) {
            next();
            expectToken(types::TokenType::TT_ID);
            unitNode->uses.push_back(token->getTokenValue());
            next();
            while (peekIs(",")) {
                next();
                expectToken(types::TokenType::TT_ID);
                unitNode->uses.push_back(token->getTokenValue());
                next();
            }
            expectToken(";");
            next();
        }

        // parse implementation declarations (full proc/func with bodies, vars, consts, types)
        unitNode->implDecls = parseDeclarations();

        // end.
        expectToken("end");
        next();
        expectToken(".");
        return unitNode;
    }

    std::vector<std::unique_ptr<ASTNode>> PascalParser::parseInterfaceDeclarations() {
        std::vector<std::unique_ptr<ASTNode>> decls;
        while (peekIs("procedure") || peekIs("function") || peekIs("type") || peekIs("const") || peekIs("var")) {
            if (peekIs("procedure")) {
                decls.push_back(parseProcedureForwardDecl());
            } else if (peekIs("function")) {
                decls.push_back(parseFunctionForwardDecl());
            } else if (peekIs("type")) {
                decls.push_back(parseTypeDeclaration());
            } else if (peekIs("const")) {
                decls.push_back(parseConstDeclaration());
            } else if (peekIs("var")) {
                if (match("var")) {
                    while (peekIs(types::TokenType::TT_ID) && !isKeyword(token->getTokenValue())) {
                        auto decl = parseVarDeclaration();
                        decls.push_back(std::move(decl));
                    }
                }
            }
        }
        return decls;
    }

    std::unique_ptr<ASTNode> PascalParser::parseProcedureForwardDecl() {
        expectToken("procedure");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string procName = token->getTokenValue();
        next();
        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (peekIs("(")) {
            next();
            parameters = parseParameterList();
            expectToken(")");
            next();
        }
        expectToken(";");
        next();
        auto procDeclNode = std::make_unique<ProcDeclNode>(procName, std::move(parameters), nullptr);
        procDeclNode->setLineNumber(lineNum);
        return procDeclNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseFunctionForwardDecl() {
        expectToken("function");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string funcName = token->getTokenValue();
        next();
        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (peekIs("(")) {
            next();
            parameters = parseParameterList();
            expectToken(")");
            next();
        }
        expectToken(":");
        next();
        std::string returnType;
        if (peekIs("^")) {
            next();
            expectToken(types::TokenType::TT_ID);
            returnType = "^" + token->getTokenValue();
            next();
        } else {
            expectToken(types::TokenType::TT_ID);
            returnType = token->getTokenValue();
            next();
        }
        expectToken(";");
        next();
        auto funcDeclNode = std::make_unique<FuncDeclNode>(funcName, std::move(parameters), returnType, nullptr);
        funcDeclNode->setLineNumber(lineNum);
        return funcDeclNode;
    }

    bool PascalParser::match(types::TokenType t) {
        if (peekIs(t)) {
            next();
            return true;
        }
        return false;
    }

    std::unique_ptr<BlockNode> PascalParser::parseBlock() {
        int lineNum = token ? token->getLine() : 1;
        if (peekIs("label")) {
            parseLabelDeclaration();
        }
        auto declarations = parseDeclarations();
        auto compoundStatement = parseCompoundStatement();
        auto blockNode = std::make_unique<BlockNode>(std::move(declarations), std::move(compoundStatement));
        blockNode->setLineNumber(lineNum);
        return blockNode;
    }

    bool PascalParser::isKeyword(const std::string &tok) {
        std::string lower = tok;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower == "program" || lower == "var" || lower == "begin" || lower == "end" ||
               lower == "if" || lower == "then" || lower == "else" || lower == "while" ||
               lower == "do" || lower == "for" || lower == "to" || lower == "downto" ||
               lower == "procedure" || lower == "function" || lower == "integer" ||
               lower == "real" || lower == "boolean" || lower == "string" || lower == "char" ||
               lower == "true" || lower == "false" || lower == "div" || lower == "mod" ||
               lower == "and" || lower == "or" || lower == "not" || lower == "in" ||
               lower == "case" || lower == "of" || lower == "repeat" || lower == "until" ||
               lower == "array" || lower == "type" || lower == "record" || lower == "exit" || lower == "break" || lower == "continue" ||
               lower == "nil" || lower == "new" || lower == "dispose" || lower == "pointer" || lower == "uses" ||
               lower == "unit" || lower == "interface" || lower == "implementation" ||
               lower == "with" || lower == "goto" || lower == "label" ||
               lower == "packed" || lower == "set" || lower == "file";
    }

    std::unique_ptr<ASTNode> PascalParser::parseProcedureDeclaration() {
        expectToken("procedure");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string procName = token->getTokenValue();
        next();
        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (peekIs("(")) {
            next();
            parameters = parseParameterList();
            expectToken(")");
            next();
        }
        expectToken(";");
        next();
        auto block = parseBlock();
        expectToken(";");
        next();
        auto procDeclNode = std::make_unique<ProcDeclNode>(procName, std::move(parameters), std::move(block));
        procDeclNode->setLineNumber(lineNum);
        return procDeclNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseFunctionDeclaration() {
        expectToken("function");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string funcName = token->getTokenValue();
        next();

        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (peekIs("(")) {
            next();
            parameters = parseParameterList();
            expectToken(")");
            next();
        }

        expectToken(":");
        next();

        std::string returnType;
        if (peekIs("^")) {
            next();
            expectToken(types::TokenType::TT_ID);
            returnType = "^" + token->getTokenValue();
            next();
        } else {
            expectToken(types::TokenType::TT_ID);
            returnType = token->getTokenValue();
            next();
        }

        expectToken(";");
        next();

        auto block = parseBlock();

        expectToken(";");
        next();

        auto funcDeclNode = std::make_unique<FuncDeclNode>(
            funcName, std::move(parameters), returnType, std::move(block));
        funcDeclNode->setLineNumber(lineNum);
        return funcDeclNode;
    }

    std::vector<std::unique_ptr<ASTNode>> PascalParser::parseParameterList() {
        std::vector<std::unique_ptr<ASTNode>> parameters;
        if (!peekIs(")")) {
            parameters.push_back(parseParameter());
            while (peekIs(";")) {
                next();
                parameters.push_back(parseParameter());
            }
        }
        return parameters;
    }

    std::unique_ptr<ASTNode> PascalParser::parseParameter() {
        int lineNum = token ? token->getLine() : 1;
        bool isVar = false;
        if (peekIs("var")) {
            isVar = true;
            next();
        }

        std::vector<std::string> identifiers;
        expectToken(types::TokenType::TT_ID);
        identifiers.push_back(token->getTokenValue());
        next();
        while (peekIs(",")) {
            next();
            expectToken(types::TokenType::TT_ID);
            identifiers.push_back(token->getTokenValue());
            next();
        }

        expectToken(":");
        next();

        std::string typeName;
        if (peekIs("^")) {
            next();
            expectToken(types::TokenType::TT_ID);
            typeName = "^" + token->getTokenValue();
            next();
        } else {
            expectToken(types::TokenType::TT_ID);
            typeName = token->getTokenValue();
            next();
        }

        auto parameterNode = std::make_unique<ParameterNode>(
            std::move(identifiers), typeName, isVar);
        parameterNode->setLineNumber(lineNum);
        return parameterNode;
    }

    std::unique_ptr<CompoundStmtNode> PascalParser::parseCompoundStatement() {
        int lineNum = token ? token->getLine() : 1;
        expectToken("begin");
        next();
        auto statements = parseStatementList();
        expectToken("end");
        next();
        auto compoundNode = std::make_unique<CompoundStmtNode>(std::move(statements));
        compoundNode->setLineNumber(lineNum);
        return compoundNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseStatement() {
        if (!token) {
            auto emptyNode = std::make_unique<EmptyStmtNode>();
            emptyNode->setLineNumber(1);
            return emptyNode;
        }
        if (peekIs("exit")) {
            int lineNum = token->getLine();
            next();
            std::unique_ptr<ASTNode> expr;
            if (peekIs("(")) {
                next();
                expr = parseExpression();
                expectToken(")");
                next();
            } else if (!peekIs(";")) {
                expr = parseExpression();
            }
            expectToken(";");
            auto exitNode = std::make_unique<ExitNode>(std::move(expr));
            exitNode->setLineNumber(lineNum);
            return exitNode;
        } else if (peekIs("break")) {
            int lineNum = token->getLine();
            next();
            expectToken(";");
            auto breakNode = std::make_unique<BreakNode>();
            breakNode->setLineNumber(lineNum);
            return breakNode;
        } else if (peekIs("continue")) {
            int lineNum = token->getLine();
            next();
            expectToken(";");
            auto continueNode = std::make_unique<ContinueNode>();
            continueNode->setLineNumber(lineNum);
            return continueNode;
        } else if (peekIs("begin")) {
            return parseCompoundStatement();
        } else if (peekIs("if")) {
            return parseIfStatement();
        } else if (peekIs("while")) {
            return parseWhileStatement();
        } else if (peekIs("for")) {
            return parseForStatement();
        } else if (peekIs("repeat")) {
            return parseRepeatStatement();
        } else if (peekIs("case")) {
            return parseCaseStatement();
        } else if (peekIs("with")) {
            return parseWithStatement();
        } else if (peekIs("goto")) {
            return parseGotoStatement();
        } else if (peekIs(types::TokenType::TT_NUM)) {
            // Label definition: 100: statement
            std::string lbl = token->getTokenValue();
            int lineNum = token->getLine();
            next();
            if (peekIs(":")) {
                next();
                auto stmt = parseStatement();
                auto labelNode = std::make_unique<LabelStmtNode>(lbl, std::move(stmt));
                labelNode->setLineNumber(lineNum);
                return labelNode;
            }
            error("Expected ':' after label number");
            return nullptr;
        } else if (peekIs(types::TokenType::TT_ID)) {
            auto lhs = parseLValue();
            int lineNum = token ? token->getLine() : 1;
            if (peekIs(":=")) {
                next();
                auto rhs = parseExpression();
                auto assignmentNode = std::make_unique<AssignmentNode>(std::move(lhs), std::move(rhs));
                assignmentNode->setLineNumber(lineNum);
                return assignmentNode;
            }
            if (auto varNode = dynamic_cast<VariableNode *>(lhs.get())) {
                if (peekIs("(")) {
                    return parseProcedureCall(varNode->name);
                }
                // Bare identifier as statement = parameterless procedure call
                auto procCallNode = std::make_unique<ProcCallNode>(varNode->name, std::vector<std::unique_ptr<ASTNode>>{});
                procCallNode->setLineNumber(lineNum);
                return procCallNode;
            }
            if (auto fieldNode = dynamic_cast<FieldAccessNode *>(lhs.get())) {
                if (dynamic_cast<VariableNode *>(fieldNode->recordExpr.get())) {
                    if (peekIs("(")) {
                        return parseProcedureCall(fieldNode->fieldName);
                    }
                    // Bare qualified name as statement = parameterless procedure call
                    auto procCallNode = std::make_unique<ProcCallNode>(fieldNode->fieldName, std::vector<std::unique_ptr<ASTNode>>{});
                    procCallNode->setLineNumber(lineNum);
                    return procCallNode;
                }
            }
            error("Invalid statement: expected ':=' for assignment or '(' for procedure call");
            return nullptr;
        } else {
            auto emptyNode = std::make_unique<EmptyStmtNode>();
            emptyNode->setLineNumber(token ? token->getLine() : 1);
            return emptyNode;
        }
    }

    std::unique_ptr<ASTNode> PascalParser::parseAssignmentOrProcCall() {
        expectToken(types::TokenType::TT_ID);
        std::string name = token->getTokenValue();
        int lineNum = token->getLine();
        next();
        if (peekIs(":=")) {
            next();
            auto variable = std::make_unique<VariableNode>(name);
            variable->setLineNumber(lineNum);
            auto expression = parseExpression();
            auto assignmentNode = std::make_unique<AssignmentNode>(std::move(variable), std::move(expression));
            assignmentNode->setLineNumber(lineNum);
            return assignmentNode;
        } else {
            return parseProcedureCall(name);
        }
    }

    std::unique_ptr<ASTNode> PascalParser::parseIfStatement() {
        expectToken("if");
        int lineNum = token->getLine();
        next();
        auto condition = parseExpression();
        expectToken("then");
        next();
        auto thenStatement = parseStatement();
        std::unique_ptr<ASTNode> elseStatement = nullptr;
        if (peekIs("else")) {
            next();
            elseStatement = parseStatement();
        }
        auto ifStmtNode = std::make_unique<IfStmtNode>(std::move(condition), std::move(thenStatement), std::move(elseStatement));
        ifStmtNode->setLineNumber(lineNum);
        return ifStmtNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseWhileStatement() {
        expectToken("while");
        int lineNum = token->getLine();
        next();
        auto condition = parseExpression();
        expectToken("do");
        next();
        auto statement = parseStatement();
        auto whileStmtNode = std::make_unique<WhileStmtNode>(std::move(condition), std::move(statement));
        whileStmtNode->setLineNumber(lineNum);
        return whileStmtNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseForStatement() {
        expectToken("for");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string variable = token->getTokenValue();
        next();
        expectToken(":=");
        next();
        auto startValue = parseExpression();
        bool isDownto = false;
        if (peekIs("to")) {
            next();
        } else if (peekIs("downto")) {
            isDownto = true;
            next();
        } else {
            error("Expected 'to' or 'downto' in for loop");
        }
        auto endValue = parseExpression();
        expectToken("do");
        next();
        auto statement = parseStatement();
        auto forStmtNode = std::make_unique<ForStmtNode>(variable, std::move(startValue), std::move(endValue), isDownto, std::move(statement));
        forStmtNode->setLineNumber(lineNum);
        return forStmtNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseRepeatStatement() {
        expectToken("repeat");
        int lineNum = token->getLine();
        next();
        std::vector<std::unique_ptr<ASTNode>> statements;
        while (!peekIs("until")) {
            statements.push_back(parseStatement());
            if (peekIs(";")) {
                next();
            } else if (!peekIs("until")) {
                error("Expected ';' or 'until'");
            }
        }
        expectToken("until");
        next();
        auto condition = parseExpression();
        auto repeatStmtNode = std::make_unique<RepeatStmtNode>(std::move(statements), std::move(condition));
        repeatStmtNode->setLineNumber(lineNum);
        return repeatStmtNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseExpression() {
        int lineNum = token ? token->getLine() : 1;
        auto left = parseSimpleExpression();
        if (isRelationalOperator()) {
            if (peekIs("in")) {
                next();
                // Allow both set literal [1,2,3] and set variable
                auto right = parseSimpleExpression();
                left = std::make_unique<BinaryOpNode>(std::move(left), BinaryOpNode::IN, std::move(right));
                left->setLineNumber(lineNum);
            } else {
                std::string op = getRelationalOp();
                next();
                auto right = parseSimpleExpression();
                BinaryOpNode::OpType enumOp = BinaryOpNode::EQUAL;
                if (op == "=")
                    enumOp = BinaryOpNode::EQUAL;
                else if (op == "<>")
                    enumOp = BinaryOpNode::NOT_EQUAL;
                else if (op == "<")
                    enumOp = BinaryOpNode::LESS;
                else if (op == "<=")
                    enumOp = BinaryOpNode::LESS_EQUAL;
                else if (op == ">")
                    enumOp = BinaryOpNode::GREATER;
                else if (op == ">=")
                    enumOp = BinaryOpNode::GREATER_EQUAL;
                left = std::make_unique<BinaryOpNode>(std::move(left), enumOp, std::move(right));
                left->setLineNumber(lineNum);
            }
        }
        while (peekIs("and") || peekIs("or")) {
            BinaryOpNode::OpType op;
            if (peekIs("and")) {
                op = BinaryOpNode::AND;
                next();
            } else {
                op = BinaryOpNode::OR;
                next();
            }
            auto right = parseSimpleExpression();
            left = std::make_unique<BinaryOpNode>(std::move(left), op, std::move(right));
            left->setLineNumber(lineNum);
        }
        return left;
    }

    std::unique_ptr<ASTNode> PascalParser::parseSimpleExpression() {
        std::unique_ptr<ASTNode> result;
        int lineNum = token ? token->getLine() : 1;
        if (((peekIs("+") || peekIs("-")) && peekIs(types::TokenType::TT_SYM)) || peekIs("not")) {
            UnaryOpNode::Operator op = UnaryOpNode::PLUS;
            if (peekIs("+"))
                op = UnaryOpNode::PLUS;
            else if (peekIs("-"))
                op = UnaryOpNode::MINUS;
            else if (peekIs("not"))
                op = UnaryOpNode::NOT;
            next();
            auto operand = parseTerm();
            result = std::make_unique<UnaryOpNode>(op, std::move(operand));
            result->setLineNumber(lineNum);
        } else {
            result = parseTerm();
        }
        while ((peekIs("+") || peekIs("-")) && peekIs(types::TokenType::TT_SYM)) {
            BinaryOpNode::OpType op;
            if (peekIs("+"))
                op = BinaryOpNode::PLUS;
            else
                op = BinaryOpNode::MINUS;
            next();
            auto right = parseTerm();
            result = std::make_unique<BinaryOpNode>(std::move(result), op, std::move(right));
            result->setLineNumber(lineNum);
        }
        return result;
    }

    std::unique_ptr<ASTNode> PascalParser::parseTerm() {
        int lineNum = token ? token->getLine() : 1;
        auto result = parseFactor();
        while (isMulOperator()) {
            BinaryOpNode::OpType op = BinaryOpNode::MULTIPLY;
            if (peekIs("*"))
                op = BinaryOpNode::MULTIPLY;
            else if (peekIs("/"))
                op = BinaryOpNode::DIVIDE;
            else if (peekIs("div"))
                op = BinaryOpNode::DIV;
            else if (peekIs("mod"))
                op = BinaryOpNode::MOD;
            next();
            auto right = parseFactor();
            result = std::make_unique<BinaryOpNode>(std::move(result), op, std::move(right));
            result->setLineNumber(lineNum);
        }
        return result;
    }

    std::unique_ptr<ASTNode> PascalParser::parseFactor() {
        int lineNum = token ? token->getLine() : 1;
        if (peekIs(types::TokenType::TT_NUM)) {
            std::string value = token->getTokenValue();
            next();
            bool isReal = value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos;
            auto numberNode = std::make_unique<NumberNode>(value, !isReal, isReal);
            numberNode->setLineNumber(lineNum);
            return numberNode;
        } else if (peekIs(types::TokenType::TT_STR)) {
            std::string value = token->getTokenValue();
            next();
            auto stringNode = std::make_unique<StringNode>(value);
            stringNode->setLineNumber(lineNum);
            return stringNode;
        } else if (peekIs("true") || peekIs("false")) {
            bool value = (token->getTokenValue() == "true");
            next();
            auto booleanNode = std::make_unique<BooleanNode>(value);
            booleanNode->setLineNumber(lineNum);
            return booleanNode;
        } else if (peekIs("nil")) {
            next();
            auto nilNode = std::make_unique<NilNode>();
            nilNode->setLineNumber(lineNum);
            return nilNode;
        } else if (peekIs("@")) {
            next();
            auto operand = parseFactor();
            auto addrNode = std::make_unique<AddressOfNode>(std::move(operand));
            addrNode->setLineNumber(lineNum);
            return addrNode;
        } else if (peekIs("[")) {
            // Set constructor: [elem1, elem2, ...]
            next(); // consume '['
            std::vector<std::unique_ptr<ASTNode>> elements;
            if (!peekIs("]")) {
                elements.push_back(parseExpression());
                while (peekIs(",")) {
                    next();
                    elements.push_back(parseExpression());
                }
            }
            expectToken("]");
            next();
            auto setNode = std::make_unique<SetLiteralNode>(std::move(elements));
            setNode->setLineNumber(lineNum);
            return setNode;
        } else if (peekIs("(")) {
            next();
            auto expr = parseExpression();
            expectToken(")");
            next();
            return expr;
        } else if (peekIs(types::TokenType::TT_ID)) {
            std::string name = token->getTokenValue();
            next();
            std::unique_ptr<ASTNode> left = std::make_unique<VariableNode>(name);
            left->setLineNumber(lineNum);
            while (true) {
                if (peekIs("[")) {
                    next();
                    auto index = parseExpression();
                    expectToken("]");
                    next();
                    left = std::make_unique<ArrayAccessNode>(std::move(left), std::move(index));
                    left->setLineNumber(lineNum);
                } else if (peekIs(".")) {
                    next();
                    expectToken(types::TokenType::TT_ID);
                    std::string fieldName = token->getTokenValue();
                    next();
                    left = std::make_unique<FieldAccessNode>(std::move(left), fieldName);
                    left->setLineNumber(lineNum);
                } else if (peekIs("^")) {
                    next();
                    left = std::make_unique<PointerDerefNode>(std::move(left));
                    left->setLineNumber(lineNum);
                } else if (peekIs("(")) {
                    if (auto varNode = dynamic_cast<VariableNode *>(left.get())) {
                        return parseFunctionCall(varNode->name);
                    } else if (auto fieldNode = dynamic_cast<FieldAccessNode *>(left.get())) {
                        if (dynamic_cast<VariableNode *>(fieldNode->recordExpr.get())) {
                            return parseFunctionCall(fieldNode->fieldName);
                        }
                        error("Function call must be on a simple or qualified identifier");
                    } else {
                        error("Function call must be on a simple identifier");
                    }
                } else {
                    break;
                }
            }
            return left;
        } else {
            error("Expected factor");
            return nullptr;
        }
    }

    std::unique_ptr<ASTNode> PascalParser::parseProcedureCall(const std::string &name) {
        int lineNum = token ? token->getLine() : 1;
        std::vector<std::unique_ptr<ASTNode>> arguments;
        if (peekIs("(")) {
            next();
            arguments = parseArgumentList();
            expectToken(")");
            next();
        }
        auto procCallNode = std::make_unique<ProcCallNode>(name, std::move(arguments));
        procCallNode->setLineNumber(lineNum);
        return procCallNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseFunctionCall(const std::string &name) {
        int lineNum = token ? token->getLine() : 1;
        std::vector<std::unique_ptr<ASTNode>> arguments;
        expectToken("(");
        next();
        arguments = parseArgumentList();
        expectToken(")");
        next();
        auto funcCallNode = std::make_unique<FuncCallNode>(name, std::move(arguments));
        funcCallNode->setLineNumber(lineNum);
        return funcCallNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseCaseStatement() {
        expectToken("case");
        int lineNum = token->getLine();
        next();
        auto expression = parseExpression();
        expectToken("of");
        next();
        std::vector<std::unique_ptr<CaseStmtNode::CaseBranch>> branches;
        while (!peekIs("end") && !peekIs("else")) {
            std::vector<std::unique_ptr<ASTNode>> values;
            values.push_back(parseExpression());
            while (peekIs(",")) {
                next();
                values.push_back(parseExpression());
            }
            expectToken(":");
            next();
            auto statement = parseStatement();
            branches.push_back(std::make_unique<CaseStmtNode::CaseBranch>(std::move(values), std::move(statement)));
            if (peekIs(";"))
                next();
        }
        std::unique_ptr<ASTNode> elseStatement = nullptr;
        if (peekIs("else")) {
            next();
            elseStatement = parseStatement();
            if (peekIs(";"))
                next();
        }
        expectToken("end");
        next();
        auto caseStmtNode = std::make_unique<CaseStmtNode>(std::move(expression), std::move(branches), std::move(elseStatement));
        caseStmtNode->setLineNumber(lineNum);
        return caseStmtNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseConstDeclaration() {
        expectToken("const");
        int lineNum = token->getLine();
        next();
        std::vector<std::unique_ptr<ConstDeclNode::ConstAssignment>> assignments;
        do {
            expectToken(types::TokenType::TT_ID);
            std::string identifier = token->getTokenValue();
            next();
            expectToken("=");
            next();
            auto value = parseExpression();
            assignments.push_back(std::make_unique<ConstDeclNode::ConstAssignment>(identifier, std::move(value)));
            expectToken(";");
            next();
        } while (peekIs(types::TokenType::TT_ID) && !isKeyword(token->getTokenValue()));
        auto constDeclNode = std::make_unique<ConstDeclNode>(std::move(assignments));
        constDeclNode->setLineNumber(lineNum);
        return constDeclNode;
    }

    std::vector<std::unique_ptr<ASTNode>> PascalParser::parseDeclarations() {
        std::vector<std::unique_ptr<ASTNode>> declarations;
        while (peekIs("type") || peekIs("const") || peekIs("var") || peekIs("procedure") || peekIs("function") || peekIs("label")) {
            if (peekIs("label")) {
                parseLabelDeclaration();
            } else if (peekIs("type")) {
                declarations.push_back(parseTypeDeclaration());
            } else if (peekIs("const")) {
                declarations.push_back(parseConstDeclaration());
            } else if (peekIs("var")) {
                if (match("var")) {
                    while (peekIs(types::TokenType::TT_ID) && !isKeyword(token->getTokenValue())) {
                        auto decl = parseVarDeclaration();
                        declarations.push_back(std::move(decl));
                    }
                }
            } else if (peekIs("procedure")) {
                declarations.push_back(parseProcedureDeclaration());
            } else if (peekIs("function")) {
                declarations.push_back(parseFunctionDeclaration());
            }
        }
        return declarations;
    }

    std::vector<std::unique_ptr<ASTNode>> PascalParser::parseStatementList() {
        std::vector<std::unique_ptr<ASTNode>> statements;
        while (!peekIs("end") && !peekIs("else") && !peekIs("until")) {
            statements.push_back(parseStatement());
            if (peekIs(";")) {
                next();
                if (peekIs("end") || peekIs("else") || peekIs("until"))
                    break;
            } else {
                break;
            }
        }
        return statements;
    }

    std::vector<std::unique_ptr<ASTNode>> PascalParser::parseArgumentList() {
        std::vector<std::unique_ptr<ASTNode>> arguments;
        if (!peekIs(")")) {
            arguments.push_back(parseExpression());
            while (peekIs(",")) {
                next();
                arguments.push_back(parseExpression());
            }
        }
        return arguments;
    }

    bool PascalParser::isMulOperator() {
        if (!peekIs(types::TokenType::TT_SYM) && !peekIs(types::TokenType::TT_ID))
            return false;
        return peekIs("*") || peekIs("/") || peekIs("div") || peekIs("mod");
    }

    bool PascalParser::isRelationalOperator() {
        if (peekIs("in"))
            return true;
        if (!peekIs(types::TokenType::TT_SYM))
            return false;
        return peekIs("=") || peekIs("<>") || peekIs("<") || peekIs("<=") || peekIs(">") || peekIs(">=");
    }

    std::string PascalParser::getRelationalOp() {
        if (peekIs("in"))
            return "in";
        if (peekIs("="))
            return "=";
        if (peekIs("<>"))
            return "<>";
        if (peekIs("<="))
            return "<=";
        if (peekIs(">="))
            return ">=";
        if (peekIs("<"))
            return "<";
        if (peekIs(">"))
            return ">";
        error("Invalid relational operator");
        return "";
    }

    std::unique_ptr<ASTNode> PascalParser::parseTypeDeclaration() {
        expectToken("type");
        int lineNum = token->getLine();
        next();

        std::vector<std::unique_ptr<ASTNode>> typeDeclarations;

        do {
            expectToken(types::TokenType::TT_ID);
            std::string typeName = token->getTokenValue();
            next();
            expectToken("=");
            next();

            std::unique_ptr<ASTNode> typeDefinition;
            //bool isPacked = false;
            if (peekIs("packed")) {
              //  isPacked = true;
                next(); // consume 'packed' modifier (accepted but has no effect)
            }
            if (peekIs("record")) {
                auto recordType = parseRecordType();
                typeDefinition = std::make_unique<RecordDeclarationNode>(
                    typeName,
                    std::unique_ptr<RecordTypeNode>(static_cast<RecordTypeNode *>(recordType.release())));
            } else if (peekIs("(")) {
                // Enumerated type: Color = (Red, Green, Blue)
                next(); // consume '('
                std::vector<std::string> enumValues;
                expectToken(types::TokenType::TT_ID);
                enumValues.push_back(token->getTokenValue());
                next();
                while (peekIs(",")) {
                    next();
                    expectToken(types::TokenType::TT_ID);
                    enumValues.push_back(token->getTokenValue());
                    next();
                }
                expectToken(")");
                next();
                typeDefinition = std::make_unique<EnumTypeDeclNode>(typeName, std::move(enumValues));
            } else if (peekIs("array")) {
                auto arrayType = parseArrayType();
                typeDefinition = std::make_unique<ArrayTypeDeclarationNode>(
                    typeName,
                    std::unique_ptr<ArrayTypeNode>(static_cast<ArrayTypeNode *>(arrayType.release())));
            } else if (peekIs("set")) {
                next(); // consume 'set'
                expectToken("of");
                next();
                expectToken(types::TokenType::TT_ID);
                std::string baseType = token->getTokenValue();
                next();
                typeDefinition = std::make_unique<TypeAliasNode>(typeName, "set of " + baseType);
            } else if (peekIs("file")) {
                next(); // consume 'file'
                if (peekIs("of")) {
                    next(); // consume 'of'
                    expectToken(types::TokenType::TT_ID);
                    next(); // consume element type (ignored — all files are byte-stream)
                }
                typeDefinition = std::make_unique<TypeAliasNode>(typeName, "file");
            } else if (peekIs("^")) {
                next();
                expectToken(types::TokenType::TT_ID);
                std::string baseType = token->getTokenValue();
                next();
                typeDefinition = std::make_unique<TypeAliasNode>(typeName, "^" + baseType);
            } else {
                expectToken(types::TokenType::TT_ID);
                std::string baseType = token->getTokenValue();
                next();
                typeDefinition = std::make_unique<TypeAliasNode>(typeName, baseType);
            }

            expectToken(";");
            next();

            typeDeclarations.push_back(std::move(typeDefinition));
        } while (peekIs(types::TokenType::TT_ID) && !isKeyword(token->getTokenValue()));

        auto typeDeclNode = std::make_unique<TypeDeclNode>(std::move(typeDeclarations));
        typeDeclNode->setLineNumber(lineNum);
        return typeDeclNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseRecordType() {
        expectToken("record");
        next();
        std::vector<std::unique_ptr<ASTNode>> fields;
        while (!peekIs("end") && !peekIs("case")) {
            std::vector<std::string> ids;
            expectToken(types::TokenType::TT_ID);
            ids.push_back(token->getTokenValue());
            next();
            while (peekIs(",")) {
                next();
                expectToken(types::TokenType::TT_ID);
                ids.push_back(token->getTokenValue());
                next();
            }
            expectToken(":");
            next();
            auto fieldType = parseTypeSpec();
            fields.push_back(std::make_unique<VarDeclNode>(
                std::move(ids),
                std::move(fieldType),
                std::vector<std::unique_ptr<ASTNode>>{}));
            expectToken(";");
            next();
        }

        auto recordNode = std::make_unique<RecordTypeNode>(std::move(fields));

        // Parse variant part: case tag: type of ...
        if (peekIs("case")) {
            next(); // consume 'case'
            expectToken(types::TokenType::TT_ID);
            recordNode->variantTagName = token->getTokenValue();
            next();
            expectToken(":");
            next();
            expectToken(types::TokenType::TT_ID);
            recordNode->variantTagType = token->getTokenValue();
            next();
            expectToken("of");
            next();

            while (!peekIs("end")) {
                VariantArm arm;
                // Parse case label(s): e.g. 1, 2:
                arm.caseLabels.push_back(parseExpression());
                while (peekIs(",")) {
                    next();
                    arm.caseLabels.push_back(parseExpression());
                }
                expectToken(":");
                next();
                expectToken("(");
                next();
                // Parse fields inside parentheses
                while (!peekIs(")")) {
                    std::vector<std::string> ids;
                    expectToken(types::TokenType::TT_ID);
                    ids.push_back(token->getTokenValue());
                    next();
                    while (peekIs(",")) {
                        next();
                        expectToken(types::TokenType::TT_ID);
                        ids.push_back(token->getTokenValue());
                        next();
                    }
                    expectToken(":");
                    next();
                    auto fieldType = parseTypeSpec();
                    arm.fields.push_back(std::make_unique<VarDeclNode>(
                        std::move(ids),
                        std::move(fieldType),
                        std::vector<std::unique_ptr<ASTNode>>{}));
                    if (peekIs(";"))
                        next();
                }
                expectToken(")");
                next();
                if (peekIs(";"))
                    next();
                recordNode->variantArms.push_back(std::move(arm));
            }
        }

        expectToken("end");
        next();
        return recordNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseVarDeclaration() {
        std::vector<std::string> identifiers;
        std::vector<std::unique_ptr<ASTNode>> initializers;

        do {
            expectToken(types::TokenType::TT_ID);
            identifiers.push_back(token->getTokenValue());
            next();
            if (peekIs(","))
                next();
            else
                break;
        } while (true);

        expectToken(":");
        next();

        auto typeAst = parseTypeSpec();

        if (peekIs(":=")) {
            next();
            initializers.push_back(parseExpression());
        }

        expectToken(";");
        next();

        return std::make_unique<VarDeclNode>(
            std::move(identifiers),
            std::move(typeAst),
            std::move(initializers));
    }

    std::unique_ptr<ASTNode> PascalParser::parseTypeSpec() {
        if (peekIs("packed"))
            next(); // consume 'packed' modifier (accepted but has no effect)
        if (peekIs("set")) {
            next(); // consume 'set'
            expectToken("of");
            next();
            expectToken(types::TokenType::TT_ID);
            std::string baseType = token->getTokenValue();
            next();
            return std::make_unique<SetTypeNode>(baseType);
        }
        if (peekIs("file")) {
            next(); // consume 'file'
            if (peekIs("of")) {
                next(); // consume 'of' (element type accepted but treated as untyped)
                expectToken(types::TokenType::TT_ID);
                next(); // consume element type (ignored — all files are byte-stream)
            }
            return std::make_unique<SimpleTypeNode>("file");
        }
        if (peekIs("array"))
            return parseArrayType();
        if (peekIs("record"))
            return parseRecordType();
        if (peekIs("^")) {
            next();
            expectToken(types::TokenType::TT_ID);
            std::string baseType = token->getTokenValue();
            next();
            return std::make_unique<PointerTypeNode>(baseType);
        }
        expectToken(types::TokenType::TT_ID);
        auto t = std::make_unique<SimpleTypeNode>(token->getTokenValue());
        next();
        return t;
    }

    std::unique_ptr<ASTNode> PascalParser::parseArrayType() {
        expectToken("array");
        next();
        // Dynamic array: array of <type> (no bounds)
        if (peekIs("of")) {
            next();
            auto elementType = parseTypeSpec();
            return std::make_unique<ArrayTypeNode>(
                std::move(elementType),
                nullptr,
                nullptr);
        }
        // Static array: array[lo..hi] of <type>
        expectToken("[");
        next();
        auto lowerBound = parseExpression();
        expectToken("..");
        next();
        auto upperBound = parseExpression();
        expectToken("]");
        next();
        expectToken("of");
        next();
        auto elementType = parseTypeSpec();
        return std::make_unique<ArrayTypeNode>(
            std::move(elementType),
            std::move(lowerBound),
            std::move(upperBound));
    }

    std::string PascalParser::tokenTypeToString(types::TokenType type) {
        switch (type) {
        case types::TokenType::TT_ID:
            return "identifier";
        case types::TokenType::TT_NUM:
            return "number";
        case types::TokenType::TT_STR:
            return "string";
        case types::TokenType::TT_SYM:
            return "symbol";
        default:
            return "unknown token";
        }
    }

    std::unique_ptr<ASTNode> PascalParser::parseLValue() {
        int lineNum = token ? token->getLine() : 1;
        expectToken(types::TokenType::TT_ID);
        std::string name = token->getTokenValue();
        next();

        std::unique_ptr<ASTNode> left = std::make_unique<VariableNode>(name);
        left->setLineNumber(lineNum);

        while (true) {
            if (peekIs("[")) {
                next();
                auto index = parseExpression();
                expectToken("]");
                next();
                left = std::make_unique<ArrayAccessNode>(std::move(left), std::move(index));
                left->setLineNumber(lineNum);
            } else if (peekIs(".")) {
                next();
                expectToken(types::TokenType::TT_ID);
                std::string fieldName = token->getTokenValue();
                next();
                left = std::make_unique<FieldAccessNode>(std::move(left), fieldName);
                left->setLineNumber(lineNum);
            } else if (peekIs("^")) {
                next();
                left = std::make_unique<PointerDerefNode>(std::move(left));
                left->setLineNumber(lineNum);
            } else {
                break;
            }
        }
        return left;
    }

    std::unique_ptr<ASTNode> PascalParser::parseWithStatement() {
        expectToken("with");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_ID);
        std::string recordVar = token->getTokenValue();
        next();
        expectToken("do");
        next();
        auto stmt = parseStatement();
        auto withNode = std::make_unique<WithStmtNode>(recordVar, std::move(stmt));
        withNode->setLineNumber(lineNum);
        return withNode;
    }

    std::unique_ptr<ASTNode> PascalParser::parseGotoStatement() {
        expectToken("goto");
        int lineNum = token->getLine();
        next();
        expectToken(types::TokenType::TT_NUM);
        std::string label = token->getTokenValue();
        next();
        auto gotoNode = std::make_unique<GotoStmtNode>(label);
        gotoNode->setLineNumber(lineNum);
        return gotoNode;
    }

    void PascalParser::parseLabelDeclaration() {
        expectToken("label");
        next();
        do {
            expectToken(types::TokenType::TT_NUM);
            declaredLabels.insert(token->getTokenValue());
            next();
            if (peekIs(",")) {
                next();
            } else {
                break;
            }
        } while (true);
        expectToken(";");
        next();
    }

} // namespace pascal
