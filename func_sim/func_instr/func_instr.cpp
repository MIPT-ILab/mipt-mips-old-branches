/**
 * func_memory.cpp
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>
#include <cstdio>

// Generic C++
#include <iostream>
#include <string> 
#include <sstream>

// uArchSim modules
#include <func_instr.h>

const FuncInstr::InstrInfo FuncInstr::ISA[ MAX_NUM_OF_INSTR] = 
{
                { I_TYPE,  "addi",  ADDI,  0x0 },
                { I_TYPE, "addiu", ADDIU,  0x0 },
                { I_TYPE,   "beq",   BEQ,  0x0 },
                { I_TYPE,   "bne",   BNE,  0x0 },
                { J_TYPE,     "j",  JUMP,  0x0 },
                { R_TYPE,   "add",   0x0,  ADD },
                { R_TYPE,  "addu",   0x0, ADDU },
                { R_TYPE,   "sub",   0x0,  SUB },
                { R_TYPE,  "subu",   0x0, SUBU },
                { R_TYPE,   "sll",   0x0,  SLL },
                { R_TYPE,   "srl",   0x0,  SRL },
                { R_TYPE,    "jr",   0x0,   JR }
};

const string FuncInstr::REGNAME[ MAX_NUM_OF_REG] = 
{   "$zero", "$at",
      "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", 
      "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
      "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9",
      "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" 
};

//to default pseudo-instruction turned on, exept clear
FuncInstr::TurnPseudo FuncInstr::pseudo_instr = { true, true, false };

int FuncInstr::setType()
{

    for ( short i = 0; i < MAX_NUM_OF_INSTR; ++i)
    {
        if ( this->convert.asR.opcode == ISA[ i].opcode)
        {
            this->type = ISA[ i].type;
            return 0;
        }
    }  
    cerr << "\nUnknown instruction!!\n"; 
    exit( EXIT_FAILURE);
}

void FuncInstr::setNameForRtype()
{
    for ( uint8 i = 0; i < MAX_NUM_OF_INSTR; ++i)
    {
        if ( ISA[ i].opcode == NULL_VAL && this->convert.asR.funct == ISA[ i].funct)
        {
            this->name   = ISA[ i].name;
            this->opcode = ISA[ i].opcode;
            this->funct  = ISA[ i].funct;
        }
    }
}

void FuncInstr::setNameForIandJtypes()
{
    for ( short i = 0; i < MAX_NUM_OF_INSTR; ++i)
    {
        if ( this->convert.asI.opcode == ISA[ i].opcode)
        {
            this->name   = ISA[ i].name;
            this->opcode = ISA[ i].opcode;
            this->funct  = ISA[ i].funct;
        }
    }   
}    
/*I don't check here if some instructions have source or target
registers in order to don't rewrite these functions if we'll have 
another R type instr. without ones*/
void FuncInstr::setSource()
{
    this->source = REGNAME[ this->convert.asR.source];
}

void FuncInstr::setTarget()
{
    this->target = REGNAME[ this->convert.asR.target];
}

void FuncInstr::setDest()
{
    this->dest = REGNAME[ this->convert.asR.dest];
}

void FuncInstr::setShamt()
{
    this->shamt = this->convert.asR.shamt;
}

void FuncInstr::setImmed()
{
    this->immed = this->convert.asI.immed;
}

void FuncInstr::setJump()
{
    this->jump = this->convert.asJ.jump;
}

FuncInstr::FuncInstr( uint32 bytes)
{
    this->convert.asBytes = bytes;
    setType();
    switch ( this->type)
    {
        case R_TYPE: setNameForRtype();
                setSource();
                setTarget();
                setDest();
                setShamt();
            break;
        case I_TYPE: setNameForIandJtypes();
                setSource();
                setTarget();
                setImmed();
            break;
        case J_TYPE: setNameForIandJtypes();
                setJump();
            break;
        default: assert( 0);
    }
}

bool FuncInstr::is_nop() const
{
    return ( this->name == "sll") && ( this->target == "$zero") &&
           ( this->dest == "$zero") && ( this->shamt == 0x0);
}

bool FuncInstr::is_move() const
{
    return ( this->name == "addu") && ( this->target == "$zero");
}

bool FuncInstr::is_clear() const
{
    return ( this->name == "add") && ( this->source == "$zero") &&
           ( this->dest == "$zero");
}

string FuncInstr::Dump( string indent) const
{
    ostringstream os;
    os << hex;
    if ( pseudo_instr.nop && is_nop())
    {
        os << indent << "nop" << endl;
    } else if ( pseudo_instr.move && is_move())
    {
        os << indent << "move " << this->dest << ", "
           << this->source << endl;
    } else if ( pseudo_instr.clear && is_clear())
    {
        os << indent << "clear " << this->target << endl;
    } else
    {
        os << indent << this->name << ' ';
        switch ( this->type)
        {
            case J_TYPE: os << this->jump << endl;
                break;
            case I_TYPE: 
                if ( this->opcode == BEQ || this->opcode == BNE)
                {
                    os << this->source << ", " << this->target 
                       << ", " << this->immed << endl;
                } else
                {
                    os << this->target << ", " << this->source 
                       << ", " << this->immed << endl;
                }
                break;
            case R_TYPE: 
                if ( this->funct == JR)
                {
                    os << this->source << endl;
                } else if ( this->funct == SLL || this->funct == SRL)
                {
                    os << this->dest << ", " << this->target << ", "
                       << this->shamt << endl;
                } else 
                {
                    os << this->dest << ", " << this->source << ", "
                       << this->target << endl;
                }
                break;
            default: assert( 0);
        }
    }
    return os.str();
}

ostream& operator << ( ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}
