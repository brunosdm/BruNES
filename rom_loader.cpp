#include <fstream>

void mem_load(unsigned char (&memory)[65536]) {
    char buffer[256];
    unsigned short int address = 0;
    std::ifstream infile; 
    infile.open("nestest.nes", std::ios::binary | std::ios::in);
    infile.read(buffer, 16);
    for (int i=0; i < 64; i++) {
        infile.read(buffer, 256);
        for (int k=0; k < 256; k++) {
            memory[address + 0x8000] = buffer[k];
            memory[address + 0xC000] = buffer[k];
            address += 1;
        }
    };
    infile.close();
}

