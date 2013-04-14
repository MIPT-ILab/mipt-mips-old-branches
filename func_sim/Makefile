
SHELL = /bin/bash

CXX = g++

# specifying relative path to the TRUNK
TRUNK = ../

# pathes to loop for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/elf_parser

# Modules on which FuncSim depends 
MODULES_DIRS = $(TRUNK)/func_sim/func_memory/ 
MODULES_DIRS += $(TRUNK)/func_sim/reg_file/
MODULES_DIRS += $(TRUNK)/func_sim/func_instr/
MODULES_DIRS += $(TRUNK)/func_sim/elf_parser/
MODULES_OBJS = $(TRUNK)/func_sim/func_memory/func_memory.o
MODULES_OBJS += $(TRUNK)/func_sim/reg_file/reg_file.o
MODULES_OBJS += $(TRUNK)/func_sim/func_instr/func_instr.o
MODULES_OBJS += $(TRUNK)/func_sim/elf_parser/elf_parser.o

# option for C++ compiler specifying directories 
# to search for headers
INCL = -I ./ -I $(TRUNK)common/ 
INCL += -I $(TRUNK)/func_sim/func_memory/
INCL += -I $(TRUNK)/func_sim/func_instr/
INCL += -I $(TRUNK)/func_sim/reg_file/
INCL += -I $(TRUNK)/func_sim/elf_parser/

# options for static linking of gtest libray
INCL_GTEST = -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB = $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter this to build simulator program
#
simulator: modules simulator.o func_sim.o
	@echo ""
	$(CXX) -o $@ simulator.o func_sim.o $(MODULES_OBJS) $(INCL) -lelf
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

asm_tests:
	@echo "Building assembly tests..."
	make -C ./tests/
	@echo "---------------------------------"
	@echo "Building assembly tests is done SUCCESSFULLY!"

#
# This builds simulator program
#
simulator.o: simulator.cpp 
	@echo ""
	$(CXX) -c $^ $(INCL) 

func_sim.o: func_sim.cpp
	$(CXX) -c $< $(INCL)

#
# Enter for building functional simulator tests
#
test: modules asm_tests simulator_test
	@echo "Running ./simulator_test..."
	@./simulator_test
	@echo "---------------------------------"
	@echo "Simulator tests have passed!"

simulator_test: func_sim.o simulator_test.o 
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $^ -o $@ $(MODULES_OBJS) -lpthread -lelf $(GTEST_LIB)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

simulator_test.o: simulator_test.cpp 
	$(CXX) -c $< $(INCL_GTEST) $(INCL) 

clean:
	@rm -f *.o
	@for obj_dir in $(MODULES_DIRS); do \
		make -C $$obj_dir clean; \
	done
	make -C ./tests/ clean

