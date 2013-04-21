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

const FuncInstr::InstrInfo FuncInstr::ISA[ ] = 
{
                { I_TYPE,  "addi",  ADDI,  0x0,  TS_ORDER },
                { I_TYPE, "addiu", ADDIU,  0x0,  TS_ORDER },
                { I_TYPE,   "beq",   BEQ,  0x0,  ST_ORDER },
                { I_TYPE,   "bne",   BNE,  0x0,  ST_ORDER },
                { I_TYPE,    "lw",  0x23,  0x0,  TS_ORDER },
                { I_TYPE,    "sw",    SW,  0x0,  TS_ORDER },
                { I_TYPE,   "lui",   LUI,  0x0,   T_ORDER },
                { J_TYPE,     "j",  JUMP,  0x0,    NO_REG },
                { R_TYPE,   "add",   0x0,  ADD, DST_ORDER },
                { R_TYPE,  "addu",   0x0, ADDU, DST_ORDER },
                { R_TYPE,   "sub",   0x0,  SUB, DST_ORDER },
                { R_TYPE,  "subu",   0x0, SUBU, DST_ORDER },
                { R_TYPE,   "sll",   0x0,  SLL,  DT_ORDER },
                { R_TYPE,   "srl",   0x0,  SRL,  DT_ORDER },
                { R_TYPE,    "jr",   0x0,   JR,   S_ORDER }
};

const short FuncInstr::MAX_NUM_OF_INSTR = sizeof( ISA) / sizeof( ISA[0]);

const string FuncInstr::REGNAME[ MAX_NUM_OF_REG] = 
{   "$zero", "$at",
      "$v0", "$v1", "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", 
      "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
      "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", "$t8", "$t9",
      "$k0", "$k1", "$gp", "$sp", "$fp", "$ra" 
};

//to default pseudo-instruction is turned on
FuncInstr::TurnPseudo FuncInstr::pseudo_instr = { true, true, true };

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
    cerr << "\nUnknown instruction!!\n" << hex << convert.asBytes << endl; 
    exit( EXIT_FAILURE);
}

void FuncInstr::setNameForRtype()
{
    for ( uint8 i = 0; i < MAX_NUM_OF_INSTR; ++i)
    {
        if ( ISA[ i].opcode == NULL_VAL && this->convert.asR.funct == ISA[ i].funct)
        {
            this->name      = ISA[ i].name;
            this->opcode    = ISA[ i].opcode;
            this->funct     = ISA[ i].funct;
            this->reg_order = ISA[ i].reg_order;
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
            this->reg_order = ISA[ i].reg_order;
        }
    }   
}    
/*I don't check here if some instructions have source or target
registers in order to don't rewrite these functions if we'll have 
another R type instr. without ones*/
void FuncInstr::setSource()
{
    this->source = this->convert.asR.source;
    this->source_name = REGNAME[ this->source];
}

void FuncInstr::setTarget()
{
    this->target = this->convert.asR.target;
    this->target_name = REGNAME[ target];
}

void FuncInstr::setDest()
{
    this->dest = this->convert.asR.dest;
    this->dest_name = REGNAME[ dest];
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
    return ( this->funct == SLL) && ( this->target_name == "$zero") &&
           ( this->dest_name == "$zero") && ( this->shamt == 0x0);
}

bool FuncInstr::is_move() const
{
    bool true_type = this->funct == ADDU || this->funct == ADD; 
    return ( true_type) && ( this->target_name == "$zero");
}

bool FuncInstr::is_clear() const
{
    return ( this->funct == ADD) && ( this->source_name == "$zero") &&
           ( this->dest_name == "$zero");
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
        os << indent << "clear " << this->target_name << endl;
    } else
    {
        os << indent << this->name << ' ';
        switch ( this->reg_order)
        {
            case DST_ORDER: os << this->dest_name   << ", "
                               << this->source_name << ", "
                               << this->target_name << endl;
                break;
            case DT_ORDER: os  << this->dest_name   << ", "
                               << this->target_name << ", "
                               << this->shamt       << endl;
                break;
            case TS_ORDER: os  << this->target_name << ", "
                               << this->source_name << ", "
                               << this->immed       << endl;
                break;
            case ST_ORDER: os  << this->source_name << ", "
                               << this->target_name << ", "
                               << this->immed       << endl;
                break;
            case T_ORDER: os   << this->target_name << ", "
                               << this->immed       << endl;
                break;
            case S_ORDER: os   << this->source_name << endl;
                break;
            case NO_REG: os    << this->jump        << endl;
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
