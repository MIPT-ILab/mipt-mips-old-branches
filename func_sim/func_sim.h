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
#include <finc_instr.h>

class MIPS {
        // storages of internal state
        RF* rf;
        uint32 PC;
        FuncMemory* mem;

    public:
        MIPS();
        void read_reg( FuncInstr& instr);
        void run( const string&, uint instr_to_run);
        void load( FuncInstr& instr); // load information from memory
        void store( const FuncInstr& instr); // store information in memory
        void ld_st( FuncInstr& instr); // calls load for loads, store for stores, nothing otherwise NOT DONE YET
        void updatePC( const FuncInstr& instr); // update PC
        uint32 fetch() const;
};

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

class RF {
       uint32 array[ MAX_REG]; // values of registers
   public:
       RF();

       uint32 read( RegNum index) const; // get value from register
       void write( RegNum index, uint32 data); // write value to register if it is not a $zero register
       void reset( RegNum index); // clears register to 0 value

   };

#endif // #ifndef FUNC_SIM__FUNC_SIM_H