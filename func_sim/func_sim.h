/*
 * func_sim.h - instruction parser for mips
 * @author Ivan Kunakhov kunakhov.ivan@gmail.com
 */

#include <elf_parser.h>
#include <func_instr.h>
#include <func_memory.h>


class MIPS {
privet:
	RF* rf;
	uint32 PC;
	FuncMemory* mem;
public:
	MIPS();
	void run( const string& tr, uint instr_to_run);
};

enum RegNum {
    ZERO = 0,
    AT,
    V0, V1,
    A0, A1, A2, A3,
    T0, T1, T2, T3, T4, T5, T6, T7,
    S0, S1, S2, S3, S4, S5, S6, S7,
    T8, T9,
    K0, K1,
    GP,
    SP,
    FP,
    RA,
    MAX_REG
};

class RF {
	uint32 array[MAX_REG];
public:
	uint32 read( RegNum index) const { if (index < MAX_REG) return(array[index]);}
	void write( RegNum index, uint32 data){ if (index < MAX_REG) array[index] = data;}
	void reset( RegNum index){ (index < MAX_REG) array[index] = 0;}
};

uint32 fetch() const;
void updatePC( const FuncInstr& instr);



