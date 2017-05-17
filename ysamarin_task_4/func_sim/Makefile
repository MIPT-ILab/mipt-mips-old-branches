# 
# Building the func_instr for the MIPS disassembling
# Copyright 2012 uArchSim iLab Project
#

vpath %.h ../common
vpath %.h elf_parser
vpath %.cpp elf_parser
vpath %.o elf_parser
vpath %.h func_instr
vpath %.cpp func_instr
vpath %.o func_instr
vpath %.h func_memory
vpath %.cpp func_memory
vpath %.o func_memory
vpath %.h reg_file
vpath %.cpp reg_file
vpath %.o reg_file

# option for C++ compiler specifying directories 
# to search for headers
include=-I ./ -I ../common/ -I elf_parser/ -I func_instr/ -I func_memory/ -I reg_file/

func_sim: func_sim.o simulator.o elf_parser.o func_instr.o func_memory.o reg_file.o 
	$(CXX) -o $@ $^ $(include) -l elf 
	@echo "---------------------------------"
	@echo "$@ is built successifully!"

func_sim.o: func_sim.cpp
	$(CXX) -c $^ $(include)

simulator.o: simulator.cpp
	$(CXX) -c $^ $(include)

elf_parser.o: elf_parser.cpp
	$(CXX) -c $^ -l elf $(include)

func_memory.o: func_memory.cpp
	$(CXX) -c $^ $(include)

func_instr.o: func_instr.cpp
	$(CXX) -c $^ $(include)

reg_file.o: reg_file.cpp
	$(CXX) -c $^ $(include)

clean:
	-rm *.o func_sim
	-rm *.o func_sim/func_instr 
	-rm *.o func_sim/func_memory 
	-rm *.o func_sim/elf_parser 
	-rm *.o func_sim/reg_file 
