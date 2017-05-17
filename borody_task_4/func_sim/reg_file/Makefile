# 
# Building the elf_parser for MIPS ElF binaries
# @author Alexander Titov <alexander.igorevich.titov@gmail.com>
# Copyright 2012 uArchSim iLab Project
#

# specifying relative path to the TRUNK
TRUNK= ../../

# paths to look for headers
vpath %.h $(TRUNK)/common
vpath %.h $(TRUNK)/func_sim/reg_file/
vpath %.cpp $(TRUNK)/func_sim/reg_file/

# option for C++ compiler specifying directories 
# to search for headers
INCL= -I ./ -I $(TRUNK)/common/ 

#options for static linking of boost Unit Test library
INCL_GTEST = -I $(TRUNK)/libs/gtest-1.6.0/include
GTEST_LIB = $(TRUNK)/libs/gtest-1.6.0/libgtest.a

#
# Enter for building reg_file stand alone program
#
reg_file: reg_file.o
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY!"

reg_file.o: reg_file.cpp
	$(CXX) -c $^ -o $@ $(INCL)

#
# Enter for building reg_file unit test
#
test: unit_test
	@echo ""
	@echo "Running ./$<\n"
	@./$<
	@echo "Unit testing of RegFile has passed SUCCESSFULLY!"

unit_test: unit_test.o reg_file.o
	@# don't forget to link ELF library using "-l elf"
	@# and use "-lpthread" options for Google Test
	$(CXX) $^ -lpthread $(GTEST_LIB) -o $@ -l elf  $(GTEST_LIB)
	@echo "---------------------------------"
	@echo "$@ is built SUCCESSFULLY"

unit_test.o: unit_test.cpp reg_file.o
	$(CXX) -c $< $(INCL_GTEST) $(INCL) 

clean:
	@rm -f *.o
	@rm -f unit_test 
