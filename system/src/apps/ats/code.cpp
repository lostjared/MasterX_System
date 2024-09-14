#include "code.hpp"


std::ostringstream stream;

namespace interp {
    
    std::vector<TextLine> lines;
    std::unordered_map<std::string, int> label_table;
    std::unordered_map<int, int> label_line_table;
    
    const char *add_mode[] = { "Non mode", "Immediate", "Zero Page", "Zero Page X", "Zero Page Y", "Absoulte", "Absoulte X", "Absoulte Y", "Implied", "Relative", "Indirect Indexed", "Indexed Indirect", "Accumulator", "Indirect", 0 };
    

    using namespace icode;

    iCodeInstruction inst[] = { {opc::ADC, i_adc }, {opc::AND, i_and}, {opc::ASL, i_asl},
        {opc::BCC, i_bcc}, {opc::BCS, i_bcs}, {opc::BEQ, i_beq}, {opc::BIT, i_bit},
        {opc::BMI, i_bmi}, {opc::BNE, i_bne}, {opc::BPL, i_bpl}, {opc::BRK, i_brk},
        {opc::BVC, i_bvc}, {opc::BVS, i_bvs}, {opc::CLC, i_clc}, {opc::CLD, i_cld},
        {opc::CLI, i_cli}, {opc::CLV, i_clv}, {opc::CMP, i_cmp}, {opc::CPX, i_cpx},
        {opc::CPY, i_cpy}, {opc::DEC, i_dec}, {opc::DEX, i_dex}, {opc::DEY, i_dey},
        {opc::END, i_end},
        {opc::EOR, i_eor}, {opc::INC, i_inc}, {opc::INT, i_int}, {opc::INX, i_inx},
        {opc::INY, i_iny}, {opc::JMP, i_jmp}, {opc::JSR, i_jsr}, {opc::LDA, i_lda},
        {opc::LDM, i_ldm}, {opc::LDX, i_ldx}, {opc::LDY, i_ldy}, {opc::LSR, i_lsr},
        {opc::NOP, i_nop}, {opc::ORA, i_ora}, {opc::PHA, i_pha}, {opc::PHP, i_php},
        {opc::PLA, i_pla}, {opc::PLP, i_plp}, {opc::ROL, i_rol}, {opc::ROR, i_ror},
        {opc::RTI, i_rti}, {opc::RTS, i_rts}, {opc::SBC, i_sbc}, {opc::SEC, i_sec},
        {opc::SED, i_sed}, {opc::SEI, i_sei}, {opc::STA, i_sta}, {opc::STX, i_stx},
        {opc::STY, i_sty}, {opc::TAX, i_tax}, {opc::TAY, i_tay}, {opc::TSX, i_tsx},
        {opc::TXA, i_txa}, {opc::TXS, i_txs}, {opc::TYA, i_tya}
    };
    
    at_code m_code[] = {
        // ADC
        { 0x69, IMMEDIATE, opc::ADC }, { 0x65, ZEROPAGE, opc::ADC },{ 0x75, ZEROPAGE_X, opc::ADC }, { 0x6D, ABSOULTE, opc::ADC }, {0x7D, ABSOULTE_X, opc::ADC }, { 0x79, ABSOULTE_Y, opc::ADC},{ 0x61, INDEXED_I, opc::ADC }, { 0x71, INDIRECT_I, opc::ADC },
        // AND
        { 0x29, IMMEDIATE, opc::AND }, {0x25, ZEROPAGE, opc::AND}, {0x35, ZEROPAGE_X, opc::AND}, {0x2D, ABSOULTE, opc::AND},{ 0x3D, ABSOULTE_X, opc::AND }, {0x39, ABSOULTE_Y, opc::AND }, {0x21, INDEXED_I, opc::AND}, {0x31, INDIRECT_I, opc::AND},
        // ASL
        { 0x0A, ACCUMULATOR, opc::ASL }, {0x06, ZEROPAGE, opc::ASL}, {0x16, ZEROPAGE_X, opc::ASL}, {0x0E, ABSOULTE, opc::ASL},{ 0x1E, ABSOULTE_X, opc::ASL },
        // BCC
        { 0x90, RELATIVE, opc::BCC},
        // BCS
        { 0xB0, RELATIVE, opc::BCS},
        // BEQ
        { 0xF0, RELATIVE, opc::BEQ},
        // BIT
        { 0x24, ZEROPAGE, opc::BIT}, {0x2C, ABSOULTE, opc::BIT},
        // BMI
        { 0x30, RELATIVE, opc::BMI},
        // BNE
        { 0xD0, RELATIVE, opc::BNE},
        // BPL
        { 0x10, RELATIVE, opc::BPL},
        // BRK
        { 0x00, IMPLIED, opc::BRK},
        // BVC
        { 0x50, RELATIVE, opc::BVC},
        // BVS
        { 0x70, RELATIVE, opc::BVC},
        // CLC
        { 0x18, IMPLIED, opc::CLC},
        // CLD
        { 0xD8, IMPLIED, opc::CLD},
        // CLI
        { 0x58, IMPLIED, opc::CLI},
        // CLV
        { 0xB8, IMPLIED, opc::CLV},
        // CMP
        { 0xC9, IMMEDIATE, opc::CMP}, {0xC5, ZEROPAGE, opc::CMP}, { 0xD5, ZEROPAGE_X, opc::CMP }, { 0xCD, ABSOULTE, opc::CMP},{ 0xDD, ABSOULTE_X, opc::CMP}, {0xD9, ABSOULTE_Y, opc::CMP}, {0xD9, ABSOULTE_Y, opc::CMP}, { 0xC1, INDEXED_I, opc::CMP},{ 0xD1, INDIRECT_I, opc::CMP},
        // CPX
        { 0xE0, IMMEDIATE, opc::CPX}, {0xE4, ZEROPAGE, opc::CPX}, {0xEC, ABSOULTE, opc::CPX},
        // CPY
        { 0xC0, IMMEDIATE, opc::CPY}, {0xC4, ZEROPAGE, opc::CPY}, {0xCC, ABSOULTE, opc::CPY},
        // DEC
        { 0xC6, ZEROPAGE, opc::DEC}, {0xD6, ZEROPAGE_X, opc::DEC}, {0xCE, ABSOULTE, opc::DEC}, {0xDE, ABSOULTE_X, opc::DEC},
        // DEX
        { 0xCA, IMPLIED, opc::DEX},
        // DEY
        { 0x88, IMPLIED, opc::DEY},
        // END
        { 0x01, IMPLIED, opc::END},
        // EOR
        { 0x49, IMMEDIATE, opc::EOR}, {0x45, ZEROPAGE, opc::EOR}, {0x55, ZEROPAGE_X, opc::EOR}, {0x4D, ABSOULTE, opc::EOR},{ 0x5D, ABSOULTE_X, opc::EOR}, {0x59, ABSOULTE_Y, opc::EOR}, {0x41, INDEXED_I, opc::EOR}, {0x51, INDIRECT_I, opc::EOR},
        // INC
        {0xE6, ZEROPAGE, opc::INC}, {0xF6, ZEROPAGE_X, opc::INC}, {0xEE, ABSOULTE, opc::INC}, {0xFE, ABSOULTE_X, opc::INC},
        // INX
        {0xE8, IMPLIED, opc::INX},
        // INY
        {0xC8, IMPLIED, opc::INY},
        // JMP
        {0x4C, ABSOULTE, opc::JMP}, {0x6C, INDIRECT, opc::JMP},
        // JSR
        {0x20, ABSOULTE, opc::JSR},
        // LDA
        { 0xA9, IMMEDIATE, opc::LDA}, {0xA5, ZEROPAGE, opc::LDA}, {0xB5, ZEROPAGE_X, opc::LDA}, {0xAD, ABSOULTE, opc::LDA},{ 0xBD, ABSOULTE_X, opc::LDA}, {0xB9, ABSOULTE_Y, opc::LDA}, {0xA1, INDEXED_I, opc::LDA}, {0xB1, INDIRECT_I, opc::LDA},
        // LDX
        {0xA2, IMMEDIATE, opc::LDX}, {0xA6, ZEROPAGE, opc::LDX}, {0xB6, ZEROPAGE_Y, opc::LDX}, {0xAE, ABSOULTE, opc::LDX},{0xA1, INDEXED_I, opc::LDX},{0xB1, INDIRECT_I, opc::LDX}, {0xBE, ABSOULTE_Y, opc::LDX},
        // LDY
        {0xA0, IMMEDIATE, opc::LDY}, {0xA4, ZEROPAGE, opc::LDY}, {0xB4, ZEROPAGE_X, opc::LDY}, {0xAC, ABSOULTE, opc::LDY},{0xBC, ABSOULTE_X, opc::LDY},
        // LSR
        {0x4A, ACCUMULATOR, opc::LSR}, {0x46, ZEROPAGE, opc::LSR}, {0x56, ZEROPAGE_X, opc::LSR}, {0x4E, ABSOULTE, opc::LSR},{0x5E, ABSOULTE_X, opc::LSR},
        // NOP
        {0xEA, IMPLIED, opc::NOP},
        // ORA
        {0x09, IMMEDIATE, opc::ORA}, {0x05, ZEROPAGE, opc::ORA}, {0x15, ZEROPAGE_X, opc::ORA}, {0x0D, ABSOULTE, opc::ORA},{0x1D, ABSOULTE_X, opc::ORA}, {0x19, ABSOULTE_Y, opc::ORA}, {0x01, INDEXED_I, opc::ORA}, {0x11, INDIRECT_I, opc::ORA},
        // PHA
        {0x48, IMPLIED, opc::PHA},
        // PHP
        {0x08, IMPLIED, opc::PHP},
        // PLA
        {0x68, IMPLIED, opc::PLA},
        // PLP
        {0x28, IMPLIED, opc::PLP},
        // ROL
        {0x21, ACCUMULATOR, opc::ROL},{0x26, ZEROPAGE, opc::ROL}, {0x36, ZEROPAGE_X, opc::ROL}, {0x2E, ABSOULTE, opc::ROL},{0x3E, ABSOULTE_X, opc::ROL},
        // ROR
        {0x6A, ACCUMULATOR, opc::ROR}, {0x66, ZEROPAGE, opc::ROR}, {0x76, ZEROPAGE_X, opc::ROR}, {0x6E, ABSOULTE, opc::ROR},{0x7E, ABSOULTE_X, opc::ROR},
        // RTI
        {0x40, IMPLIED, opc::RTI},
        // RTS
        {0x60, IMPLIED, opc::RTS},
        // SBC
        { 0xE9, IMMEDIATE, opc::SBC}, {0xE5, ZEROPAGE, opc::SBC}, {0xF5, ZEROPAGE_X, opc::SBC}, {0xED, ABSOULTE, opc::SBC},{ 0xFD, ABSOULTE_X, opc::SBC}, {0xF9, ABSOULTE_Y, opc::SBC}, {0xE1, INDEXED_I, opc::SBC}, {0xF1, INDIRECT_I, opc::SBC},
        // SEC
        {0x38, IMPLIED, opc::SEC},
        // SED
        {0xF8, IMPLIED, opc::SED},
        // SEI
        {0x78, IMPLIED, opc::SEI},
        // STA
        { 0x85, ZEROPAGE, opc::STA}, {0x95, ZEROPAGE_X, opc::STA}, {0x8D, ABSOULTE, opc::STA}, {0x9D, ABSOULTE_X, opc::STA},{ 0x99, ABSOULTE_Y, opc::STA}, {0x81, INDEXED_I, opc::STA}, {0x91, INDIRECT_I, opc::STA},
        // STX
        { 0x86, ZEROPAGE, opc::STX}, {0x96,  ZEROPAGE_Y, opc::STX}, {0x8E, ABSOULTE, opc::STX},
        // STY
        { 0x84, ZEROPAGE, opc::STY}, {0x94, ZEROPAGE_X, opc::STY}, {0x8C, ABSOULTE, opc::STY},
        // TAX
        { 0xAA, IMPLIED, opc::TAX},
        // TAY
        { 0xA8, IMPLIED, opc::TAY},
        // TSX
        { 0xBA, IMPLIED, opc::TSX},
        // TXA
        { 0x8A, IMPLIED, opc::TXA},
        // TXS
        { 0x9A, IMPLIED, opc::TXS},
        // TYA
        { 0x98, IMPLIED, opc::TYA},
        { 0x0, 0x0, opc::NOTINC }
    };
    
    void testcodes() {
        for(unsigned int i = 0; icode::op_array[i] != 0; ++i) {
            icode::opc c = static_cast<icode::opc>(i);
            if(inst[i].opcode == c) {
                stream << icode::op_array[i] << " correct.\r\n";
            } else {
                stream << icode::op_array[i] << " incorrect.\r\n";
            }
        }
    }
    
    Code::Code() {
        reset_memory();
    }
    
    void Code::start(bool debug) {
        run = true;
        if(!in_stack.empty()) in_stack.erase(in_stack.begin(), in_stack.end());
        proc.clrFlags();
        proc.ip = 0;
        execute(debug);
    }
    
    void Code::stop() {
        run = false;
    }
    
    void Code::end() {
        run = false;
        proc.ip = 0;
         if(!in_stack.empty()) in_stack.erase(in_stack.begin(), in_stack.end());
        if(!stack.empty()) stack.erase(stack.begin(), stack.end());
    }
    
    void Code::pause() {
        run = false;
    }
    
    void Code::execute(bool debug) {
        while(run == true) {
            if(proc.ip >= 0) {
                if(debug == true)
                    printCurrent();
                procInstruct(instruct[proc.ip]);
                if(run == true && debug == true)
                    print();
                ++proc.ip;
                if(proc.ip > instruct.size()) {
                    run = false;
                    proc.ip = 0;
                    stream << "Program end reached without END, use END to terminate your program.\r\n";
                     if(!in_stack.empty()) in_stack.erase(in_stack.begin(), in_stack.end());
                    if(!stack.empty()) stack.erase(stack.begin(), stack.end());
                    break;
                }
            }
        }
    }
    
    
    void Code::cont() {
        if(proc.ip == 0) {
            stream << "You must run your program first with execute.\r\n";
            return;
        }
        run = true;
        execute();
    }
    
    void Code::step() {
        if(instruct.size()==0) return;
        if(proc.ip >= 0 && proc.ip < instruct.size()) {
            if(instruct[proc.ip].opcode == opc::END)
                procInstruct(instruct[proc.ip]);
            else {
                procInstruct(instruct[proc.ip++]);
            }
        } else {
            stream << "Program finished executing..\r\n";
            run = false;
            proc.ip = 0;
        }
    }
    
    
    void Code::printCurrent() {
        if(instruct.size()==0) return;
        if(proc.ip >= 0 && proc.ip < instruct.size())
        	stream << instruct[proc.ip];
    }
    
    void Code::reset() {
        if(!instruct.empty())
            instruct.erase(instruct.begin(), instruct.end());
        
        proc.clrFlags();
        
        if(!stack.empty())
            stack.erase(stack.begin(), stack.end());
        
        proc.reset();
        
        if(!in_stack.empty())
        	in_stack.erase(in_stack.begin(), in_stack.end());
    }
    
    void Code::reset_memory() {
        memset(mem, 0, sizeof(mem));
    }

    void Code::print() {
        stream << "{ ";
        proc.printRegisters();
        proc.printFlags();
        stream << "}\r\n";
    }
    
    void Code::procInstruct(icode::Instruction &i) {
        unsigned int op_code = static_cast<unsigned int>(i.opcode);
        if(op_code < static_cast<int>(icode::opc::NOTINC))
        	inst[op_code].call_back(*this);
    }
    
    
    uint8_t Code::peek(uint16_t address) const {
        return mem[address];
    }
    
    void Code::poke(uint16_t address, uint8_t value) {
        mem[address] = value;
    }
}
