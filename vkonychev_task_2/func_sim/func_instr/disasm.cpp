//My headers
#include <func_instr.h>

//uArch headers
#include <elf_parser.h>

//C++ headers
#include <iostream>
#include <cstdlib>
#include <cstring>


int main ( int argc, char* argv[] )
{
	if ( argc != 2)
	{
         	std :: cerr << "ERROR!: need input_file_name\n";
         	exit ( EXIT_FAILURE );
     	}

	vector<ElfSection> sections;
     	ElfSection :: getAllElfSections ( argv[1], sections );

	for ( int i = 0; i < sections.size(); ++i )
     	{
        	if ( strcmp ( sections[i].name, ".text" ) == 0 )
         	{
                	uint32 *content = (uint32 *) sections[i].content;
                 	uint64 size = (sections[i].size)/(sizeof(uint32));

                 	for (uint64 j=0; j < size; j++)
                 	{
                     		FuncInstr func_instr ( content[j] );
                     		std :: cout << j << ":" << func_instr.Dump(" ") << "\n";
                 	}
                 	return 0;
         	}
     	}

     	std :: cerr << "No .text section in " << argv[1] << '\n';
     	exit ( EXIT_FAILURE );

}
