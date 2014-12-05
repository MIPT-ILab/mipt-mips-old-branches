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

    void InitFormat( uint32 bytes);
    void ParseR();
    void ParseI();
    void ParseJ();
    const uint32 opcMask;
    const uint32 funcMask;
    uint32 format;
    char* name, *reg1, *reg2, *reg3;
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
    } ;
    static const ISAEntry isaTable[];
    struct Reg
    {
        const char* name;
        uint8 num;
        uint32 val;
        
    };
    static const Reg regTable[];


    
    std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, FuncInstr& instr);
const FuncInstr::ISAEntry FuncInstr::isaTable[] = 
{
    //name  opcode func  format               type
    { "add ", 0x0, 0x20, FuncInstr::FORMAT_R, FuncInstr::ADD },
    { "addu ", 0x0, 0x21, FuncInstr::FORMAT_R, FuncInstr::ADDU },
    { "sub ", 0x0, 0x22, FuncInstr::FORMAT_R, FuncInstr::SUB },
    { "subu ", 0x0, 0x23, FuncInstr::FORMAT_R, FuncInstr::SUBU },
    { "addi ", 0x8, 0, FuncInstr::FORMAT_I, FuncInstr::ADDI },
    { "addiu ", 0x9, 0, FuncInstr::FORMAT_I, FuncInstr::ADDIU },
    { "sll ", 0x0, 0x0, FuncInstr::FORMAT_R, FuncInstr::SLL },
    { "srl ", 0x0, 0x2, FuncInstr::FORMAT_R, FuncInstr::SRL },
    { "beq ", 0x4, 0, FuncInstr::FORMAT_I, FuncInstr::BEQ },
    { "bne ", 0x5, 0, FuncInstr::FORMAT_I, FuncInstr::BNE },
    { "j ", 0x2, 0, FuncInstr::FORMAT_J, FuncInstr::J },
    { "jr ", 0x0, 0x8, FuncInstr::FORMAT_R, FuncInstr::JR }
};
const FuncInstr::Reg FuncInstr::regTable[] = 
{
    { "$zero ", 0, 0 },
    { "$at ", 1, 0 },
};

#endif