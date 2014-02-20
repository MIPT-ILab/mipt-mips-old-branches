#include "func_instr.h"

const InstrData FuncInstr::isaTable[ ] =
{
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


//------------------------------------------------------------------

// Constructor ofclass FuncInstr

FuncInstr::FuncInstr( uint32 bytes)
{
	bytes_.raw = bytes;
    
// Find instruction in ISAtable by opcode and funct
    uint8 flag = 0;

    for(uint8 counter = 0; counter < SIZE_OF_ARRAY; counter++)
    {
        if
        (((bytes_.asR.opcode == 0x00) && (bytes_.asR.funct == isaTable[counter].funct)) || \
        ((bytes_.asI.opcode == isaTable[counter].opcode) && (bytes_.asI.opcode != 0x00)))  \
        {
            InstrInfo = isaTable[counter];

            if(InstrInfo.format == "FORMAT_R")
            {
                parse_R();
            }
            if(InstrInfo.format == "FORMAT_I")
            {
                parse_I();
            }
            if(InstrInfo.format == "FORMAT_J")
            {
                parse_J();
            }

            flag++;
        }
    }
    
// If instruction don't found
    if(flag == 0)
    {
        InstrInfo = isaTable[SIZE_OF_ARRAY];
        rs = registers[32];
        rt = registers[32];
        rd = registers[32];
        imm = NO_IMM;
        shamt = NO_SHAMT;
    }
}

// -----------------------------------------------------------------

// Method, which parse R-type instruction

void FuncInstr::parse_R()
{
    rs = registers[bytes_.asR.s];
    rt = registers[bytes_.asR.t];
    rd = registers[bytes_.asR.d];
    imm = NO_IMM;
    shamt = (uint32)bytes_.asR.shamt;
    
    if((InstrInfo.type == "SLL") || (InstrInfo.type == "SRL"))
    {
        rs = registers[32];
    } 
    else if(InstrInfo.type == "JR")
    {
        rt = registers[32];
        rd = registers[32];
        shamt = NO_SHAMT;
    }
    else
    {
        shamt = NO_SHAMT;
    }   
}

// -----------------------------------------------------------------

// Method, which parse I-type instruction

void FuncInstr::parse_I()
{
    rs = registers[bytes_.asI.s];
    rt = registers[bytes_.asI.t];
    rd = registers[32];
    imm = (uint32)bytes_.asI.imm;
    shamt = NO_SHAMT;
}

// -----------------------------------------------------------------

// Method, which parse J-type instruction

void FuncInstr::parse_J()
{
    rs = registers[32];
    rt = registers[32];
    rd = registers[32];
    imm = (uint32)bytes_.asJ.imm;
    shamt = NO_SHAMT;
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
	if( rs != registers[ 32])
	{
		cout << rs << " ";
	} 
// Printf (if defined) rt
	if( rt != registers[ 32])
	{
		cout << rt << " ";
	}
// Printf (if defined) rd
	if( rd != registers[ 32])
	{
		cout << rd << " ";
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
			cout << ( int16)imm;
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
