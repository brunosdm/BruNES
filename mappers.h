#ifndef MAPPERS_H
#define MAPPERS_H

class Mapper {
    public:
        virtual void cpu_mem_store(unsigned short int address, unsigned char value) = 0;
        virtual unsigned char cpu_mem(unsigned short int address) = 0;
        virtual void ppu_mem_store(unsigned short int address, unsigned char value) = 0;
        virtual unsigned char ppu_mem(unsigned short int address) = 0;
};

class Mapper_0: public Mapper {
    public:
        void cpu_mem_store(unsigned short int address, unsigned char value);
        unsigned char cpu_mem(unsigned short int address);
        void ppu_mem_store(unsigned short int address, unsigned char value);
        unsigned char ppu_mem(unsigned short int address);

    private:
        unsigned char cpu_memory[65536];
        unsigned char ppu_memory[16384];
};

#endif