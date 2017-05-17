# 
# Building simulator of MIPS
# @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
# Copyright 2013 uArchSim iLab Project
#

TRUNK = ..

vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser
vpath %.h $(TRUNK)/func_sim/func_instr
vpath %.h $(TRUNK)/func_sim/func_memory
vpath %.h $(TRUNK)/func_sim/reg_file
vpath %.cpp $(TRUNK)/func_sim/elf_parser
vpath %.cpp $(TRUNK)/func_sim/func_instr
vpath %.cpp $(TRUNK)/func_sim/func_memory
vpath %.cpp $(TRUNK)/func_sim/reg_file

# Create a list of all assembly files in the current folder
ASM_FILES= $(wildcard *.s)

# Using names of the assembly files create a list of output 
# execution files 
OUT_FILES= $(patsubst %.s,%.out,$(ASM_FILES))

# Option for C++ compiler specifying directories 
# to search for headers
INCL = -I ./ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/func_instr -I $(TRUNK)/func_sim/func_memory -I $(TRUNK)/func_sim/reg_file

simulator: simulator.o func_sim.o elf_parser.o func_instr.o reg_file.o func_memory.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

tests: $(OUT_FILES)

%.out: %.o
	@mips-ld $< -o $@
	@echo $@ is built

%.o: %.s
	@mips-as -O0 $< -o $@ # Compile without optimization

simulator.o: simulator.cpp func_sim.h func_instr.h elf_parser.h reg_file.h func_memory.h types.h 
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

reg_file.o: reg_file.cpp reg_file.h types.h
	$(CXX) -c $< $(INCL)

func_memory.o: func_memory.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

func_sim.o: func_sim.cpp func_sim.h func_instr.h elf_parser.h reg_file.h func_memory.h types.h
	$(CXX) -c $< $(INCL)

clean:
	-rm *.o *.out simulator 