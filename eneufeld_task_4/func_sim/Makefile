# 
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK:= ../

# pathes to loop for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/ -O0

#
# Enter for building func_sim stand alone program
#

all: elf_parser.o func_memory.o reg_file.o func_sim.cpp	
	$(CXX) func_memory.o reg_file.o func_sim.cpp elf_parser.o $(INCL) -o func_sim -l elf

elf_parser.o: elf_parser/elf_parser.cpp elf_parser/elf_parser.h types.h
	$(CXX) -c $< $(INCL)

func_memory.o: func_memory/func_memory.cpp func_memory/func_memory.h types.h
	$(CXX) -c $< $(INCL)

reg_file.o: reg_file/reg_file.cpp reg_file/reg_file.h types.h
	$(CXX) -c $< $(INCL)


clean:
	@-rm *.o
	@-rm func_sim
