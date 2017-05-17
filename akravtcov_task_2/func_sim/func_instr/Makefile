# 
# Building the disasm 
# @author Alexander Kravtcov
#

vpath %.h ../../common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser

include=-I ./ -I ../elf_parser/ -I ../../common/

disasm: func_instr.o elf_parser.o disasm.o
	$(CXX) -o $@ $^ -l elf
	@echo "---------------------------------"
	@echo "$@ is built successifully"

func_instr.o: func_instr.cpp func_instr.h types.h
	$(CXX) -c $< $(include)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(include)

disasm.o: disasm.cpp func_instr.h types.h
	$(CXX) -c $< $(include)

clean:
	-rm *.o disasm
