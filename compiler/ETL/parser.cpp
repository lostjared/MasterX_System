#include "parser.hpp"
#include<iostream>
#include<memory>
#include<sstream>

namespace parse {

    Parser::Parser(scan::Scanner *scanner) : scan{scanner} {}

    Parser::~Parser() {}

    void Parser::match(const types::TokenType &type) {
        if (token_index < scan->size()) {
            scan::TToken &token = scan->operator[](token_index);
            if (token.getTokenType() != type) {
                std::cerr << "Error: Token type mismatch found: ";
                types::print_type_TokenType(std::cerr, token.getTokenType());
                std::cerr << " expected: ";
                types::print_type_TokenType(std::cerr, type);
                std::cerr << " in File: " << token.get_filename() << " on Line: " << token.get_pos().first << " Col: " << token.get_pos().second << "\n";
                exit(EXIT_FAILURE);
            } else {
                token_index++;
            }
        }
    }

    void Parser::match(const string_type &t) {
        if (token_index < scan->size()) {
            scan::TToken &token = scan->operator[](token_index);
            const std::string &s = token.getTokenValue();
            if (s != t) {
                std::cerr << "Error: Token value mismatch found: ";
                std::cerr << token.getTokenValue() << " expected: " << t << " in File: " << token.get_filename() << " On Line: " << token.get_pos().first << " Col: " << token.get_pos().second << "\n";
                exit(EXIT_FAILURE);
            } else {
                token_index++;
            }
        }
    }

    void Parser::match(const types::OperatorType op) {
        if (token_index < scan->size()) {
            scan::TToken &token = scan->operator[](token_index);
            auto op_t = types::lookUp(token.getTokenValue());
            if (op_t.has_value()) {
                if (op == *op_t) {
                    token_index++;
                } else {
                    auto pos = token.get_pos();
                    std::cerr << "Error: Operator type mismatch found: " << token.getTokenValue() << " expected: " << types::opStrings[static_cast<int>(op)] << " File: " << token.get_filename() << " Line: " << pos.first << " Col: " << pos.second << "\n";
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    void Parser::match(const types::KeywordType kw) {
        if (token_index < scan->size()) {
            scan::TToken &token = scan->operator[](token_index);
            auto op_t = types::lookUp_Keyword(token.getTokenValue());
            if (op_t.has_value()) {
                if (kw == *op_t) {
                    token_index++;
                } else {
                    std::cerr << "Error: Keyword type mismatch found: " << token.getTokenValue() << " expected: " << types::kwStr[static_cast<int>(kw)] << " in File: " << token.get_filename() << " Line: " << token.get_pos().first << " Col: " << token.get_pos().second << "\n";
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    bool Parser::test(const types::TokenType &type) {
        scan::TToken &token = scan->operator[](token_index);
        return token.getTokenType() == type;
    }

    bool Parser::test(const string_type &t) {
        scan::TToken &token = scan->operator[](token_index);
        return token.getTokenValue() == t;
    }

    bool Parser::test(const types::OperatorType &op) {
        scan::TToken &token = scan->operator[](token_index);
        auto op_t = types::lookUp(token.getTokenValue());
        return token.getTokenType() == types::TokenType::TT_SYM && op_t.has_value() && op == *op_t;
    }

    bool Parser::test(const types::KeywordType &kw) {
        scan::TToken &token = scan->operator[](token_index);
        auto op_t = types::lookUp_Keyword(token.getTokenValue());
        return token.getTokenType() == types::TokenType::TT_ID && op_t.has_value() && kw == *op_t;
    }

    void Parser::inc(const uint64_t num) {
        token_index += num;
    }

    void Parser::dec(const uint64_t num) {
        if (token_index >= num) {
            token_index -= num;
        }
    }

    bool Parser::parse() {
        try {
            uint64_t num = scan->scan();
            if (num > 0) {
                std::cout << "ETL: Scanned " << num << " tokens.\n";
                std::cout << "ETL: parsing ... ";
                proc_tokens();
                std::cout << " [complete]\n";
                return true;
            } else {
                std::cerr << "ETL: Error zero tokens or failure...\n";
                return false;
            }
        } catch (scan::ScanExcept &e) {
            std::cerr << "ETL: Fatal: " << e.why() << "\n";
            exit(EXIT_FAILURE);
        } catch (ParseException &p) {
            throw p;
        }
        return false;
    }

    void Parser::proc_tokens() {
        auto program = std::make_unique<ast::Program>();
        while (token_index < scan->size()) {
           if (test(types::KeywordType::KW_PROC)) {
                inc();
                auto function = parseFunction();
                program->body.push_back(std::move(function));
            } else if (test(types::KeywordType::KW_DEFINE)) {
                inc();
                auto fdef = parseDefine();
                program->body.push_back(std::move(fdef));
            } else if (test(types::KeywordType::KW_LET)) {
                std::ostringstream stream;
                auto pos = scan->operator[](token_index).get_pos();
                stream << "Let statement must be in function body File: " << scan->operator[](token_index).get_filename() << "  Line: " << pos.first << " Col: " << pos.second;
                throw ParseException(stream.str());
            } else {
                std::ostringstream stream;
                auto pos = scan->operator[](token_index).get_pos();
                stream << "Unknown token: " << scan->operator[](token_index).getTokenValue() << " in File: " << scan->operator[](token_index).get_filename() << " at Line: " << pos.first << " Col: " << pos.second << "\n";
                throw ParseException(stream.str());
            }
        }
        root = std::move(program);
    }

    std::unique_ptr<ast::Expression> Parser::parseExpression() {
        return parseLogicalOr();
    }

    std::unique_ptr<ast::Expression> Parser::parseLogicalOr() {
        auto left = parseLogicalAnd();
        while (test(types::OperatorType::OP_OR_OR)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseLogicalAnd();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseLogicalAnd() {
        auto left = parseBitwiseOr();
        while (test(types::OperatorType::OP_AND_AND)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseBitwiseOr();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseBitwiseOr() {
        auto left = parseBitwiseXor();
        while (test(types::OperatorType::OP_OR)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseBitwiseXor();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseBitwiseXor() {
        auto left = parseBitwiseAnd();
        while (test(types::OperatorType::OP_XOR)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseBitwiseAnd();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseBitwiseAnd() {
        auto left = parseEquality();
        while (test(types::OperatorType::OP_AND)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseEquality();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseEquality() {
        auto left = parseRelational();
        while (test(types::OperatorType::OP_EQ) || test(types::OperatorType::OP_NEQ)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseRelational();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseRelational() {
        auto left = parseShift();
        while (test(types::OperatorType::OP_LT) || test(types::OperatorType::OP_LE) ||
               test(types::OperatorType::OP_GT) || test(types::OperatorType::OP_GE)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseShift();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseShift() {
        auto left = parseAdditive();
        while (test(types::OperatorType::OP_LSHIFT) || test(types::OperatorType::OP_RSHIFT)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseAdditive();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseAdditive() {
        auto left = parseMultiplicative();
        while (test(types::OperatorType::OP_PLUS) || test(types::OperatorType::OP_MINUS)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseMultiplicative();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseMultiplicative() {
        auto left = parseFactor();
        while (test(types::OperatorType::OP_MUL) || test(types::OperatorType::OP_DIV) || test(types::OperatorType::OP_MOD)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto right = parseFactor();
            left = std::make_unique<ast::BinaryOp>(*op, std::move(left), std::move(right));
        }
        return left;
    }

    std::unique_ptr<ast::Expression> Parser::parseFactor() {
        if (test(types::OperatorType::OP_MINUS)) {
            inc();
            auto operand = parseFactor();
            return std::make_unique<ast::UnaryOp>(types::OperatorType::OP_MINUS, std::move(operand));
        } else if (test(types::OperatorType::OP_NOT) && test(types::TokenType::TT_SYM)) {
            inc();
            auto operand = parseFactor();
            return std::make_unique<ast::UnaryOp>(types::OperatorType::OP_NOT, std::move(operand));
        } else if (test(types::OperatorType::OP_TILDE)) {
            inc();
            auto operand = parseFactor();
            return std::make_unique<ast::UnaryOp>(types::OperatorType::OP_TILDE, std::move(operand));
        } else if (test(types::OperatorType::OP_INC) || test(types::OperatorType::OP_DEC)) {
            auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
            inc();
            auto operand = parseFactor();
            return std::make_unique<ast::UnaryOp>(*op, std::move(operand));
        }
        if (test(types::OperatorType::OP_LPAREN)) {
            inc();
            auto expr = parseExpression();
            match(types::OperatorType::OP_RPAREN);
            return expr;
        }
        return parsePrimary();
    }

    std::unique_ptr<ast::Expression> Parser::parsePrimary() {
        if (test(types::TokenType::TT_NUM) || test(types::TokenType::TT_STR)) {
            auto token = scan->operator[](token_index);
            inc();
            return std::make_unique<ast::Literal>(token.getTokenValue(), token.getTokenType());
        } else if (test(types::TokenType::TT_ID)) {
            auto token = scan->operator[](token_index);
            inc();
            if (test(types::OperatorType::OP_LPAREN)) {
                return parseCall(token.getTokenValue());
            }
            return std::make_unique<ast::Identifier>(token.getTokenValue());
        }
        std::ostringstream stream;
        auto pos = scan->operator[](token_index).get_pos();
        stream << "Parse Error: Expected primary expression found: " << static_cast<int>(scan->operator[](token_index).getTokenType()) << ":" << scan->operator[](token_index).getTokenValue() << " in File: " << scan->operator[](token_index).get_filename() << "  at Line: " << pos.first << " Col: " << pos.second << "\n";
        throw ParseException(stream.str());
        return nullptr;
    }

    std::unique_ptr<ast::Expression> Parser::parseCall(const std::string &functionName) {
        match(types::OperatorType::OP_LPAREN);
        std::vector<std::unique_ptr<ast::Expression>> arguments;
        if (!test(types::OperatorType::OP_RPAREN)) {
            do {
                arguments.push_back(parseExpression());
            } while (test(types::OperatorType::OP_COMMA) && token_index++ < scan->size());
        }
        match(types::OperatorType::OP_RPAREN);
        auto callExpr = std::make_unique<ast::Call>(functionName, std::move(arguments));
        if (functionName == "str") {
            callExpr->vtype = ast::VarType::STRING;
        }
        return callExpr;
    }

    std::unique_ptr<ast::Assignment> Parser::parseAssignment(bool there, bool chk) {
        if (test(types::TokenType::TT_ID)) {
            auto lhs = parsePrimary();
            if (test(types::OperatorType::OP_ASSIGN) ||
                test(types::OperatorType::OP_PLUS_ASSIGN) ||
                test(types::OperatorType::OP_MINUS_ASSIGN) ||
                test(types::OperatorType::OP_MUL_ASSIGN) ||
                test(types::OperatorType::OP_DIV_ASSIGN) ||
                test(types::OperatorType::OP_RSHIFT_ASSIGN) ||
                test(types::OperatorType::OP_LSHIFT_ASSIGN) ||
                test(types::OperatorType::OP_AND_ASSIGN) ||
                test(types::OperatorType::OP_XOR_ASSIGN) ||
                test(types::OperatorType::OP_OR_ASSIGN))  {
                
                auto op = types::lookUp(scan->operator[](token_index).getTokenValue());
                inc();
                auto rhs = parseExpression();

                if (auto literal = dynamic_cast<ast::Literal*>(rhs.get())) {
                    if (literal->type == types::TokenType::TT_STR) {
                        if (auto identifier = dynamic_cast<ast::Identifier*>(lhs.get())) {
                            identifier->vtype = ast::VarType::STRING;
                        }
                    } else if (literal->type == types::TokenType::TT_NUM) {
                        if (auto identifier = dynamic_cast<ast::Identifier*>(lhs.get())) {
                            identifier->vtype = ast::VarType::NUMBER;
                        }
                    }
                }
                if (auto callExpr = dynamic_cast<ast::Call*>(rhs.get())) {
                    auto functionName = callExpr->functionName;
                    if (functionName == "str") {
                        if (auto identifier = dynamic_cast<ast::Identifier*>(lhs.get())) {
                            identifier->vtype = ast::VarType::STRING;
                        }
                    }
                }
                if (auto rhsIdentifier = dynamic_cast<ast::Identifier*>(rhs.get())) {
                    if (rhsIdentifier->vtype == ast::VarType::STRING) {
                        if (auto lhsIdentifier = dynamic_cast<ast::Identifier*>(lhs.get())) {
                            lhsIdentifier->vtype = ast::VarType::STRING;
                        }
                    } else if (rhsIdentifier->vtype == ast::VarType::NUMBER) {
                        if (auto lhsIdentifier = dynamic_cast<ast::Identifier*>(lhs.get())) {
                            lhsIdentifier->vtype = ast::VarType::NUMBER;
                        }
                    }
                }

                if (op != types::OperatorType::OP_ASSIGN) {
                    auto binOpType = getBinaryOpForCompoundAssign(*op);
                    auto binaryOp = std::make_unique<ast::BinaryOp>(binOpType, std::move(lhs), std::move(rhs));
                    if(auto id = dynamic_cast<ast::Identifier *>(binaryOp->left.get())) {
                        lhs = id->copy();
                        rhs = std::move(binaryOp);
                    }
                }

                if (chk == false) {
                    match(types::OperatorType::OP_SEMICOLON);
                }

                return std::make_unique<ast::Assignment>(std::move(lhs), std::move(rhs), ast::VarType::NUMBER, there);
            }
        }

        std::ostringstream stream;
        auto pos = scan->operator[](token_index).get_pos();
        stream << "Parse Error: Invalid assignment expression at Line: " << pos.first << ", Col: " << pos.second << "\n";
        throw ParseException(stream.str());
        return nullptr;
    }

    std::unique_ptr<ast::DefineFunction> Parser::parseDefine() {
        ast::VarType rt_type = ast::VarType::NUMBER;
        if (test(types::OperatorType::OP_DOLLAR)) {
            inc();
            rt_type = ast::VarType::STRING;
        } else if(test(types::OperatorType::OP_AT)) {
            inc();
            rt_type = ast::VarType::POINTER;
        }
        if (test(types::TokenType::TT_ID)) {
            std::string name = scan->operator[](token_index).getTokenValue();
            inc();
            match(types::OperatorType::OP_LPAREN);
            std::vector<std::pair<std::string, ast::VarType>> parameters;
            if (!test(types::OperatorType::OP_RPAREN)) {
                do {
                    ast::VarType ptype = ast::VarType::NUMBER;
                    if (test(types::OperatorType::OP_DOLLAR)) {
                        inc();
                        ptype = ast::VarType::STRING;
                    } else if(test(types::OperatorType::OP_AT)) {
                        inc();
                        ptype = ast::VarType::POINTER;
                    }
                    if (!test(types::TokenType::TT_ID)) {
                        std::ostringstream stream;
                        auto pos = scan->operator[](token_index).get_pos();
                        stream << "Parse Error: Expected identifier for parameter in File: " << scan->operator[](token_index).get_filename() << "  Line: " << pos.first << " Col: " << pos.second << "\n";
                        throw ParseException(stream.str());
                    }
                    std::string param = scan->operator[](token_index).getTokenValue();
                    parameters.push_back(std::make_pair(param, ptype));
                    inc();
                    if (test(types::OperatorType::OP_COMMA)) {
                        inc();
                    } else {
                        break;
                    }
                } while (true);
            }
            match(types::OperatorType::OP_RPAREN);
            match(types::OperatorType::OP_SEMICOLON);
            return std::make_unique<ast::DefineFunction>(name, parameters, rt_type);
        }
        std::ostringstream stream;
        stream << "Parse Error on function Define\n";
        throw ParseException(stream.str());
        return nullptr;
    }

    std::unique_ptr<ast::Function> Parser::parseFunction() {
        ast::VarType rt_type = ast::VarType::NUMBER;
        if (test(types::OperatorType::OP_DOLLAR)) {
            inc();
            rt_type = ast::VarType::STRING;
        } else if(test(types::OperatorType::OP_AT)) {
            inc();
            rt_type = ast::VarType::POINTER;
        }
        if (test(types::TokenType::TT_ID)) {
            std::string name = scan->operator[](token_index).getTokenValue();
            inc();
            match(types::OperatorType::OP_LPAREN);
            std::vector<std::pair<std::string, ast::VarType>> parameters;
            if (!test(types::OperatorType::OP_RPAREN)) {
                do {
                    ast::VarType ptype = ast::VarType::NUMBER;
                    if (test(types::OperatorType::OP_DOLLAR)) {
                        inc();
                        ptype = ast::VarType::STRING;
                    } else if(test(types::OperatorType::OP_AT)) {
                        inc();
                        ptype = ast::VarType::POINTER;
                    }
                    if (!test(types::TokenType::TT_ID)) {
                        std::ostringstream stream;
                        auto pos = scan->operator[](token_index).get_pos();
                        stream << "Parse Error: Expected identifier for parameter on in File: " << scan->operator[](token_index).get_filename() << " Line: " << pos.first << " Col: " << pos.second << "\n";
                        throw ParseException(stream.str());
                    }
                    std::string param = scan->operator[](token_index).getTokenValue();
                    parameters.push_back(std::make_pair(param, ptype));
                    inc();
                    if (test(types::OperatorType::OP_COMMA)) {
                        inc();
                    } else {
                        break;
                    }
                } while (true);
            }
            match(types::OperatorType::OP_RPAREN);
            match(types::OperatorType::OP_LBRACE);
            bool return_found = false;
            auto function = std::make_unique<ast::Function>(name, parameters, rt_type);
            while (!test(types::OperatorType::OP_RBRACE)) {
                  if (test(types::KeywordType::KW_IF)) {
                    auto if_statement = parseIfStatement();
                    function->body.push_back(std::move(if_statement));
                } else if (test(types::KeywordType::KW_FOR)) {
                    auto for_stmt = parseForStatement();
                    function->body.push_back(std::move(for_stmt));
                } else if (test(types::KeywordType::KW_WHILE)) {
                    auto while_stmt = parseWhileStatement();
                    function->body.push_back(std::move(while_stmt));
                } else if (test(types::KeywordType::KW_RETURN)) {
                    inc();
                    auto e = parseExpression();
                    if (e) {
                        match(types::OperatorType::OP_SEMICOLON);
                        function->body.push_back(std::make_unique<ast::Return>(std::move(e)));
                        return_found = true;
                    }
                } else if (test(types::KeywordType::KW_LET)) {
                    inc();
                    auto stmt = parseAssignment();
                    if (stmt) {
                        function->body.push_back(std::move(stmt));
                    }
                } else if (test(types::TokenType::TT_ID)) {
                    auto token = scan->operator[](token_index);
                    inc();
                    if (test(types::OperatorType::OP_ASSIGN) ||
                        test(types::OperatorType::OP_PLUS_ASSIGN) ||
                        test(types::OperatorType::OP_MINUS_ASSIGN) ||
                        test(types::OperatorType::OP_MUL_ASSIGN) ||
                        test(types::OperatorType::OP_DIV_ASSIGN) ||
                        test(types::OperatorType::OP_RSHIFT_ASSIGN) ||
                        test(types::OperatorType::OP_LSHIFT_ASSIGN) ||
                        test(types::OperatorType::OP_AND_ASSIGN) ||
                        test(types::OperatorType::OP_XOR_ASSIGN) ||
                        test(types::OperatorType::OP_OR_ASSIGN))  {
                        dec();
                        auto stmt = parseAssignment(true);
                        function->body.push_back(std::move(stmt));
                    } else {
                        auto call_st = parseCall(token.getTokenValue());
                        inc();
                        if (call_st) {
                            function->body.push_back(std::move(call_st));
                        }
                    }
                } else {
                    break;
                }
            }
            match(types::OperatorType::OP_RBRACE);
            if (!return_found) {
                std::ostringstream stream;
                stream << "Error: Function " << name << " does not include a return statement.\n";
                throw ParseException(stream.str());
            }
            return function;
        }
        std::ostringstream stream;
        auto pos = scan->operator[](token_index).get_pos();
        stream << "Parse Error: Expected Function in File: " << scan->operator[](token_index).get_filename()<< " on Line: " << pos.first << " Col: " << pos.second << "\n";
        throw ParseException(stream.str());
        return nullptr;
    }


    std::unique_ptr<ast::IfStatement> Parser::parseIfStatement() {
        match(types::KeywordType::KW_IF);
        match(types::OperatorType::OP_LPAREN);
        auto condition = parseExpression();
        match(types::OperatorType::OP_RPAREN);
        match(types::OperatorType::OP_LBRACE);

        std::vector<std::unique_ptr<ast::ASTNode>> if_body;
        while (!test(types::OperatorType::OP_RBRACE)) {
                if (test(types::KeywordType::KW_BREAK)) {
                    inc();
                    match(types::OperatorType::OP_SEMICOLON);
                    if_body.push_back(std::make_unique<ast::Break>());
                } else if (test(types::KeywordType::KW_CONTINUE)) {
                    inc();
                    match(types::OperatorType::OP_SEMICOLON);
                    if_body.push_back(std::make_unique<ast::Continue>());
                } else if (test(types::KeywordType::KW_IF)) {
                    auto nested_if = parseIfStatement();
                    if_body.push_back(std::move(nested_if));
                } else if (test(types::KeywordType::KW_WHILE)) {
                    auto while_stmt = parseWhileStatement();
                    if_body.push_back(std::move(while_stmt));
                } else if (test(types::KeywordType::KW_FOR)) {
                    auto for_stmt = parseForStatement();
                    if_body.push_back(std::move(for_stmt));
                } else if (test(types::KeywordType::KW_RETURN)) {
                    inc();
                    auto e = parseExpression();
                    match(types::OperatorType::OP_SEMICOLON);
                    if_body.push_back(std::make_unique<ast::Return>(std::move(e)));
            } else if (test(types::KeywordType::KW_LET)) {
                inc();
                auto stmt = parseAssignment(false);
                if_body.push_back(std::move(stmt));
            } else if (test(types::TokenType::TT_ID)) {
                auto token = scan->operator[](token_index);
                inc();
                if (test(types::OperatorType::OP_ASSIGN) ||
                    test(types::OperatorType::OP_PLUS_ASSIGN) ||
                    test(types::OperatorType::OP_MINUS_ASSIGN) ||
                    test(types::OperatorType::OP_MUL_ASSIGN) ||
                    test(types::OperatorType::OP_DIV_ASSIGN) ||
                    test(types::OperatorType::OP_RSHIFT_ASSIGN) ||
                    test(types::OperatorType::OP_LSHIFT_ASSIGN) ||
                    test(types::OperatorType::OP_AND_ASSIGN) ||
                    test(types::OperatorType::OP_XOR_ASSIGN) ||
                    test(types::OperatorType::OP_OR_ASSIGN))  {
                        dec();
                        auto stmt = parseAssignment(true);
                        if_body.push_back(std::move(stmt));
                } else {
                    auto call_st = parseCall(token.getTokenValue());
                    match(types::OperatorType::OP_SEMICOLON);
                    if_body.push_back(std::move(call_st));
                }
            } else {
                break;
            }
        }
        match(types::OperatorType::OP_RBRACE);

        std::vector<std::unique_ptr<ast::ASTNode>> else_body;
        if (test(types::KeywordType::KW_ELSE)) {
            inc();
            if(test(types::KeywordType::KW_IF)) {
                std::ostringstream stream;
                stream << "Error else if not supported yet.\n";
                throw ParseException(stream.str());
            } else {
                match(types::OperatorType::OP_LBRACE);
                while (!test(types::OperatorType::OP_RBRACE)) {
                    if (test(types::KeywordType::KW_BREAK)) {
                        inc();
                        match(types::OperatorType::OP_SEMICOLON);
                        else_body.push_back(std::make_unique<ast::Break>());
                    } else if (test(types::KeywordType::KW_CONTINUE)) {
                        inc();
                        match(types::OperatorType::OP_SEMICOLON);
                        else_body.push_back(std::make_unique<ast::Continue>());
                    } else if (test(types::KeywordType::KW_IF)) {
                        auto nested_if = parseIfStatement();
                        else_body.push_back(std::move(nested_if));
                    } else if (test(types::KeywordType::KW_WHILE)) {
                        auto while_stmt = parseWhileStatement();
                        else_body.push_back(std::move(while_stmt));
                    } else if (test(types::KeywordType::KW_FOR)) {
                        auto for_stmt = parseForStatement();
                        else_body.push_back(std::move(for_stmt));
                    }
                    else if (test(types::KeywordType::KW_RETURN)) {
                        inc();
                        auto e = parseExpression();
                        match(types::OperatorType::OP_SEMICOLON);
                        else_body.push_back(std::make_unique<ast::Return>(std::move(e)));
                    } else if (test(types::KeywordType::KW_LET)) {
                        inc();
                        auto stmt = parseAssignment();
                        else_body.push_back(std::move(stmt));
                    } else if (test(types::TokenType::TT_ID)) {
                        auto token = scan->operator[](token_index);
                        inc();
                        if (test(types::OperatorType::OP_ASSIGN) ||
                            test(types::OperatorType::OP_PLUS_ASSIGN) ||
                            test(types::OperatorType::OP_MINUS_ASSIGN) ||
                            test(types::OperatorType::OP_MUL_ASSIGN) ||
                            test(types::OperatorType::OP_DIV_ASSIGN) ||
                            test(types::OperatorType::OP_RSHIFT_ASSIGN) ||
                            test(types::OperatorType::OP_LSHIFT_ASSIGN) ||
                            test(types::OperatorType::OP_AND_ASSIGN) ||
                            test(types::OperatorType::OP_XOR_ASSIGN) ||
                            test(types::OperatorType::OP_OR_ASSIGN))  {
    
                            dec();
                            auto stmt = parseAssignment(true);
                            else_body.push_back(std::move(stmt));
                        } else {
                            auto call_st = parseCall(token.getTokenValue());
                            match(types::OperatorType::OP_SEMICOLON);
                            else_body.push_back(std::move(call_st));
                        }
                    } else {
                        break;
                    }
                }
                match(types::OperatorType::OP_RBRACE);
             }
        }
        return std::make_unique<ast::IfStatement>(std::move(condition), std::move(if_body), std::move(else_body));
    }

    std::unique_ptr<ast::WhileStatement> Parser::parseWhileStatement() {
        match(types::KeywordType::KW_WHILE);
        match(types::OperatorType::OP_LPAREN);
        auto condition = parseExpression();
        match(types::OperatorType::OP_RPAREN);
        match(types::OperatorType::OP_LBRACE);

        std::vector<std::unique_ptr<ast::ASTNode>> body;
        while (!test(types::OperatorType::OP_RBRACE)) {
            if (test(types::KeywordType::KW_BREAK)) {
                inc();
                match(types::OperatorType::OP_SEMICOLON);
                body.push_back(std::make_unique<ast::Break>());
            } else if (test(types::KeywordType::KW_CONTINUE)) {
                inc();
                match(types::OperatorType::OP_SEMICOLON);
                body.push_back(std::make_unique<ast::Continue>());
            } else if (test(types::KeywordType::KW_WHILE)) {
                auto nested_while = parseWhileStatement();
                body.push_back(std::move(nested_while));
            } else if (test(types::KeywordType::KW_FOR)) {
                    auto for_stmt = parseForStatement();
                    body.push_back(std::move(for_stmt));
            } else if (test(types::KeywordType::KW_IF)) {
                auto if_statement = parseIfStatement();
                body.push_back(std::move(if_statement));
            } else if (test(types::KeywordType::KW_RETURN)) {
                inc();
                auto e = parseExpression();
                match(types::OperatorType::OP_SEMICOLON);
                body.push_back(std::make_unique<ast::Return>(std::move(e)));
            } else if (test(types::KeywordType::KW_LET)) {
                inc();
                auto stmt = parseAssignment(false);
                body.push_back(std::move(stmt));
            } else if (test(types::TokenType::TT_ID)) {
                auto token = scan->operator[](token_index);
                inc(); 
                if (test(types::OperatorType::OP_ASSIGN) ||
                    test(types::OperatorType::OP_PLUS_ASSIGN) ||
                    test(types::OperatorType::OP_MINUS_ASSIGN) ||
                    test(types::OperatorType::OP_MUL_ASSIGN) ||
                    test(types::OperatorType::OP_DIV_ASSIGN) ||
                    test(types::OperatorType::OP_RSHIFT_ASSIGN) ||
                    test(types::OperatorType::OP_LSHIFT_ASSIGN) ||
                    test(types::OperatorType::OP_AND_ASSIGN) ||
                    test(types::OperatorType::OP_XOR_ASSIGN) ||
                    test(types::OperatorType::OP_OR_ASSIGN))  {
                    dec();
                     auto stmt = parseAssignment(true);
                    body.push_back(std::move(stmt));
                } else {
                    auto call_st = parseCall(token.getTokenValue());
                    match(types::OperatorType::OP_SEMICOLON);
                    body.push_back(std::move(call_st));
                }
            } else {
                std::ostringstream stream;
                auto pos = scan->operator[](token_index).get_pos();
                stream << "Parse Error: Unexpected token inside while loop in File: " << scan->operator[](token_index).get_filename() << " at Line: " << pos.first << " Col: " << pos.second << "\n";
                throw ParseException(stream.str());
            }
        }
        match(types::OperatorType::OP_RBRACE);
        return std::make_unique<ast::WhileStatement>(std::move(condition), std::move(body));
    }

    std::unique_ptr<ast::ForStatement> Parser::parseForStatement() {
        match(types::KeywordType::KW_FOR);
        match(types::OperatorType::OP_LPAREN);

        std::unique_ptr<ast::Assignment> init_statement;
        if (test(types::KeywordType::KW_LET)) {
            inc();
            init_statement = parseAssignment(false);
        } else if (test(types::TokenType::TT_ID)) {
            init_statement = parseAssignment(true);
        } else {
            // handle the case where there is no initialization
            init_statement = nullptr;
        }
        match(types::OperatorType::OP_SEMICOLON);
        std::unique_ptr<ast::Expression> condition = parseExpression();
        match(types::OperatorType::OP_SEMICOLON);
        std::unique_ptr<ast::Assignment> post;
        if (test(types::KeywordType::KW_LET) || test(types::TokenType::TT_ID)) {
            post = parseAssignment(true, true);
        } else {
            post = nullptr;
        }
        match(types::OperatorType::OP_RPAREN);
        match(types::OperatorType::OP_LBRACE);

        std::vector<std::unique_ptr<ast::ASTNode>> body;
        while (!test(types::OperatorType::OP_RBRACE)) {
            if (test(types::KeywordType::KW_IF)) {
                auto if_statement = parseIfStatement();
                body.push_back(std::move(if_statement));
            } else if (test(types::KeywordType::KW_WHILE)) {
                auto while_stmt = parseWhileStatement();
                body.push_back(std::move(while_stmt));
            } else if (test(types::KeywordType::KW_RETURN)) {
                inc();
                auto e = parseExpression();
                match(types::OperatorType::OP_SEMICOLON);
                body.push_back(std::make_unique<ast::Return>(std::move(e)));
            } else if (test(types::KeywordType::KW_LET)) {
                inc();
                auto stmt = parseAssignment(false);
                body.push_back(std::move(stmt));
            } else if (test(types::KeywordType::KW_FOR)) {
                auto for_stmt = parseForStatement();
                body.push_back(std::move(for_stmt));
            } else if (test(types::TokenType::TT_ID)) {
                auto token = scan->operator[](token_index);
                inc();
                if (test(types::OperatorType::OP_ASSIGN) ||
                    test(types::OperatorType::OP_PLUS_ASSIGN) ||
                    test(types::OperatorType::OP_MINUS_ASSIGN) ||
                    test(types::OperatorType::OP_MUL_ASSIGN) ||
                    test(types::OperatorType::OP_DIV_ASSIGN) ||
                    test(types::OperatorType::OP_RSHIFT_ASSIGN) ||
                    test(types::OperatorType::OP_LSHIFT_ASSIGN) ||
                    test(types::OperatorType::OP_AND_ASSIGN) ||
                    test(types::OperatorType::OP_XOR_ASSIGN) ||
                    test(types::OperatorType::OP_OR_ASSIGN))  {
                    dec();
                    auto stmt = parseAssignment(true);
                    body.push_back(std::move(stmt));
                } else {
                    auto call_st = parseCall(token.getTokenValue());
                    match(types::OperatorType::OP_SEMICOLON);
                    body.push_back(std::move(call_st));
                }
            } else {
                std::ostringstream stream;
                auto pos = scan->operator[](token_index).get_pos();
                stream << "Parse Error: Unexpected token inside for loop in File: " << scan->operator[](token_index).get_filename() << "  Line: " << pos.first << " Col: " << pos.second << "\n";
                throw ParseException(stream.str());
            }
        }

        match(types::OperatorType::OP_RBRACE);

        return std::make_unique<ast::ForStatement>(
            std::move(init_statement),
            std::move(condition),
            std::move(post),
            std::move(body)
        );
    }

    types::OperatorType Parser::getBinaryOpForCompoundAssign(types::OperatorType op) {
        switch (op) {
            case types::OperatorType::OP_PLUS_ASSIGN: return types::OperatorType::OP_PLUS;
            case types::OperatorType::OP_MINUS_ASSIGN: return types::OperatorType::OP_MINUS;
            case types::OperatorType::OP_MUL_ASSIGN: return types::OperatorType::OP_MUL;
            case types::OperatorType::OP_DIV_ASSIGN: return types::OperatorType::OP_DIV;
            case types::OperatorType::OP_RSHIFT_ASSIGN: return types::OperatorType::OP_RSHIFT;
            case types::OperatorType::OP_LSHIFT_ASSIGN: return types::OperatorType::OP_LSHIFT;
            case types::OperatorType::OP_AND_ASSIGN: return types::OperatorType::OP_AND;
            case types::OperatorType::OP_XOR_ASSIGN: return types::OperatorType::OP_XOR;
            case types::OperatorType::OP_OR_ASSIGN: return types::OperatorType::OP_OR;
            default:
                std::cerr << "Unsupported compound assignment operator.\n";
                exit(EXIT_FAILURE);
        }
        
    }
}