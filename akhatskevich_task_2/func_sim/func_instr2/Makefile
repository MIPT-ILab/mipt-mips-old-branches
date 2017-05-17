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

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building func_instr stand alone program
#
func_instr: func_instr.o elf_parser.o disasm.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h
	$(CXX) -c $< $(INCL)

disasm.o: disasm.cpp func_instr.h types.h 
	$(CXX) -c $< $(INCL)

#
# Enter for building func_instr unit test
#
#test: unit_test
#	@echo ""
#	@echo "Running ./$<\n"
#	@./$<
#	@echo "Unit testing for the moduler functional memory passed SUCCESSFULLY!"

#unit_test: unit_test.o func_instr.o elf_parser.o
#	@# don't forget to link ELF library using "-l elf"
#	@# and use "-lpthread" options for Google Test
#	$(CXX) $^ -lpthread $(GTEST_LIB) -o $@ -l elf  $(GTEST_LIB)
#	@echo "---------------------------------"
#	@echo "$@ is built SUCCESSFULLY"

#unit_test.o: unit_test.cpp func_instr.o elf_parser.o
#	$(CXX) -c $< $(INCL_GTEST) $(INCL) 

clean:
	@-rm *.o
	@-rm func_instr unit_test

