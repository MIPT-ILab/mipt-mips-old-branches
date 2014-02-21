output : elf_parser.o disasm.o func_instr.o
	g++ disasm.o func_instr.o elf_parser.o -o output -l elf

disasm.o: disasm.cpp
	g++ -c disasm.cpp 

elf_parser.o : ../elf_parser/elf_parser.cpp ../elf_parser/elf_parser.h
	g++ -c ../elf_parser/elf_parser.cpp 

func_instr.o : func_instr.cpp 
	g++ -c func_instr.cpp

clear : 
	rm output func_instr.o elf_parser.o disasm.o 
