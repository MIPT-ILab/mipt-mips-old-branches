#
# Building the disasm for MIPS ElF binaries
# Copyright 2012 uArchSim iLab Project
#

vpath %.h ../../common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

# option for C++ compiler specifying directories
# to search for headers
include=-I ./ -I ../elf_parser/ -I ../../common/

disasm: disasm.o elf_parser.o func_instr.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

disasm.o: disasm.cpp func_instr.h types.h instructions.h
	$(CXX) -c $< $(include)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(include)

func_instr.o: func_instr.cpp func_instr.h types.h instructions.h
	$(CXX) -c $< $(include)

#clean:
#       -rm *.o disasm
