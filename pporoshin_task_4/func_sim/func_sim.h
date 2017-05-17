/**
 * func_sim.h - header of module implementing 
 * functional simulator of MIPS
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_SIM__FUNC_SIM_H
#define FUNC_SIM__FUNC_SIM_H

// Generic C++
#include <string>

// uArchSim modules
#include <reg_file.h>
#include <func_instr.h>
#include <func_memory.h>
#include <types.h>

class ProgramExecuter
{
    static const FuncInstr::Register termination_reg = FuncInstr::REG_V0;

    std::string program_name; // Name of executing elf file
    FuncMemory *mem;          // Pointer to functional memory that executer operates 
    RegFile *reg_file;        // Pointer to register file that executer operates

    FuncInstr *cur_instr;     // Current parsed instruction
    uint32 cur_instr_code;    // Current instruction code
    uint32 pc;                // Current PC
    uint32 next_pc;           // PC of next instruction

    uint32 alu_res;           // Result of ALU
    uint32 load_res;          // Value, loaded from memory
    uint32 d_val;
    uint32 s_val;
    uint32 t_val;
    uint32 addr_val;          
    uint16 immed_val;
    uint8  shamt_val;

    bool   is_terminated;
    uint32 term_status;       // Value, written to termination register

    void fetch();
    void decode();
    void execute();
    void memoryOp();
    void writeBack();

public:

    ProgramExecuter( const char* elf_file_name);
    ~ProgramExecuter();
    void execInstr();
    void updatePC();
    bool isTerminated();
    uint32 getTermStatus();
    std::string dump( std::string indent); 
};

#endif