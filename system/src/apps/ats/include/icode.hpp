#ifndef __ICODE__H__
#define __ICODE__H__

#include<iostream>
#include<string>
#include<cstdint>
#include<bitset>
#include<sstream>

namespace icode {
    
    extern const char *op_array[];
    
    enum class opc { ADC=0, AND, ASL, BCC, BCS, BEQ, BIT, BMI, BNE, BPL, BRK, BVC, BVS, CLC, CLD, CLI, CLV, CMP, CPX, CPY, DEC, DEX, DEY, END, EOR, INC, INT, INX, INY, JMP, JSR, LDA, LDM, LDX, LDY, LSR, NOP, ORA, PHA, PHP, PLA, PLP,ROL, ROR, RTI, RTS, SBC, SEC, SED, SEI, STA, STX, STY, TAX, TAY, TSX, TXA, TXS, TYA, NOTINC };
    
    opc strtoInc(const std::string &inc);
    std::string lcase(const std::string &text);
    uint32_t toHex(const std::string &text);
    std::ostream &operator<<(std::ostream &out, opc c);
    
    enum class op_type { NULL_TYPE=0, OP_MEMORY, OP_DECIMAL, OP_REGISTER_A, OP_REGISTER_X, OP_REGISTER_Y, OP_LABEL, OP_LABELTEXT };
    
    class Operand {
    public:
        uint32_t op;
        op_type  op_t;
        std::string label_text;
        int label_index;
        Operand();
        Operand(const uint32_t &operand, const op_type &operand_type);
        Operand(const Operand &o);
        Operand &operator=(const Operand &o);
    };
    
    class Instruction {
    public:
        std::string instruction_text;
        unsigned char op_byte;
        unsigned int line_num;
        opc opcode;
        Operand op1, op2;
        unsigned int mode;
        bool label;
        std::string label_text;
        int label_index;
        std::string text;
        Instruction();
        Instruction(unsigned int iline, const opc &op_code, unsigned int m, const Operand &i_op1, const Operand &i_op2);
        Instruction(const Instruction &i);
        Instruction &operator=(const Instruction &i);
        void set(unsigned int iline, const opc &c, unsigned int m, const Operand &o1, const Operand &o2);
        
    };
    
    std::ostream &operator<<(std::ostream &out, Instruction &i);
    std::string instructionToHex(Instruction &i);
    std::string instructionToBinary(Instruction &i);
    
    enum proc_Flags { FLAG_CARRY=0, FLAG_ZERO, FLAG_INTERRUPT, FLAG_DECIMAL, FLAG_BREAK, FLAG_EXPANSION, FLAG_OVERFLOW, FLAG_NEGATIVE };
    
    
    typedef uint8_t reg_size;
    typedef uint16_t address_size;
    
    class Processor {
        std::bitset<8> flags;
        /* 
         C = 0 - carry bit
         Z = 1 - zero bit
         I = 2 - interrupt disable bit
         D = 3 - decimal mode bit
         B = 4 - break command bit
         5 = [expansion bit [not used]
         V = 6 - overflow bit
         N = 7 - negative bit
        */
        
    public:
        reg_size reg_x, reg_y, reg_a;
        address_size sp;
        address_size ip;
        
        Processor();
        void setFlag(proc_Flags flag, unsigned int set);
        void setFlags(uint8_t flag);
        unsigned int getFlag(proc_Flags flag);
        void clrFlags();
        uint8_t valFlags();
        void reset();
        void setIp(const address_size &ip_val);
        address_size getIp() const;
        void printFlags();
        void printRegisters();
    };
}

#endif
