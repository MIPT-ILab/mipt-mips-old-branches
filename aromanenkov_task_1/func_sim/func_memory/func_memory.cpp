/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++
#include <sstream>
#include <cstring>
// uArchSim modules
#include <func_memory.h>

/**
* FuncMemory::FuncMemory - constructs FuncMemory class using binary ELF file
*
* @param(in)    executable_file_name        name of binary ELF file
* @param(in)    addr_size                   size of address in bits
* ||don't use in the simpliest case:
* @param(in)    page_bits                   size of page part of
*                                           address in bits
* @param(in)    offset_bits                 size of offset part of
*                                           address in bits
*
*/
FuncMemory::FuncMemory(const char* executable_file_name,
	uint64 addr_size,
	uint64 page_bits,
	uint64 offset_bits)
{
	assert( executable_file_name);
	assert( addr_size);
	assert( page_bits);
	assert( offset_bits);

	vector<ElfSection> sections;
	ElfSection::getAllElfSections( executable_file_name, sections);
	
	this->val = ( char*)calloc( 0x20000);
	this->initilized = ( bool*)calloc( 0x20000, sizeof( char));
	
	for ( int i = 0; i < sections.size(); i++)
	{
		unsigned short num_of_bytes = 1;
		if ( !strcmp(sections_vector[i].name, ".text"))
		{
			starting_PC = sections[i].start_addr;
		}
		for ( int k = 0; k < sections_[i].size; k++)
		{
			this->write( sections[i].content[k], sections[i].start_addr + k, num_of_bytes);
		}
	}

}

/**
* FuncMemory::~FuncMemory - destructs FuncMemory class
*
*/
FuncMemory::~FuncMemory()
{
	assert( this->val);
	assert( this->initilized);
	
	free( this->val);
	free( this->initilized);
}
/**
* FuncMemory::startPC - returns address of the ".text" section beginning
*
*/
uint64 FuncMemory::startPC() const
{
	return this->starting_PC;
}
/**
* FuncMemory::read - reads data from functional memory
*
* @param(in)    addr                        address to read from
* @param(in)    num_of_bytes                number of bytes to read
*
*/
uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
	assert( num_of_bytes > 0 && num_of_bytes <= 8);
	uint64 datum = 0;
	uint8 data[8];

	for ( int i = 0; i < num_of_bytes; i++)
	{
		data[i] = this->val[addr - 0x3FFFF0 + i];
	}

	for ( int k = num_of_bytes - 1; k >= 0; k--)
	{
		datum = datum * 256 + data[k];
	}

	return datum;
}
/**
* FuncMemory::write - writes data to functional memory
*
* @param(in)    value                       input data
* @param(in)    addr                        address for reading
* @param(in)    num_of_bytes                number of bytes to read
*
*/
void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
	assert( num_of_bytes > 0 && num_of_bytes <= 8);
	uint64 datum = value;
	
	for ( int i = 0; i < num_of_bytes; i++)
	{
		this->val[addr - 0x3FFFF0 + i] = datum % 256;
		this->initilized[addr - 0x3FFFF0 + i] = true;
		datum = datum/256;
	}
}

string FuncMemory::dump( string indent) const
{
	cout << "DUMP" << endl;
	ostringstream oss;
	oss << "Dumping Functional Memory :";
	oss << "Reason: " << indent << endl;
	for (uint32 i = 0; i < sections.size(); i++)
	{
		oss << " name of section["<< i <<"]: " << sections[i].name << ", "
			<< "size = " << sections[i].size << ", " << hex << "start addres = "
			<< sections[i].start_addr << endl;
	}

	return oss.str();
}