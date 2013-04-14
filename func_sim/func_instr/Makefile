# 
# Building the disasm for MIPS ElF binaries
# @author Dmitry Borody <dmitry.borody@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK = ../../

# pathes to loop for headers
vpath %.h $(TRUNK)/common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

# option for C++ compiler specifying directories 
# to search for headers
INCL = -I ./ -I $(TRUNK)common/ -I ../elf_parser/ -I $(TRUNK)func_sim/reg_file/ -I $(TRUNK)/func_sim/func_memory/

#options for static linking of boost Unit Test library
INCL_GTEST = -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB = $(TRUNK)/libs/gtest-1.6.0/libgtest.a

# Modules on which FuncInstr depends
MODULES_DIRS = $(TRUNK)/func_sim/elf_parser/ 
MODULES_DIRS += $(TRUNK)/func_sim/reg_file/
MODULES_DIRS += $(TRUNK)/func_sim/func_memory/
MODULES_OBJS = $(TRUNK)/func_sim/elf_parser/elf_parser.o
MODULES_OBJS += $(TRUNK)/func_sim/reg_file/reg_file.o
MODULES_OBJS += $(TRUNK)/func_sim/func_memory/func_memory.o

#
# Enter for building disasm stand alone program
#
disasm: modules disasm.o func_instr.o 
	@# don't forget to link ELF library using "-l elf"
	$(CXX) disasm.o func_instr.o  $(MODULES_OBJS) -o $@ -lelf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

#
# Build dependencies
#
modules: 
	@echo "Building modules..."
	@for obj_dir in $(MODULES_DIRS); do make -C $$obj_dir; done
	@echo "---------------------------------"
	@echo "Building modules is done SUCCESSFULLY!"

disasm.o: disasm.cpp types.h
	$(CXX) -c $< $(INCL) 

func_instr.o: func_instr.cpp types.h func_instr.h 
	$(CXX) -c $< $(INCL) 

#
# Enter for building func_instr unit test
#
test: modules unit_test
	@echo ""
	@echo "Running ./unit_test\n"
	@./unit_test
	@echo "Unit testing for the class FuncIntr has passed SUCCESSFULLY!"

unit_test: unit_test.o 
	$(CXX) $^ -lpthread $(GTEST_LIB) -o $@
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY!"

unit_test.o: unit_test.cpp func_instr.o
	$(CXX) -c $< -o $@ $(INCL_GTEST) $(INCL) 

clean:
	@rm -f *.o
	@rm -f disasm
	@rm -f unit_test
