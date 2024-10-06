#ifndef PARSER_HPP
#define PARSER_HPP

#include "scanner.hpp"
#include "ast.hpp"
#include <memory>


namespace parse {

    class ParseException {
    public:
        using string_type = scan::token::Token<char>::string_type;
        ParseException(const string_type &w) : what{w} {}
        string_type why() const { return what; }
    private:
        string_type what;
    };

    class Parser {
    public:
        using string_type = scan::token::Token<char>::string_type;
        Parser(scan::Scanner *scanner);
        ~Parser();
        bool parse();

        std::unique_ptr<ast::ASTNode> getAST() { return std::move(root); }

    private:
        std::unique_ptr<scan::Scanner> scan;
        uint64_t token_index = 0;

        std::unique_ptr<ast::ASTNode> root;

        void match(const types::TokenType &type);
        void match(const string_type &t);
        void match(const types::OperatorType op);
        void match(const types::KeywordType kw);
        bool test(const types::TokenType &type);
        bool test(const types::OperatorType &op);
        bool test(const types::KeywordType &kw);
        bool test(const string_type &t);
        
        void inc(const uint64_t num = 1);
        void dec(const uint64_t num = 1);
        void proc_tokens();

        std::unique_ptr<ast::Expression> parseExpression();
        std::unique_ptr<ast::Expression> parseLogicalOr();
        std::unique_ptr<ast::Expression> parseLogicalAnd();
        std::unique_ptr<ast::Expression> parseBitwiseOr();
        std::unique_ptr<ast::Expression> parseBitwiseXor();
        std::unique_ptr<ast::Expression> parseBitwiseAnd();
        std::unique_ptr<ast::Expression> parseEquality();
        std::unique_ptr<ast::Expression> parseRelational();
        std::unique_ptr<ast::Expression> parseShift();
        std::unique_ptr<ast::Expression> parseAdditive();
        std::unique_ptr<ast::Expression> parseMultiplicative();
        std::unique_ptr<ast::Expression> parseFactor();
        std::unique_ptr<ast::Expression> parsePrimary();
        std::unique_ptr<ast::Assignment> parseAssignment(bool there = false, bool chk = false);
        std::unique_ptr<ast::Function> parseFunction();
        std::unique_ptr<ast::DefineFunction> parseDefine();
        std::unique_ptr<ast::Expression> parseCall(const std::string &functionName);
        std::unique_ptr<ast::IfStatement> parseIfStatement();
        std::unique_ptr<ast::WhileStatement> parseWhileStatement();
        std::unique_ptr<ast::ForStatement> parseForStatement();

        types::OperatorType getBinaryOpForCompoundAssign(types::OperatorType op);
    };
}


#endif 