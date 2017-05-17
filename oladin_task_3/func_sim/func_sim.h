/**
 * func_sim.h
 * Header of the MIPS functional simulator.
 * MIPT-MIPS Assignment 3.
 * Ladin Oleg.
 */

#ifndef FUNC_SIM_H
#define FUNC_SIM_H

/* Functional simulator modules. */
#include <types.h>
#include <func_memory.h>
#include <func_instr.h>

/* Enumeration of registers. */
enum RegNum
{
    REG_ZERO,
    REG_AT,
    REG_V0, REG_V1,
    REG_A0, REG_A1, REG_A2, REG_A3,
    REG_T0, REG_T1, REG_T2, REG_T3, REG_T4, REG_T5, REG_T6, REG_T7,
    REG_S0, REG_S1, REG_S2, REG_S3, REG_S4, REG_S5, REG_S6, REG_S7,
    REG_T8, REG_T9,
    REG_K0, REG_K1,
    REG_GP,
    REG_SP,
    REG_FP,
    REG_RA,
    REG_MAX
};

/* Register file. */
class RF
{
    private:
        uint32 array[ REG_MAX]; // register storage
    public:
        uint32 read( RegNum index) const;
        void write( RegNum index, uint32 data);
        void reset( RegNum index); // set register to 0 value
};

/* Main class - simulator implementation. */
class MIPS
{
    private:
        uint32 PC; // programm counter
        FuncMemory* mem; // functional memory
        RF* rf; // register file

        uint32 fetch() const; // fetch
        void read_src( FuncInstr& instr); // read sources
        void ld_st( FuncInstr& instr); // check memory access
        void load( FuncInstr& instr); // read from memory
        void store( const FuncInstr& instr); // write into memory
        void wb( const FuncInstr& instr); // writeback
        void updatePC( const FuncInstr& instr); // update programm counter
        string dmp( const FuncInstr& instr); // runtime disassembler dump

    public:
        MIPS();
        ~MIPS();
        /* Launches simulator. */
        void run( const string& tr, unsigned int instr_to_run);
};

#endif // #ifndef FUNC_SIM_H
