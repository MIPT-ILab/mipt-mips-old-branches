/*
 * func_instr.cpp - instruction parser for mips
 * @author Pavel Kryukov pavel.kryukov@phystech.edu
 * Copyright 2015 MIPT-MIPS
 */

//uArchSim modules
#include <func_instr.h>

//Generic C++
#include <iostream>
#include <sstream>
#include <iomanip>

//Generic C
#include <stdio.h>
#include <stdlib.h>

const FuncInstr::ISAEntry FuncInstr::isaTable[] =
{
    // ISA Table of MIPS
	// name  opcode  func   format    operation     pointer 
	//												to Function
	{ "add",    0x0, 0x20,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::add   },
	{ "addu",   0x0, 0x21,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::addu  },
	{ "sub",    0x0, 0x22,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::sub   },
	{ "subu",   0x0, 0x23,  FORMAT_R, OUT_R_ARITHM, &FuncInstr::subu  },
	{ "addi",   0x8, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addi  },
	{ "addiu",  0x9, 0x0,   FORMAT_I, OUT_I_ARITHM, &FuncInstr::addiu },
	{ "sll",    0x0, 0x0,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::sll   },
	{ "srl",    0x0, 0x2,   FORMAT_R, OUT_R_SHAMT,  &FuncInstr::srl   },
	{ "beq",    0x4, 0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::beq   },
	{ "bne",    0x5, 0x0,   FORMAT_I, OUT_I_BRANCH, &FuncInstr::bne   },
	{ "j",      0x2, 0x0,   FORMAT_J, OUT_J_JUMP,   &FuncInstr::j     },
	{ "jr",     0x0, 0x8,   FORMAT_R, OUT_R_JUMP,   &FuncInstr::jr    },
	{ "lb",     0x20,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lb    },
	{ "lw",     0x23,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lw    },
	{ "lh",     0x21,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lh    },
	{ "lhu",    0x25,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lhu   },
	{ "lbu",    0x24,0x0,   FORMAT_I, OUT_I_LOAD,   &FuncInstr::lbu   },
	{ "sb",     0x28,0x0,   FORMAT_I, OUT_I_STORE,  &FuncInstr::sb    },
	{ "sw",     0x2b,0x0,   FORMAT_I, OUT_I_STORE,  &FuncInstr::sw    },
	{ "sh",     0x29,0x0,   FORMAT_I, OUT_I_STORE,  &FuncInstr::sh    },
	{ "and",    0x0, 0x24,  FORMAT_R, OUT_R_LOGIC,  &FuncInstr::andOp },
	{ "or",     0x0, 0x25,  FORMAT_R, OUT_R_LOGIC,  &FuncInstr::orOp  },
	{ "xor",    0x0, 0x26,  FORMAT_R, OUT_R_LOGIC,  &FuncInstr::xorOp },
	{ "andi",   0xc, 0x0,   FORMAT_I, OUT_I_LOGIC,  &FuncInstr::andi  },
	{ "ori",    0xd, 0x0,   FORMAT_I, OUT_I_LOGIC,  &FuncInstr::ori   },
	{ "xori",   0xe, 0x0,   FORMAT_I, OUT_I_LOGIC,  &FuncInstr::xori  },
	{ "lui",    0xf, 0x0,   FORMAT_I, OUT_I_SHAMT,  &FuncInstr::lui   }
};

const uint32 FuncInstr::isaTableSize = sizeof(isaTable) / sizeof(isaTable[0]);

/* Register table of MIPS */
const char *FuncInstr::regTable[] = 
{
    "zero",
    "at",
    "v0", "v1",
    "a0", "a1", "a2", "a3",
    "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
    "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
    "t8", "t9", 
    "k0", "k1",
    "gp",
    "sp",
    "fp",
    "ra"
};


FuncInstr::FuncInstr( uint32 bytes, uint32 PC) : instr( bytes), PC( PC)
{
    initFormat(); 
    switch ( format)
    {
        case FORMAT_R:
            initR();
            break;
        case FORMAT_I:
            initI();
            break;
        case FORMAT_J:
            initJ();
            break;
        case FORMAT_UNKNOWN:
            initUnknown();
            break;
    }
	
}

void FuncInstr::initFormat()
{
	for ( size_t i = 0; i < isaTableSize; i++) 
	{
		if ( instr.asR.opcode == isaTable[ i].opcode)
		{
			format = isaTable[ i].format;
			operation = isaTable[ i].operation;
			isaNum = i;
			return;
		}
	}
	format = FORMAT_UNKNOWN;
}

void FuncInstr::initR()
{
    // find instr by functor
    for ( isaNum = 0; isaNum < isaTableSize; ++isaNum) 
	{
		if (( instr.asR.opcode == isaTable[ isaNum].opcode) &&
			( instr.asR.funct == isaTable[ isaNum].funct))
		{
			operation = isaTable[ isaNum].operation;
			pointer = isaTable[ isaNum].toFunction;
			break;
		}
	}
	if ( isaNum == isaTableSize)     // if didn't found funct
	{
		initUnknown();
		return;
	}
    
	std::ostringstream oss;
	std::ostringstream oss1;
	std::ostringstream oss2;
	std::ostringstream oss3;
	
	oss << isaTable[ isaNum].name << " $";
    
	disasmName = oss.str();
	
	switch ( operation)
	{
		case OUT_R_ARITHM:
			oss1 << regTable[ instr.asR.rd] ; 
			oss2 << ", $" << regTable[ instr.asR.rs] ; 
			oss3 << ", $" << regTable[ instr.asR.rt];
			disasmrd = oss1.str();
			disasmrs = oss2.str();
			disasmrt = oss3.str();
			break;
		case OUT_R_SHAMT:
			oss1 << regTable[ instr.asR.rd] ;
			oss2 << ", $" << regTable[ instr.asR.rt];
			oss3 <<  ", " << dec << instr.asR.shamt;
			disasmrd = oss1.str();
			disasmrt = oss2.str();
			disasmShamt = oss3.str();
			break;
		case OUT_R_JUMP:  
			oss1 << regTable[ instr.asR.rs];
			disasmrs = oss1.str();
			break;
		case OUT_R_LOGIC:
			oss1 << regTable[ instr.asR.rd] ;
			oss2 << ", $" << regTable[ instr.asR.rt] ;
			oss3 << ", $" << regTable[ instr.asR.rs];
			disasmrd = oss1.str();
			disasmrt = oss2.str();
			disasmrs = oss3.str();
			break;
	}
    
}


void FuncInstr::initI()
{
	std::ostringstream oss;
	std::ostringstream oss1;
	std::ostringstream oss2;
	std::ostringstream oss3;
	
	oss << isaTable[ isaNum].name << " $";
	disasmName = oss.str();
	
	pointer = isaTable[ isaNum].toFunction;
	
	switch ( operation)
    {
		case OUT_I_ARITHM:
			oss1 << regTable[ instr.asI.rt];
			oss2 << ", $" << regTable[ instr.asI.rs];
			oss3 << ", " << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
			disasmrt = oss1.str();
			disasmrs = oss2.str();
			disasmImm = oss3.str();
			break;
		case OUT_I_BRANCH:
			oss1 << regTable[ instr.asI.rs];
			oss2 << ", $" << regTable[ instr.asI.rt];
			oss3 << ", " << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
			disasmrs = oss1.str();
			disasmrt = oss2.str();
			disasmImm = oss3.str();
			break;
		case OUT_I_LOAD:
			oss1 << regTable[ instr.asI.rt] ;
			oss2 << ", " << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec; 
			oss3 << "($" <<regTable[ instr.asI.rs];
			disasmrt = oss1.str();
			disasmImm = oss2.str();
			disasmrs = oss3.str();
			break;
		case OUT_I_STORE:
			oss1 << regTable[ instr.asI.rt];
			oss2 << ", " << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec; 
			oss3 << "($" << regTable[ instr.asI.rs];
			disasmrt = oss1.str();
			disasmImm = oss2.str();
			disasmrs = oss3.str();
			break;
		case OUT_I_LOGIC:
			oss1 << regTable[ instr.asI.rs];
			oss2 << ", $" <<  regTable[ instr.asI.rt];
			oss3 << ", " << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
			disasmrs = oss1.str();
			disasmrt = oss2.str();
			disasmImm = oss3.str();
			break;
		case OUT_I_SHAMT:
			oss1 << regTable[ instr.asI.rt];
			oss2 << ", " << std::hex << "0x" << static_cast< signed int>( instr.asI.imm) << std::dec;
			disasmrt = oss1.str();
			disasmImm = oss2.str();
			break;
	}
}

void FuncInstr::initJ()
{
	std::ostringstream oss;
	
	pointer = isaTable[ isaNum].toFunction;
	
	oss << isaTable[ isaNum].name << " "
		<< std::hex << "0x" <<instr.asJ.imm;
    
	disasm = oss.str();
}

void FuncInstr::initUnknown()
{
    std::ostringstream oss;
    oss << std::hex << std::setfill( '0')
        << "0x" << std::setw( 8) << instr.raw << '\t' << "Unknown" << std::endl;
    disasm = oss.str();
    std::cerr << "ERROR.Incorrect instruction: " << disasm << std::endl;
    exit( EXIT_FAILURE);
}

std::ostream& operator<< ( std::ostream& out, FuncInstr& instr)
{
    return out << instr.Dump( "");
}

int FuncInstr::get_src1_num_index( ) const
{
	switch( operation)
	{
		case OUT_R_ARITHM:
			return  instr.asR.rs;
			break;
		case OUT_R_SHAMT:
			return instr.asR.rt;
			break;
		case OUT_R_JUMP:
			return instr.asR.rs;
			break;
		case OUT_I_ARITHM:
			return instr.asI.rs;
			break;
		case OUT_I_BRANCH:
			return instr.asI.rs;
			break;
		case OUT_I_LOAD:
			return instr.asI.rs;
			break;
		case OUT_I_STORE:
			return instr.asI.rs;
			break;
		case OUT_J_JUMP:
			return 0;
			break;
		case OUT_R_LOGIC:
			return instr.asR.rt;
			break;
		case OUT_I_LOGIC:
			return instr.asI.rs;
			break;
		case OUT_I_SHAMT:
			return 0;
			break;
	}
	
}

int FuncInstr::get_src2_num_index( ) const
{
	switch( operation)
	{
		case OUT_R_ARITHM:
			return instr.asR.rt;
			break;
		case OUT_R_SHAMT:
			return instr.asR.shamt;
			break;
		case OUT_R_JUMP:
			return 0;
			break;
		case OUT_I_ARITHM:
			return instr.asI.imm;
			break;
		case OUT_I_BRANCH:
			return instr.asI.rt;
			break;
		case OUT_I_LOAD:
			return 0;
			break;
		case OUT_I_STORE:
			return 0;
			break;
		case OUT_J_JUMP:
			return 0;
			break;
		case OUT_R_LOGIC:
			return instr.asR.rs;
			break;
		case OUT_I_LOGIC:
			return 0;
			break;
		case OUT_I_SHAMT:
			return 0;
			break;
	}
}


int FuncInstr::get_dst_num_index( ) const
{
	switch( operation)
	{
		case OUT_R_ARITHM:
			return instr.asR.rd;
			break;
		case OUT_R_SHAMT:
			return instr.asR.rd;
			break;
		case OUT_R_JUMP:
			return 0;
			break;
		case OUT_I_ARITHM:
			return instr.asI.rt;
			break;
		case OUT_I_BRANCH:
			return 0;
			break;
		case OUT_I_LOAD:
			return instr.asI.rt;
			break;
		case OUT_I_STORE:
			return instr.asI.rt;
			break;
		case OUT_J_JUMP:
			return 0;
			break;
		case OUT_R_LOGIC:
			return instr.asR.rd;
			break;
		case OUT_I_LOGIC:
			return instr.asI.rt;
			break;
		case OUT_I_SHAMT:
			return instr.asI.rt;
			break;
	}
	
}

/* FUNCTIONS FOR EXECUTION */

void FuncInstr::add()
{
	v_dst = v_src1 + v_src2;
	new_PC = PC + 4;
	write_back = 1;
} 

void FuncInstr::sub()
{
	v_dst = v_src1 - v_src2;
	new_PC = PC + 4;
	write_back = 1;
} 

void FuncInstr::addu()
{
	v_dst = v_src1 + v_src2;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::subu()
{
	v_dst = v_src1 - v_src2;
	new_PC = PC + 4;
	write_back = 1;
} 

void FuncInstr::addiu()
{
	v_dst = v_src1 + C ;
	new_PC = PC + 4;
	write_back = 1;
} 

void FuncInstr::addi()
{
	v_dst = v_src1 + C;
	new_PC = PC + 4;
	write_back = 1;
} 

void FuncInstr::sll()
{
	v_dst = v_src1 << Sh;
	new_PC = PC + 4;
	write_back = 1;
} 

void FuncInstr::srl()
{
	v_dst = v_src1 >> Sh;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::beq()
{
	
	if ( v_src1 == v_src2)
	{
		new_PC = PC + 4 + ( C << 2);
		
	}
	else
	{
		new_PC = PC + 4;
	}
	write_back = 0;
}

void FuncInstr::bne()
{
	
	if ( v_src1 != v_src2)
	{
		new_PC = PC + 4 + ( C << 2);
	}
	else
	{
		new_PC = PC + 4;
	}
	write_back = 0;
}

void FuncInstr::j()
{
	new_PC = (PC & 0xf0000000) | ( CJump << 2);
	write_back = 0;
}

void FuncInstr::jr()
{
	new_PC = v_src1;
	write_back = 0;
}

void FuncInstr::andOp()
{
	v_dst = v_src1 & v_src2;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::orOp()
{
	v_dst = v_src1 | v_src2;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::xorOp()
{
	v_dst = v_src1 ^ v_src2;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::andi()
{
	v_dst = v_src1 & C;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::ori()
{
	v_dst = v_src1 | C;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::xori()
{
	v_dst = v_src1 ^ C;
	new_PC = PC + 4;
	write_back = 1;
}

void FuncInstr::lui()
{
	v_dst = C << 16;
	new_PC = PC + 4;
	write_back = 1;
}
 
void FuncInstr::lb()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::lw()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::sb()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::sw()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::lh()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::lbu()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::lhu()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

void FuncInstr::sh()
{
	this->mem_addr = v_src1 + C;
	new_PC = PC + 4;
	write_back = 0;
}

int FuncInstr::getImm()
{
	if ( format == FORMAT_I)
	{
		return instr.asI.imm;
	}
	else
	{
		return 0;
	}
}

int FuncInstr::getShamt()
{
	if ( format == FORMAT_R)
	{	
		return instr.asR.shamt;
	}
	else
	{
		return 0;
	}
}

int FuncInstr::getJImm()
{
	if ( format == FORMAT_J)
	{
		return instr.asJ.imm;
	}
	else 
	{
		return 0;
	}
}

int FuncInstr::getWB() const
{
	return write_back;
}

/* Main function of the process of execution */

void FuncInstr::execute()
{
	( this->*pointer)();
}

/* Function for parsing instruction */

void FuncInstr::parseInstr()
{
	std::ostringstream oss1;
	std::ostringstream oss2;
	std::ostringstream oss3;
	std::ostringstream oss4;
	switch( operation)
	{
		case OUT_R_ARITHM:
			oss1 << " [" << std::hex << "0x" << v_src1 << "]";
			oss2 << " [" << std::hex << "0x" << v_src2 << "]";
			oss3 << " [" << std::hex << "0x" << v_dst  << "]";
			dissrc1 = oss1.str();
			dissrc2 = oss2.str();
			disdst = oss3.str();
			disasm = disasmName + disasmrd + disdst + disasmrs + dissrc1 + disasmrt + dissrc2;
			break;
		case OUT_R_SHAMT:
			oss1 << " [" << std::hex << "0x" << v_dst << "]";
			oss2 << " [" << std::hex << "0x" << v_src1 << "]";
			disdst = oss1.str();
			dissrc1 = oss2.str();
			disasm = disasmName + disasmrd + disdst + disasmrt + dissrc1 + disasmShamt;
			break;
		case OUT_R_JUMP:
			oss1 << " [" << std::hex << "0x" << v_src1 << "]";
			dissrc1 = oss1.str();
			disasm = disasmName + disasmrs + dissrc1;
			break;
		case OUT_I_ARITHM:
			oss1 << " [" << std::hex << "0x" << v_dst << "]";
			oss2 << " [" << std::hex << "0x" << v_src1 << "]";
			disdst = oss1.str();
			dissrc1 = oss2.str();
			disasm = disasmName + disasmrt + disdst + disasmrs + dissrc1 + disasmImm;
			break;
		case OUT_I_BRANCH:
			oss1 << " [" << std::hex << "0x" << v_src1 << "]";
			oss2 << " [" << std::hex << "0x" << v_src2 << "]";
			dissrc1 = oss1.str();
			dissrc2 = oss2.str();
			disasm = disasmName + disasmrs + dissrc1 + disasmrt + dissrc2 + disasmImm;
			break;
		case OUT_I_LOAD:
			oss1 << " [" << std::hex << "0x" << v_dst << "]";
			oss2 << " [" << std::hex << "0x" << v_src1 << "])";
			disdst = oss1.str();
			dissrc1 = oss2.str();
			disasm = disasmName + disasmrt + disdst + disasmImm + disasmrs + dissrc1;  
			break;
		case OUT_I_STORE:
			oss1 << " [" << std::hex << "0x" << v_dst << "]";
			oss2 << " [" << std::hex << "0x" << v_src1 << "])";
			disdst = oss1.str();
			dissrc1 = oss2.str();
			disasm = disasmName + disasmrt + disdst + disasmImm + disasmrs + dissrc1;
			break;
		case OUT_J_JUMP:
			break;
		case OUT_R_LOGIC:
			oss1 << " [" << std::hex << "0x" << v_dst << "]";
			oss2 << " [" << std::hex << "0x" << v_src1 << "]";
			oss3 << " [" << std::hex << "0x" << v_src2 << "]";
			disdst = oss1.str();
			dissrc1 = oss2.str();
			dissrc2 = oss3.str();
			disasm = disasmName + disasmrd + disdst + disasmrt +  dissrc1 + disasmrs + dissrc2;
			break;
		case OUT_I_LOGIC:
			oss1 << " [" << std::hex << "0x" << v_src1 << "]";
			oss2 << " [" << std::hex << "0x" << v_dst << "]";
			dissrc1 = oss1.str();
			disdst = oss2.str();
			disasm = disasmName + disasmrs + dissrc1 + disasmrt + disdst + disasmImm;
			break;
		case OUT_I_SHAMT:
			oss1 << " [" << std::hex << "0x" << v_dst << "]";
			disdst = oss1.str();
			disasm = disasmName + disasmrt + disdst + disasmImm;
			break;
	}
}

std::string FuncInstr::Dump( std::string indent) const
{
	return indent + disasm;
}
