/**
 * func_instr.h - Header of module implementing the concept of 
 * disassembler of MIPS ISA.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2014 uArchSim iLab project
 */

#ifndef FUNC_INSTR_H
#define FUNC_INSTR_H

#include <string>
#include <ostream>
#include <cassert>

#include <types.h>
#include <func_memory.h>


class FuncInstr
{
private:
    enum Format
    {
        FORMAT_R,
        FORMAT_I,
        FORMAT_J
    };
    enum Type
    {
        ADD = 0,
        ADDU,
        SUB,
        SUBU,
        ADDI,
        ADDIU,
        SLL,
        SRL,
        BEQ,
        BNE,
        J,
        JR
    };
    union
    {
        struct
        {
            unsigned imm:16;
            unsigned t:5;
            unsigned s:5;
            unsigned opcode:6;
        } asI;
        struct
        {
            unsigned funct:6;
            unsigned s2:5;
            unsigned d:5;
            unsigned t:5;
            unsigned s1:5;
            unsigned opcode:6;
        } asR;
        struct
        {
            unsigned addr:26;
            unsigned opcode:6;
        } asJ;
        uint32 raw;
    } bytes;

    void InitFormat( uint32 _bytes);
    void ParseR();
    void ParseI();
    void ParseJ();
    const uint32 opcMask;
    const uint32 funcMask;
    uint32 format;
    string name, reg1, reg2, reg3;
    uint32 cnst; //const


public:
    FuncInstr( uint32 bytes);
    ~FuncInstr();
    struct ISAEntry
    {
        const char* name;
        uint8 opcode;
        uint8 funct;
    
        FuncInstr::Format format;
        FuncInstr::Type type;
    };
    static const ISAEntry isaTable[12];
    struct Reg
    {
        const char* name;
        uint8 num; 
    };
    static const Reg regTable[];


    
    std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, FuncInstr& instr);
#endif
