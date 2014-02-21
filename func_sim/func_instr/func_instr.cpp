#include "func_instr.h"

// Enter our ISA in isaTable. Last instruction means indefenite instruction
const InstrData FuncInstr::isaTable[ ] =
{
//    opcode funct format      type
    { 0x00,  0x20, "FORMAT_R", "ADD"},
    { 0x00,  0x21, "FORMAT_R", "ADDU"},
    { 0x00,  0x22, "FORMAT_R", "SUB"},
    { 0x00,  0x23, "FORMAT_R", "SUBU"},
    { 0x08,  0xFF, "FORMAT_I", "ADDI"},
    { 0x09,  0xFF, "FORMAT_I", "ADDIU"},
    { 0x00,  0x00, "FORMAT_R", "SLL"},
    { 0x00,  0x02, "FORMAT_R", "SRL"},
    { 0x04,  0xFF, "FORMAT_I", "BEQ"},
    { 0x05,  0xFF, "FORMAT_I", "BNE"},
    { 0x02,  0xFF, "FORMAT_J", "J"},
    { 0x00,  0x08, "FORMAT_J", "JR"},
    { 0xFF,  0xFF, "NO_FORMAT", "NO_TYPE"}
};

// Enter name of registers in our ISA. Last name means that register don't need
const string FuncInstr::registers[ 33] =
{
    "$zero",
    "$at",
    "$v0", "$v1",
    "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9",
    "$k0", "$k1",
    "$gp",
    "$sp",
    "$fp",
    "$ra",
    "NOTHING"
};


//------------------------------------------------------------------

// Constructor ofclass FuncInstr

FuncInstr::FuncInstr( uint32 bytes)
{
	bytes_.raw = bytes;
    
// Find instruction in ISAtable by opcode and funct

// This flag show, does instruction find in isaTable
    uint8 flag = 0;

    for( uint8 counter = 0; counter < SIZE_OF_ARRAY; counter++)
    {
        assert((counter >= 0) && (counter < SIZE_OF_ARRAY));
// If we find instruction in isaTable
        if( ( ( bytes_.asR.opcode == 0x00) && ( bytes_.asR.funct == isaTable[ counter].funct)) || \
        ( ( bytes_.asI.opcode == isaTable[ counter].opcode) && ( bytes_.asI.opcode != 0x00)))     \
        {
// Take info about this instruction by isaTable
            InstrInfo = isaTable[ counter];

// If this instruction has R-format
            if( InstrInfo.format == "FORMAT_R")
            {
                parse_R( );
            }
// If this instruction has I-format
            if( InstrInfo.format == "FORMAT_I")
            {
                parse_I( );
            }
// If this instruction has J-format
            if( InstrInfo.format == "FORMAT_J")
            {
                parse_J( );
            }
// After parse we should call check on pseudo-instruction
            pseudo( );
// Set flag "finded" instruction
            flag++;
        }
    }
    
// If instruction don't found
    if( flag == 0)
    {
        InstrInfo = isaTable[ SIZE_OF_ARRAY];
        rs = registers[ 32];
        rt = registers[ 32];
        rd = registers[ 32];
        imm = NO_IMM;
        shamt = NO_SHAMT;
    }
}

// -----------------------------------------------------------------

// Method, which parse R-type instruction

void FuncInstr::parse_R( )
{
// Use parse instruction by R-format. Enter standart options for R-format instructions
    rs = registers[ bytes_.asR.s];
    rt = registers[ bytes_.asR.t];
    rd = registers[ bytes_.asR.d];
    imm = NO_IMM;
    shamt = ( uint32)bytes_.asR.shamt;
// Further we check instruction on unusual cases
    if(( InstrInfo.type == "SLL") || ( InstrInfo.type == "SRL"))
    {
        rs = registers[ 32];
    } 
    else if( InstrInfo.type == "JR")
    {
        rt = registers[ 32];
        rd = registers[ 32];
        shamt = NO_SHAMT;
    }
    else
    {
        shamt = NO_SHAMT;
    }   
}

// -----------------------------------------------------------------

// Method, which parse I-type instruction

void FuncInstr::parse_I( )
{
// Use parse instruction by I-format. Enter standart options for I-type instructions
    rs = registers[ bytes_.asI.s];
    rt = registers[ bytes_.asI.t];
    rd = registers[ 32];
    imm = ( uint32)bytes_.asI.imm;
    shamt = NO_SHAMT;
}

// -----------------------------------------------------------------

// Method, which parse J-type instruction

void FuncInstr::parse_J( )
{
// Use parse instructions by J-format. Enter standart options for J-type instructions
    rs = registers[ 32];
    rt = registers[ 32];
    rd = registers[ 32];
    imm = ( uint32)bytes_.asJ.imm;
    shamt = NO_SHAMT;
}

// -----------------------------------------------------------------

// Method, which find pseudo-instructions

void FuncInstr::pseudo( )
{
// We find cases, when we can swap standart instruction on pseudo-instruction. If we find it, we swap 
// info about instruction on info about pseudo-instruction
    if( ( InstrInfo.type == "ADDIU") && ( rd == registers[ 0]))
    {
        InstrInfo.type = "MOVE";
        imm = NO_IMM;
    }
    if( ( InstrInfo.type == "ADDU") && ( rs == registers[ 0]) && ( rt == registers[ 0]))
    {
        InstrInfo.type = "CLEAR";
        rs = registers[ 32];
        rt = registers[ 32];
    }
    if( ( InstrInfo.type == "SLL") && ( rt == registers[ 0]) && ( rd == registers[ 0]) && ( shamt == 0))
    {
        InstrInfo.type = "NOP";
        rt = registers[ 32];
        rd = registers[ 32];
        shamt = NO_SHAMT;
    }
}

// -----------------------------------------------------------------

// Method, which print instruction

string FuncInstr::Dump( string indent) const
{
// If instruction have unknown type
    if( InstrInfo.type == "NO_TYPE")
    {
        cout << "---instruction is not defined---\n";
        return "";
    }
// Print type of instruction
	if( InstrInfo.type != "NO_TYPE")
	{
		cout << indent << InstrInfo.type << " ";
	}
// Printf (if defined) rs
	if( rd != registers[ 32])
	{
		cout << rd << " ";
	} 
// Printf (if defined) rt
	if( rt != registers[ 32])
	{
		cout << rt << " ";
	}
// Printf (if defined) rd
	if( rs != registers[ 32])
	{
		cout << rs << " ";
	}
// Printf (if defined) shamt
	if( shamt != NO_SHAMT)
	{
		cout << shamt << " ";
	}
// Printf (if defined) immidiate
	if( imm != NO_IMM)
	{
// If size of immidiate == 2 bytes
		if( InstrInfo.format == "FORMAT_I")
		{
			if( InstrInfo.type == "ADDIU")
            {
                cout << ( uint16)imm;
            }
            else
            {
                cout << ( int16)imm;
            }
		}
// If size of immidiate > 2 bytes
		else
		{
			cout << imm;
		}
	}
	
	cout << "\n";
	return "";
}

// ----------------------------------------------------------------
