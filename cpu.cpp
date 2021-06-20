#include <unordered_map>

struct CPU {
    unsigned char memory[65536];
    unsigned long long int cycles;
    unsigned char A;
    unsigned char X;
    unsigned char Y;
    unsigned char SP;
    unsigned short int PC;
    unsigned char STATUS; // N,V,0,B,D,I,Z,C

    void reset() {
        cycles = 7;
        A = 0;
        X = 0;
        Y = 0;
        SP = 0xFD;
        PC = mem(0xFFFD)*256 + mem(0xFFFC);
        STATUS = 0x24;
    }

    unsigned char mem(unsigned short int address) {
        if (address < 0x2000) {
            // Zero page memory mirroring
            return memory[address % 0x800];
        }
        else if (address < 0x4000) {
            // PPU registers mirroring
            return memory[address % 8 + 0x2000];
        } 
        else return memory[address];
    }

    void mem_store(unsigned short int address, unsigned char value) {
        if (address < 0x2000) {
            // Zero page memory mirroring
            memory[address % 0x800] = value;
        }
        else if (address < 0x4000) {
            // PPU registers mirroring
            memory[address % 8 + 0x2000] = value;
        } 
        else memory[address] = value;
    }

    void set_ZN(unsigned char value) {
        if (value == 0) set_zero();
        else clear_zero();

        if (value >= 128) set_negative();
        else clear_negative();
    }

    void stack_push(unsigned char value) {
        mem_store(0x0100 + SP, value);
        SP--;
    }

    void stack_push_16bit(unsigned short int value) {
        stack_push(value >> 8);
        stack_push(value & 0xFF);
    }

    unsigned char stack_pull() {
        SP++;
        return mem(0x0100 + SP);
    }

    unsigned short int stack_pull_16bit() {
        unsigned short int value = stack_pull();
        return stack_pull()*256 + value;
    }

    bool page_cross_AX() {
        if (mem(PC+1) + X > 0xFF) return true;
        return false;
    }

    bool page_cross_AY() {
        if (mem(PC+1) + Y > 0xFF) return true;
        return false;
    }

    bool page_cross_IY() {
        if (mem(mem(PC+1)) + Y > 0xFF) return true;
        return false;
    }

    unsigned char get_carry() {
        return (bool) (STATUS & 0x01);
    }

    unsigned char get_zero() {
        return (bool) (STATUS & 0x02);
    }

    unsigned char get_interrupt_disable() {
        return (bool) (STATUS & 0x04);
    }

    unsigned char get_brk() {
        return (bool) (STATUS & 0x10);
    }

    unsigned char get_overflow() {
        return (bool) (STATUS & 0x40);
    }

    unsigned char get_negative() {
        return (bool) (STATUS & 0x80);
    }

    void set_carry() {
        STATUS = STATUS | 0x01;
    }

    void set_zero() {
        STATUS = STATUS | 0x02;
    }

    void set_interrupt_disable() {
        STATUS = STATUS | 0x04;
    }

    void set_decimal() {
        STATUS = STATUS | 0x08;
    }

    void set_brk() {
        STATUS = STATUS | 0x10;
    }

    void set_overflow() {
        STATUS = STATUS | 0x40;
    }

    void set_negative() {
        STATUS = STATUS | 0x80;
    }

    void clear_carry() {
        STATUS = STATUS & 0xFE;
    }

    void clear_zero() {
        STATUS = STATUS & 0xFD;
    }

    void clear_interrupt_disable() {
        STATUS = STATUS & 0xFB;
    }

    void clear_decimal() {
        STATUS = STATUS & 0xF7;
    }

    void clear_brk() {
        STATUS = STATUS & 0xEF;
    }

    void clear_overflow() {
        STATUS = STATUS & 0xBF;
    }

    void clear_negative() {
        STATUS = STATUS & 0x7F;
    }

    // The addressing functions return the address of the operand, not the operand itself.
    unsigned short int ZP_addressing() {
        return mem(PC+1);
    }

    unsigned short int ZPX_addressing() {
        return (mem(PC+1) + X) & 0xFF;
    }

    unsigned short int ZPY_addressing() {
        return (mem(PC+1) + Y) & 0xFF;
    }

    unsigned short int A_addressing() {
        return mem(PC+2)*256 + mem(PC+1);
    }

    unsigned short int AX_addressing() {
        return mem(PC+2)*256 + mem(PC+1) + X;
    }

    unsigned short int AY_addressing() {
        return mem(PC+2)*256 + mem(PC+1) + Y;
    }

    unsigned short int IX_addressing() {
        return mem((mem(PC+1) + X + 1) & 0xFF)*256 + mem((mem(PC+1) + X) & 0xFF);
    }

    unsigned short int IY_addressing() {
        return mem((mem(PC+1) + 1) & 0xFF)*256 + mem(mem(PC+1)) + Y;
    }

};

void ADC(CPU &cpu, unsigned char operand) {
    unsigned short int add = cpu.A + operand + cpu.get_carry();

    if (add > 255) cpu.set_carry();
    else cpu.clear_carry();

    if (cpu.A < 128 and operand < 128 and (add & 0xFF) >= 128) cpu.set_overflow();
    else if (cpu.A >= 128 and operand >= 128 and (add & 0xFF) < 128) cpu.set_overflow();
    else cpu.clear_overflow();

    cpu.A = add & 0xFF;

    cpu.set_ZN(cpu.A);
}

void ADC_I(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void ADC_ZP(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void ADC_ZPX(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void ADC_A(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void ADC_AX(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void ADC_AY(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void ADC_IX(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void ADC_IY(CPU &cpu) {
    ADC(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void AND(CPU &cpu, unsigned char operand) {
    cpu.A = cpu.A & operand;

    cpu.set_ZN(cpu.A);
}

void AND_I(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void AND_ZP(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void AND_ZPX(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void AND_A(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void AND_AX(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void AND_AY(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void AND_IX(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void AND_IY(CPU &cpu) {
    AND(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void ASL_AC(CPU &cpu) {
    if (cpu.A >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.A = cpu.A << 1;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void ASL_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void ASL_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void ASL_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void ASL_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    if (cpu.mem(address) >= 128) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) << 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

void BCC(CPU &cpu) {
    if (cpu.get_carry() == 0) {
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BCS(CPU &cpu) {
    if (cpu.get_carry() == 1) { 
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BEQ(CPU &cpu) {
    if (cpu.get_zero() == 1) { 
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BIT(CPU &cpu, unsigned char operand) {
    if ((bool)(operand & 0x40) == 1) cpu.set_overflow();
    else cpu.clear_overflow();

    if ((bool)(operand & 0x80) == 1) cpu.set_negative();
    else cpu.clear_negative();

    if ((bool)(operand & cpu.A) == 0) cpu.set_zero();
    else cpu.clear_zero();
}

void BIT_ZP(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.ZP_addressing());

    BIT(cpu, operand);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void BIT_A(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.A_addressing());

    BIT(cpu, operand);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void BMI(CPU &cpu) {
    if (cpu.get_negative() == 1) { 
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BNE(CPU &cpu) {
    if (cpu.get_zero() == 0) { 
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BPL(CPU &cpu) {
    if (cpu.get_negative() == 0) {
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BRK(CPU &cpu) {
    cpu.stack_push_16bit(cpu.PC);
    cpu.stack_push(cpu.STATUS);
    cpu.set_brk();
    cpu.PC = cpu.mem(0xFFFF)*256 + cpu.mem(0xFFFE);

    cpu.cycles += 7;
    cpu.PC += 1;
}

void BVC(CPU &cpu) {
    if (cpu.get_overflow() == 0) {
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++; 
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void BVS(CPU &cpu) {
    if (cpu.get_overflow() == 1) { 
        if ((cpu.PC+2 & 0xFF) + cpu.mem(cpu.PC+1) > 0xFF) cpu.cycles++;
        cpu.PC += (char) cpu.mem(cpu.PC+1);
        cpu.cycles++;
    }

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CLC(CPU &cpu) {
    cpu.clear_carry();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CLD(CPU &cpu) {
    cpu.clear_decimal();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CLI(CPU &cpu) {
    cpu.clear_interrupt_disable();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CLV(CPU &cpu) {
    cpu.clear_overflow();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void CMP(CPU &cpu, unsigned char operand) {
    if (cpu.A >= operand) {
        cpu.set_carry();
    }
    else cpu.clear_carry();

    cpu.set_ZN(cpu.A - operand);
}

void CMP_I(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CMP_ZP(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CMP_ZPX(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void CMP_A(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CMP_AX(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CMP_AY(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void CMP_IX(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void CMP_IY(CPU &cpu) {
    CMP(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void CPX(CPU &cpu, unsigned char operand) {
    if (cpu.X >= operand) {
        cpu.set_carry();
    }
    else cpu.clear_carry();
    
    cpu.set_ZN(cpu.X - operand);
}

void CPX_I(CPU &cpu) {
    CPX(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPX_ZP(CPU &cpu) {
    CPX(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPX_A(CPU &cpu) {
    CPX(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void CPY(CPU &cpu, unsigned char operand) {
    if (cpu.Y >= operand) {
        cpu.set_carry();
    }
    else cpu.clear_carry();
    
    cpu.set_ZN(cpu.Y - operand);
}

void CPY_I(CPU &cpu) {
    CPY(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void CPY_ZP(CPU &cpu) {
    CPY(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void CPY_A(CPU &cpu) {
    CPY(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void DEC(CPU &cpu, unsigned short int address) {
    cpu.mem_store(address, cpu.mem(address)-1);

    cpu.set_ZN(cpu.mem(address));
}

void DEC_ZP(CPU &cpu) {
    DEC(cpu, cpu.ZP_addressing());

    cpu.cycles += 5;
    cpu.PC += 2;
}

void DEC_ZPX(CPU &cpu) {
    DEC(cpu, cpu.ZPX_addressing());

    cpu.cycles += 6;
    cpu.PC += 2;
}

void DEC_A(CPU &cpu) {
    DEC(cpu, cpu.A_addressing());

    cpu.cycles += 6;
    cpu.PC += 3;
}

void DEC_AX(CPU &cpu) {
    DEC(cpu, cpu.AX_addressing());

    cpu.cycles += 7;
    cpu.PC += 3;
}

void DEX(CPU &cpu) {
    cpu.X--;
    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void DEY(CPU &cpu) {
    cpu.Y--;
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void EOR(CPU &cpu, unsigned char operand) {
    cpu.A = cpu.A ^ operand;
    cpu.set_ZN(cpu.A);
}

void EOR_I(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.PC+1));

    cpu.cycles += 2;
    cpu.PC += 2;
}

void EOR_ZP(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.ZP_addressing()));

    cpu.cycles += 3;
    cpu.PC += 2;
}

void EOR_ZPX(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.ZPX_addressing()));

    cpu.cycles += 4;
    cpu.PC += 2;
}

void EOR_A(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.A_addressing()));

    cpu.cycles += 4;
    cpu.PC += 3;
}

void EOR_AX(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.AX_addressing()));

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void EOR_AY(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.AY_addressing()));

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void EOR_IX(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.IX_addressing()));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void EOR_IY(CPU &cpu) {
    EOR(cpu, cpu.mem(cpu.IY_addressing()));

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void INC(CPU &cpu, unsigned short int address) {
    cpu.mem_store(address, cpu.mem(address)+1);
    cpu.set_ZN(cpu.mem(address));
}

void INC_ZP(CPU &cpu) {
    INC(cpu, cpu.ZP_addressing());

    cpu.cycles += 5;
    cpu.PC += 2;
}

void INC_ZPX(CPU &cpu) {
    INC(cpu, cpu.ZPX_addressing());

    cpu.cycles += 6;
    cpu.PC += 2;
}

void INC_A(CPU &cpu) {
    INC(cpu, cpu.A_addressing());

    cpu.cycles += 6;
    cpu.PC += 3;
}

void INC_AX(CPU &cpu) {
    INC(cpu, cpu.AX_addressing());

    cpu.cycles += 7;
    cpu.PC += 3;
}

void INX(CPU &cpu) {
    cpu.X++;
    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void INY(CPU &cpu) {
    cpu.Y++;
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void JMP_A(CPU &cpu) {
    cpu.PC = cpu.A_addressing();

    cpu.cycles += 3;
}

void JMP_I(CPU &cpu) {
    if ((cpu.mem(cpu.PC+1) & 0xFF) == 0xFF) {
        // Implements JMP instruction bug.
        cpu.PC = cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(0x00))*256 + 
                 cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(cpu.PC+1));
    }
    else cpu.PC = cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(cpu.PC+1)+1)*256 + 
                  cpu.mem(cpu.mem(cpu.PC+2)*256 + cpu.mem(cpu.PC+1));

    cpu.cycles += 5;
}

void JSR(CPU &cpu) {
    cpu.stack_push_16bit(cpu.PC+2);
    cpu.PC = cpu.A_addressing();

    cpu.cycles += 6;
}

void LDA_I(CPU &cpu) {
    cpu.A = cpu.mem(cpu.PC+1);
    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void LDA_ZP(CPU &cpu) {
    cpu.A = cpu.mem(cpu.ZP_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void LDA_ZPX(CPU &cpu) {
    cpu.A = cpu.mem(cpu.ZPX_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void LDA_A(CPU &cpu) {
    cpu.A = cpu.mem(cpu.A_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void LDA_AX(CPU &cpu) {
    cpu.A = cpu.mem(cpu.AX_addressing());
    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void LDA_AY(CPU &cpu) {
    cpu.A = cpu.mem(cpu.AY_addressing());
    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void LDA_IX(CPU &cpu) {
    cpu.A = cpu.mem(cpu.IX_addressing());
    cpu.set_ZN(cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void LDA_IY(CPU &cpu) {
    cpu.A = cpu.mem(cpu.IY_addressing());
    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void LDX_I(CPU &cpu) {
    cpu.X = cpu.mem(cpu.PC+1);
    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void LDX_ZP(CPU &cpu) {
    cpu.X = cpu.mem(cpu.ZP_addressing());
    cpu.set_ZN(cpu.X);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void LDX_ZPY(CPU &cpu) {
    cpu.X = cpu.mem(cpu.ZPY_addressing());
    cpu.set_ZN(cpu.X);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void LDX_A(CPU &cpu) {
    cpu.X = cpu.mem(cpu.A_addressing());
    cpu.set_ZN(cpu.X);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void LDX_AY(CPU &cpu) {
    cpu.X = cpu.mem(cpu.AY_addressing());
    cpu.set_ZN(cpu.X);

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void LDY_I(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.PC+1);
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void LDY_ZP(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.ZP_addressing());
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void LDY_ZPX(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.ZPX_addressing());
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void LDY_A(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.A_addressing());
    cpu.set_ZN(cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void LDY_AX(CPU &cpu) {
    cpu.Y = cpu.mem(cpu.AX_addressing());

    cpu.set_ZN(cpu.Y);

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void LSR_AC(CPU &cpu) {
    if ((cpu.A & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.A = cpu.A >> 1;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void LSR_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void LSR_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void LSR_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void LSR_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    if ((cpu.mem(address) & 0x1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    cpu.mem_store(address, cpu.mem(address) >> 1);

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

void NOP(CPU &cpu) {
    cpu.cycles += 2;
    cpu.PC += 1;
}

void ORA_I(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.PC+1);

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void ORA_ZP(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.ZP_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void ORA_ZPX(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.ZPX_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void ORA_A(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.A_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void ORA_AX(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.AX_addressing());

    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AX()) cpu.cycles += 1;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void ORA_AY(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.AY_addressing());

    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_AY()) cpu.cycles += 1;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void ORA_IX(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.IX_addressing());

    cpu.set_ZN(cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void ORA_IY(CPU &cpu) {
    cpu.A = cpu.A | cpu.mem(cpu.IY_addressing());

    cpu.set_ZN(cpu.A);

    if (cpu.page_cross_IY()) cpu.cycles += 1;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void PHA(CPU &cpu) {
    cpu.stack_push(cpu.A);

    cpu.cycles += 3;
    cpu.PC += 1;
}

void PHP(CPU &cpu) {
    cpu.stack_push(cpu.STATUS);

    cpu.cycles += 3;
    cpu.PC += 1;
}

void PLA(CPU &cpu) {
    cpu.A = cpu.stack_pull();

    cpu.set_ZN(cpu.A);

    cpu.cycles += 4;
    cpu.PC += 1;
}

void PLP(CPU &cpu) {
    // Break flag is discarded and the unused bit is always set on PLP and RTI
    cpu.STATUS = (cpu.stack_pull() & 0xEF) | 0x20;

    cpu.cycles += 4;
    cpu.PC += 1;
}

unsigned char ROL(CPU &cpu, unsigned char operand) {
    unsigned char aux_carry = cpu.get_carry();

    if ((operand >> 7) == 1) cpu.set_carry();
    else cpu.clear_carry();

    return (operand << 1) + aux_carry;
}

void ROL_AC(CPU &cpu) {
    cpu.A = ROL(cpu, cpu.A);

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void ROL_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void ROL_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void ROL_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void ROL_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    cpu.mem_store(address, ROL(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

unsigned char ROR(CPU &cpu, unsigned char operand) {
    unsigned char aux_carry = cpu.get_carry();

    if ((operand & 1) == 1) cpu.set_carry();
    else cpu.clear_carry();

    return (operand >> 1) + (aux_carry << 7);
}

void ROR_AC(CPU &cpu) {
    cpu.A = ROR(cpu, cpu.A);

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void ROR_ZP(CPU &cpu) {
    unsigned short int address = cpu.ZP_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 5;
    cpu.PC += 2;
}

void ROR_ZPX(CPU &cpu) {
    unsigned short int address = cpu.ZPX_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 2;
}

void ROR_A(CPU &cpu) {
    unsigned short int address = cpu.A_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 6;
    cpu.PC += 3;
}

void ROR_AX(CPU &cpu) {
    unsigned short int address = cpu.AX_addressing();

    cpu.mem_store(address, ROR(cpu, cpu.mem(address)));

    cpu.set_ZN(cpu.mem(address));

    cpu.cycles += 7;
    cpu.PC += 3;
}

void RTI(CPU &cpu) {
    // Break flag is discarded and the unused bit is always set on PLP and RTI
    cpu.STATUS = (cpu.stack_pull() & 0xEF) | 0x20;
    cpu.PC = cpu.stack_pull_16bit();

    cpu.cycles += 6;
}

void RTS(CPU &cpu) {
    cpu.PC = cpu.stack_pull_16bit();

    cpu.cycles += 6;
    cpu.PC += 1;
}

void SBC_I(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.PC+1);

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 2;
    cpu.PC += 2;
}

void SBC_ZP(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.ZP_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void SBC_ZPX(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.ZPX_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void SBC_A(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.A_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void SBC_AX(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.AX_addressing());

    ADC(cpu, operand ^ 0xFF);

    if (cpu.page_cross_AX()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void SBC_AY(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.AY_addressing());

    ADC(cpu, operand ^ 0xFF);

    if (cpu.page_cross_AY()) cpu.cycles++;
    cpu.cycles += 4;
    cpu.PC += 3;
}

void SBC_IX(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.IX_addressing());

    ADC(cpu, operand ^ 0xFF);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void SBC_IY(CPU &cpu) {
    unsigned char operand = cpu.mem(cpu.IY_addressing());

    ADC(cpu, operand ^ 0xFF);

    if (cpu.page_cross_IY()) cpu.cycles++;
    cpu.cycles += 5;
    cpu.PC += 2;
}

void SEC(CPU &cpu) {
    cpu.set_carry();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void SED(CPU &cpu) {
    cpu.set_decimal();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void SEI(CPU &cpu) {
    cpu.set_interrupt_disable();

    cpu.cycles += 2;
    cpu.PC += 1;
}

void STA_ZP(CPU &cpu) {
    cpu.mem_store(cpu.ZP_addressing(), cpu.A);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void STA_ZPX(CPU &cpu) {
    cpu.mem_store(cpu.ZPX_addressing(), cpu.A);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void STA_A(CPU &cpu) {
    cpu.mem_store(cpu.A_addressing(), cpu.A);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void STA_AX(CPU &cpu) {
    cpu.mem_store(cpu.AX_addressing(), cpu.A);

    cpu.cycles += 5;
    cpu.PC += 3;
}

void STA_AY(CPU &cpu) {
    cpu.mem_store(cpu.AY_addressing(), cpu.A);

    cpu.cycles += 5;
    cpu.PC += 3;
}

void STA_IX(CPU &cpu) {
    cpu.mem_store(cpu.IX_addressing(), cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void STA_IY(CPU &cpu) {
    cpu.mem_store(cpu.IY_addressing(), cpu.A);

    cpu.cycles += 6;
    cpu.PC += 2;
}

void STX_ZP(CPU &cpu) {
    cpu.mem_store(cpu.ZP_addressing(), cpu.X);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void STX_ZPY(CPU &cpu) {
    cpu.mem_store(cpu.ZPY_addressing(), cpu.X);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void STX_A(CPU &cpu) {
    cpu.mem_store(cpu.A_addressing(), cpu.X);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void STY_ZP(CPU &cpu) {
    cpu.mem_store(cpu.ZP_addressing(), cpu.Y);

    cpu.cycles += 3;
    cpu.PC += 2;
}

void STY_ZPX(CPU &cpu) {
    cpu.mem_store(cpu.ZPX_addressing(), cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 2;
}

void STY_A(CPU &cpu) {
    cpu.mem_store(cpu.A_addressing(), cpu.Y);

    cpu.cycles += 4;
    cpu.PC += 3;
}

void TAX(CPU &cpu) {
    cpu.X = cpu.A;

    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void TAY(CPU &cpu) {
    cpu.Y = cpu.A;

    cpu.set_ZN(cpu.Y);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void TSX(CPU &cpu) {
    cpu.X = cpu.SP;

    cpu.set_ZN(cpu.X);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void TXA(CPU &cpu) {
    cpu.A = cpu.X;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

void TXS(CPU &cpu) {
    cpu.SP = cpu.X;

    cpu.cycles += 2;
    cpu.PC += 1;
}

void TYA(CPU &cpu) {
    cpu.A = cpu.Y;

    cpu.set_ZN(cpu.A);

    cpu.cycles += 2;
    cpu.PC += 1;
}

std::unordered_map<unsigned char, std::function<void(CPU &)> > opcode_map() {
    /* Returns an unordered map that receives an opcode (unsigned char) as key and returns
       its respective function as a function pointer */

    std::unordered_map<unsigned char, std::function<void(CPU &)> > opcode_map;

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
    // DEC
    opcode_map.insert(std::make_pair(0xC6, DEC_ZP));
    opcode_map.insert(std::make_pair(0xD6, DEC_ZPX));
    opcode_map.insert(std::make_pair(0xCE, DEC_A));
    opcode_map.insert(std::make_pair(0xDE, DEC_AX));
    // DEX
    opcode_map.insert(std::make_pair(0xCA, DEX));
    // DEY
    opcode_map.insert(std::make_pair(0x88, DEY));
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
    opcode_map.insert(std::make_pair(0xEA, NOP));
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