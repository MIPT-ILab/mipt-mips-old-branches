/*******************************************************

    Interface for a functional simulator

    @file: func_sim.h
    @date: March 7, 2015
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @vertion: 1.0 (March 7, 2015) 

*******************************************************/

#ifndef DEF_ONCE
#define DEF_ONCE

#include <cstdio>
#include <cstdlib>
#include <string>

#include <types.h>
#include <func_memory/func_memory.h>
#include <func_instr/func_instr.h>

enum RegNum;

class RF
{
	uint32 array[MAX_REG];
public:
	uint32 read(RegNum index) const;
	inline void write(RegNum index, uint32 data) { if (index < MAX_REG) array[index] = data; }
	inline void reset(RegNum index) { if (index < MAX_REG) array[index] = 0; }
};

class MIPS
{
	RF *rf;
	uint32 PC;
	FuncMemory *mem;

	inline uint32 fetch() const { return mem->read(PC); }
	inline void updatePC(const FuncInstr &instr) { PC = instr.new_PC; }

	inline void load(FuncInstr& instr) 
	{ instr.v_dst = mem->read(instr.mem_addr, instr.mem_num_bytes); }
	inline void store(const FuncInstr& instr) 
	{ mem->write(instr.mem_addr, instr.v_dst, instr.mem_num_bytes); }
	void ld_st(FuncInstr& instr);

	inline void wb(FuncInstr &instr) { rf->write(instr.get_dst_num_index(), instr.v_dst); }
	void read_src(FuncInstr& instr);
public:
	inline  MIPS(): PC(0), mem(NULL) { rf = new RF; }
        inline ~MIPS() { delete rf; }
	void run(const char *tr, uint instr_to_run); 
};

#endif //DEF_ONCE