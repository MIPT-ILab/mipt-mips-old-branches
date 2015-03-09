# 
# Building the disasm for MIPS binaries
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2014 MIPT-MIPS iLab Project
#

#
# Edited by Ladin Oleg.
#
# Building funcsim.
#

# specifying relative path to the TRUNK
TRUNK= ..

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.h $(TRUNK)/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ \
 -I $(TRUNK)/func_sim/func_memory/ -I $(TRUNK)/func_sim/func_instr/

#
# Enter for building funcsim stand alone program
#
funcsim: func_memory.o elf_parser.o func_instr.o func_sim.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

func_sim.o: func_sim.cpp func_sim.h types.h
	$(CXX) -c $< $(INCL)

main.o: main.cpp types.h
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm funcsim
