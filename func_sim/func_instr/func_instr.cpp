#include "func_instr.h"

//------------------------------------------------------------------

// Constructor ofclass FuncInstr

FuncInstr::FuncInstr( uint32 bytes)
{
	bytes_.raw = bytes;

// When instruction have R-format
	if( bytes_.asR.opcode == 0x00)
	{
// Enter same field for R-format instructions
		InstrInfo.opcode = ( uint8)bytes_.asR.opcode;
		InstrInfo.funct = ( uint8)bytes_.asR.funct;
		InstrInfo.shamt = ( uint32)bytes_.asR.shamt;
		
		InstrInfo.rs = registers[ bytes_.asR.s];
		InstrInfo.rt = registers[ bytes_.asR.t];
		InstrInfo.rd = registers[ bytes_.asR.d];

		InstrInfo.imm = NO_IMM;

		InstrInfo.format = "FORMAT_R";
// SLL
		if( InstrInfo.funct == 0x00)
		{
			InstrInfo.type = "SLL";
			InstrInfo.rs = registers[ 32];
		}
// SRL 
		else if( InstrInfo.funct == 0x02)
		{
			InstrInfo.type = "SRL";
			InstrInfo.rs = registers[ 32]; 
		}
// JR
		else if( InstrInfo.funct == 0x08)
		{
			InstrInfo.type = "JR";
			InstrInfo.rt = registers[ 32];
			InstrInfo.rd = registers[ 32];
			InstrInfo.shamt = NO_SHAMT;
		}
// ADD
		else if( InstrInfo.funct == 0x20)
		{
			InstrInfo.type = "ADD";
			InstrInfo.shamt = NO_SHAMT;
		}
// ADDU
		else if( InstrInfo.funct == 0x21)
		{
			InstrInfo.type = "ADDU";
			InstrInfo.shamt = NO_SHAMT;
		}
// SUB
		else if( InstrInfo.funct == 0x22)
		{
			InstrInfo.type = "SUB";
			InstrInfo.shamt = NO_SHAMT;
		}
// SUBU
		else if( InstrInfo.funct == 0x23)
		{
			InstrInfo.type = "SUBU";
			InstrInfo.shamt = NO_SHAMT;
		}
// SYSCALL
		else if( InstrInfo.funct == 0x0C)
		{
			InstrInfo.type = "SYSCALL";
			InstrInfo.shamt = NO_SHAMT;
			InstrInfo.rs = registers[ 32];
			InstrInfo.rt = registers[ 32];
			InstrInfo.rd = registers[32];
		}
// Else instruction have unknown type
		else
		{
            InstrInfo.type = "NO_TYPE";
            InstrInfo.opcode = INSTR_NOT_DEF;
            InstrInfo.rs = registers[ 32];
            InstrInfo.rt = registers[ 32];
            InstrInfo.rd = registers[ 32];
            InstrInfo.shamt = NO_SHAMT;
            InstrInfo.imm = NO_IMM;
		}
	} 
 
// When instruction have I-format
	else if( ( bytes_.asI.opcode == 0x04) || ( bytes_.asI.opcode == 0x05) ||  \
	( bytes_.asI.opcode == 0x08) || ( bytes_.asI.opcode == 0x09) || 	      \
	( bytes_.asI.opcode == 0x23) || ( bytes_.asI.opcode == 0x2B) ||           \
	( bytes_.asI.opcode == 0x0F))
	{
// Enter same field for I-type instructions
		InstrInfo.opcode = ( uint8)bytes_.asI.opcode;
		InstrInfo.funct = NO_FUNCT;
		InstrInfo.shamt = NO_SHAMT;

		InstrInfo.rs = registers[ bytes_.asI.s];
		InstrInfo.rt = registers[bytes_.asI.t];
		InstrInfo.rd = registers[ 32];
		
		InstrInfo.imm = ( uint32)bytes_.asI.imm;
		
		InstrInfo.format = "FORMAT_I";
// BEQ
		if( InstrInfo.opcode == 0x04)
		{
			InstrInfo.type = "BEQ";
		}
// BNE
		else if( InstrInfo.opcode == 0x05)
		{
			InstrInfo.type = "BNE";
		}
// ADDI
		else if( InstrInfo.opcode == 0x08)
		{
			InstrInfo.type = "ADDI";
		}
// ADDIU
		else if( InstrInfo.opcode == 0x09)
		{
			InstrInfo.type = "ADDIU";
		}
// LW
		else if( InstrInfo.opcode == 0x23)
		{
			InstrInfo.type = "LW";
		}
// SW
		else if( InstrInfo.opcode == 0x2B)
		{
			InstrInfo.type = "SW";
		}
// LUI
		else if( InstrInfo.opcode == 0x0F)
		{
			InstrInfo.type = "LUI";
			InstrInfo.rs = registers[ 32];
		}
	}

// When instruction have J-format
	else if( ( bytes_.asJ.opcode == 0x02) || ( bytes_.asJ.opcode == 0x03))
	{
// Enter same field for J-type instructions
		InstrInfo.opcode = ( uint8)bytes_.asJ.opcode;
		InstrInfo.funct = NO_FUNCT;
		InstrInfo.shamt = NO_SHAMT;
		
		InstrInfo.rs = registers[ 32];
		InstrInfo.rt = registers[ 32];
		InstrInfo.rd = registers[ 32];
		
		InstrInfo.imm = ( uint32)bytes_.asJ.imm;
		
		InstrInfo.format = "FORMAT_J";
// J
		if( InstrInfo.opcode == 0x02)
		{
			InstrInfo.type = "J";
		}
// JAL
		else if( InstrInfo.opcode == 0x03)
		{
			InstrInfo.type = "JAL";
		}
	}

// Else instruction have unknown format
	else
	{
		InstrInfo.format = "NO_FORMAT";
		InstrInfo.type = "NO_TYPE";
		InstrInfo.opcode = INSTR_NOT_DEF;
		InstrInfo.rs = registers[ 32];
		InstrInfo.rt = registers[ 32];
		InstrInfo.rd = registers[ 32];
		InstrInfo.shamt = NO_SHAMT;
		InstrInfo.imm = NO_IMM;		
	}
// Added this instruction to ISA array
        
    //isaTable[ISA_number] = InstrInfo;
    //ISA_number++;
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
	if( InstrInfo.rs != registers[ 32])
	{
		cout << InstrInfo.rs << " ";
	} 
// Printf (if defined) rt
	if( InstrInfo.rt != registers[ 32])
	{
		cout << InstrInfo.rt << " ";
	}
// Printf (if defined) rd
	if( InstrInfo.rd != registers[ 32])
	{
		cout << InstrInfo.rd << " ";
	}
// Printf (if defined) shamt
	if( InstrInfo.shamt != NO_SHAMT)
	{
		cout << InstrInfo.shamt << " ";
	}
// Printf (if defined) immidiate
	if( InstrInfo.imm != NO_IMM)
	{
// If size of immidiate == 2 bytes
		if( InstrInfo.format == "FORMAT_I")
		{
			cout << ( int16)InstrInfo.imm;
		}
// If size of immidiate > 2 bytes
		else
		{
			cout << InstrInfo.imm;
		}
	}
	
	cout << "\n";
	return "";
}

// ----------------------------------------------------------------
