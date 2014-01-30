/**
 * func_instr.cpp - the module implementing a simple disassembler for MIPS processors.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */


// Generic C
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <cmath>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <elf_parser.h>
#include "func_instr.h"


FuncInstr::FuncInstr( uint32 bytes)
{
    this->initFormat( bytes);
    switch( this->format)
    {
        case FORMAT_R:
            this->parseR( bytes);
            break;
        case FORMAT_I:
            this->parseI( bytes);
            break;
        case FORMAT_J:
            this->parseJ( bytes);
            break;
        default:
        assert( 0); // This line actually never gets reached
    }
};



void FuncInstr::initFormat( uint32 bytes)
{
    this->bytes.raw = bytes;
    switch( this->bytes.asR.opcode)
    {
        case ( 0x0):
            this->format = FORMAT_R;
            break;
        case ( 0x4 || 0x5 || 0x8 || 0x9):
            this->format = FORMAT_I;
            break;
        case ( 0x2):
            this->format = FORMAT_J;
            break;
        default:
        cerr << "ERROR: invalid opcode\n";
        assert( 0);
    }

};



void FuncInstr::initRType()
{
    for( this->isa_id = 0; this->isa_id < 16; ++this->isa_id)
    {
        if( this->isa_id == 15)
        {
            cerr << "Unable to find a command in the ISA table. "
                 << "Please contact your system administrator" << endl;
        }

        if(( this->bytes.asR.opcode == this->isaTable[ this->isa_id].opcode) &&
           ( this->bytes.asR.funct  == this->isaTable[ this->isa_id].funct ))
        {
                this->type = this->isaTable[ this->isa_id].type;
                break;
        }
    }

    // Handling ambiguous types

    // sll -> nop (could-ev jst write "if byte==0", buut this is not cool
    if( this->isa_id == 6 && bytes.asR.t == 0
                          && bytes.asR.d == 0
                          && bytes.asR.S == 0)
    {
        this->isa_id = 14;
        this->type = this->isaTable[ this->isa_id].type;
    }

    // addu -> clear
    if( this->isa_id == 1 && bytes.asR.t == 0
                          && bytes.asR.s == 0)
    {
        this->isa_id = 13;
        this->type = this->isaTable[ this->isa_id].type;
    }
};



void FuncInstr::initIType()
{
    for( this->isa_id = 0; this->isa_id < 16; ++this->isa_id)
    {
        if( this->isa_id == 15)
        {
            cerr << "Unable to find a command in the ISA table. "
                 << "Please contact your system administrator" << endl;
        }

        if( this->bytes.asI.opcode == this->isaTable[ this->isa_id].opcode)
        {
            this->type = this->isaTable[ this->isa_id].type;
            break;
        }
    }
    // Handling ambiguous types

    // addiu -> move
    if( this->isa_id == 5 && bytes.asI.imm == 0)
    {
        this->isa_id = 12;
        this->type = this->isaTable[ this->isa_id].type;
    }



};



void FuncInstr::initJType()
{
    for( this->isa_id = 0; this->isa_id < 16; ++this->isa_id)
    {
        if( this->isa_id == 15)
        {
            cerr << "Unable to find a command in the ISA table. "
                 << "Please contact your system administrator" << endl;
        }

        if( this->bytes.asJ.opcode == this->isaTable[ this->isa_id].opcode)
        {
            this->type = this->isaTable[ this->isa_id].type;
            break;
        }
    }


    // Handling ambiguous types
    // NONE


};



void FuncInstr::parseR( uint32 bytes)
{
    this->initRType();
    switch( this->type)
    {
        case ADD:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asR.d] <<
                ", $"  << this->REG_NAME[ this->bytes.asR.s] <<
                ", $"  << this->REG_NAME[ this->bytes.asR.t];
            break;
        case ROLL:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asR.d] <<
                ", $"  << this->REG_NAME[ this->bytes.asR.t] <<
                ", 0x" << hex << (uint32)this->bytes.asR.S << dec;
            break;
        case JR:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asR.s];
            break;
        case CLR:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asR.t];
            break;
        case NOP:
            this->ss   << this->isaTable[ this->isa_id].name;
            break;
        default:
        cerr << "ERROR: unknown command\n";
        assert( 0);
    }
};



void FuncInstr::parseI( uint32 bytes)
{
    this->initIType();
    switch( this->type)
    {
        case ADDI:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asI.t] <<
                ", $"  << this->REG_NAME[ this->bytes.asI.s] <<
                ", 0x" << hex << (int)this->bytes.asI.imm    << dec;
            break;
        case ADDU:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asI.t] <<
                ", $"  << this->REG_NAME[ this->bytes.asI.s] <<
                ", 0x" << hex << (uint32)this->bytes.asI.imm << dec;
            break;
        case BEQ:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asI.s] <<
                ", $"  << this->REG_NAME[ this->bytes.asI.t] <<
                ", 0x" << hex << (uint32)this->bytes.asI.imm << dec;
            break;
        case MOVE:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " $"   << this->REG_NAME[ this->bytes.asI.t] <<
                ", $"  << this->REG_NAME[ this->bytes.asI.s];
            break;
        default:
        cerr << "ERROR: unknown command\n";
        assert( 0);
    }
};



void FuncInstr::parseJ( uint32 bytes)
{
    this->initJType();
    switch( this->type)
    {
        case JUMP:
            this->ss   << this->isaTable[ this->isa_id].name <<
                " 0x"  << hex << (uint32)this->bytes.asJ.addr << dec;
            break;
        default:
        cerr << "ERROR: unknown command\n";
        assert( 0);
    }
};



std::string FuncInstr::Dump( std::string indent) const
{
    stringstream dump;
    dump << indent << ss.str();
    return dump.str();
};



const ISAEntry FuncInstr::isaTable[ 15] =               // Important! Do NOT change the order of ISA entries
{                                                       // DO change the MAX_ISA_ENTRIES constant if added a new entry
    //  name      opcode      func    format       type
    {  "add",     0x0,        0x20,   FORMAT_R,    ADD },
    {  "addu",    0x0,        0x21,   FORMAT_R,    ADD },
    {  "sub",     0x0,        0x22,   FORMAT_R,    ADD },
    {  "subu",    0x0,        0x23,   FORMAT_R,    ADD },
    {  "addi",    0x8,        0x00,   FORMAT_I,    ADDI},
    {  "addiu",   0x9,        0x00,   FORMAT_I,    ADDU},
    {  "sll",     0x0,        0x00,   FORMAT_R,    ROLL},
    {  "srl",     0x0,        0x02,   FORMAT_R,    ROLL},
    {  "beq",     0x4,        0x00,   FORMAT_I,    BEQ },
    {  "bne",     0x5,        0x00,   FORMAT_I,    BEQ },
    {  "j",       0x2,        0x00,   FORMAT_J,    JUMP},
    {  "jr",      0x0,        0x08,   FORMAT_R,    JR  },
    {  "move",    0x9,        0x00,   FORMAT_I,    MOVE},
    {  "clear",   0x0,        0x21,   FORMAT_R,    CLR },
    {  "nop",     0x0,        0x00,   FORMAT_R,    NOP }
};



const string FuncInstr::REG_NAME[ 32] =
{
    "zero", "at", "v0", "v1",
    "a0",   "a1", "a2", "a3",
    "t0",   "t1", "t2", "t3",
    "t4",   "t5", "t6", "t7",
    "s0",   "s1", "s2", "s3",
    "s4",   "s5", "s6", "s7",
    "t8",   "t9", "k0", "k1",
    "gp",   "sp", "s8", "ra"
};

/*
std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr)
{
     return instr->Dump("");
};
*/
