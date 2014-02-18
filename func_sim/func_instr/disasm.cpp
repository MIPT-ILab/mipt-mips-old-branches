#include <iostream>
#include <string>

#include "../elf_parser/elf_parser.h"
#include "func_instr.h"

int main( int argc, char* argv[ ])
{
// Check quantity of arguments
	if( argc != 3)
	{
		cout << "Need all arguments\n";
		return 0;
	}
    
	char* file_name = argv[ 1];
	char* section_name = argv[ 2];
// Create array for sections of data
	vector<ElfSection> sections_array;
// Load sections from binary file
	ElfSection::getAllElfSections( file_name, sections_array);

// Search necessary section
	uint8 sections_counter = 0;
	for( sections_counter = 0; sections_counter < sections_array.size( ); \
	sections_counter++)
	{
// If we find necessary section
		if( strcmp( sections_array[ sections_counter].name, section_name) == 0)
		{
// For each instruction in this section, we build it, parse it, and print it
			for( uint8 byte_counter = 0; byte_counter < sections_array[ sections_counter].size; \
			byte_counter = byte_counter + 4)
			{
				uint32 instruction = 														    \
				( uint32)sections_array[ sections_counter].content[ byte_counter] +			    \
				( uint32)( sections_array[ sections_counter].content[ byte_counter + 1] << 8) + \
				( uint32)( sections_array[ sections_counter].content[ byte_counter + 2] << 16) +\
				( uint32)( sections_array[ sections_counter].content[ byte_counter + 3] << 24);
				FuncInstr ISA_instr( instruction);
				ISA_instr.Dump( );
			}
			return 0;
		}
	}
// If we don't find necessary section
	cout << "section not found\n";
	return 0;
}
