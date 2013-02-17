//Generic C
#include <assert.h>
#include <stdlib.h>

//Generic C++
#include <iostream>
#include <string>
#include <sstream>

//uArchSim modules
#include <func_instr.h>
#include <types.h>
#include <elf_parser.h>

int main(int argc, char *argv[])
{
    const int args_num = 3;
    if ( argc == args_num)
    {
        ElfSection section( argv[1], argv[2]);
        uint64 start = section.startAddr();
        while( section.isInside( start, sizeof( uint32)))
        {
            FuncInstr current( section.read( start, sizeof( uint32)));
            std::cout << current << endl;
            start += sizeof( uint32);
        }
    } 
    else if ( argc != args_num)
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl;
    }
    return 0;
}

