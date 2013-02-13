# 
# Building disasm for MIPS
# @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
# Copyright 2013 uArchSim iLab Project
#

TRUNK = ../..

vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser
vpath %.cpp $(TRUNK)/func_sim/elf_parser

# Option for C++ compiler specifying directories 
# to search for headers
INCL = -I ./ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/common/

disasm: disasm.o elf_parser.o func_instr.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

disasm.o: disasm.cpp func_instr.h elf_parser.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

clean:
	-rm *.o disasm