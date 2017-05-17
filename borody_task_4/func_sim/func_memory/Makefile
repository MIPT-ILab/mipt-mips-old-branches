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

# Modules on which FuncMemory depends
MODULES_DIRS= $(TRUNK)/func_sim/elf_parser/
MODULES_OBJS= $(TRUNK)/func_sim/elf_parser/elf_parser.o

#
# Enter for building func_memory stand alone program
#
func_memory: modules func_memory.o main.o
	@# don't forget to link ELF library using "-l elf"
	@echo ""
	$(CXX) -o $@ func_memory.o main.o $(MODULES_OBJS) -l elf
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY!"

#
# Build dependencies
#
modules: 
	@echo "Building modules..."
	@for obj_dir in $(MODULES_DIRS); do make -C $$obj_dir; done
	@echo "---------------------------------"
	@echo "Building modules is done SUCCESSFULLY!"

func_memory.o: func_memory.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL)

main.o: main.cpp func_memory.h types.h
	$(CXX) -c $< $(INCL)

#
# Enter for building func_memory unit test
#
test: modules unit_test
	@echo ""
	@echo "Running ./unit_test\n"
	@./unit_test
	@echo "Unit testing for FuncMemory has passed SUCCESSFULLY!"

unit_test: unit_test.o func_memory.o
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) -o $@ $^ $(MODULES_OBJS) -lpthread $(GTEST_LIB) -lelf $(INCL_GTEST) $(INCL)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp  
	$(CXX) -o $@ -c $^ $(INCL_GTEST) $(INCL) 

clean:
	@rm -f *.o
	@rm -f func_memory unit_test
