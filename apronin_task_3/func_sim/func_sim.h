/**
 * func_sim.h - Header of module implementing single-cycle MIPS simulator
 * @author Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * Copyright 2015 MIPT-MIPS
 */

// protection from multi-include
#ifndef FUNC_SIM__FUNC_SIM_H
#define FUNC_SIM__FUNC_SIM_H

// Generic C++
#include <string>
#include <iostream>
#include <cassert>

// MIPT-MIPS modules
#include <types.h>
#include <elf_parser.h>
#include <func_memory.h>
#include <func_instr.h>

// registers numbers
enum RegNum {
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
    MAX_REG
};

// registers
class RF {
       uint32 array[ MAX_REG]; // values of registers
   public:
       RF(); // stores NULL in $zero register

       uint32 read( int index) const; // get value from register
       void write( int index, uint32 data); // write value to register if it is not a $zero register
       void reset( int index); // clears register to 0 value

};

// current mips state
class MIPS {
        // storages of internal state
        RF* rf;
        uint32 PC;
        FuncMemory* mem;

    public:
        MIPS(); // READY
        virtual ~MIPS(); // READY
        void read_reg( FuncInstr& instr); // get the values from working registers
        void wb(const FuncInstr& instr); // save value of dst register
        void run( const string&, uint32 instr_to_run); // run the program
        void load( FuncInstr& instr); // load information from memory
        void store( const FuncInstr& instr); // store information in memory
        void ld_st( FuncInstr& instr); // calls load for loads, store for stores, nothing otherwise
        void updatePC( const FuncInstr& instr); // update PC
        uint32 fetch() const; // fetch
};

#endif // #ifndef FUNC_SIM__FUNC_SIM_H
