#include <iostream>
#include "../elf_parser/elf_parser.h"
#include "func_instr.h" 
#include <string>

int main(int argc, char* argv[])
{
	if(argc != 3)
	{
		cout << "Need all arguments\n";
		return 0;
	}
	char* file_name = argv[1];
	char* section_name = argv[2];

	vector<ElfSection> sections_array;
	ElfSection::getAllElfSections(file_name, sections_array);

	uint8 sections_counter = 0;
	for(sections_counter = 0; sections_counter < sections_array.size(); sections_counter++)
	{

		if(strcmp(sections_array[sections_counter].name, section_name) == 0)
		{
			for(uint8 byte_counter = 0; byte_counter < sections_array[sections_counter].size; byte_counter = byte_counter + 4)
			{
				uint32 instruction = (uint32)sections_array[sections_counter].content[byte_counter] + (uint32)(sections_array[sections_counter].content[byte_counter + 1] << 8) + (uint32)(sections_array[sections_counter].content[byte_counter + 2] << 16) + (uint32)(sections_array[sections_counter].content[byte_counter + 3] << 24);
				FuncInstr ISA_instr(instruction);
				ISA_instr.Dump();
			}
			return 0;
		}
	}
	cout << "section not found\n";
	return 0;
}
