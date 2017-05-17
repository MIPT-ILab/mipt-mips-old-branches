/**
 * func_sim.h - Header of module implementing the concept of 
 * MIPS single-sycle implementation.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2015 uArchSim iLab project
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

#include <func_memory.h>
#include <types.h>
#include <func_instr.h>
#include <sstream>
#include <array>



class RF 
{
    array<uint32, MAXREG> regArr;
public:
    RF(){regArr.fill( 0);};
    RF( array<uint32, MAXREG>& prev){std::copy( prev.begin(), prev.end(), regArr.begin());};
    uint32 read( RegNum index){ return regArr[index];};
    void write( RegNum index, uint32 data);
    void reset( RegNum index){ regArr[index] = 0;};
};

class MIPS
{
    RF* rf;
    uint64 PC;
    FuncMemory* mem;
    uint32 fetch(){ return mem->read(PC);}
    void updatePC( const FuncInstr& instr);
    void readSrc( FuncInstr& instr);
    void load(FuncInstr& instr, uint8 numOfBytes = 4) { instr.v_tgt = ( int32)mem->read(instr.mem_addr, numOfBytes);};
	void loadu( FuncInstr& instr, uint8 numOfBytes = 1) { instr.v_tgt = ( uint32)mem->read(instr.mem_addr, numOfBytes);};
    void store( FuncInstr& instr, uint8 numOfBytes = 4) { mem->write(instr.v_tgt, instr.mem_addr, numOfBytes);};
    void ld_st(FuncInstr& instr);
    void wb( FuncInstr& instr);
    void dump( FuncInstr& instr);
public:
    MIPS();
    void run( const string& mem_copy, uint32 instr_to_run);

};


#endif
