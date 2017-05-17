/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>
#include <bitset>
#include <string.h>

using namespace std;

/**function invert
input: var - number in that num bits will be inverted
       num - quantity of bits that wil be inverted
output: res - number with inverted bits**/
uint64 invert (uint64 var, uint8 num)
{
	uint64 res = 0;
	uint64	var_1 = var;
	for (int i = 0; i < num; i++)
	{
	res |= (var_1 & 1)<<(num - i -1);	
	var_1 >>= 1;
	}
return (res);		
} 


/**function maskfunc
input: addr_size_len - length in bits of input nmber
       size_right - indent from left side of number
       size - quantity of 1 that will be in output number 
output: mask_data - number with "size" 1 with indent "size_right"**/
uint64 maskfunc(uint64 addr_size_len, uint64 size_right, uint64 size)
{
	uint64 mask_data = 1;
	for (int i = 1; i < size; i++)
	{
		mask_data = mask_data << 1;
		mask_data++;
	}
	mask_data = mask_data << (addr_size_len - size_right - size);  
	return(mask_data);
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
	int num = 0;
	vector<ElfSection> SLT_vector;//create a vector 
	addr_size_tmp = addr_size;// initialization fields of class
	page_bits_tmp = page_bits;
	offset_bits_tmp = offset_bits;
	start_addres = 0;
		

	ElfSection::getAllElfSections(executable_file_name, SLT_vector);
	num = SLT_vector.size(); // quantity of sections in vector


	for(int i = 0; i < num; i++)
	{
		for(int j = 0; j < (SLT_vector[i].size); j++)
			write ( SLT_vector[i].content[j], SLT_vector[i].start_addr + j, 1);
		
		if (!(strcmp(SLT_vector[i].name,".text")))// for START PC, record start addres of section .text
			start_addres = SLT_vector[i].start_addr;	
	}
}

FuncMemory::~FuncMemory()
{
//nothing		
}

uint64 FuncMemory::startPC() const
{	
	return(start_addres);	
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
	uint64 set_bits = addr_size_tmp - page_bits_tmp - offset_bits_tmp;
	uint64 mask_set_bits =  maskfunc( addr_size_tmp, 0, set_bits);
	uint64 mask_page_bits = maskfunc( addr_size_tmp, set_bits, page_bits_tmp);
	uint64 mask_offset_bits = maskfunc( addr_size_tmp, set_bits + page_bits_tmp, offset_bits_tmp);
	uint64 addr_set = (addr & mask_set_bits) >> (page_bits_tmp + offset_bits_tmp);
	uint64 addr_page = (addr & mask_page_bits) >> (offset_bits_tmp);
	uint64 addr_offset = addr & mask_offset_bits;
	uint64 value = 0;
		
	assert(num_of_bytes > 0);//num_of_bytes must be more than 0
	
	for (int i = 0; i < num_of_bytes; i++)
	{
		value <<= 8;
		//define addreses of set page offset with masks
		uint64 addr_set = ((addr + i) & mask_set_bits) >> (page_bits_tmp + offset_bits_tmp);
		uint64 addr_page = ((addr + i) & mask_page_bits) >> (offset_bits_tmp);
		uint64 addr_offset = (addr + i) & mask_offset_bits;

		//check memory after read, may be it is not allocate
		assert (memory.size() > addr_set );
		assert (memory[addr_set].size() > addr_page );
		assert (memory[addr_set][addr_page].size() > addr_offset);
		value |= memory[addr_set][addr_page][addr_offset];	
	}
	return (invert(value,num_of_bytes * 8));
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
	uint64 set_bits = addr_size_tmp - page_bits_tmp - offset_bits_tmp;
	uint64 mask_set_bits = maskfunc(addr_size_tmp, 0, set_bits);
	uint64 mask_page_bits = maskfunc(addr_size_tmp, set_bits, page_bits_tmp);
	uint64 mask_offset_bits = maskfunc(addr_size_tmp, set_bits + page_bits_tmp, offset_bits_tmp);
	uint64 mask_byte = maskfunc(64, 56, 8);
	uint64 mask_data = 1;
	
	assert(num_of_bytes > 0);// check num_of_bytes 		
		
	uint64 value_1 = invert(value, 8 * num_of_bytes);

	for (int i = num_of_bytes - 1; i >= 0; i--)
	{	//define addreses of set page offset with masks
		uint64  addr_set = ((addr + i) & mask_set_bits) >> (page_bits_tmp + offset_bits_tmp);
		uint64 	addr_page = ((addr + i) & mask_page_bits) >> (offset_bits_tmp);
		uint64 	addr_offset = (addr + i)  & mask_offset_bits;
		//check memory and allocate it if it needed
		if (memory.size() <= addr_set)
			memory.resize(addr_set + 1);
		if (memory[addr_set].size() <= addr_page)
			memory[addr_set].resize(addr_page + 1);
		if (memory[addr_set][addr_page].size() <= addr_offset )
			memory[addr_set][addr_page].resize(addr_offset + 1);
		
		memory[addr_set][addr_page][addr_offset] = (value_1 & mask_byte) >> ((num_of_bytes - 1 -i)*8);
		mask_byte <<= 8; 
	}
}
string FuncMemory::dump( string indent) const
{
	
	ostringstream oss;
	int size_set = memory.size() ;

	oss << indent << "Dump Functional memory " << endl;
	
	for (int i = 0; i < size_set; i++)
	{ 
		int  size_page = memory[i].size();
		for (int j = 0; j < size_page; j++)
		{
			int size_offset = memory[i][j].size();
			for (int k = 0; k <size_offset; k++)
			{
				if (memory[i][j][k])//dump inputs only not zero elements
				{
					oss << indent <<  i <<"       "<<j<< "       "<<hex<< k <<"   "
					<< bitset<8> (uint64(memory[i][j][k])) << endl;
				}
			} 
		}

	}   
    return oss.str();
}
