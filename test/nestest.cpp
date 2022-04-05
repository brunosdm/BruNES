#include <iostream>
#include <iomanip>
#include <unordered_map>
#include "../cpu/cpu.h"
#include "../loader/rom_loader.h"

void print_line(CPU &nes) {
    std::cout <<  "PC: " << std::setw(4) << std::setfill('0') << std::hex << (int) nes.get_PC();
    std::cout << "  A: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.get_A();
    std::cout << "  X: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.get_X();
    std::cout << "  Y: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.get_Y();
    std::cout << "  P: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.get_STATUS();
    std::cout << "  SP: "   << std::setw(2) << std::setfill('0') << std::hex << (int) nes.get_SP();
    std::cout << "  CYC: "  << std::dec <<  (int) nes.get_cycles() <<  std::endl;
}

int main() {
    Mapper* mapper;
    nestest_load(&mapper);
    CPU cpu = CPU(mapper);
    cpu.reset();

    for (int i=0; i < 8991; i++) {
        print_line(cpu);
        cpu.run_next_instruction();
    }
    
    return 0;
}

