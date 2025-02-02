#include "code.hpp"
#include "translate.hpp"
#include <bitset>

namespace interp {

    void updateZNFlags(icode::Processor &proc, uint8_t value) {
        proc.setFlag(icode::FLAG_ZERO, (value == 0));
        proc.setFlag(icode::FLAG_NEGATIVE, ((value & 0x80) != 0));
    }

    void i_adc(Code &c) {
        int in = c.proc.getIp();
        uint8_t carry = c.proc.getFlag(icode::FLAG_CARRY); // Get carry flag

        switch (c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                uint8_t val = 0;
                switch (c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        val = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
                // Perform 8-bit addition with carry.
                uint16_t total = c.proc.reg_a + val + carry;
                c.proc.setFlag(icode::FLAG_CARRY, (total > 0xFF));

                // Set Overflow flag if signed overflow occurs.
                bool A7 = (c.proc.reg_a & 0x80);
                bool M7 = (val & 0x80);
                bool R7 = ((static_cast<uint8_t>(total) & 0x80) != 0);
                c.proc.setFlag(icode::FLAG_OVERFLOW, ((A7 == M7) && (A7 != R7)));

                c.proc.reg_a = static_cast<uint8_t>(total & 0xFF);
                updateZNFlags(c.proc, c.proc.reg_a);
            }
            break;

            case icode::op_type::OP_DECIMAL: {
                // Handle Decimal Mode (BCD Arithmetic) if required
                uint8_t val = c.instruct[in].op1.op;
                uint16_t total = c.proc.reg_a + val + carry;
                if (c.proc.getFlag(icode::FLAG_DECIMAL)) {
                    uint8_t lo = (c.proc.reg_a & 0x0F) + (val & 0x0F) + carry;
                    if (lo > 9) lo += 6;
                    uint8_t hi = (c.proc.reg_a >> 4) + (val >> 4) + (lo > 0x0F ? 1 : 0);
                    if (hi > 9) hi += 6;
                    total = (hi << 4) | (lo & 0x0F);
                    c.proc.setFlag(icode::FLAG_CARRY, (hi > 0x0F));
                } else {
                    c.proc.setFlag(icode::FLAG_CARRY, (total > 0xFF));
                }
                bool A7 = (c.proc.reg_a & 0x80);
                bool M7 = (val & 0x80);
                bool R7 = (total & 0x80);
                c.proc.setFlag(icode::FLAG_OVERFLOW, ((A7 == M7) && (A7 != R7)));
                c.proc.reg_a = static_cast<uint8_t>(total & 0xFF);
                updateZNFlags(c.proc, c.proc.reg_a);
            }
            break;

            default:
                break;
        }
    }

    void i_sbc(Code &c) {
        int in = c.proc.getIp();
        uint8_t carry = c.proc.getFlag(icode::FLAG_CARRY); // Note: carry is used inverted in SBC

        switch (c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                uint8_t val = 0;
                switch (c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        val = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
       
                uint16_t total = c.proc.reg_a - val - (1 - carry);
                c.proc.setFlag(icode::FLAG_CARRY, (c.proc.reg_a >= (val + (1 - carry))));

                bool A7 = (c.proc.reg_a & 0x80);
                bool M7 = (val & 0x80);
                bool R7 = (total & 0x80);
                c.proc.setFlag(icode::FLAG_OVERFLOW, ((A7 != M7) && (A7 != R7)));

                c.proc.reg_a = static_cast<uint8_t>(total & 0xFF);
                updateZNFlags(c.proc, c.proc.reg_a);
            }
            break;

            case icode::op_type::OP_DECIMAL: {
                uint8_t val = c.instruct[in].op1.op;
                uint16_t total = c.proc.reg_a - val - (1 - carry);
                if (c.proc.getFlag(icode::FLAG_DECIMAL)) {
                    uint8_t lo = (c.proc.reg_a & 0x0F) - (val & 0x0F) - (1 - carry);
                    if (lo & 0x10) lo -= 6;
                    uint8_t hi = (c.proc.reg_a >> 4) - (val >> 4) - ((lo & 0x10) ? 1 : 0);
                    if (hi & 0x10) hi -= 6;
                    total = (hi << 4) | (lo & 0x0F);
                    c.proc.setFlag(icode::FLAG_CARRY, !(hi & 0x10));
                } else {
                    c.proc.setFlag(icode::FLAG_CARRY, (c.proc.reg_a >= (val + (1 - carry))));
                }
                bool A7 = (c.proc.reg_a & 0x80);
                bool M7 = (val & 0x80);
                bool R7 = (total & 0x80);
                c.proc.setFlag(icode::FLAG_OVERFLOW, ((A7 != M7) && (A7 != R7)));
                c.proc.reg_a = static_cast<uint8_t>(total & 0xFF);
                updateZNFlags(c.proc, c.proc.reg_a);
            }
            break;

            default:
                break;
        }
    }

    void i_and(Code &c) {
        int in = c.proc.getIp();
        uint8_t val = 0;
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        val = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                    case ZEROPAGE_Y:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
            }
            break;
            case icode::op_type::OP_DECIMAL: {
                val = c.instruct[in].op1.op;
            }
            break;
            default:
                return;
        }
        c.proc.reg_a &= val;
        updateZNFlags(c.proc, c.proc.reg_a);
    }

    void i_asl(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                uint8_t newCarry = (cc & 0x80) ? 1 : 0;
                cc <<= 1;
                c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                updateZNFlags(c.proc, cc);
            }
            break;
            case icode::op_type::OP_MEMORY: {
                uint8_t addr = c.instruct[ip].op1.op;
                switch(c.instruct[ip].mode) {
                    case ABSOULTE:
                    case ZEROPAGE: {
                        uint8_t cc = c.peek(addr);
                        uint8_t newCarry = (cc & 0x80) ? 1 : 0;
                        cc <<= 1;
                        c.poke(addr, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X: {
                        uint8_t addrX = addr + c.proc.reg_x;
                        uint8_t cc = c.peek(addrX);
                        uint8_t newCarry = (cc & 0x80) ? 1 : 0;
                        cc <<= 1;
                        c.poke(addrX, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    default:
                        return;
                }
            }
            break;
            default:
                break;
        }
    }

    void i_bcc(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_CARRY) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_bcs(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_CARRY) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_beq(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_ZERO) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_bit(Code &c) {
        int in = c.proc.getIp();
        
        if (c.instruct[in].op1.op_t != icode::op_type::OP_MEMORY) {
            return;
        }   
        uint8_t operand = 0;
        switch (c.instruct[in].mode) {
            case interp::ABSOULTE:
            case interp::ZEROPAGE:
                operand = c.peek(c.instruct[in].op1.op);
                break;
            default:
                return;
        }
        uint8_t result = c.proc.reg_a & operand;
        c.proc.setFlag(icode::FLAG_ZERO, (result == 0));
        c.proc.setFlag(icode::FLAG_NEGATIVE, ((operand & 0x80) != 0));
        c.proc.setFlag(icode::FLAG_OVERFLOW, ((operand & 0x40) != 0));
    }

    void i_bmi(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_NEGATIVE) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_bne(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_ZERO) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_bpl(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_NEGATIVE) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_brk(Code &c) {
        c.stop();
        std::cout << "Breakpoint occurred\n";
    }

    void i_bvc(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_OVERFLOW) == 0) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
        }
    }

    void i_bvs(Code &c) {
        unsigned int ip = c.proc.getIp();
        if(c.proc.getFlag(icode::FLAG_OVERFLOW) == 1) {
            c.proc.ip = c.instruct[ip].op1.label_index - 1;
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

    void i_cmp(Code &c) {
        int in = c.proc.getIp();
        uint8_t operand = 0;
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        operand = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        operand = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                    case ZEROPAGE_Y:
                        operand = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
                break;
            case icode::op_type::OP_DECIMAL:
                operand = c.instruct[in].op1.op;
                break;
            default:
                return;
        }
        uint8_t result = c.proc.reg_a - operand;
        c.proc.setFlag(icode::FLAG_CARRY, (c.proc.reg_a >= operand));
        updateZNFlags(c.proc, result);
    }

    void i_cpx(Code &c) {
        int in = c.proc.getIp();
        uint8_t operand = 0;
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        operand = c.peek(c.instruct[in].op1.op);
                        break;
                    default:
                        return;
                }
                break;
            case icode::op_type::OP_DECIMAL:
                operand = c.instruct[in].op1.op;
                break;
            default:
                return;
        }
        uint8_t result = c.proc.reg_x - operand;
        c.proc.setFlag(icode::FLAG_CARRY, (c.proc.reg_x >= operand));
        updateZNFlags(c.proc, result);
    }

    void i_cpy(Code &c) {
        int in = c.proc.getIp();
        uint8_t operand = 0;
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        operand = c.peek(c.instruct[in].op1.op);
                        break;
                    default:
                        return;
                }
                break;
            case icode::op_type::OP_DECIMAL:
                operand = c.instruct[in].op1.op;
                break;
            default:
                return;
        }
        uint8_t result = c.proc.reg_y - operand;
        c.proc.setFlag(icode::FLAG_CARRY, (c.proc.reg_y >= operand));
        updateZNFlags(c.proc, result);
    }

    void i_dec(Code &c) {
        int in = c.proc.getIp();
        uint8_t addr = c.instruct[in].op1.op;
        switch(c.instruct[in].mode) {
            case ABSOULTE:
            case ZEROPAGE: {
                uint8_t value = c.peek(addr) - 1;
                c.poke(addr, value);
                updateZNFlags(c.proc, value);
            }
            break;
            case ABSOULTE_X:
            case ZEROPAGE_X: {
                uint8_t addrX = addr + c.proc.reg_x;
                uint8_t value = c.peek(addrX) - 1;
                c.poke(addrX, value);
                updateZNFlags(c.proc, value);
            }
            break;
            default:
                return;
        }
    }

    void i_dex(Code &c) {
        c.proc.reg_x = c.proc.reg_x - 1;
        updateZNFlags(c.proc, c.proc.reg_x);
    }

    void i_dey(Code &c) {
        c.proc.reg_y = c.proc.reg_y - 1;
        updateZNFlags(c.proc, c.proc.reg_y);
    }

    void i_end(Code &c) {
        c.end();
        std::cout << "Program stopped, END reached. Program reset\n";
    }

    void i_eor(Code &c) {
        int in = c.proc.getIp();
        uint8_t val = 0;
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        val = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
            }
            break;
            case icode::op_type::OP_DECIMAL:
                val = c.instruct[in].op1.op;
                break;
            default:
                return;
        }
        c.proc.reg_a ^= val;
        updateZNFlags(c.proc, c.proc.reg_a);
    }

    void i_inc(Code &c) {
        int in = c.proc.getIp();
        uint8_t addr = c.instruct[in].op1.op;
        switch(c.instruct[in].mode) {
            case ABSOULTE:
            case ZEROPAGE: {
                uint8_t value = c.peek(addr) + 1;
                c.poke(addr, value);
                updateZNFlags(c.proc, value);
            }
            break;
            case ABSOULTE_X:
            case ZEROPAGE_X: {
                uint8_t addrX = addr + c.proc.reg_x;
                uint8_t value = c.peek(addrX) + 1;
                c.poke(addrX, value);
                updateZNFlags(c.proc, value);
            }
            break;
            default:
                return;
        }
    }

    void i_int(Code &c) {
        // To be implemented if needed.
    }

    void i_inx(Code &c) {
        c.proc.reg_x = c.proc.reg_x + 1;
        updateZNFlags(c.proc, c.proc.reg_x);
    }

    void i_iny(Code &c) {
        c.proc.reg_y = c.proc.reg_y + 1;
        updateZNFlags(c.proc, c.proc.reg_y);
    }

    void i_jmp(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_LABELTEXT:
            case icode::op_type::OP_LABEL:
                c.proc.ip = c.instruct[in].op1.label_index - 1;
                return;
            default:
                break;
        }
    }

    void i_jsr(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_LABELTEXT:
            case icode::op_type::OP_LABEL:
                c.in_stack.push_back(c.proc.ip);
                c.proc.ip = c.instruct[in].op1.label_index - 1;
                break;
            default:
                break;
        }
    }

    void i_lda(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        c.proc.reg_a = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        c.proc.reg_a = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                    case ZEROPAGE_Y:
                        c.proc.reg_a = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
                updateZNFlags(c.proc, c.proc.reg_a);
                break;
            case icode::op_type::OP_DECIMAL:
                c.proc.reg_a = c.instruct[in].op1.op;
                updateZNFlags(c.proc, c.proc.reg_a);
                break;
            default:
                break;
        }
    }

    void i_ldm(Code &c) {
        // To be implemented.
    }

    void i_ldx(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        c.proc.reg_x = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_Y:
                    case ZEROPAGE_Y:
                        c.proc.reg_x = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
                updateZNFlags(c.proc, c.proc.reg_x);
                break;
            case icode::op_type::OP_DECIMAL:
                c.proc.reg_x = c.instruct[in].op1.op;
                updateZNFlags(c.proc, c.proc.reg_x);
                break;
            default:
                break;
        }
    }

    void i_ldy(Code &c) {
        int in = c.proc.getIp();
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY:
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        c.proc.reg_y = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        c.proc.reg_y = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    default:
                        return;
                }
                updateZNFlags(c.proc, c.proc.reg_y);
                break;
            case icode::op_type::OP_DECIMAL:
                c.proc.reg_y = c.instruct[in].op1.op;
                updateZNFlags(c.proc, c.proc.reg_y);
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
                uint8_t newCarry = (cc & 0x01) ? 1 : 0;
                cc >>= 1;
                c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                updateZNFlags(c.proc, cc);
            }
            break;
            case icode::op_type::OP_MEMORY: {
                uint8_t addr = c.instruct[ip].op1.op;
                switch(c.instruct[ip].mode) {
                    case ABSOULTE:
                    case ZEROPAGE: {
                        uint8_t cc = c.peek(addr);
                        uint8_t newCarry = (cc & 0x01) ? 1 : 0;
                        cc >>= 1;
                        c.poke(addr, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X: {
                        uint8_t addrX = addr + c.proc.reg_x;
                        uint8_t cc = c.peek(addrX);
                        uint8_t newCarry = (cc & 0x01) ? 1 : 0;
                        cc >>= 1;
                        c.poke(addrX, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    default:
                        return;
                }
            }
            break;
            default:
                break;
        }
    }

    void i_nop(Code &c) {
        std::cout << "nop\n";
    }

    void i_ora(Code &c) {
        int in = c.proc.getIp();
        uint8_t val = 0;
        switch(c.instruct[in].op1.op_t) {
            case icode::op_type::OP_MEMORY: {
                switch(c.instruct[in].mode) {
                    case ABSOULTE:
                    case ZEROPAGE:
                        val = c.peek(c.instruct[in].op1.op);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                        val = c.peek(c.instruct[in].op1.op + c.proc.reg_y);
                        break;
                    default:
                        return;
                }
            }
            break;
            case icode::op_type::OP_DECIMAL: {
                val = c.instruct[in].op1.op;
            }
            break;
            default:
                return;
        }
        c.proc.reg_a |= val;
        updateZNFlags(c.proc, c.proc.reg_a);
    }

    void i_pha(Code &c) {
        c.stack.push_back(c.proc.reg_a);
    }

    void i_php(Code &c) {
        c.stack.push_back(c.proc.valFlags());
    }

    void i_pla(Code &c) {
        if(c.stack.empty()) {
            throw Runtime_E("Error: stack underflow");
        }
        uint8_t accum = c.stack.back();
        c.stack.pop_back();
        c.proc.reg_a = accum;
        updateZNFlags(c.proc, c.proc.reg_a);
    }

    void i_plp(Code &c) {
        if(c.stack.empty()) {
            throw Runtime_E("Error: stack underflow");
        }
        uint8_t flags = c.stack.back();
        c.stack.pop_back();
        c.proc.setFlags(flags);
    }

    // ROL (Rotate Left through Carry)
    void i_rol(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                uint8_t oldCarry = c.proc.getFlag(icode::FLAG_CARRY);
                uint8_t newCarry = (cc & 0x80) ? 1 : 0;
                cc = (cc << 1) | oldCarry;
                c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                updateZNFlags(c.proc, cc);
            }
            break;
            case icode::op_type::OP_MEMORY: {
                uint8_t addr = c.instruct[ip].op1.op;
                switch(c.instruct[ip].mode) {
                    case ABSOULTE:
                    case ZEROPAGE: {
                        uint8_t cc = c.peek(addr);
                        uint8_t oldCarry = c.proc.getFlag(icode::FLAG_CARRY);
                        uint8_t newCarry = (cc & 0x80) ? 1 : 0;
                        cc = (cc << 1) | oldCarry;
                        c.poke(addr, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X: {
                        uint8_t addrX = addr + c.proc.reg_x;
                        uint8_t cc = c.peek(addrX);
                        uint8_t oldCarry = c.proc.getFlag(icode::FLAG_CARRY);
                        uint8_t newCarry = (cc & 0x80) ? 1 : 0;
                        cc = (cc << 1) | oldCarry;
                        c.poke(addrX, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    default:
                        return;
                }
            }
            break;
            default:
                break;
        }
    }

    // ROR (Rotate Right through Carry)
    void i_ror(Code &c) {
        unsigned int ip = c.proc.getIp();
        switch(c.instruct[ip].op1.op_t) {
            case icode::op_type::OP_REGISTER_A: {
                uint8_t &cc = c.proc.reg_a;
                uint8_t oldCarry = c.proc.getFlag(icode::FLAG_CARRY);
                uint8_t newCarry = (cc & 0x01) ? 1 : 0;
                cc = (cc >> 1) | (oldCarry << 7);
                c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                updateZNFlags(c.proc, cc);
            }
            break;
            case icode::op_type::OP_MEMORY: {
                uint8_t addr = c.instruct[ip].op1.op;
                switch(c.instruct[ip].mode) {
                    case ABSOULTE:
                    case ZEROPAGE: {
                        uint8_t cc = c.peek(addr);
                        uint8_t oldCarry = c.proc.getFlag(icode::FLAG_CARRY);
                        uint8_t newCarry = (cc & 0x01) ? 1 : 0;
                        cc = (cc >> 1) | (oldCarry << 7);
                        c.poke(addr, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X: {
                        uint8_t addrX = addr + c.proc.reg_x;
                        uint8_t cc = c.peek(addrX);
                        uint8_t oldCarry = c.proc.getFlag(icode::FLAG_CARRY);
                        uint8_t newCarry = (cc & 0x01) ? 1 : 0;
                        cc = (cc >> 1) | (oldCarry << 7);
                        c.poke(addrX, cc);
                        c.proc.setFlag(icode::FLAG_CARRY, newCarry);
                        updateZNFlags(c.proc, cc);
                    }
                    break;
                    default:
                        return;
                }
            }
            break;
            default:
                break;
        }
    }

    void i_rti(Code &c) {
        if (c.stack.size() < 3) {
            throw Runtime_E("RTI: Stack underflow");
        }
    
        uint8_t status = c.stack.back();
        c.stack.pop_back();
        uint8_t pcl = c.stack.back();
        c.stack.pop_back();
        uint8_t pch = c.stack.back();
        c.stack.pop_back();
        c.proc.setFlags(status);
        c.proc.ip = (static_cast<uint16_t>(pch) << 8) | pcl;
    }

    void i_rts(Code &c) {
        if(c.in_stack.empty()) {
            throw Runtime_E("JSR stack underflow.\n");
        }
        int top = c.in_stack.back();
        c.in_stack.pop_back();
        c.proc.ip = top;
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
                    case ABSOULTE:
                    case ZEROPAGE:
                        c.poke(c.instruct[in].op1.op, c.proc.reg_a);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        c.poke(c.instruct[in].op1.op + c.proc.reg_x, c.proc.reg_a);
                        break;
                    case ABSOULTE_Y:
                    case ZEROPAGE_Y:
                        c.poke(c.instruct[in].op1.op + c.proc.reg_y, c.proc.reg_a);
                        break;
                    default:
                        return;
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
                    case ABSOULTE:
                    case ZEROPAGE:
                        c.poke(c.instruct[in].op1.op, c.proc.reg_x);
                        break;
                    case ABSOULTE_Y:
                    case ZEROPAGE_Y:
                        c.poke(c.instruct[in].op1.op + c.proc.reg_y, c.proc.reg_x);
                        break;
                    default:
                        return;
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
                    case ABSOULTE:
                    case ZEROPAGE:
                        c.poke(c.instruct[in].op1.op, c.proc.reg_y);
                        break;
                    case ABSOULTE_X:
                    case ZEROPAGE_X:
                        c.poke(c.instruct[in].op1.op + c.proc.reg_x, c.proc.reg_y);
                        break;
                    default:
                        return;
                }
                break;
            default:
                break;
        }
    }

    void i_tax(Code &c) {
        c.proc.reg_x = c.proc.reg_a;
        updateZNFlags(c.proc, c.proc.reg_x);
    }

    void i_tay(Code &c) {
        c.proc.reg_y = c.proc.reg_a;
        updateZNFlags(c.proc, c.proc.reg_y);
    }

    void i_tsx(Code &c) {
        c.proc.reg_x = c.proc.sp;
        updateZNFlags(c.proc, c.proc.reg_x);
    }

    void i_txs(Code &c) {
        c.proc.sp = c.proc.reg_x;
    }

    void i_txa(Code &c) {
        c.proc.reg_a = c.proc.reg_x;
        updateZNFlags(c.proc, c.proc.reg_a);
    }

    void i_tya(Code &c) {
        c.proc.reg_a = c.proc.reg_y;
        updateZNFlags(c.proc, c.proc.reg_a);
    }

} 
