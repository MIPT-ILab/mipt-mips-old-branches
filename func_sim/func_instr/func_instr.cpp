#include "func_instr.h"

//------------------------------------------------------------------

// Конструктор класса FuncInstr

FuncInstr::FuncInstr(uint32 bytes)
{
	bytes_.raw = bytes;

// Случай, когда инструкция имеет R-формат
	if(bytes_.asR.opcode == 0x00)
	{
		InstrInfo.opcode = (uint8)bytes_.asR.opcode;
		InstrInfo.funct = (uint8)bytes_.asR.funct;
		InstrInfo.shamt = (uint32)bytes_.asR.shamt;
		
		InstrInfo.rs = registers[bytes_.asR.s];
		InstrInfo.rt = registers[bytes_.asR.t];
		InstrInfo.rd = registers[bytes_.asR.d];

		InstrInfo.imm = NO_IMM;

		InstrInfo.format = "FORMAT_I";
		
		if(InstrInfo.funct == 0x00)
		{
			InstrInfo.type = "SLL";
			InstrInfo.rs = registers[32];
		} 
		else if(InstrInfo.funct == 0x02)
		{
			InstrInfo.type = "SRL";
			InstrInfo.rs = registers[32]; 
		}
		else if(InstrInfo.funct == 0x08)
		{
			InstrInfo.type = "JR";
			InstrInfo.rs = registers[32];
			InstrInfo.rt = registers[32];
			InstrInfo.shamt = NO_SHAMT;
		}
		else if(InstrInfo.funct == 0x20)
		{
			InstrInfo.type = "ADD";
			InstrInfo.shamt = NO_SHAMT;
		}
		else if(InstrInfo.funct == 0x21)
		{
			InstrInfo.type = "ADDU";
			InstrInfo.shamt = NO_SHAMT;
		}
		else if(InstrInfo.funct == 0x22)
		{
			InstrInfo.type = "SUB";
			InstrInfo.shamt = NO_SHAMT;
		}
		else if(InstrInfo.funct == 0x23)
		{
			InstrInfo.type = "SUBU";
			InstrInfo.shamt = NO_SHAMT;
		}
		// Иначе, инструкция имеет неизвестный формат
		else
		{
			cout << "instruction not defined\n";
			InstrInfo.opcode = INSTR_NOT_DEF;
		}
	} 
 
// Случай, когда инструкция имеет I-формат
	else if((bytes_.asI.opcode == 0x04) || (bytes_.asI.opcode == 0x05) || (bytes_.asI.opcode == 0x08) || (bytes_.asI.opcode == 0x09))
	{
		InstrInfo.opcode = (uint8)bytes_.asI.opcode;
		InstrInfo.funct = NO_FUNCT;
		InstrInfo.shamt = NO_SHAMT;

		InstrInfo.rs = registers[bytes_.asI.s];
		InstrInfo.rt = registers[bytes_.asI.t];
		InstrInfo.rd = registers[32];
		
		InstrInfo.imm = (uint32)bytes_.asI.imm;
		
		InstrInfo.format = "FORMAT_I";

		if(InstrInfo.opcode == 0x04)
		{
			InstrInfo.type = "BEQ";
		}
		else if(InstrInfo.opcode == 0x05)
		{
			InstrInfo.type = "BNE";
		}
		else if(InstrInfo.opcode == 0x08)
		{
			InstrInfo.type = "ADDI";
		}
		else if(InstrInfo.opcode == 0x09)
		{
			InstrInfo.type = "ADDIU";
		}
	}

// Случай, когда инструкция имеет J-формат
	else if(bytes_.asJ.opcode == 0x02)
	{
		InstrInfo.opcode = (uint8)bytes_.asJ.opcode;
		InstrInfo.funct = NO_FUNCT;
		InstrInfo.shamt = NO_SHAMT;
		
		InstrInfo.rs = registers[32];
		InstrInfo.rt = registers[32];
		InstrInfo.rd = registers[32];
		
		InstrInfo.imm = (uint32)bytes_.asJ.imm;
		
		InstrInfo.format = "FORMAT_J";
		InstrInfo.type = "J";
	}

// Иначе, инструкция имеет неизвестный формат
	else
	{
		cout << "instruction not defined\n";
		InstrInfo.opcode = INSTR_NOT_DEF;
	}
}

// -----------------------------------------------------------------

// Функция, выводящая инструкцию на экран

string FuncInstr::Dump(string indent)
{
	cout << indent << InstrInfo.type << " ";

	if(InstrInfo.rs != registers[32])
	{
		cout << InstrInfo.rs << " ";
	} 
	if(InstrInfo.rt != registers[32])
	{
		cout << InstrInfo.rt << " ";
	}
	if(InstrInfo.rd != registers[32])
	{
		cout << InstrInfo.rd << " ";
	}
	if(InstrInfo.shamt != NO_SHAMT)
	{
		cout << InstrInfo.shamt << " ";
	}
	if(InstrInfo.imm != NO_IMM)
	{
		cout << InstrInfo.imm;
	}
	
	cout << "\n";
	return " ";
}

// -----------------------------------------------------------------
