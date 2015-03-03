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

class MIPS
{
    RF* rf;
    uint32 PC;
    FuncMemory* mem;
    uint32 fetch(){ return mem->read(PC);}
    void updatePC( const FuncInstr& instr);
    void readSrc( FuncInstr& instr);
    void load(FuncInstr& instr, uint8 numOfBytes = 4) { instr.v_dst = mem->read(instr.mem_addr, numOfBytes);};
    void store(const FuncInstr& instr, uint8 numOfBytes = 4) { mem->write(instr.mem_addr, instr.v_dst, numOfBytes);};
    void ld_st(FuncInstr& instr);
    void wb( FuncInstr& instr);
    void dump( FuncInstr& instr);
public:
    MIPS();
    void run( const string& mem_copy, uint32 instr_to_run);

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
    MAXREG
};

class RF 
{
    std::array<uint32, MAXREG> regArr;
public:
    RF(){regArr.fill( 0);};
    RF( array<uint32, MAXREG>& prev){std::copy( prev.begin(), prev.end(), regArr.begin());};
    uint32 read( RegNum index){ return regArr[index];};
    void write( RegNum index, uint32 data);
    void reset( RegNum index){ regArr[index] = 0;};

};

#endif