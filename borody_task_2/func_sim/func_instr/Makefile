# 
# Building the disasm for MIPS ElF binaries
# @author Dmitry Borody <dmitry.borody@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# pathes to loop for headers
vpath %.h $(TRUNK)/common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)common/ -I ../elf_parser/

#options for static linking of boost Unit Test library
INCL_GTEST= -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB= $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building disasm stand alone program
#
disasm: disasm.o func_instr.o elf_parser.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

disasm.o: disasm.cpp types.h
	$(CXX) -c $< $(INCL)

func_instr.o: func_instr.cpp types.h func_instr.h 
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

clean:
	@-rm *.o
	@-rm disasm
