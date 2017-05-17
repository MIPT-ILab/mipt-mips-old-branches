# 
# Building the func_instr for the MIPS disassembling
# Copyright 2012 uArchSim iLab Project
#

vpath %.h ../../common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

# option for C++ compiler specifying directories 
# to search for headers
include=-I ./ -I ../../common/ -I ../elf_parser/

func_instr: func_instr.o elf_parser.o disasm.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully!"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(include)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(include)

disasm.o: disasm.cpp elf_parser.h func_instr.h types.h
	$(CXX) -c $< $(include)

clean:
	-rm *.o func_instr
