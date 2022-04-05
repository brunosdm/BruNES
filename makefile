CC = g++
COPTS = -c
LOPS = 

all : BruNES
BruNES : cpu.o rom_loader.o mappers.o test.o
	# Link the objects together
	$(CC) $(LOPS) *.o -o BruNES

cpu.o : cpu/cpu.cpp cpu/instructions.cpp
	$(CC) $(COPTS) cpu/cpu.cpp cpu/instructions.cpp

rom_loader.o : loader/rom_loader.cpp
	$(CC) $(COPTS) loader/rom_loader.cpp

mappers.o : mappers/mappers.cpp
	$(CC) $(COPTS) mappers/mappers.cpp

test.o : test/nestest.cpp
	$(CC) $(COPTS) test/nestest.cpp

run : BruNES
	./BruNES

clean :
	rm -f *.o
	rm -f BruNES