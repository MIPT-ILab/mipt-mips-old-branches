/**
 * func_memory.cpp
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

int FuncInstr::setType()
{

    for ( uint8 i = 0; i < MAX_NUM_OF_INSTR; ++i)
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

int FuncInstr::setName()
{
    if ( this->type == R)
    {
        for ( uint8 i = 0; i < MAX_NUM_OF_INSTR; ++i)
        {
            if ( ISA[ i].opcode == 0x0 && this->convert.asR.funct == ISA[ i].funct)
            {
                this->name = ISA[ i].name;
                return 0;
            }
        }
    } else /*if ( this->type == I || this->type == J)*/
    {
        for ( uint8 i = 0; i < MAX_NUM_OF_INSTR; ++i)
        {
            if ( this->convert.asI.opcode == ISA[ i].opcode)
            {
                this->name = ISA[ i].name;
                return 0;
            }
        }
    } 
    cerr << "\nUnknown instruction!!\n";
    exit( EXIT_FAILURE);
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
    setName();
    switch ( this->type)
    {
        case R: setSource();
                setTarget();
                setDest();
                setShamt();
            break;
        case I: setSource();
                setTarget();
                setImmed();
            break;
        case J: setJump();
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

/*bool FuncInstr::is_clear() const
{
    return ( this->name == "add") && ( this->source == "$zero") &&
           ( this->dest == "$zero");
}
*/

string FuncInstr::Dump( string indent) const
{
    ostringstream os;
    os << hex;
    if ( is_nop())
    {
        os << indent << "nop" << endl;
    } else if ( is_move())
    {
        os << indent << "move " << this->dest << ", "
           << this->source << endl;
    }/* else if ( is_clear())
    {
        os << indent << "clear " << this->target << endl;
    }*/ else
    {
        os << indent << this->name << ' ';
        if ( this->type == J)
        {
            os << this->jump << endl;
        } else if ( this->type == I)
        {
            if ( this->name == "beq" || this->name == "bne")
            {
                os << this->source << ", " << this->target 
                   << ", " << this->immed << endl;
            } else
            {
                os << this->target << ", " << this->source 
                   << ", " << this->immed << endl;
            }
        } else
        {
            if ( this->name == "jr")
            {
                os << this->source << endl;
            } else if ( this->name == "sll" || this->name == "srl")
            {
                os << this->dest << ", " << this->target << ", "
                   << this->shamt << endl;
            } else 
            {
                os << this->dest << ", " << this->source << ", "
                   << this->target << endl;
            }
        }
    }
    return os.str();
}

ostream& operator << ( ostream& out, const FuncInstr& instr)
{
    return out << instr.Dump( "");
}
