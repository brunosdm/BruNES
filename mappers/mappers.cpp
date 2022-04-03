#include "mappers.h"


void Mapper_0::cpu_mem_store(unsigned short int address, unsigned char value) {
    if (address < 0x2000) {
    // Zero page memory mirroring
    cpu_memory[address % 0x800] = value;
    }
    else if (address < 0x4000) {
        // PPU registers mirroring
        cpu_memory[address % 8 + 0x2000] = value;
    } 
    else cpu_memory[address] = value;
}

unsigned char Mapper_0::cpu_mem(unsigned short int address) {
    if (address < 0x2000) {
        // Zero page memory mirroring
        return cpu_memory[address % 0x800];
    }
    else if (address < 0x4000) {
        // PPU registers mirroring
        return cpu_memory[address % 8 + 0x2000];
    } 
    else return cpu_memory[address];
}

void Mapper_0::ppu_mem_store(unsigned short int address, unsigned char value) {
    return;
}

unsigned char Mapper_0::ppu_mem(unsigned short int address) {
    return 0;
}





