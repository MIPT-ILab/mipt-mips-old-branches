#
# Building the disasm for MIPS ElF binaries
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

vpath %.h $(TRUNK)/common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

# option for C++ compiler specifying directories 
# to search for headers

INCL=-I ./ -I ../elf_parser/ -I $(TRUNK)common/

disasm: disasm.o elf_parser.o func_instr.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

disasm.o: disasm.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)
clean:
	@-rm *.o

