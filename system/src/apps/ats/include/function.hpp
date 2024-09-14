#ifndef __FUNCTION_HPP_
#define __FUNCTION_HPP_

#include<string>
#include<iostream>
#include<vector>
#include"code.hpp"
#include<unordered_map>
#include<sstream>

extern interp::Code code;
extern std::ostringstream stream;

typedef void (*tokenFunc)(const std::string &command, std::vector<lex::Token> &tokens);
struct Command {
    int args;
    bool greater;
    tokenFunc func;
    std::string help_text;
    
    Command() {
        args = 0;
        func = 0;
        greater = false;
    }
    
    Command(tokenFunc ifunc, int iargs, const std::string &text) {
        args = iargs;
        func = ifunc;
        greater = false;
        help_text = text;
    }
    Command(tokenFunc ifunc, int iargs, bool g, const std::string &text) {
        args = iargs;
        func = ifunc;
        greater = g;
        help_text = text;
    }
    Command(const Command &c) : args(c.args), greater(c.greater), func(c.func), help_text(c.help_text) { }
    Command &operator=(const Command &c) {
        args = c.args;
        func = c.func;
        greater = c.greater;
        help_text = c.help_text;
        return *this;
    }
};
extern std::unordered_map<std::string, Command> function_map;

namespace token {
    void token_Convert(const std::string &command, std::vector<lex::Token> &tokens);
    void token_List(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Save(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Open(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Clear(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Remove(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Display(const std::string &command, std::vector<lex::Token> &tokens);
    void token_DisplaySymbols(const std::string &command, std::vector<lex::Token> &tokens);
    void token_DisplaySymbol(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Unsym(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Execute(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Help(const std::string &command, std::vector<lex::Token> &tokens);
    void token_sh(const std::string &command, std::vector<lex::Token> &tokens);
    void token_HTML(const std::string &command, std::vector<lex::Token> &tokens);
    void token_ProcInfo(const std::string &command, std::vector<lex::Token> &tokens);
    void token_SetReg(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Peek(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Poke(const std::string &command, std::vector<lex::Token> &tokens);
    void token_SetSym(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Tokenize(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Expression(const std::string &command, std::vector<lex::Token> &tokens);
    void token_Build(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Print(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Code(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Step(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Memory(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Peek16(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Insert(const std::string &text, std::vector<lex::Token> &tokens);
    void token_Continue(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Debug(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Stack(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Bin(const std::string &cmd, std::vector<lex::Token> &tokens);
    void token_Integer(const std::string &cmd, std::vector<lex::Token> &tokens);
}

#endif
