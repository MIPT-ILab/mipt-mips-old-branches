# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/


#
# Enter for building func_memory stand alone program
#
disasm: func_instr.o disasm.o elf_parser.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

disasm.o: disasm.cpp func_instr.h elf_parser.h
	$(CXX) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h my_assert.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)
clean:
	@-rm *.o


