/**
 * func_instr.cpp - Implementation of FuncInstr class
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

//Generic
#include <assert.h>

//Generic C++
#include <iostream>
#include <string>
#include <sstream>

//uArchSim modules
#include <func_instr.h>
#include <types.h>

void FuncInstr::convertInstr( FormatType type, uint32 instr) // Decoding instr as type 
{	
	Convert *data = ( Convert*) &instr;
    opcode = data->asR.opcode;
    type   = getType( opcode);
    funct  = 0;
    switch( type)
	{
		case TYPE_R:
			source    = ( Register) data->asR.s;
			target    = ( Register) data->asR.t;
			dest      = ( Register) data->asR.d;
			shamt     = data->asR.shamt;
			funct     = data->asR.funct;
            return;
		case TYPE_I:
			source    = ( Register)data->asI.s;
			target    = ( Register)data->asI.t;
			iConstVal = data->asI.iConst;
            return;
		case TYPE_J:
			address   = data->asJ.addr;
			return;
	}
	assert( 0);
}		

const char * FuncInstr::myRegNames[] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1",
                                         "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", 
                                         "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
                                         "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
                                         "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", 
                                         "$fp", "$ra"};
FuncInstr::InstrInfo FuncInstr::ISA[] = { 
    { "add",   INSTR_ADD,   PRINT_DST, TYPE_R, 0, 20 }, 
    { "addu",  INSTR_ADDU,  PRINT_DST, TYPE_R, 0, 21 },
    { "sub",   INSTR_SUB,   PRINT_DST, TYPE_R, 0, 22 },
    { "subu",  INSTR_SUBU,  PRINT_DST, TYPE_R, 0, 23 },
    { "addi",  INSTR_ADDI,  PRINT_TSC, TYPE_I, 8, 0  }, 
    { "addiu", INSTR_ADDIU, PRINT_TSC, TYPE_I, 9, 0  }, 
    { "sll",   INSTR_SLL,   PRINT_DTC, TYPE_R, 0, 0  }, 
    { "srl",   INSTR_SRL,   PRINT_DTC, TYPE_R, 0, 2  }, 
    { "beq",   INSTR_BEQ,   PRINT_STC, TYPE_I, 4, 0  }, 
    { "bne",   INSTR_BNE,   PRINT_STC, TYPE_I, 5, 0  }, 
    { "j",     INSTR_J,     PRINT_C,   TYPE_J, 2, 0  }, 
    { "jr",    INSTR_JR,    PRINT_S,   TYPE_R, 0, 8  }
}; 

const uint32 NUM_OF_INSTR = sizeof( FuncInstr::ISA) / sizeof( FuncInstr::ISA[0]);

FuncInstr::FormatType FuncInstr::getType( uint32 opcode)
{
    for( int i = 0; i < NUM_OF_INSTR; i++)
    {
        if ( opcode == ISA[i].opcode)
        {
            return ISA[i].type;
        }
    } 
    assert( 0);
}


FuncInstr::FuncInstr( uint32 bytes)
{
    convertInstr( type, bytes);
    for (int i = 0; i < NUM_OF_INSTR; i++)
    {
		if ( type == ISA[i].type && opcode == ISA[i].opcode && funct == ISA[i].funct)
		{
			name  = ISA[i].name;
			instr = ISA[i].instr;
			pType = ISA[i].pType;
			break;
		}
	}
	/* Add pseudoinstructions */
    if ( instr == INSTR_ADDIU && iConstVal == 0)
    {
        instr = INSTR_MOVE;
        name  = "move";
        pType = PRINT_TS;
    }
    if ( instr == INSTR_ADDU && source == REG_ZERO && dest == REG_ZERO)
    {
        instr = INSTR_CLEAR;
        name  = "clear";
        pType = PRINT_T;
    }
    if ( instr == INSTR_SLL && dest == REG_ZERO && target == REG_ZERO && shamt == 0)
    {
        instr = INSTR_NOP;
        name  = "nop";
        pType = PRINT_NONE;
    }
}

std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;
    oss << std::hex << indent << name << " ";
    switch( pType)
    {
        case PRINT_DST:
            oss << myRegNames[ dest]   << " " << myRegNames[ source] << " " << myRegNames[ target];
            break;
        case PRINT_TSC:
            oss << myRegNames[ target] << " " << myRegNames[ source] << " " << iConstVal;
            break;
        case PRINT_STC:
            oss << myRegNames[ source] << " " << myRegNames[ target] << " " << iConstVal;
            break;
        case PRINT_DTC:
            oss << myRegNames[ dest]   << " " << myRegNames[ target] << " " << shamt;
          break;
        case PRINT_C:
            oss << address;
            break;
        case PRINT_S:
            oss << myRegNames[ source];
            break;
        case PRINT_TS:
            oss << myRegNames[ target] << " " << myRegNames[ source]; 
            break;
        case PRINT_T:
            oss << myRegNames[ target];
            break;
        case PRINT_NONE:
            break;
        default:
            assert( 0);
    }
    return oss.str();
}
    
std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump("");
    return out;
}
