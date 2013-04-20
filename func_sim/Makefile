#
# Building the functional simulator
# author Alexander Kravtcov
#

# paths to look for headers
vpath %.h ../common/
vpath %.h ./func_instr/
vpath %.h ./reg_file/
vpath %.h ./func_memory/
vpath %.h ./elf_parser/
vpath %.cpp ./func_instr/
vpath %.cpp ./reg_file/
vpath %.cpp ./func_memory/
vpath %.cpp ./elf_parser/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I ../common/ -I ./func_instr/ -I ./reg_file/ -I ./func_memory/ -I ./elf_parser/

ifeq ($(debug), 1)
	CXX_FLAGS := $(CXX_FLAGS) -g -O0 -Wall -DDEBUG
endif

all: simulator tests

simulator: simulator.o func_sim.o func_memory.o elf_parser.o func_instr.o reg_file.o
	$(CXX) -o $@ $^ $(CXX_FLAGS) -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

simulator.o: simulator.cpp func_sim.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

func_sim.o: func_sim.cpp func_sim.h types.h reg_file.h func_instr.h func_memory.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

func_instr.o: func_instr.cpp func_instr.h reg_file.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

reg_file.o: reg_file.cpp reg_file.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL) $(CXX_FLAGS)

clean:
	rm -f *.o *.out
	rm -f simulator

tests: fibonacci.out factorial.out 

factorial.out: factorial.o
	@mips-ld $< -o $@  
	@echo $@ is built

factorial.o: factorial.s
	@mips-as $< -o $@ -O0

fibonacci.out: fibonacci.o
	@mips-ld $< -o $@  
	@echo $@ is built

fibonacci.o: fibonacci.s
	@mips-as $< -o $@ -O0

# it is needed to preven make from 
# deleting .o files automatically
