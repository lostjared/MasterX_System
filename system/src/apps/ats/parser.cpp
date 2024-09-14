#include "parser.hpp"
#include "code.hpp"
#include "function.hpp"

namespace lex {
    std::unordered_map<std::string, lex::Func> function = {
        {"exit", Func("exit",app_exit) }, {"clear", Func("clear",app_clear)}
        
    };
    
    double app_exit(double d) {
        exit(static_cast<int>(d));
        return 0;
    }
    
    double app_clear(double d) {
        return system("clear");
    }
    
    bool Parser::eval() {
        try {
            while(1) {
                Token t = input.GetToken();
                if(t.getType() == TOKEN_EOF) {
                    return true;
                }
                double e = expr(false);
                bool ohex = code.symbols["hex"].get_double()!=0;
                if(ohex == false) {
                    if(v_assign != "")
                    	stream << "expr: [" << v_assign << "] = " << e << "\r\n";
                    else
                        stream << "expr: " << e << "\r\n";
                }
                else
                    if(v_assign != "")
                    	stream << "expr: [" << v_assign << "] = $" << std::uppercase << std::hex << static_cast<int>(e) << "\r\n";
                	else
                        stream << "expr: $" << std::uppercase << std::hex << static_cast<int>(e) << "\r\n";
                
                if(input.GetCurrent().getToken() == ";")
                    continue;// eat token
            }
        }
        catch (Scanner_EOF) {
            stream << "end of evalulation.\r\n";
        }
        catch(Scanner_Error) {
            stream << " A error has occoured.\r\n";
        }
        return false;
    }
    
    
    
    double Parser::prim(bool get) {
        
        if(get) input.GetToken();
        switch(input.GetCurrent().getType()) {
            case TOKEN_EOF:
                throw lex::Scanner_EOF();
                break;
            case TOKEN_DIGIT: {
                double v = atof(input.GetCurrent().getToken().c_str());
                input.GetToken();
                return v;
            }
            case TOKEN_HEX: {
                double v = icode::toHex(input.GetCurrent().getToken());
                input.GetToken();
                return v;
            }
            case TOKEN_CHAR: {
                std::string var_name = input.GetCurrent().getToken();
                input.GetToken();
                if(input.GetCurrent().getToken() == "(") {
                    
                    if (function.find(var_name) != function.end()) {
                        double d = expr(true);
                        if(input.GetCurrent().getToken() != ")") {
                            err("missing closing ).");
                            
                        }
                        input.GetToken();
                        if(function[var_name].func != 0)
                            return function[var_name].func(d);
                        
                        err("Error function not found.\r\n");
                    } else err("Unknown function");
                }
                
                if(!code.symbols.exisits(var_name)) {
                    code.symbols[var_name].create(var_name, symbol::Value("", 0));
                }
                
                
                double &v = code.symbols[var_name].get_double();

                if(input.GetCurrent().getToken() == "=")  {
                    v_assign = var_name;
                    v = expr(true);
                } else if(input.GetCurrent().getToken() == "+=") {
                    v_assign = var_name;
                    v += expr(true);
                    
                } else if(input.GetCurrent().getToken() == "-=") {
                    v_assign = var_name;
                    v -= expr(true);
                } else if(input.GetCurrent().getToken() == "*=") {
                    v_assign = var_name;
                    v *= expr(true);
                } else if(input.GetCurrent().getToken() == "/=") {
                    v_assign = var_name;
                    double d = expr(true);
                    if(d == 0) err("Divide by zero");
                    v /= d;
                }
                return v;
                
            }
                break;
            case TOKEN_OPERATOR: {
                if(input.GetCurrent().getToken() == "-") {
                    return -prim(true);
                    
                } else if (input.GetCurrent().getToken() == "(") {
                    auto e = expr(true);
                    if(input.GetCurrent().getToken() != ")") err("Expected )");
                    input.GetToken();
                    return e;
                }
            }
                break;
            default:
                return 0;
        }
        
        
        if(input.GetCurrent().getToken() == ")")
            err("No argument");
        return 0;
    }

    double Parser::term(bool get) {
        double left = prim(get);
        while(1) {
            std::string temp=input.GetCurrent().getToken();
            Token_type tok_type = input.GetCurrent().getType();
            if(tok_type == TOKEN_CHAR || tok_type == TOKEN_DIGIT) {
                err("Invalid type expected operator");
            }
            switch(tok_type) {
                case TOKEN_OPERATOR: {
                    switch(temp[0]) {
                        case '*':
                            left *= prim(true);
                            break;
                        case '/':
                            if(auto d = prim(true)) {
                                left /= d;
                                break;
                            }
                            err("Divide by Zero");
                            break;
                        default:
                            return left;
                    }
                    break;
                default:
                    break;
                    
                }
            }
        }
        return 0;
    }
    
    double Parser::expr(bool get) {
        double left = term(get);
        while(1) {
            switch(input.GetCurrent().getType()) {
                case TOKEN_EOF:
                    throw lex::Scanner_EOF();
                    break;
                case TOKEN_OPERATOR:
                {
                    std::string temp=input.GetCurrent().getToken();
                    
                    if(temp == "++") err("No ++ right now\r\n");
                    if(temp == "--") err("No -- right now\r\n");
                    
                    switch(temp[0]) {
                        case '+':
                            left += term(true);
                            break;
                        case '-':
                            left -= term(true);
                            break;
                        default:
                            return left;
                    }
                }
                    break;
                default:
                    return 0;
            }
        }
    }
}
