# 
# Building the disasm for MIPS binaries
# @author Pavel Kryukov <pavel.kryukov@phystech.com>
# Copyright 2014 MIPT-MIPS iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser/
vpath %.h $(TRUNK)/func_sim/func_instr/
vpath %.h $(TRUNK)/func_sim/func_memory/
vpath %.h $(TRUNK)/func_sim/func_sim/
vpath %.cpp $(TRUNK)/func_sim/elf_parser/
vpath %.cpp $(TRUNK)/func_sim/func_instr/
vpath %.cpp $(TRUNK)/func_sim/func_memory/
vpath %.cpp $(TRUNK)/func_sim/func_sim/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ -I $(TRUNK)/func_sim/elf_parser/ -I $(TRUNK)/func_sim/func_memory/ -I $(TRUNK)/func_sim/func_sim/ -I $(TRUNK)/func_sim/func_instr/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_memory stand alone program
#
sngl_ccl: func_memory.o elf_parser.o func_instr.o func_sim.o main.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf -std=c++11
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL) -std=c++11
    
func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL) -std=c++11

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL) -std=c++11

func_sim.o: func_sim.cpp func_sim.h types.h
	$(CXX) -c $< $(INCL) -std=c++11

main.o: main.cpp func_memory.h types.h func_instr.h func_sim.h
	$(CXX) -c $< $(INCL) -std=c++11

#
# Enter for building func_memory unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing for the moduler functional memory passed SUCCESSFULLY!"

unit_test: unit_test.o func_memory.o elf_parser.o func_instr.o func_sim.o
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $^ -lpthread $(GTEST_LIB) -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp func_sim.h
	$(CXX) -c $< $(INCL_GTEST) $(INCL) 

clean:
	@-rm *.o
	@-rm sngl_ccl unit_test

