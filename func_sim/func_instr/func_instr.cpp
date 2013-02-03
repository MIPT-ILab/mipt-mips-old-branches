/**
 * func_instr.cpp - Implementation of FuncInstr class
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

//Generic C
#include <assert.h>
#include <string.h>

//Generic C++
#include <iostream>
#include <string>
#include <sstream>

//uArchSim modules
#include <func_instr.h>
#include <types.h>

FuncInstr::InstrInfo::InstrInfo( std::string instrName, Instruction instrInstr, PrintType printType, 
                      FormatType instrType, uint32 instrOpcode, uint32 instrFunct)
{
	name = instrName;
	instr = instrInstr;
	type = instrType;
	pType = printType;
	opcode = instrOpcode;
	funct = instrFunct;
}

void FuncInstr::convertInstr( FormatType type, uint32 instr) // Decoding instr as type 
{	
	Convert data;
	memcpy( &data, &instr, sizeof(uint32)); 
	switch( type)
	{
		case RType:
			source = ( Register)data.asR.s;
			target = ( Register)data.asR.t;
			dest = ( Register)data.asR.d;
			shamt = data.asR.shamt;
			return;
		case IType:
			source = ( Register)data.asI.s;
			target = ( Register)data.asI.t;
			iConstVal = data.asI.iConst;
			return;
		case JType:
			address = data.asJ.addr;
			return;
	}
	assert( 0);
}		

uint32 FuncInstr::getOpcode( uint32 bytes)
{
    return ( bytes & 0xFC000000) >> 26;
}

uint32 FuncInstr::getFunct( uint32 bytes)
{
    return bytes & 0x3F;
}

FuncInstr::FormatType FuncInstr::getType( uint32 opcode)
{
    switch( opcode)
    {
        case 0: 
            return RType;
        case 2:
            return JType;
        case 4:
        case 5:
        case 8:
        case 9:
            return IType;
    }
    assert( 0);
}

const char * FuncInstr::myRegNames[ 32] = { "$zero", "$at", "$v0", "$v1", "$a0", "$a1",
                                            "$a2", "$a3", "$t0", "$t1", "$t2", "$t3", 
                                            "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", 
                                            "$s2", "$s3", "$s4", "$s5", "$s6", "$s7", 
                                            "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", 
                                            "$fp", "$ra"};
FuncInstr::InstrInfo FuncInstr::ISA[NUM_OF_INSTR] = { 
    InstrInfo( "add", add, dst, RType, 0, 20), 
    InstrInfo( "addu", addu, dst, RType, 0, 21),
    InstrInfo( "sub", sub, dst, RType, 0, 22),
    InstrInfo( "subu", subu, dst, RType, 0, 23),
    InstrInfo( "addi", addi, tsc, IType, 8, 0), 
    InstrInfo( "addiu", addiu, tsc, IType, 9, 0), 
    InstrInfo( "sll", sll, dtc, RType, 0, 0), 
    InstrInfo( "srl", srl, dtc, RType, 0, 2), 
    InstrInfo( "beq", beq, stc,  IType, 4, 0), 
    InstrInfo( "bne", bne, stc, IType, 5, 0), 
    InstrInfo( "j", j, c, JType, 2, 0), 
    InstrInfo( "jr", jr, s, RType, 0, 8)
}; 

FuncInstr::FuncInstr( uint32 bytes)
{
    uint32 opcode = getOpcode( bytes);
    uint32 funct = getFunct( bytes);    
    type = getType( opcode);
    convertInstr( type, bytes);
    for (int i = 0; i < NUM_OF_INSTR; i++)
    {
		if (type == ISA[i].type && opcode == ISA[i].opcode && funct == ISA[i].funct)
		{
			name = ISA[i].name;
			instr = ISA[i].instr;
			pType = ISA[i].pType;
			break;
		}
	}
	/* Add pseudoinstructions */
    if ( instr == addiu && iConstVal == 0)
    {
        instr = move;
        name = "move";
        pType = ts;
    }
    if ( instr == addu && source == zero && dest == zero)
    {
        instr = clear;
        name = "clear";
        pType = t;
    }
    
    if ( instr == sll && dest == zero && target == zero && shamt == 0)
    {
        instr = nop;
        name = "nop";
        pType = none;
    }
}

std::string FuncInstr::Dump( std::string indent) const
{
    std::ostringstream oss;
    oss << std::hex << indent << name << " ";
    switch( pType)
    {
        case dst:
            oss << myRegNames[ dest] << " " << myRegNames[ source] << " " 
                      << myRegNames[ target];
            break;
        case tsc:
            oss << myRegNames[ target] << " " << myRegNames[ source] << " " 
                      << iConstVal;
            break;
        case stc:
            oss << myRegNames[ source] << " " << myRegNames[ target] << " " 
                      << iConstVal;
            break;
        case dtc:
            oss << myRegNames[ dest] << " " << myRegNames[ target] << " " << shamt;
          break;
        case c:
            oss << address;
            break;
        case s:
            oss << myRegNames[ source];
            break;
        case ts:
            oss << myRegNames[ target] << " " << myRegNames[ source]; 
            break;
        case t:
            oss << myRegNames[ target];
            break;
        case none:
            break;
        default:
            assert( 0);
    }
    oss << std::endl;
    return oss.str();
}
    
std::ostream& operator<<( std::ostream& out, const FuncInstr& instr)
{
    out << instr.Dump("");
    return out;
}
