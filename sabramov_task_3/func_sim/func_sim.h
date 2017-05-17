/**
 * func_sim.h - Header of simulator of the MIPS CPU
 * @author Semyon Abramov <semyon.abramov.mipt@gmail.com>
 * 2014 - 2015 iLab project MIPT - MIPS
 */

 
// protection from multi-include
#ifndef FUNC_SIM_H
#define FUNC_SIM_H


// Generic C++
#include<iostream>
#include<string>

// UArchSim modules
#include<func_memory.h>
#include<func_instr.h>
#include<types.h>


using namespace std;

enum RegNum 
{
	zero,
	at,
	v0, v1,
	a0, a1, a2, a3,
	t0, t1, t2, t3, t4, t5, t6, t7,
	s0, s1, s2, s3, s4, s5, s6, s7,
	t8, t9, 
	k0, k1,
	gp,
	sp,
	fp,
	ra, 
	MAX_REG = 32
};

/* Class RF implements Register File */

class RF 
{
		uint32 array[MAX_REG];
	public:
		RF ();	
		~RF();
		uint32 read( RegNum index) const;
		void write( RegNum index, uint32 data);
		void reset( RegNum index);
};

/* Class MIPS with modules implement single - circle CPU */

class MIPS
{
		RF* rf;
		FuncMemory* mem;
		uint32 PC;
	public:
		MIPS();
		~MIPS();
		void run(const string&, int instr_to_run);   	
		uint32 fetch() const;
		void read_src( FuncInstr& instr);
		void load(FuncInstr& instr); 
		void store(FuncInstr& instr); 
		void ld_st(FuncInstr& instr);
		void wb( FuncInstr& instr);
		uint32 updatePC( const FuncInstr& instr );
	
	
};

#endif // FUNC_SIM_H 