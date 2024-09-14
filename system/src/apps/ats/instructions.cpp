#include "code.hpp"
#include "translate.hpp"

namespace interp {
    
    void i_adc(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t val = c.peek(c.instruct[in].op1.op);
                        uint16_t total = c.proc.reg_a + val;
                        c.proc.reg_a += val;
                        if(c.proc.getFlag(icode::FLAG_CARRY))
                            ++c.proc.reg_a, ++total;
                        if(total > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        uint16_t total = c.proc.reg_a + val;
                        c.proc.reg_a += val;
                        
                        if(c.proc.getFlag(icode::FLAG_CARRY))
                            ++c.proc.reg_a, ++total;
                        
                        if(total > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    case interp::ABSOULTE_Y: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        uint16_t total = c.proc.reg_a + val;
                        c.proc.reg_a += val;
                        
                        if(c.proc.getFlag(icode::FLAG_CARRY))
                            ++c.proc.reg_a, ++total;
                        
                        if(total > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL: {
                uint8_t val = c.instruct[in].op1.op;
                uint16_t total = c.proc.reg_a + val;
                
                c.proc.reg_a += val;
                
                if(c.proc.getFlag(icode::FLAG_CARRY)) {
                    ++c.proc.reg_a, ++total;
                }
                
                if(total > 255)
                    c.proc.setFlag(icode::FLAG_CARRY, 1);
                else
                    c.proc.setFlag(icode::FLAG_CARRY, 0);
                
                
                if((char)c.proc.reg_a < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                
                if(c.proc.reg_a == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
            }
                break;
            default:
                break;
        }
        
        
    }
    
    void i_and(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t val = c.peek(c.instruct[in].op1.op);
                        uint16_t total = c.proc.reg_a & val;
                        c.proc.reg_a &= val;
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        uint16_t total = c.proc.reg_a & val;
                        c.proc.reg_a &= val;
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    case interp::ABSOULTE_Y: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        uint16_t total = c.proc.reg_a & val;
                        c.proc.reg_a &= val;
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL: {
                uint8_t val = c.instruct[in].op1.op;
                uint16_t total = c.proc.reg_a & val;
                
                c.proc.reg_a &= val;
                
                if((char)c.proc.reg_a < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                
                
                if(c.proc.reg_a == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
            }
                break;
            default:
                break;
        }
    }
    
    void i_asl(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                std::bitset<8> bits(cc);
                if(bits[7] == 1) c.proc.setFlag(icode::FLAG_CARRY, 1);
                else c.proc.setFlag(icode::FLAG_CARRY, 0);
                cc <<= 1;
                if(cc == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                if((char)cc < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
            }
                break;
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[ip].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op);
                        std::bitset<8> bits(cc);
                        if(bits[7] == 1) c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc <<= 1;
                        c.poke(c.instruct[ip].op1.op, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op+c.proc.reg_x);
                        std::bitset<8> bits(cc);
                        if(bits[7] == 1) c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc <<= 1;
                        c.poke(c.instruct[ip].op1.op+c.proc.reg_x, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                break;
            default:
                break;
        }
        
    }
    
    void i_bcc(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_CARRY) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_bcs(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_CARRY) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_beq(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_ZERO) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_bit(Code &c) {
        
    }
    
    void i_bmi(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_NEGATIVE) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_bne(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_ZERO) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_bpl(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_NEGATIVE) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_brk(Code &c) {
        c.stop();
        stream << "Breakpoint occured\r\n";
    }
    
    void i_bvc(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_OVERFLOW) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_bvs(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_OVERFLOW) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index-1;
        }
    }
    
    void i_clc(Code &c) {
        c.proc.setFlag(icode::FLAG_CARRY, 0);
    }
    void i_cld(Code &c) {
        c.proc.setFlag(icode::FLAG_DECIMAL, 0);
    }
    void i_cli(Code &c) {
        c.proc.setFlag(icode::FLAG_INTERRUPT, 0);
    }
    
    void i_clv(Code &c) {
        c.proc.setFlag(icode::FLAG_OVERFLOW, 0);
    }
    // still need to add negative/ carry flag setting for cmp,cpx,cpy
    void i_cmp(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t compare = c.peek(c.instruct[in].op1.op);
                        c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_a == compare));
                    }
                        break;
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_a == val));
                    }
                        break;
                    case interp::ABSOULTE_Y:
                    case interp::ZEROPAGE_Y: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_a == val));
                    }
                        break;
                }
                break;
            case icode::op_type::OP_DECIMAL: {
                c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_a == c.instruct[in].op1.op));
            }
                break;
            default:
                break;
        }
    }
    
    void i_cpx(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t compare = c.peek(c.instruct[in].op1.op);
                        c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_y == compare));
                    }
                        break;
                }
                break;
            case icode::op_type::OP_DECIMAL: {
                c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_y == c.instruct[in].op1.op));
            }
                break;
            default:
                break;
        }
    }
    
    void i_cpy(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t compare = c.peek(c.instruct[in].op1.op);
                        c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_y == compare));
                    }
                        break;
                }
                break;
            case icode::op_type::OP_DECIMAL: {
                c.proc.setFlag(icode::FLAG_ZERO, (c.proc.reg_y == c.instruct[in].op1.op));
            }
                break;
            default:
                break;
        }
    }
    
    void i_dec(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t value = c.peek(c.instruct[in].op1.op)-1;
                        if((char)value < 0) {
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        } else {
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        }
                        if(value == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        
                        c.poke(c.instruct[in].op1.op, value);
                    }
                        break;
                    case interp::ZEROPAGE_X:
                    case interp::ABSOULTE_X: {
                        uint8_t value = c.peek(c.instruct[in].op1.op+c.proc.reg_x)-1;
                        if((char)value < 0) {
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        } else {
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        }
                        if(value == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        
                        c.poke(c.instruct[in].op1.op+c.proc.reg_x,value);
                    }
                        break;
                }
                break;
            default:
                break;
        }
    }
    
    void i_dex(Code &c) {
        uint8_t value = c.proc.reg_x-1;
        if((char)value < 0) {
            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
        } else {
            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
        }
        --c.proc.reg_x;
        if(c.proc.reg_x == 0) {
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        } else {
            c.proc.setFlag(icode::FLAG_ZERO, 0);
        }
    }
    
    void i_dey(Code &c) {
        uint8_t value = c.proc.reg_y-1;
        if((char)value < 0) {
            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
        } else {
            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
        }
        
        --c.proc.reg_y;
        if(c.proc.reg_y == 0) {
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        } else {
            c.proc.setFlag(icode::FLAG_ZERO, 0);
        }
    }
    
    void i_end(Code &c) {
        c.end();
        stream << "Program stopped, END reached Program reset\r\n";
    }
    
    void i_eor(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t val = c.peek(c.instruct[in].op1.op);
                        uint16_t total = c.proc.reg_a ^ val;
                        c.proc.reg_a ^= val;
                        
                        if(total > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                        
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        uint16_t total = c.proc.reg_a ^ val;
                        c.proc.reg_a ^= val;
                        
                        if(total > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    case interp::ABSOULTE_Y: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        uint16_t total = c.proc.reg_a ^ val;
                        c.proc.reg_a ^= val;
                        
                        if(total > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL: {
                uint8_t val = c.instruct[in].op1.op;
                uint16_t total = c.proc.reg_a ^ val;
                
                c.proc.reg_a ^= val;
                
                if(c.proc.reg_a == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
            }
                break;
            default:
                break;
        }
    }
    
    void i_inc(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint16_t val = c.peek(c.instruct[in].op1.op);
                        ++val;
                        if(val > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        uint8_t value=static_cast<uint8_t>(val);
                        if(value == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        c.poke(c.instruct[in].op1.op, value);
                    }
                        break;
                    case interp::ZEROPAGE_X:
                    case interp::ABSOULTE_X: {
                        uint16_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        ++val;
                        if(val > 255)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        
                        uint8_t value = static_cast<uint8_t>(val);
                        if(value == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        c.poke(c.instruct[in].op1.op+c.proc.reg_x,value);
                    }
                        break;
                }
                break;
            default:
                break;
        }
    }
    
    void i_int(Code &c) {
        
    }
    
    void i_inx(Code &c) {
        uint16_t total = c.proc.reg_x+1;
        if(total > 255) {
            c.proc.setFlag(icode::FLAG_CARRY, 1);
        } else {
            c.proc.setFlag(icode::FLAG_CARRY, 0);
        }
        ++c.proc.reg_x;
        if(c.proc.reg_x == 0) {
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        } else {
            c.proc.setFlag(icode::FLAG_ZERO, 0);
        }
    }
    
    void i_iny(Code &c) {
        uint16_t total = c.proc.reg_y+1;
        if(total > 255) {
            c.proc.setFlag(icode::FLAG_CARRY, 1);
        } else {
            c.proc.setFlag(icode::FLAG_CARRY, 0);
        }
        ++c.proc.reg_y;
        if(c.proc.reg_y == 0) {
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        } else {
            c.proc.setFlag(icode::FLAG_ZERO, 0);
        }
    }
    
    void i_jmp(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_LABELTEXT:
            case icode::op_type::OP_LABEL: {
                c.proc.ip = c.instruct[in].op1.label_index-1;
                return;
            }
            default:
                break;
        }
    }
    
    void i_jsr(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_LABELTEXT:
            case icode::op_type::OP_LABEL: {
                c.in_stack.push_back(c.proc.ip);
                c.proc.ip = c.instruct[in].op1.label_index-1;
            }
                break;
            default:
                break;
        }
    }
    
    void i_lda(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE:
                        c.proc.reg_a = c.peek(c.instruct[in].op1.op);
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X:
                        c.proc.reg_a = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    case interp::ABSOULTE_Y:
                    case interp::ZEROPAGE_Y:
                        c.proc.reg_a = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                c.proc.reg_a = c.instruct[in].op1.op;
                if(c.proc.reg_a == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                break;
            default:
                break;
        }
    }
    
    void i_ldm(Code &c) {
        
    }
    void i_ldx(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE:
                        c.proc.reg_x = c.peek(c.instruct[in].op1.op);
                        if(c.proc.reg_x == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    case interp::ABSOULTE_Y:
                    case interp::ZEROPAGE_Y:
                        c.proc.reg_x = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        if(c.proc.reg_x == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                c.proc.reg_x = c.instruct[in].op1.op;
                if(c.proc.reg_x == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                
                break;
            default:
                break;
        }
    }
    
    void i_ldy(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE:
                        c.proc.reg_y = c.peek(c.instruct[in].op1.op);
                        if(c.proc.reg_y == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X:
                        c.proc.reg_y = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        if(c.proc.reg_y == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                c.proc.reg_y = c.instruct[in].op1.op;
                if(c.proc.reg_y == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                break;
            default:
                break;
        }
    }
    
    void i_lsr(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                std::bitset<8> bits(cc);
                if(bits[0] == 1) c.proc.setFlag(icode::FLAG_CARRY, 1);
                else c.proc.setFlag(icode::FLAG_CARRY, 0);
                cc >>= 1;
                if(cc == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                if((char)cc < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
            }
                break;
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[ip].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op);
                        std::bitset<8> bits(cc);
                        if(bits[0] == 1) c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc >>= 1;
                        c.poke(c.instruct[ip].op1.op, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op+c.proc.reg_x);
                        std::bitset<8> bits(cc);
                        if(bits[0] == 1) c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc >>= 1;
                        c.poke(c.instruct[ip].op1.op+c.proc.reg_x, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                break;
            default:
                break;
        }
    }
    
    void i_nop(Code &c) {
        stream << "nop\r\n";
    }
    
    void i_ora(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t val = c.peek(c.instruct[in].op1.op);
                        uint16_t total = c.proc.reg_a | val;
                        c.proc.reg_a |= val;
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_x);
                        uint16_t total = c.proc.reg_a | val;
                        c.proc.reg_a |= val;
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    case interp::ABSOULTE_Y: {
                        uint8_t val = c.peek(c.instruct[in].op1.op+c.proc.reg_y);
                        uint16_t total = c.proc.reg_a | val;
                        c.proc.reg_a |= val;
                        
                        if((char)c.proc.reg_a < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                        
                        if(c.proc.reg_a == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL: {
                uint8_t val = c.instruct[in].op1.op;
                uint16_t total = c.proc.reg_a | val;
                
                c.proc.reg_a |= val;
                
                if((char)c.proc.reg_a < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                
                
                if(c.proc.reg_a == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
            }
                break;
            default:
                break;
        }
    }
    
    void i_pha(Code &c) {
        c.stack.push_back(c.proc.reg_a);
    }
    
    void i_php(Code &c) {
        c.stack.push_back(c.proc.valFlags());
    }
    
    void i_pla(Code &c) {
        if(c.stack.size()==0) {
            throw interp::Runtime_E("error stack underflow");
        }
        uint8_t accum = c.stack.back();
        c.stack.pop_back();
        c.proc.reg_a = accum;
    }
    
    void i_plp(Code &c) {
        if(c.stack.size()==0) {
            throw interp::Runtime_E("error stack underflow");
        }
        uint8_t flags = c.stack.back();
        c.stack.pop_back();
        c.proc.setFlags(flags);
    }
    
    template<typename T>
    T ror(T x, unsigned int m)
    {
        return (x >> m) | (x << sizeof(T)*8 - m);
    }
    template<typename T>
    T rol(T x, unsigned int m) {
        return (x << m) | (x >> (sizeof(T)*8 -m));
    }
    
    
    void i_rol(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                std::bitset<8> bits(cc);
                if(bits[7] == 1)
                    c.proc.setFlag(icode::FLAG_CARRY, 1);
                else
                    c.proc.setFlag(icode::FLAG_CARRY, 0);
                cc = rol(cc, 1);
                if(cc == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                if((char)cc < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
            }
                break;
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[ip].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op);
                        std::bitset<8> bits(cc);
                        if(bits[7] == 1)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc = rol(cc, 1);
                        c.poke(c.instruct[ip].op1.op, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op+c.proc.reg_x);
                        std::bitset<8> bits(cc);
                        if(bits[7] == 1)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc = rol(cc, 1);
                        c.poke(c.instruct[ip].op1.op+c.proc.reg_x, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                break;
            default:
                break;
        }
    }
    
    void i_ror(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                std::bitset<8> bits(cc);
                if(bits[0] == 1)
                    c.proc.setFlag(icode::FLAG_CARRY, 1);
                else
                    c.proc.setFlag(icode::FLAG_CARRY, 0);
                cc = ror(cc, 1);
                if(cc == 0)
                    c.proc.setFlag(icode::FLAG_ZERO, 1);
                else
                    c.proc.setFlag(icode::FLAG_ZERO, 0);
                if((char)cc < 0)
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                else
                    c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
            }
                break;
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[ip].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op);
                        std::bitset<8> bits(cc);
                        if(bits[0] == 1)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc = ror(cc, 1);
                        c.poke(c.instruct[ip].op1.op, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                        
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X: {
                        uint8_t cc = c.peek(c.instruct[ip].op1.op+c.proc.reg_x);
                        std::bitset<8> bits(cc);
                        if(bits[0] == 1)
                            c.proc.setFlag(icode::FLAG_CARRY, 1);
                        else
                            c.proc.setFlag(icode::FLAG_CARRY, 0);
                        cc = ror(cc, 1);
                        c.poke(c.instruct[ip].op1.op+c.proc.reg_x, cc);
                        if(cc == 0)
                            c.proc.setFlag(icode::FLAG_ZERO, 1);
                        else
                            c.proc.setFlag(icode::FLAG_ZERO, 0);
                        if((char)cc < 0)
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 1);
                        else
                            c.proc.setFlag(icode::FLAG_NEGATIVE, 0);
                    }
                        break;
                    default:
                        return;
                }
            }
                break;
            case icode::op_type::OP_DECIMAL:
                break;
            default:
                break;
        }
    }
    
    void i_rti(Code &c) {
        
    }
    
    void i_rts(Code &c) {
        if(c.in_stack.size()==0) {
            throw Runtime_E("jsr stack underflow.\r\n");
        }
        int top = c.in_stack.back();
        c.in_stack.pop_back();
        c.proc.ip = top;
    }
    // still working on this
    void i_sbc(Code &c) {
        /*
         int in = c.proc.getIp();
         switch(c.instruct[in].op1.op_t) {
         case icode::op_type::OP_MEMORY: {
         switch(c.instruct[in].mode) {
         case interp::ABSOULTE:
         case interp::ZEROPAGE: {
         uint8_t val = c.peek(c.instruct[in].op1.op);
         char total = c.proc.reg_a-val;
         
         c.proc.reg_a = static_cast<unsigned char>(total);
         
         if(total < 0) {
         c.proc.setFlag(icode::FLAG_CARRY, 1);
         } else {
         c.proc.setFlag(icode::FLAG_CARRY, 0);
         }
         if(c.proc.reg_a == 0)
         c.proc.setFlag(icode::FLAG_ZERO, 1);
         else
         c.proc.setFlag(icode::FLAG_ZERO, 0);
         }
         break;
         default:
         return;
         }
         }
         break;
         case icode::op_type::OP_DECIMAL: {
         uint8_t val = c.instruct[in].op1.op;
         int total = c.proc.reg_a + val;
         c.proc.reg_a -= val;
         if(c.proc.getFlag(icode::FLAG_CARRY)) {
         --c.proc.reg_a;
         }
         
         if(total < 0) {
         --c.proc.reg_a;
         c.proc.setFlag(icode::FLAG_CARRY, 1);
         } else {
         c.proc.setFlag(icode::FLAG_CARRY, 0);
         }
         
         if(c.proc.reg_a == 0)
         c.proc.setFlag(icode::FLAG_ZERO, 1);
         else
         c.proc.setFlag(icode::FLAG_ZERO, 0);
         }
         break;
         default:
         break;
         }*/
    }
    
    void i_sec(Code &c) {
        c.proc.setFlag(icode::FLAG_CARRY, 1);
    }
    
    void i_sed(Code &c) {
        c.proc.setFlag(icode::FLAG_DECIMAL, 1);
    }
    
    void i_sei(Code &c) {
        c.proc.setFlag(icode::FLAG_INTERRUPT, 1);
    }
    
    void i_sta(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE:
                        c.poke(c.instruct[in].op1.op, c.proc.reg_a);
                        break;
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X:
                        c.poke(c.instruct[in].op1.op+c.proc.reg_x, c.proc.reg_a);
                        break;
                    case interp::ABSOULTE_Y:
                    case interp::ZEROPAGE_Y:
                        c.poke(c.instruct[in].op1.op+c.proc.reg_y, c.proc.reg_y);
                        break;
                        
                }
                break;
            default:
                break;
        }
    }
    
    void i_stx(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE:
                        c.poke(c.instruct[in].op1.op, c.proc.reg_x);
                        break;
                    case interp::ABSOULTE_Y:
                    case interp::ZEROPAGE_Y:
                        c.poke(c.instruct[in].op1.op+c.proc.reg_y, c.proc.reg_x);
                        break;
                        
                }
                break;
            default:
                break;
        }
    }
    
    void i_sty(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case interp::ABSOULTE:
                    case interp::ZEROPAGE:
                        c.poke(c.instruct[in].op1.op, c.proc.reg_y);
                        break;
                    case interp::ABSOULTE_X:
                    case interp::ZEROPAGE_X:
                        c.poke(c.instruct[in].op1.op+c.proc.reg_x, c.proc.reg_y);
                        break;
                }
                break;
            default:
                break;
        }
    }
    
    void i_tax(Code &c) {
        c.proc.reg_x = c.proc.reg_a;
        if(c.proc.reg_x == 0)
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        else
            c.proc.setFlag(icode::FLAG_ZERO, 0);
    }
    
    void i_tay(Code &c) {
        c.proc.reg_y = c.proc.reg_a;
        if(c.proc.reg_y == 0)
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        else
            c.proc.setFlag(icode::FLAG_ZERO, 0);
    }
    void i_tsx(Code &c) {
        c.proc.reg_x = c.proc.sp;
        if(c.proc.reg_x == 0)
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        else
            c.proc.setFlag(icode::FLAG_ZERO, 0);
    }
    
    void i_txs(Code &c) {
        c.proc.sp = c.proc.reg_x;
    }
    
    void i_txa(Code &c) {
        c.proc.reg_a = c.proc.reg_x;
    }
    
    void i_tya(Code &c) {
        c.proc.reg_a = c.proc.reg_y;
        if(c.proc.reg_a == 0)
            c.proc.setFlag(icode::FLAG_ZERO, 1);
        else
            c.proc.setFlag(icode::FLAG_ZERO, 0);
    }
    
}
