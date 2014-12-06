#include<func_instr.h>

uint32 get_instr (uint8 *pointer)
{
	uint32 instr = 0;
	for (uint8 i = 0; i < 4; i++)
		instr = instr | (pointer[i] << 8 * (3 - i))
	return instr;
}

FuncInstr::FuncInstr(uint32 bytes)
{
	this->parser(bytes);
}
pareser(uint32 bytes)
{
	info.raw = bytes;
	this->initformat(bytes);
}	

FuncInstr::~FuncInstr()
{
	info.raw = 0;
}

char FuncInstr::initreg(uint32 num)
{
	char reg[7];
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
		ell = (num - 8);
		reg[2] = ell;
		return(reg);
	}
	
	if ((num == 24) || (num == 25))
	{
		reg = "$t";
		ell = (num - 16);
		reg[2] = ell;
		return(reg);
	}
}
	
void FuncInstr::initformat(uint32 num)
{
	uint32 mask_format = 1;
	mask_format = ((mask_format << 7) - 1) <<26;
	
	if (((num & mask_format) >> 26) == 0)
		this->format = FORMAT_R;
	if ((((num & mask_format) >> 26) == 8) || (((num & mask_format) >> 26) == 9) || (((num & mask_format) >> 26) == 4) || (((num & mask_format) >> 26) == 5))
		this->format = FORMAT_I;
	if (((num & mask_format) >> 26) == 2)
		this->format = FORMAT_J;
}
	

string FuncInstr::dump(string indent) const
{
	ostringstream oss;
	
	if (this->format ==  FORMAT_R)
		{
			if (info.asR.f == 32)
				oss<<"add"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<endl;
			if (info.asR.f == 33)
				oss<<"addu"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<endl;
			if (info.asR.f == 34)
				oss<<"sub"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<endl;
			if (info.asR.f == 35)
				oss<<"subu"<<" "<<this->initreg(info.asR.d)<<", "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<endl;
			if (info.asR.f == 0)
				oss<<"sll"<<" "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<", "<<info.asR.S<<endl;
			if (info.asR.f == 2)
				oss<<"srl"<<" "<<this->initreg(info.asR.s)<<", "<<this->initreg(info.asR.t)<<", "<<info.asR.S<<endl; 
			if (info.asR.f == 8)
				oss<<"jr"<<" "<<this->initreg(info.asR.s)<<endl;
		} 
	if (this->format == FORMAT_I)
		{
			if (info.asI.opcode == 8)
				oss<<"addi"<<" "<<this->initreg(info.asI.t)<<", "<<this->initreg(info.asI.s)<<", "<<info.asI.C<<endl;
			if (info.asI.opcode == 9)
				oss<<"addiu"<<" "<<this->initreg(info.asI.t)<<", "<<this->initreg(info.asI.s)<<", "<<info.asI.C<<endl;
			if (info.asI.opcode == 4)
				oss<<"beq"<<" "<<this->initreg(info.asI.s)<<", "<<this->initreg(info.asI.t)<<", "<<info.asI.C<<endl;
			if (info.asI.opcode == 5)
				oss<<"bne"<<" "<<this->initreg(info.asI.s)<<", "<<this->initreg(info.asI.t)<<", "<<info.asI.C<<endl;
		}
	
	if (this->format == FORMAT_J)
		oss<<"j"<<" "<<info.asJ.A<<endl;
	
	return oss.str();
}		 
