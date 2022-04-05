#include <iostream>
#include "cpu.h"

CPU::CPU(Mapper *mapper) {
    CPU::mapper = mapper;
    inst = new instructions();
}

void CPU::reset() {
    cycles = 7;
    A = 0;
    X = 0;
    Y = 0;
    SP = 0xFD;
    //PC = mem(0xFFFD)*256 + mem(0xFFFC);
    PC = 0xC000;
    STATUS = 0x24;
}

void CPU::run_next_instruction() {
    inst->run_instruction((*this), mem(PC));
}

unsigned char CPU::get_A() {
    return A;
}

unsigned char CPU::get_X() {
    return X;
}

unsigned char CPU::get_Y() {
    return Y;
}

unsigned char CPU::get_SP() {
    return SP;
}

unsigned char CPU::get_STATUS() {
    return STATUS;
}

unsigned short int CPU::get_PC() {
    return PC;
}

unsigned long long int CPU::get_cycles() {
    return cycles;
}

unsigned char CPU::mem(unsigned short int address) {
    return mapper->cpu_mem(address);
}

void CPU::mem_store(unsigned short int address, unsigned char value) {
    mapper->cpu_mem_store(address, value);
}

void CPU::set_ZN(unsigned char value) {
    if (value == 0) set_zero();
    else clear_zero();

    if (value >= 128) set_negative();
    else clear_negative();
}

void CPU::stack_push(unsigned char value) {
    mem_store(0x0100 + SP, value);
    SP--;
}

void CPU::stack_push_16bit(unsigned short int value) {
    stack_push(value >> 8);
    stack_push(value & 0xFF);
}

unsigned char CPU::stack_pull() {
    SP++;
    return mem(0x0100 + SP);
}

unsigned short int CPU::stack_pull_16bit() {
    unsigned short int value = stack_pull();
    return stack_pull()*256 + value;
}

bool CPU::page_cross_AX() {
    if (mem(PC+1) + X > 0xFF) return true;
    return false;
}

bool CPU::page_cross_AY() {
    if (mem(PC+1) + Y > 0xFF) return true;
    return false;
}

bool CPU::page_cross_IY() {
    if (mem(mem(PC+1)) + Y > 0xFF) return true;
    return false;
}

bool CPU::branch_page_cross() {
    if ((PC+2 & 0xFF) + (char) mem(PC+1) > 0xFF) return true;
    return false;
}

unsigned char CPU::get_carry() {
    return (bool) (STATUS & 0x01);
}

unsigned char CPU::get_zero() {
    return (bool) (STATUS & 0x02);
}

unsigned char CPU::get_interrupt_disable() {
    return (bool) (STATUS & 0x04);
}

unsigned char CPU::get_brk() {
    return (bool) (STATUS & 0x10);
}

unsigned char CPU::get_overflow() {
    return (bool) (STATUS & 0x40);
}

unsigned char CPU::get_negative() {
    return (bool) (STATUS & 0x80);
}

void CPU::set_carry() {
    STATUS = STATUS | 0x01;
}

void CPU::set_zero() {
    STATUS = STATUS | 0x02;
}

void CPU::set_interrupt_disable() {
    STATUS = STATUS | 0x04;
}

void CPU::set_decimal() {
    STATUS = STATUS | 0x08;
}

void CPU::set_brk() {
    STATUS = STATUS | 0x10;
}

void CPU::set_overflow() {
    STATUS = STATUS | 0x40;
}

void CPU::set_negative() {
    STATUS = STATUS | 0x80;
}

void CPU::clear_carry() {
    STATUS = STATUS & 0xFE;
}

void CPU::clear_zero() {
    STATUS = STATUS & 0xFD;
}

void CPU::clear_interrupt_disable() {
    STATUS = STATUS & 0xFB;
}

void CPU::clear_decimal() {
    STATUS = STATUS & 0xF7;
}

void CPU::clear_brk() {
    STATUS = STATUS & 0xEF;
}

void CPU::clear_overflow() {
    STATUS = STATUS & 0xBF;
}

void CPU::clear_negative() {
    STATUS = STATUS & 0x7F;
}

// The addressing functions return the address of the operand, not the operand itself.
unsigned short int CPU::ZP_addressing() {
    return mem(PC+1);
}

unsigned short int CPU::ZPX_addressing() {
    return (mem(PC+1) + X) & 0xFF;
}

unsigned short int CPU::ZPY_addressing() {
    return (mem(PC+1) + Y) & 0xFF;
}

unsigned short int CPU::A_addressing() {
    return mem(PC+2)*256 + mem(PC+1);
}

unsigned short int CPU::AX_addressing() {
    return mem(PC+2)*256 + mem(PC+1) + X;
}

unsigned short int CPU::AY_addressing() {
    return mem(PC+2)*256 + mem(PC+1) + Y;
}

unsigned short int CPU::IX_addressing() {
    return mem((mem(PC+1) + X + 1) & 0xFF)*256 + mem((mem(PC+1) + X) & 0xFF);
}

unsigned short int CPU::IY_addressing() {
    return mem((mem(PC+1) + 1) & 0xFF)*256 + mem(mem(PC+1)) + Y;
}