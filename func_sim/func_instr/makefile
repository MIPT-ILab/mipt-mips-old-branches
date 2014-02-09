output : test1.o func_instr.o
	g++ -o output test1.o func_instr.o

test1.o : test1.cpp func_instr.h
	g++ -c test1.cpp

func_instr.o : func_instr.cpp func_instr.h
	g++ -c func_instr.cpp

clear : 
	rm output test1.o func_instr.o
