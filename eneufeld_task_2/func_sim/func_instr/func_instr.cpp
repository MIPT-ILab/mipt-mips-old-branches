/**
* disasm.cpp - disassembler for MIPS
* @author Erwin Neufeld <neufeld.erwin@gmail.com>
* Copyright 2013 uArchSim iLab project
*/

//Generic C
#include <assert.h>
#include <stdlib.h>

//Generic C++
#include <iostream>
#include <string>
#include <sstream>

//uArchSim modules
#include <func_instr.h>
#include <types.h>
#include <instructions.h>


int FuncInstr::GetType( uint32 bytes) 
{
    unsigned int opcode = bytes >> 26;
    if( opcode <= Rtype) 
    {
       this->type = Rtype; 
       GetRInstr( bytes);
       return 0;
    }
    else if( opcode <= Jtype) 
    {
        this->type = Jtype;
        GetJInstr( bytes);
        return 0;
    }
    else if( opcode <= Itype) 
    {
        this->type = Itype;
        GetIInstr( bytes);
        return 0;
    }
    return -1;
}

int FuncInstr::GetRInstr( uint32 bytes)
{
    this->instruction = bytes & 0x3F;
    bytes >>= 6;
    this->shamt = bytes & 0x1F;
    bytes >>= 5;
    this->dest_reg = bytes & 0x1F;
    bytes >>= 5;
    this->target_reg = bytes & 0x1F;
    bytes >>= 5;
    this->source_reg = bytes & 0x1F;
    bytes >>= 5;
    assert( bytes == 0);
    return 0;
}
    
int FuncInstr::GetIInstr( uint32 bytes)
{
    this->constant = bytes & 0xFF;
    bytes >>=16;
    this->target_reg = bytes & 0x1F;
    bytes >>= 5;
    this->source_reg = bytes & 0x1F;
    bytes >>= 5;
    this->instruction = bytes & 0x3F;
    bytes >>= 6;
    assert( bytes == 0);
    return 0;
}

int FuncInstr::GetJInstr( uint32 bytes)
{
    this->address = bytes & 0x03FFffFF;
    bytes >>= 26;
    this->instruction = bytes;
    bytes >> 6;
    assert( 0);
}

FuncInstr::FuncInstr( uint32 bytes)
{
    GetType( bytes);
}
const char * registers[] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1",
                             "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", "$t4",
                             "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3", 
                             "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0",
                             "$k1", "$gp", "$sp", "$fp", "$ra"};
#define PRINT_IF_INSTR(x) if(this->instruction == x)  oss << #x << ' ' 
std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;
    oss << std::hex << indent;
    if( this->type == Rtype) {
        if( this->instruction + this->source_reg + this->target_reg 
            + this->dest_reg == 0)
        {
            oss << "nop";
        }
        else if( this->instruction <= srl)
        {
            PRINT_IF_INSTR(sll);
            PRINT_IF_INSTR(srl);
            oss << registers[ this->dest_reg] << ' ' << 
                   registers[ this->target_reg] << ' ' <<
                   this->constant;
        }
        else if( this->instruction == 8)
        {
            PRINT_IF_INSTR(jr);
            oss << registers[ this->source_reg];
        }
        else
        {
            PRINT_IF_INSTR(add);
            PRINT_IF_INSTR(addu);
            PRINT_IF_INSTR(sub);
            PRINT_IF_INSTR(subu);
            oss << registers[ this->source_reg] << ' '<<
                   registers[ this->dest_reg] << ' ' << 
                   registers[ this->target_reg];
        }
    }
    else if( this->type == Itype) 
    {
        PRINT_IF_INSTR(addi);
        PRINT_IF_INSTR(addiu);
        PRINT_IF_INSTR(beq);
        PRINT_IF_INSTR(bne);
        oss << registers[ this->source_reg] << registers[ this->target_reg]
            << this->constant;
    }
    else if( this->type == Jtype)
    {
        PRINT_IF_INSTR(j);
        oss << this->address;
    }
    return oss.str();;
}

std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump();
    return out;
}
