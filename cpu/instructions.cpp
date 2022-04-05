#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_map>
#include "cpu.h"

CPU::instructions::instructions() {
    opcode_to_inst = opcode_map();
}

void CPU::instructions::run_instruction(CPU &cpu, unsigned char opcode) {
    if (opcode_to_inst.find(opcode) == opcode_to_inst.end()) {
            std::stringstream error;
            error << std::cout << "\nOpcode " << std::setw(2) << std::setfill('0') << std::hex << (int) opcode << " not implemented or invalid!";
            throw std::runtime_error(error.str());
    };
    opcode_to_inst[opcode](cpu);
}

void CPU::instructions::AAX_ZP(CPU &cpu)  {
    cpu.mem_store(cpu.ZP_addressing() ,cpu.A & cpu.X);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::AAX_ZPY(CPU &cpu)  {
    cpu.mem_store(cpu.ZPY_addressing() ,cpu.A & cpu.X);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::AAX_IX(CPU &cpu)  {
    cpu.mem_store(cpu.IX_addressing() ,cpu.A & cpu.X);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::AAX_A(CPU &cpu)  {
    cpu.mem_store(cpu.A_addressing() ,cpu.A & cpu.X);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ADC(CPU &cpu, unsigned char operand) {
    unsigned short int add = cpu.A + operand + cpu.get_carry();

    if (add > 255) cpu.set_carry();
    else cpu.clear_carry();

    if (cpu.A < 128 and operand < 128 and (add & 0xFF) >= 128) cpu.set_overflow();
    else if (cpu.A >= 128 and operand >= 128 and (add & 0xFF) < 128) cpu.set_overflow();
    else cpu.clear_overflow();

    cpu.A = add & 0xFF;

    cpu.set_ZN(cpu.A);
}

void CPU::instructions:: ADC_I(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::ADC_ZP(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::ADC_ZPX(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::ADC_A(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ADC_AX(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ADC_AY(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ADC_IX(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::ADC_IY(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::AND(CPU &cpu, unsigned char operand) {
    cpu.A = cpu.A & operand;

    cpu.set_ZN(cpu.A);
}

void CPU::instructions::AND_I(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::AND_ZP(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::AND_ZPX(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::AND_A(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::AND_AX(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::AND_AY(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::AND_IX(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::AND_IY(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::ASL_AC(CPU &cpu) {
    if (cpu.A >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.A = cpu.A << 1;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::ASL_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::ASL_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::ASL_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::ASL_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

void CPU::instructions::BCC(CPU &cpu) {
    if (cpu.get_carry() == 0) {
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BCS(CPU &cpu) {
    if (cpu.get_carry() == 1) { 
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BEQ(CPU &cpu) {
    if (cpu.get_zero() == 1) { 
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BIT(CPU &cpu, unsigned char operand) {
    if ((bool)(operand & 0x40) == 1) cpu.set_overflow();
    else cpu.clear_overflow();

    if ((bool)(operand & 0x80) == 1) cpu.set_negative();
    else cpu.clear_negative();

    if ((bool)(operand & cpu.A) == 0) cpu.set_zero();
    else cpu.clear_zero();
}

void CPU::instructions::BIT_ZP(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.ZP_addressing());

    BIT(cpu, operand);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::BIT_A(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.A_addressing());

    BIT(cpu, operand);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::BMI(CPU &cpu) {
    if (cpu.get_negative() == 1) { 
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BNE(CPU &cpu) {
    if (cpu.get_zero() == 0) { 
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BPL(CPU &cpu) {
    if (cpu.get_negative() == 0) {
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BRK(CPU &cpu) {
    cpu.stack_push_16bit(cpu.PC);
    cpu.stack_push(cpu.STATUS);
    cpu.set_brk();
    cpu.PC = cpu.mem(0xFFFF)*256 + cpu.mem(0xFFFE);

    cpu.cycles += 7;
    cpu.PC += 1;
}

void CPU::instructions::BVC(CPU &cpu) {
    if (cpu.get_overflow() == 0) {
        if (cpu.branch_page_cross()) cpu.cycles++; 
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::BVS(CPU &cpu) {
    if (cpu.get_overflow() == 1) { 
        if (cpu.branch_page_cross()) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::CLC(CPU &cpu) {
    cpu.clear_carry();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::CLD(CPU &cpu) {
    cpu.clear_decimal();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::CLI(CPU &cpu) {
    cpu.clear_interrupt_disable();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::CLV(CPU &cpu) {
    cpu.clear_overflow();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::CMP(CPU &cpu, unsigned char operand) {
    if (cpu.A >= operand) {
        cpu.set_carry();
    }
    else cpu.clear_carry();

    cpu.set_ZN(cpu.A - operand);
}

void CPU::instructions::CMP_I(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::CMP_ZP(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::CMP_ZPX(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::CMP_A(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::CMP_AX(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::CMP_AY(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::CMP_IX(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::CMP_IY(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::CPX(CPU &cpu, unsigned char operand) {
    if (cpu.X >= operand) {
        cpu.set_carry();
    }
    else cpu.clear_carry();
    
    cpu.set_ZN(cpu.X - operand);
}

void CPU::instructions::CPX_I(CPU &cpu) {
    CPX(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::CPX_ZP(CPU &cpu) {
    CPX(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::CPX_A(CPU &cpu) {
    CPX(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::CPY(CPU &cpu, unsigned char operand) {
    if (cpu.Y >= operand) {
        cpu.set_carry();
    }
    else cpu.clear_carry();
    
    cpu.set_ZN(cpu.Y - operand);
}

void CPU::instructions::CPY_I(CPU &cpu) {
    CPY(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::CPY_ZP(CPU &cpu) {
    CPY(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::CPY_A(CPU &cpu) {
    CPY(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::DCP(CPU &cpu, unsigned short int address) {
    DEC(cpu, address);
    CMP(cpu, cpu.mem(address));
}

void CPU::instructions::DCP_ZP(CPU &cpu) {
    DCP(cpu, cpu.ZP_addressing());

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::DCP_ZPX(CPU &cpu) {
    DCP(cpu, cpu.ZPX_addressing());

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::DCP_A(CPU &cpu) {
    DCP(cpu, cpu.A_addressing());

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::DCP_AX(CPU &cpu) {
    DCP(cpu, cpu.AX_addressing());

    cpu.cycles += 7;
    cpu.PC += 3;
}

void CPU::instructions::DCP_AY(CPU &cpu) {
    DCP(cpu, cpu.AY_addressing());

    cpu.cycles += 7;
    cpu.PC += 3;
}
void CPU::instructions::DCP_IX(CPU &cpu) {
    DCP(cpu, cpu.IX_addressing());

    cpu.cycles += 8;
    cpu.PC += 2;
}

void CPU::instructions::DCP_IY(CPU &cpu) {
    DCP(cpu, cpu.IY_addressing());

    cpu.cycles += 8;
    cpu.PC += 2;
}

void CPU::instructions::DEC(CPU &cpu, unsigned short int address) {
    cpu.mem_store(address, cpu.mem(address)-1);

    cpu.set_ZN(cpu.mem(address));
}

void CPU::instructions::DEC_ZP(CPU &cpu) {
    DEC(cpu, cpu.ZP_addressing());

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::DEC_ZPX(CPU &cpu) {
    DEC(cpu, cpu.ZPX_addressing());

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::DEC_A(CPU &cpu) {
    DEC(cpu, cpu.A_addressing());

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::DEC_AX(CPU &cpu) {
    DEC(cpu, cpu.AX_addressing());

    cpu.cycles += 7;
    cpu.PC += 3;
}

void CPU::instructions::DEX(CPU &cpu) {
    cpu.X--;
    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::DEY(CPU &cpu) {
    cpu.Y--;
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::DOP_I(CPU &cpu) {
    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::DOP_ZP(CPU &cpu) {
    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::DOP_ZPX(CPU &cpu) {
    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::EOR(CPU &cpu, unsigned char operand) {
    cpu.A = cpu.A ^ operand;
    cpu.set_ZN(cpu.A);
}

void CPU::instructions::EOR_I(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::EOR_ZP(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::EOR_ZPX(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::EOR_A(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::EOR_AX(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::EOR_AY(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::EOR_IX(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::EOR_IY(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::INC(CPU &cpu, unsigned short int address) {
    cpu.mem_store(address, cpu.mem(address)+1);
    cpu.set_ZN(cpu.mem(address));
}

void CPU::instructions::INC_ZP(CPU &cpu) {
    INC(cpu, cpu.ZP_addressing());

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::INC_ZPX(CPU &cpu) {
    INC(cpu, cpu.ZPX_addressing());

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::INC_A(CPU &cpu) {
    INC(cpu, cpu.A_addressing());

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::INC_AX(CPU &cpu) {
    INC(cpu, cpu.AX_addressing());

    cpu.cycles += 7;
    cpu.PC += 3;
}

void CPU::instructions::INX(CPU &cpu) {
    cpu.X++;
    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::INY(CPU &cpu) {
    cpu.Y++;
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::JMP_A(CPU &cpu) {
    cpu.PC = cpu.A_addressing();

    cpu.cycles += 3;
}

void CPU::instructions::JMP_I(CPU &cpu) {
    if ((cpu.mem(cpu.PC+1) & 0xFF) == 0xFF) {
        // Implements JMP instruction bug.
        cpu.PC = cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(0x00))*256 + 
                 cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(cpu.PC+1));
    }
    else cpu.PC = cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(cpu.PC+1)+1)*256 + 
                  cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(cpu.PC+1));

    cpu.cycles += 5;
}

void CPU::instructions::JSR(CPU &cpu) {
    cpu.stack_push_16bit(cpu.PC+2);
    cpu.PC = cpu.A_addressing();

    cpu.cycles += 6;
}

void CPU::instructions::LAX(CPU &cpu, unsigned char operand) {
    cpu.A = operand;
    cpu.X = operand;

    cpu.set_ZN((char) operand);
}

void CPU::instructions::LAX_ZP(CPU &cpu) {
    LAX(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::LAX_ZPY(CPU &cpu) {
    LAX(cpu, cpu.mem(cpu.ZPY_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::LAX_A(CPU &cpu) {
    LAX(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LAX_AY(CPU &cpu) {
    LAX(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LAX_IX(CPU &cpu) {
    LAX(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::LAX_IY(CPU &cpu) {
    LAX(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::LDA_I(CPU &cpu) {
    cpu.A = cpu.mem(cpu.PC+1);
    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::LDA_ZP(CPU &cpu) {
    cpu.A = cpu.mem(cpu.ZP_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::LDA_ZPX(CPU &cpu) {
    cpu.A = cpu.mem(cpu.ZPX_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::LDA_A(CPU &cpu) {
    cpu.A = cpu.mem(cpu.A_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LDA_AX(CPU &cpu) {
    cpu.A = cpu.mem(cpu.AX_addressing());
    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LDA_AY(CPU &cpu) {
    cpu.A = cpu.mem(cpu.AY_addressing());
    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LDA_IX(CPU &cpu) {
    cpu.A = cpu.mem(cpu.IX_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::LDA_IY(CPU &cpu) {
    cpu.A = cpu.mem(cpu.IY_addressing());
    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::LDX_I(CPU &cpu) {
    cpu.X = cpu.mem(cpu.PC+1);
    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::LDX_ZP(CPU &cpu) {
    cpu.X = cpu.mem(cpu.ZP_addressing());
    cpu.set_ZN(cpu.X);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::LDX_ZPY(CPU &cpu) {
    cpu.X = cpu.mem(cpu.ZPY_addressing());
    cpu.set_ZN(cpu.X);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::LDX_A(CPU &cpu) {
    cpu.X = cpu.mem(cpu.A_addressing());
    cpu.set_ZN(cpu.X);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LDX_AY(CPU &cpu) {
    cpu.X = cpu.mem(cpu.AY_addressing());
    cpu.set_ZN(cpu.X);

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LDY_I(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.PC+1);
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::LDY_ZP(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.ZP_addressing());
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::LDY_ZPX(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.ZPX_addressing());
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::LDY_A(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.A_addressing());
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LDY_AX(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.AX_addressing());

    cpu.set_ZN(cpu.Y);

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::LSR_AC(CPU &cpu) {
    if ((cpu.A & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.A = cpu.A >> 1;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::LSR_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::LSR_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::LSR_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::LSR_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

void CPU::instructions::NOP(CPU &cpu) {
    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::ORA_I(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.PC+1);

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::ORA_ZP(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.ZP_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::ORA_ZPX(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.ZPX_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::ORA_A(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.A_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ORA_AX(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.AX_addressing());

    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AX()) cpu.cycles += 1;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ORA_AY(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.AY_addressing());

    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AY()) cpu.cycles += 1;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::ORA_IX(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.IX_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::ORA_IY(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.IY_addressing());

    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_IY()) cpu.cycles += 1;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::PHA(CPU &cpu) {
    cpu.stack_push(cpu.A);

    cpu.cycles += 3;
    cpu.PC += 1;
}

void CPU::instructions::PHP(CPU &cpu) {
    cpu.stack_push(cpu.STATUS);

    cpu.cycles += 3;
    cpu.PC += 1;
}

void CPU::instructions::PLA(CPU &cpu) {
    cpu.A = cpu.stack_pull();

    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 1;
}

void CPU::instructions::PLP(CPU &cpu) {
    // Break flag is discarded and the unused bit is always set on PLP and RTI
    cpu.STATUS = (cpu.stack_pull() & 0xEF) | 0x20;

    cpu.cycles += 4;
    cpu.PC += 1;
}

unsigned char CPU::instructions::ROL(CPU &cpu, unsigned char operand) {
    unsigned char aux_carry = cpu.get_carry();

    if ((operand >> 7) == 1) cpu.set_carry();
    else cpu.clear_carry();

    return (operand << 1) + aux_carry;
}

void CPU::instructions::ROL_AC(CPU &cpu) {
    cpu.A = ROL(cpu, cpu.A);

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::ROL_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::ROL_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::ROL_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::ROL_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

unsigned char CPU::instructions::ROR(CPU &cpu, unsigned char operand) {
    unsigned char aux_carry = cpu.get_carry();

    if ((operand & 1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    return (operand >> 1) + (aux_carry << 7);
}

void CPU::instructions::ROR_AC(CPU &cpu) {
    cpu.A = ROR(cpu, cpu.A);

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::ROR_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::ROR_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::ROR_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void CPU::instructions::ROR_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

void CPU::instructions::RTI(CPU &cpu) {
    // Break flag is discarded and the unused bit is always set on PLP and RTI
    cpu.STATUS = (cpu.stack_pull() & 0xEF) | 0x20;
    cpu.PC = cpu.stack_pull_16bit();

    cpu.cycles += 6;
}

void CPU::instructions::RTS(CPU &cpu) {
    cpu.PC = cpu.stack_pull_16bit();

    cpu.cycles += 6;
    cpu.PC += 1;
}

void CPU::instructions::SBC_I(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.PC+1);

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPU::instructions::SBC_ZP(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.ZP_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::SBC_ZPX(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.ZPX_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::SBC_A(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.A_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::SBC_AX(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.AX_addressing());

    ADC(cpu, operand ^ 0xFF);

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::SBC_AY(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.AY_addressing());

    ADC(cpu, operand ^ 0xFF);

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::SBC_IX(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.IX_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::SBC_IY(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.IY_addressing());

    ADC(cpu, operand ^ 0xFF);

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPU::instructions::SEC(CPU &cpu) {
    cpu.set_carry();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::SED(CPU &cpu) {
    cpu.set_decimal();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::SEI(CPU &cpu) {
    cpu.set_interrupt_disable();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::STA_ZP(CPU &cpu) {
    cpu.mem_store(cpu.ZP_addressing(), cpu.A);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::STA_ZPX(CPU &cpu) {
    cpu.mem_store(cpu.ZPX_addressing(), cpu.A);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::STA_A(CPU &cpu) {
    cpu.mem_store(cpu.A_addressing(), cpu.A);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::STA_AX(CPU &cpu) {
    cpu.mem_store(cpu.AX_addressing(), cpu.A);

    cpu.cycles += 5;
    cpu.PC += 3;
}

void CPU::instructions::STA_AY(CPU &cpu) {
    cpu.mem_store(cpu.AY_addressing(), cpu.A);

    cpu.cycles += 5;
    cpu.PC += 3;
}

void CPU::instructions::STA_IX(CPU &cpu) {
    cpu.mem_store(cpu.IX_addressing(), cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::STA_IY(CPU &cpu) {
    cpu.mem_store(cpu.IY_addressing(), cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CPU::instructions::STX_ZP(CPU &cpu) {
    cpu.mem_store(cpu.ZP_addressing(), cpu.X);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::STX_ZPY(CPU &cpu) {
    cpu.mem_store(cpu.ZPY_addressing(), cpu.X);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::STX_A(CPU &cpu) {
    cpu.mem_store(cpu.A_addressing(), cpu.X);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::STY_ZP(CPU &cpu) {
    cpu.mem_store(cpu.ZP_addressing(), cpu.Y);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPU::instructions::STY_ZPX(CPU &cpu) {
    cpu.mem_store(cpu.ZPX_addressing(), cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CPU::instructions::STY_A(CPU &cpu) {
    cpu.mem_store(cpu.A_addressing(), cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::TAX(CPU &cpu) {
    cpu.X = cpu.A;

    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::TAY(CPU &cpu) {
    cpu.Y = cpu.A;

    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::TOP_A(CPU &cpu) {
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::TOP_AX(CPU &cpu) {
    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPU::instructions::TSX(CPU &cpu) {
    cpu.X = cpu.SP;

    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::TXA(CPU &cpu) {
    cpu.A = cpu.X;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::TXS(CPU &cpu) {
    cpu.SP = cpu.X;

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CPU::instructions::TYA(CPU &cpu) {
    cpu.A = cpu.Y;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

std::unordered_map<unsigned char, std::function<void(CPU &)> > CPU::instructions::opcode_map() {
    /* Returns an unordered map that receives an opcode (unsigned char) as key and returns
       its respective function as a function pointer */

    std::unordered_map<unsigned char, std::function<void(CPU &)> > opcode_map;

    // AAX
    opcode_map.insert(std::make_pair(0x87, AAX_ZP));
    opcode_map.insert(std::make_pair(0x97, AAX_ZPY));
    opcode_map.insert(std::make_pair(0x83, AAX_IX));
    opcode_map.insert(std::make_pair(0x8F, AAX_A));
    // ADC
    opcode_map.insert(std::make_pair(0x69, ADC_I));
    opcode_map.insert(std::make_pair(0x65, ADC_ZP));
    opcode_map.insert(std::make_pair(0x75, ADC_ZPX));
    opcode_map.insert(std::make_pair(0x6D, ADC_A));
    opcode_map.insert(std::make_pair(0x7D, ADC_AX));
    opcode_map.insert(std::make_pair(0x79, ADC_AY));
    opcode_map.insert(std::make_pair(0x61, ADC_IX));
    opcode_map.insert(std::make_pair(0x71, ADC_IY));
    // AND
    opcode_map.insert(std::make_pair(0x29, AND_I));
    opcode_map.insert(std::make_pair(0x25, AND_ZP));
    opcode_map.insert(std::make_pair(0x35, AND_ZPX));
    opcode_map.insert(std::make_pair(0x2D, AND_A));
    opcode_map.insert(std::make_pair(0x3D, AND_AX));
    opcode_map.insert(std::make_pair(0x39, AND_AY));
    opcode_map.insert(std::make_pair(0x21, AND_IX));
    opcode_map.insert(std::make_pair(0x31, AND_IY));
    // ASL
    opcode_map.insert(std::make_pair(0x0A, ASL_AC));
    opcode_map.insert(std::make_pair(0x06, ASL_ZP));
    opcode_map.insert(std::make_pair(0x16, ASL_ZPX));
    opcode_map.insert(std::make_pair(0x0E, ASL_A));
    opcode_map.insert(std::make_pair(0x1E, ASL_AX));
    // BCC
    opcode_map.insert(std::make_pair(0x90, BCC));
    // BCS
    opcode_map.insert(std::make_pair(0xB0, BCS));
    // BEQ
    opcode_map.insert(std::make_pair(0xF0, BEQ));
    // BIT
    opcode_map.insert(std::make_pair(0x24, BIT_ZP));
    opcode_map.insert(std::make_pair(0x2C, BIT_A));
    // BMI
    opcode_map.insert(std::make_pair(0x30, BMI));
    // BNE
    opcode_map.insert(std::make_pair(0xD0, BNE));
    // BPL
    opcode_map.insert(std::make_pair(0x10, BPL));
    // BRK
    opcode_map.insert(std::make_pair(0x00, BRK));
    // BVC
    opcode_map.insert(std::make_pair(0x50, BVC));
    // BVS
    opcode_map.insert(std::make_pair(0x70, BVS));
    // CLC
    opcode_map.insert(std::make_pair(0x18, CLC));
    // CLD
    opcode_map.insert(std::make_pair(0xD8, CLD));
    // CLI
    opcode_map.insert(std::make_pair(0x58, CLI));
    // CLV
    opcode_map.insert(std::make_pair(0xB8, CLV));
    // CMP
    opcode_map.insert(std::make_pair(0xC9, CMP_I));
    opcode_map.insert(std::make_pair(0xC5, CMP_ZP));
    opcode_map.insert(std::make_pair(0xD5, CMP_ZPX));
    opcode_map.insert(std::make_pair(0xCD, CMP_A));
    opcode_map.insert(std::make_pair(0xDD, CMP_AX));
    opcode_map.insert(std::make_pair(0xD9, CMP_AY));
    opcode_map.insert(std::make_pair(0xC1, CMP_IX));
    opcode_map.insert(std::make_pair(0xD1, CMP_IY));
    // CPX
    opcode_map.insert(std::make_pair(0xE0, CPX_I));
    opcode_map.insert(std::make_pair(0xE4, CPX_ZP));
    opcode_map.insert(std::make_pair(0xEC, CPX_A));
    // CPY
    opcode_map.insert(std::make_pair(0xC0, CPY_I));
    opcode_map.insert(std::make_pair(0xC4, CPY_ZP));
    opcode_map.insert(std::make_pair(0xCC, CPY_A));
    // DCP
    opcode_map.insert(std::make_pair(0xC7, DCP_ZP));
    opcode_map.insert(std::make_pair(0xD7, DCP_ZPX));
    opcode_map.insert(std::make_pair(0xCF, DCP_A));
    opcode_map.insert(std::make_pair(0xDF, DCP_AX));
    opcode_map.insert(std::make_pair(0xDB, DCP_AY));
    opcode_map.insert(std::make_pair(0xC3, DCP_IX));
    opcode_map.insert(std::make_pair(0xD3, DCP_IY));
    // DEC
    opcode_map.insert(std::make_pair(0xC6, DEC_ZP));
    opcode_map.insert(std::make_pair(0xD6, DEC_ZPX));
    opcode_map.insert(std::make_pair(0xCE, DEC_A));
    opcode_map.insert(std::make_pair(0xDE, DEC_AX));
    // DEX
    opcode_map.insert(std::make_pair(0xCA, DEX));
    // DEY
    opcode_map.insert(std::make_pair(0x88, DEY));
    // DOP
    opcode_map.insert(std::make_pair(0x80, DOP_I));
    opcode_map.insert(std::make_pair(0x82, DOP_I));
    opcode_map.insert(std::make_pair(0x89, DOP_I));
    opcode_map.insert(std::make_pair(0xC2, DOP_I));
    opcode_map.insert(std::make_pair(0xE2, DOP_I));
    opcode_map.insert(std::make_pair(0x04, DOP_ZP));
    opcode_map.insert(std::make_pair(0x44, DOP_ZP));
    opcode_map.insert(std::make_pair(0x64, DOP_ZP));
    opcode_map.insert(std::make_pair(0x14, DOP_ZPX));
    opcode_map.insert(std::make_pair(0x34, DOP_ZPX));
    opcode_map.insert(std::make_pair(0x54, DOP_ZPX));
    opcode_map.insert(std::make_pair(0x74, DOP_ZPX));
    opcode_map.insert(std::make_pair(0xD4, DOP_ZPX));
    opcode_map.insert(std::make_pair(0xF4, DOP_ZPX));
    // EOR
    opcode_map.insert(std::make_pair(0x49, EOR_I));
    opcode_map.insert(std::make_pair(0x45, EOR_ZP));
    opcode_map.insert(std::make_pair(0x55, EOR_ZPX));
    opcode_map.insert(std::make_pair(0x4D, EOR_A));
    opcode_map.insert(std::make_pair(0x5D, EOR_AX));
    opcode_map.insert(std::make_pair(0x59, EOR_AY));
    opcode_map.insert(std::make_pair(0x41, EOR_IX));
    opcode_map.insert(std::make_pair(0x51, EOR_IY));
    // INC
    opcode_map.insert(std::make_pair(0xE6, INC_ZP));
    opcode_map.insert(std::make_pair(0xF6, INC_ZPX));
    opcode_map.insert(std::make_pair(0xEE, INC_A));
    opcode_map.insert(std::make_pair(0xFE, INC_AX));
    // INX
    opcode_map.insert(std::make_pair(0xE8, INX));
    // INY
    opcode_map.insert(std::make_pair(0xC8, INY));
    // JMP
    opcode_map.insert(std::make_pair(0x4C, JMP_A));
    opcode_map.insert(std::make_pair(0x6C, JMP_I));
    // JSR
    opcode_map.insert(std::make_pair(0x20, JSR));
    // LAX
    opcode_map.insert(std::make_pair(0xA7, LAX_ZP));
    opcode_map.insert(std::make_pair(0xB7, LAX_ZPY));
    opcode_map.insert(std::make_pair(0xAF, LAX_A));
    opcode_map.insert(std::make_pair(0xBF, LAX_AY));
    opcode_map.insert(std::make_pair(0xA3, LAX_IX));
    opcode_map.insert(std::make_pair(0xB3, LAX_IY));
    // LDA
    opcode_map.insert(std::make_pair(0xA9, LDA_I));
    opcode_map.insert(std::make_pair(0xA5, LDA_ZP));
    opcode_map.insert(std::make_pair(0xB5, LDA_ZPX));
    opcode_map.insert(std::make_pair(0xAD, LDA_A));
    opcode_map.insert(std::make_pair(0xBD, LDA_AX));
    opcode_map.insert(std::make_pair(0xB9, LDA_AY));
    opcode_map.insert(std::make_pair(0xA1, LDA_IX));
    opcode_map.insert(std::make_pair(0xB1, LDA_IY));
    // LDX
    opcode_map.insert(std::make_pair(0xA2, LDX_I));
    opcode_map.insert(std::make_pair(0xA6, LDX_ZP));
    opcode_map.insert(std::make_pair(0xB6, LDX_ZPY));
    opcode_map.insert(std::make_pair(0xAE, LDX_A));
    opcode_map.insert(std::make_pair(0xBE, LDX_AY));
    // LDY
    opcode_map.insert(std::make_pair(0xA0, LDY_I));
    opcode_map.insert(std::make_pair(0xA4, LDY_ZP));
    opcode_map.insert(std::make_pair(0xB4, LDY_ZPX));
    opcode_map.insert(std::make_pair(0xAC, LDY_A));
    opcode_map.insert(std::make_pair(0xBC, LDY_AX));
    // LSR
    opcode_map.insert(std::make_pair(0x4A, LSR_AC));
    opcode_map.insert(std::make_pair(0x46, LSR_ZP));
    opcode_map.insert(std::make_pair(0x56, LSR_ZPX));
    opcode_map.insert(std::make_pair(0x4E, LSR_A));
    opcode_map.insert(std::make_pair(0x5E, LSR_AX));
    // NOP
    opcode_map.insert(std::make_pair(0x1A, NOP));
    opcode_map.insert(std::make_pair(0x3A, NOP));
    opcode_map.insert(std::make_pair(0x5A, NOP));
    opcode_map.insert(std::make_pair(0x7A, NOP));
    opcode_map.insert(std::make_pair(0xDA, NOP));
    opcode_map.insert(std::make_pair(0xEA, NOP));
    opcode_map.insert(std::make_pair(0xFA, NOP));
    // ORA
    opcode_map.insert(std::make_pair(0x09, ORA_I));
    opcode_map.insert(std::make_pair(0x05, ORA_ZP));
    opcode_map.insert(std::make_pair(0x15, ORA_ZPX));
    opcode_map.insert(std::make_pair(0x0D, ORA_A));
    opcode_map.insert(std::make_pair(0x1D, ORA_AX));
    opcode_map.insert(std::make_pair(0x19, ORA_AY));
    opcode_map.insert(std::make_pair(0x01, ORA_IX));
    opcode_map.insert(std::make_pair(0x11, ORA_IY));
    // PHA
    opcode_map.insert(std::make_pair(0x48, PHA));
    // PHP
    opcode_map.insert(std::make_pair(0x08, PHP));
    // PLA
    opcode_map.insert(std::make_pair(0x68, PLA));
    // PLP
    opcode_map.insert(std::make_pair(0x28, PLP));
    // ROL
    opcode_map.insert(std::make_pair(0x2A, ROL_AC));
    opcode_map.insert(std::make_pair(0x26, ROL_ZP));
    opcode_map.insert(std::make_pair(0x36, ROL_ZPX));
    opcode_map.insert(std::make_pair(0x2E, ROL_A));
    opcode_map.insert(std::make_pair(0x3E, ROL_AX));
    // ROR
    opcode_map.insert(std::make_pair(0x6A, ROR_AC));
    opcode_map.insert(std::make_pair(0x66, ROR_ZP));
    opcode_map.insert(std::make_pair(0x76, ROR_ZPX));
    opcode_map.insert(std::make_pair(0x6E, ROR_A));
    opcode_map.insert(std::make_pair(0x7E, ROR_AX));
    // RTI
    opcode_map.insert(std::make_pair(0x40, RTI));
    // RTS
    opcode_map.insert(std::make_pair(0x60, RTS));
    // SBC
    opcode_map.insert(std::make_pair(0xE9, SBC_I));
    opcode_map.insert(std::make_pair(0xEB, SBC_I));
    opcode_map.insert(std::make_pair(0xE5, SBC_ZP));
    opcode_map.insert(std::make_pair(0xF5, SBC_ZPX));
    opcode_map.insert(std::make_pair(0xED, SBC_A));
    opcode_map.insert(std::make_pair(0xFD, SBC_AX));
    opcode_map.insert(std::make_pair(0xF9, SBC_AY));
    opcode_map.insert(std::make_pair(0xE1, SBC_IX));
    opcode_map.insert(std::make_pair(0xF1, SBC_IY));
    // SEC
    opcode_map.insert(std::make_pair(0x38, SEC));
    // SED
    opcode_map.insert(std::make_pair(0xF8, SED));
    // SEI
    opcode_map.insert(std::make_pair(0x78, SEI));
    // STA
    opcode_map.insert(std::make_pair(0x85, STA_ZP));
    opcode_map.insert(std::make_pair(0x95, STA_ZPX));
    opcode_map.insert(std::make_pair(0x8D, STA_A));
    opcode_map.insert(std::make_pair(0x9D, STA_AX));
    opcode_map.insert(std::make_pair(0x99, STA_AY));
    opcode_map.insert(std::make_pair(0x81, STA_IX));
    opcode_map.insert(std::make_pair(0x91, STA_IY));
    // STX
    opcode_map.insert(std::make_pair(0x86, STX_ZP));
    opcode_map.insert(std::make_pair(0x96, STX_ZPY));
    opcode_map.insert(std::make_pair(0x8E, STX_A));
    // STY
    opcode_map.insert(std::make_pair(0x84, STY_ZP));
    opcode_map.insert(std::make_pair(0x94, STY_ZPX));
    opcode_map.insert(std::make_pair(0x8C, STY_A));
    // TAX
    opcode_map.insert(std::make_pair(0xAA, TAX));
    // TAY
    opcode_map.insert(std::make_pair(0xA8, TAY));
    // TOP
    opcode_map.insert(std::make_pair(0x0C, TOP_A));
    opcode_map.insert(std::make_pair(0x1C, TOP_AX));
    opcode_map.insert(std::make_pair(0x3C, TOP_AX));
    opcode_map.insert(std::make_pair(0x5C, TOP_AX));
    opcode_map.insert(std::make_pair(0x7C, TOP_AX));
    opcode_map.insert(std::make_pair(0xDC, TOP_AX));
    opcode_map.insert(std::make_pair(0xFC, TOP_AX));
    // TSX
    opcode_map.insert(std::make_pair(0xBA, TSX));
    // TXA
    opcode_map.insert(std::make_pair(0x8A, TXA));
    // TXS
    opcode_map.insert(std::make_pair(0x9A, TXS));
    // TYA
    opcode_map.insert(std::make_pair(0x98, TYA));

    return opcode_map;
}