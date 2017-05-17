# 
# Building the disasm 
# author Alexander Kravtcov
#

vpath %.h ../../common
vpath %.h ../elf_parser
vpath %.cpp ../elf_parser
vpath %.h ../reg_file


include=-I ./ -I ../elf_parser/ -I ../../common/ -I ../reg_file

ifeq ($(debug), 1)
	CXX_FLAGS := -g -O0 -Wall -DDEBUG
endif

disasm: func_instr.o elf_parser.o disasm.o
	$(CXX) -o $@ $^ -l elf $(CXX_FLAGS)
	@echo "---------------------------------"
	@echo "$@ is built successifully"

func_instr.o: func_instr.cpp func_instr.h types.h reg_file.h
	$(CXX) -c $< $(include) $(CXX_FLAGS)

elf_parser.o: elf_parser.cpp elf_parser.h types.h
	$(CXX) -c $< $(include) $(CXX_FLAGS)

disasm.o: disasm.cpp func_instr.h types.h reg_file.h
	$(CXX) -c $< $(include) $(CXX_FLAGS)

clean:
	-rm *.o disasm
