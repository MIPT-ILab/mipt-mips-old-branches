// Generic C
#include <cstdio>
#include <cstdlib>

// Generic C++
#include <iostream>
#include <string>

//uArchSim modules
#include <func_instr.h>
#include <elf_parser.h>
#include <types.h>

using namespace std;

/*uint32 reverse_bits( uint32 bytes)
{
    uint32 tmp[4] = {};
    uint32 res = 0;
    uint32 *p = &bytes;
    for ( short i = 0; i < 4; ++i)
    {
        tmp[i] = *((uint8 *)p + i);
        res |= tmp[i] << (24 - 8 * i);
    }
    return res;
}
*/

int main( int argc, char *argv[])
{
    const short num_of_arg = 2;
    if ( argc > num_of_arg + 1)
    {
        cerr << "Error: too many arguments!\n"
             << "Just enter elf-file and elf-section\n";
        exit( EXIT_FAILURE);
    } else if ( argc < num_of_arg + 1)
    {
        cerr << "Error: too few arguments!\n"
             << "You should enter elf-file and elf-section\n";
        exit( EXIT_FAILURE);
    }
    ElfSection section( argv[ 1], argv[ 2]);
    cout << section.dump( "    ") << endl;
    const short offset = 4;
    for ( uint64 addr = section.startAddr(); section.isInside( addr, offset); addr += offset)
    {
        FuncInstr instr( section.read( addr));
        cout << instr.Dump( "    ") << endl;
    }
    return 0;
} 
