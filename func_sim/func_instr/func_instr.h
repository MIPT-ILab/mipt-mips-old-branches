/**
 * func_instr.h - Header of module implementing the 
 * functional model of MIPS instruction.
 * @author Dmitry Borody <dmitry.borody@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_INSTR__FUNC_INSTR_H
#define FUNC_INSTR__FUNC_INSTR_H

// Generic C++
#include <string>
#include <iostream>

// uArchSim modules
#include <types.h>
#include <func_instr.h>

using namespace std;

struct Register 
{
    int num;
    const char* name;
};

class FuncInstr
{
    // You could not create the object
    // using this default constructor
    FuncInstr(){}

public:

    enum FormatType 
    {
        RTYPE, 
        ITYPE, 
        JTYPE,
    };

    enum Type 
    {
        ADD,       // add
        ADDU,      // unsigned add
        SUB,       // substract
        SUBU,      // unsigned substract
        ADDI,      // add immediate
        ADDIU,     // add immediate unsigned
        SLL,       // left logical shift
        SRL,       // right logical shift
        BEQ,       // branch equel
        BNE,       // branch not equal
        JMP,       // unconditional jump
        JRE,       // jump to address in a register
    };
    
    explicit FuncInstr( uint32 bits);

    virtual ~FuncInstr();
    
    std::string Dump( std::string indent = " ") const;

private:

    Register      rd,       // Destination register
                  rs,       // Source register
                  rt;       // Target register

    uint16        imm;      // Immediate value for I-instructions
    uint32        addr;     // Jump address for J-instructions
    uint8         shimm;    // Immediate value for shift 
                              // instructions

    Type          type;
    FormatType    format;

    const char*   name;     // Human-readable instruction name
};

std::ostream& operator<<( std::ostream &out, const FuncInstr &instr);

#endif // #ifndef FUNC_INSTR__FUNC_INSTR_H
