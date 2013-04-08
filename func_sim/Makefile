
SHELL = /bin/bash

COMPONENT_FOLDERS = elf_parser/ func_instr/ func_memory/ reg_file/

# Color codes used for color printing
NO_COLOR = \x1b[0m
OK_COLOR = \x1b[32;01m
ERROR_COLOR = \x1b[31;01m
WARN_COLOR = \x1b[33;01m

CXX = g++

# specifying relative path to the TRUNK
TRUNK= ../

# pathes to loop for headers
vpath %.h $(TRUNK)/common

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/ -I $(TRUNK)/func_sim/elf_parser/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter this to build simulator program
#
simulator: simulator.o 
	

#
# This builds simulator program
#
simlator.o: simulator.cpp func_sim.o
	$(CXX) -c $< $(INCL)

func_sim.o: func_sim.cpp
	$(CXX) -c $< $(INCL)

#
# Enter for building functional simulator tests
#

test: simulator_test
	@echo "Running ./$<..."
	@./$<
	@echo "---------------------------------"
	@echo "Simulator tests have passed!"


simulator_test: simulator.o simulator_test.o
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $^ -lpthread $(GTEST_LIB) -o $@ -l elf  $(GTEST_LIB)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

simulator_test.o: simulator_test.cpp simulator.cpp 
	$(CXX) -c $< $(INCL_GTEST) $(INCL) 

clean:
	@-rm *.o
	@-rm $(TEST_BIN)

