// Generic C
#include <cstdlib>

// Generic C++
#include <iostream>
#include <string>

//uArchSim modules
#include <func_instr.h>
#include <elf_parser.h>
#include <types.h>

using namespace std;

void sect_dump( char *elf_file, char *file_sect)
{
    ElfSection section( elf_file, file_sect);
    cout << section.dump( "    ") << endl;
    const short offset = 4;
    for ( uint64 addr = section.startAddr(); section.isInside( addr, offset); addr += offset)
    {
        FuncInstr instr( section.read( addr));
        cout << instr.Dump( "    ") << endl;
    }
}
    
int main( int argc, char *argv[])
{
    const short num_of_arg = 2;
    if ( argc != num_of_arg + 1)
    {
        cerr << "Error: wrong number of arguments!\n"
             << "Just enter elf-file and elf-section\n";
        exit( EXIT_FAILURE);
    }
    sect_dump( argv[ 1], argv[ 2]);
    return 0;
} 
