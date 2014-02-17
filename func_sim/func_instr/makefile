output : elf_parser.o disasm.o func_instr.o
	g++ disasm.o func_instr.o elf_parser.o -o output -l elf

output_test1 : test1.o func_instr.o 
	g++ -o output_test1 test1.o func_instr.o -l elf

disasm.o: disasm.cpp
	g++ -c disasm.cpp 

elf_parser.o : ../elf_parser/elf_parser.cpp ../elf_parser/elf_parser.h
	g++ -c ../elf_parser/elf_parser.cpp 

test1.o : test1.cpp 
	g++ -c test1.cpp 

func_instr.o : func_instr.cpp 
	g++ -c func_instr.cpp

clear : 
	rm output output_test1 test1.o func_instr.o elf_parser.o libelf_parser.a disasm.o 
