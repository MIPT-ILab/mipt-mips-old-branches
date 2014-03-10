// Generic C/C++
#include <iostream>
#include <cstring>
#include <stdio.h>
#include <cerrno>

//uArch sim modules
#include <types.h>
#include <elf_parser.h>
#include <func_instr.h>


int main( int argc, char *argv[])
{
    if( argc != 2)
    {
        // error
        cout << "Wrong elf file name!" << endl;
        exit( EXIT_FAILURE);
    }
    else
    {
        vector<ElfSection> section;
        ElfSection::getAllElfSections( argv[ 1], section);

        for( int i = 0; i < section.size(); i++)
        {
            if( strcmp(section[ i].name, ".text") == 0)
            {
                uint32 *content = ( uint32*) section[ i].content;
                uint64 size = ( section[ i].size) / sizeof( uint32);

                for( uint64 j = 0; j < size; j++)
                {
                    FuncInstr func_instr( content[ j]);
                    std::cout << func_instr.Dump("Dumping :  ");
                }
                // success
                exit( EXIT_SUCCESS);
            }
        }

    }
    //failure
    exit( EXIT_FAILURE);
    return 0;
}
