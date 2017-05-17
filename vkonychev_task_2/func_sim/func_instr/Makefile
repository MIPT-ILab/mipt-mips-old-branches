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
# Enter for building func_memory stand alone program
#
func_instr: func_instr.o elf_parser.o disasm.o
	@# don't forget to link ELF library using "-l elf"
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(INCL)

disasm.o: disasm.cpp func_instr.h types.h
	$(CXX) -c $< $(INCL)



clean:
	@-rm *.o
	@-rm func_instr
