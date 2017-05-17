#include <func_instr.h>
#include <stdio.h>

uint32 get_instr (uint8 *pointer)
{
	uint32 instr = 0;
	for (uint8 i = 0; i < 4; i++)
		instr = instr | (pointer[i] << 8 * (3 - i));
	return instr;
}

FuncInstr::FuncInstr(uint32 bytes)
{
	this->initformat(bytes);
	switch (this->format)
	{
		case FORMAT_R:
			this->parser(bytes);
			break;
		case FORMAT_I:
			this->parser(bytes);
			break;
		case FORMAT_J:
			this->parser(bytes);
			break;
		case UNIX:
			ASSERT (0, "ERROR.*")
	}

}
void FuncInstr::parser(uint32 bytes)
{
	this->info.raw = bytes;
}	

FuncInstr::~FuncInstr()
{
	this->info.raw = 0;
}

string FuncInstr::initreg(uint8 num)
{
	string reg;
	string reg1;
	char ell;
	
	if (num == 0)
	{
		reg = "$zero";
		return (reg);
	}
	
	if (num == 1)
	{
		reg = "$at";
		return (reg);
	}
	
	if ((num >= 8) && (num <= 15))
	{
		reg = "$t";
		reg1 = (num - 8) + '0';
		reg.append(reg1);
		return(reg);
	}
	
	if ((num == 24) || (num == 25))
	{
		reg = "$t";
		reg1 = (num - 16) + '0';
		reg.append(reg1);
		return(reg);
	}
}
	
void FuncInstr::initformat(uint32 num)
{
	uint32 mask = 1;
	mask = ((mask << 7) - 1) << 26;
	
	num = (num & mask) >> 26;
	if (num == 0)
		this->format = FORMAT_R;
	else
		if ((num == 8) || (num == 9) || (num == 4) || (num == 5))
			this->format = FORMAT_I;
		else
			if (num == 2)
				this->format = FORMAT_J;
			else
				this->format = UNIX;
	
}
	

string FuncInstr::Dump(string indent)
{
	ostringstream oss;
	
	if (this->format ==  FORMAT_R)
		{
			if (info.asR.f == 32)
				oss<<"add"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t);
			if (info.asR.f == 33)
				oss<<"addu"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t);
			if (info.asR.f == 34)
				oss<<"sub"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t);
			if (info.asR.f == 35)
				oss<<"subu"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t);
			if (info.asR.f == 0)
				oss<<"sll"<<" "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<", 0x"<<hex<<info.asR.S;
			if (info.asR.f == 2)
				oss<<"srl"<<" "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<", 0x"<<hex<<info.asR.S; 
			if (info.asR.f == 8)
				oss<<"jr"<<" "<<this->initreg(info.asR.s)<<endl;
		} 
	if (this->format == FORMAT_I)
		{
			if (info.asI.opcode == 8)
				oss<<"addi"<<" "<<this->initreg(info.asI.t)<<", "<<this->initreg(info.asI.s)<<", 0x"<<hex<<info.asI.C;
			if (info.asI.opcode == 9)
				oss<<"addiu"<<" "<<this->initreg(info.asI.t)<<", "<<this->initreg(info.asI.s)<<", 0x"<<hex<<info.asI.C;
			if (info.asI.opcode == 4)
				oss<<"beq"<<" "<<this->initreg(info.asI.s)<<", "<<this->initreg(info.asI.t)<<", 0x"<<hex<<info.asI.C;
			if (info.asI.opcode == 5)
				oss<<"bne"<<" "<<this->initreg(info.asI.s)<<", "<<this->initreg(info.asI.t)<<", 0x"<<hex<<info.asI.C;
		}
	
	if (this->format == FORMAT_J)
		oss<<"j"<<" 0x"<<hex<<info.asJ.A;
	
	return oss.str();
}		 
