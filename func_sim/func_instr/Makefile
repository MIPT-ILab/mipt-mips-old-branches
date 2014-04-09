

INCL=-I ../../common/ -I ../elf_parser -I ./

.PHONY: all
all: disasm

vpath %.h ../elf_parser/
vpath %.cpp ../elf_parser/


disasm: disasm.o func_instr.o elf_parser.o
	g++ $(INCL) $^ -o $@ -l elf
	@echo '=== disasm built succesful'

disasm.o: disasm.cpp func_instr.h
	g++ $(INCL) -c $<

func_instr.o: func_instr.cpp func_instr.h
	g++ $(INCL) -c $<

elf_parser.o: elf_parser.cpp elf_parser.h
	g++ $(INCL) -c $<


.PHONY: clean
clean: 
	rm -rf *.o disasm

