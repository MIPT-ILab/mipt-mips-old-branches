/**
 * func_sim.h 
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_SIM__FUNC_SIM_H
#define FUNC_SIM__FUNC_SIM_H

// Generic C++
#include <sstream>
#include <string>

// uArchSim modules
#include <func_memory.h>
#include <func_instr.h>
#include <types.h>
#include <reg_file.h>

class FuncSim
{
    unsigned int instr_count; //store current number of instruction
    FuncMemory *mem_file; // here will be stored binary file
    uint32 PC; //program counter
    RegArray reg_data; // all registers will be stored here
    ostringstream for_dump; 
    mutable bool dump_state; // dump is needed or not?
    
    //You can't create object with this default constructor
    FuncSim();
public:
/* Elf_file - binary file, that will be executed on our simulator
 * FuncSim creates FuncMemory object and prepares work with it
 * */
    FuncSim( const char *elf_file);
//decode the instruction, located in bytes
    FuncInstr decodeInstr( uint32 bytes);
//calculate the output of ALU for any instruction
    void execInstr( const FuncInstr& instr);
//for each instruction separate function
    void execAdd(  const FuncInstr& instr);
    void execAddu( const FuncInstr& instr);
    void execSub(  const FuncInstr& instr);
    void execSubu( const FuncInstr& instr);
    void execSll(  const FuncInstr& instr);
    void execSrl(  const FuncInstr& instr);
    void execJr(   const FuncInstr& instr);
    void execAddi( const FuncInstr& instr);
    void execAddiu(const FuncInstr& instr);
    void execBeq(  const FuncInstr& instr);
    void execBne(  const FuncInstr& instr);
    void execJump( const FuncInstr& instr);
    void execLw(   const FuncInstr& instr);
    void execSw(   const FuncInstr& instr);
    void execLui(  const FuncInstr& instr);
    void execNop(  const FuncInstr& instr);
    void execMove( const FuncInstr& instr);
    void execClear(const FuncInstr& instr);
//print instruction
    string dump( ) const;
// You can choose dump state outside of programm.
    void setDumpState( bool state) const;
//run the whole simulation process
    void simulRun();
};

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H
