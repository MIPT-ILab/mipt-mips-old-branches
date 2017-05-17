// Generic C
#include <stdlib.h>
#include <string.h>
// Generic C++
#include <iostream>

// uArchSim modules
#include <func_instr.h>
#include <elf_parser.h>

using namespace std;

int main (int argc, char* argv[])
{
    const int num_of_args = 2;
    if ( argc - 1 == num_of_args)
    {
        vector<ElfSection> sections_array;
        const char* file_name = argv[1];
        const char* section_name = argv[2];
        
        ElfSection::getAllElfSections( file_name, sections_array);
        for( uint8 i = 0; i < sections_array.size(); i++)
        {
            if( strcmp( section_name, sections_array[ i].name) == 0)
            {
                uint64 section_size = sections_array[ i].size;
                uint8* section_content = sections_array[ i].content;
                
                // Reading 4 bytes and uniting them into uint32
                for( uint64 j = 0; j < ( section_size / 4); j++)
                {
                    uint32 data = 0;
                    for( int8 k = 3; k >= 0; k--)
                    {
                        uint8 temp = section_content[ 4 * j + k];
                        data = ( data << 8) | temp;
                    }
                    FuncInstr disasm( data);
                    cout << disasm.Dump("    ");
                }
                return 0;
            }
        }
        cerr << "Invalid section name" << endl;
        exit( EXIT_FAILURE);
    }
    else
    {
        cerr << "Incorrect number of arguments" << endl
        << "Input ELF file name and section name" << endl;
        exit( EXIT_FAILURE);
    }
    return 0;
}

