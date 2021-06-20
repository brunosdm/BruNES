#include <iostream>
#include <unordered_map>
#include "cpu.cpp"
#include "rom_loader.cpp"

void print_line(CPU &nes) {
    std::cout <<  "PC: " << std::setw(4) << std::setfill('0') << std::hex << (int) nes.PC << "-";
    std::cout << std::setw(2) << std::setfill('0') << (int) nes.mem(nes.PC);
    std::cout << "  A: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.A;
    std::cout << "  X: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.X;
    std::cout << "  Y: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.Y;
    std::cout << "  P: "    << std::setw(2) << std::setfill('0') << std::hex << (int) nes.STATUS;
    std::cout << "  SP: "   << std::setw(2) << std::setfill('0') << std::hex << (int) nes.SP;
    std::cout << "  CYC: "  << std::dec <<  (int) nes.cycles <<  std::endl;
}

int main() {
    CPU nes;
    std::unordered_map<unsigned char, std::function<void(CPU &)> > opcode_dict = opcode_map();
    mem_load(nes.memory);
    nes.reset();
    nes.PC = 0xC000;
    for (int i=0; i < 6000; i++) {
        print_line(nes);
        if (opcode_dict.find(nes.mem(nes.PC)) == opcode_dict.end()){
            std::cout << "Opcode " << std::hex << (int)nes.mem(nes.PC) << " doesnt exist!" << std::endl;
            break;
        }
        else opcode_dict[nes.mem(nes.PC)](nes); 
    };
    return 0;
}

