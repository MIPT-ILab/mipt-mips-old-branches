# 
# Building disassembler 
# for MIPS instructions
# @author Timur Mullayanov <timur.r.mullayanov@gmail.com>
# Copyright 2013 uArchSim iLab Project
#

# pathes to loop for headers
vpath %.h ../../common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

#seаrch for headers
include= -I ./ -I ../elf_parser -I ../../common

disasm: disasm.o func_instr.o elf_parser.o 
	@# don't forget to link ELF library using "-l elf"
	$(CXX) $^ -o $@ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

disasm.o: disasm.cpp types.h
	$(CXX) -c $< $(include)

func_instr.o: types.h func_instr.h func_instr.cpp 
	$(CXX) -c $< $(include)

elf_parser.o: types.h elf_parser.h elf_parser.cpp
	$(CXX) -c $< $(include)

clean:
	@-rm *.o 
	@-rm disasm
