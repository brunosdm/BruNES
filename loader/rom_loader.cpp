#include <fstream>
#include <iostream>
#include "../mappers/mappers.h"

void nestest_load(Mapper **cartridge) {
    char buffer[256];
    unsigned short int address = 0;
    *cartridge = new Mapper_0();
    std::ifstream infile; 
    infile.open("test/nestest.nes", std::ios::binary | std::ios::in);
    infile.read(buffer, 16);
    for (int i=0; i < 64; i++) {
        infile.read(buffer, 256);
        for (int k=0; k < 256; k++) {
            (*cartridge)->cpu_mem_store(address + 0x8000, buffer[k]);
            (*cartridge)->cpu_mem_store(address + 0xC000, buffer[k]);
            address += 1;
        }
    };
    infile.close();
}


