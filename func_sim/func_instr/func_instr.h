/**
 * func_instr.h - header of module implementing parsing
 * of 32-bit words-instructions for further use
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_INSTR__FUNCT_INSTR_H
#define FUNC_INSTR__FUNCT_INSTR_H

// Generic C++
#include <string>
#include <iostream>

// uArchSim modules
#include <types.h>

class FuncInstr
{
    // Enumeration for specifying type of instructions
    enum Type
    {
        TYPE_R, 
        TYPE_I, 
        TYPE_J
    };

    // Enumeration for specifying instructions
    enum Instr
    {
        INSTR_ADD,  
        INSTR_ADDU,  
        INSTR_SUB,  
        INSTR_SUBU,
        INSTR_ADDI, 
        INSTR_ADDIU, 
        INSTR_SLL,  
        INSTR_SRL,
        INSTR_BEQ,  
        INSTR_BNE,   
        INSTR_J,    
        INSTR_JR
    };

    // Enumeration for specifying pseudo instructions
    enum PseudoInstr
    {
        PINSTR_NON_PSEUDO, 
        PINSTR_NOP, 
        PINSTR_MOVE, 
        PINSTR_CLEAR,
    };

    // Enumeration for registers
    enum Register
    {
        REG_ZERO = 0, 
        REG_AT, 
        REG_V0, 
        REG_V1,
        REG_A0,       
        REG_A1, 
        REG_A2, 
        REG_A3,
        REG_T0,       
        REG_T1, 
        REG_T2, 
        REG_T3,
        REG_T4,       
        REG_T5, 
        REG_T6, 
        REG_T7,
        REG_S0,       
        REG_S1, 
        REG_S2, 
        REG_S3,
        REG_S4,       
        REG_S5, 
        REG_S6, 
        REG_S7,
        REG_T8,       
        REG_T9, 
        REG_K0, 
        REG_K1,
        REG_GP,       
        REG_SP, 
        REG_FP, 
        REG_RA
    };

    // Enumeration that specifying the way
    // the asm code of instruction must look like
    enum PrintType
    {
        PT_DST,
        PT_TSI, 
        PT_DTSH, 
        PT_STI, 
        PT_ADDR, 
        PT_S,   
        PT_TS,  
        PT_T,    
        PT_NAME
    };

    // A struct for containing information
    // about supported instructions
    struct InstrInfo_t
    {
        std::string name;
        Type        type;
        Instr       instr;
        PrintType   printType;
        uint8       opcode;
        uint8       funct;
    };

    // A union-helper for parsing of instruction
    union InstrParser_t
    {
        struct
        {
            unsigned funct  : 6;
            unsigned shamt  : 5;
            unsigned d_reg  : 5;
            unsigned t_reg  : 5;
            unsigned s_reg  : 5;
            unsigned opcode : 6;
        } asR;
        struct
        {
            unsigned addr   : 26;
            unsigned opcode : 6;
        } asJ;
        struct
        {
            unsigned immed  : 16;
            unsigned t_reg  : 5;
            unsigned s_reg  : 5;
            unsigned opcode : 6;
        } asI;
        uint32 bytes;
    };

    static const int InstrInfosSize;
    static const int RegNamesSize;
    static const InstrInfo_t InstrInfos[];
    static const std::string RegNames[];

    std::string name;
    Type        type;
    Instr       instr;
    PseudoInstr pseudoInstr;
    PrintType   printType;
    uint32      instrBytes;
    uint32      addr; 
    uint16      immed;
    uint8       opcode;
    uint8       funct;
    uint8       s_reg;
    uint8       t_reg;
    uint8       d_reg;
    uint8       shamt;

    void DefineType();
    void DefineInnnerFields();
    void DefineInstr();
    void DefinePseudoInstr();

public:
    FuncInstr( uint32 bytes);
    std::string Dump( std::string indent = " ") const;
};

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);

#endif