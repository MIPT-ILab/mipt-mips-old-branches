//Generic C++
#include <iostream>

//Generic uArch sim modules
#include "func_instr.h"
#include "elf_parser.h"
#include "func_memory.h"

//Generic C
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

/* input: argument1: elf filename, argument2: section's name */
int main( int argc, char* argv[ ])
{
    const int num_of_args = 3;
    if ( argc == num_of_args && strcmp( argv[ 1],"--help"))
    {
        uint64 PC;
        uint64 section_size;
        vector<ElfSection> sections_array;
        /* Using of ElfParser for elf file */
        ElfSection::getAllElfSections( argv[ 1], sections_array);
        /* Using FuncMemory simulator */
        FuncMemory func_mem( argv[ 1], 32, 10, 12);
        for ( int i = 0; i < sections_array.size( ); ++i)
        {
            if ( strcmp( sections_array[ i].name,argv[ 2]) == 0)
            {
                PC = sections_array[ i].start_addr;
                section_size = sections_array[ i].size;
                break;
            }
            else
            {
                if( i ==  sections_array.size( ) - 1)
                {
                    cerr << "ERROR: there is no such section!!! " << endl;
                    exit( EXIT_FAILURE);
                }
            }

        }
        uint64 max_size = PC + section_size;
        while ( PC < max_size)
        {
            FuncInstr com(func_mem.read( PC, 4));
            PC = PC + 4;
            std::cout << com << endl;

        }
    }
    else
    {
        cerr << "ERROR: wrong number of arguments!!!" << endl;
        exit( EXIT_FAILURE);
    }
    return 0;
}

