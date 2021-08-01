#include <iostream>
#include <iomanip>
#include <unordered_map>
#include "mappers.h"
#include "cpu.h"
#include "rom_loader.h"

struct NES {
    private:
        Mapper *cartridge;
        CPU cpu;
        std::unordered_map<unsigned char, std::function<void(CPU &)> > opcode_dict;
        bool NMI;

    public:
        NES() {
            opcode_dict = opcode_map();
            nestest_load(&cartridge);
            cpu.mapper = cartridge;
            cpu.reset();
            cpu.PC = 0xC000;
            NMI = false;

        }

        void print_line() {
            std::cout <<  "PC: " << std::setw(4) << std::setfill('0') << std::hex << (int) cpu.PC << "-";
            std::cout << std::setw(2) << std::setfill('0') << (int) cpu.mem(cpu.PC);
            std::cout << "  A: "    << std::setw(2) << std::setfill('0') << std::hex << (int) cpu.A;
            std::cout << "  X: "    << std::setw(2) << std::setfill('0') << std::hex << (int) cpu.X;
            std::cout << "  Y: "    << std::setw(2) << std::setfill('0') << std::hex << (int) cpu.Y;
            std::cout << "  P: "    << std::setw(2) << std::setfill('0') << std::hex << (int) cpu.STATUS;
            std::cout << "  SP: "   << std::setw(2) << std::setfill('0') << std::hex << (int) cpu.SP;
            std::cout << "  CYC: "  << std::dec <<  (int) cpu.cycles <<  std::endl;
        }

        void run_one_instruction() {
            if (opcode_dict.find(cpu.mem(cpu.PC)) == opcode_dict.end()) {
                std::cout << "Opcode " << std::hex << (int)cpu.mem(cpu.PC) << " doesnt exist!" << std::endl;
                exit(EXIT_FAILURE);
            }
            opcode_dict[cpu.mem(cpu.PC)](cpu);
        }

};

int main() {
    NES nes;
    for(int i = 0; i < 6000; i++) {
        nes.print_line();
        nes.run_one_instruction();
    }
}


