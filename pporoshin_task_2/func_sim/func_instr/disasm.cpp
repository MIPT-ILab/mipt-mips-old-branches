/**
 * disasm - disassembler utility for printing out
 * content of section of Elf file in human-readable way 
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <cstring>
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <types.h>
#include <func_instr.h>
#include <elf_parser.h>


int main( int argc, char *argv[])
{
    if( argc == 3) // Check of number of arguments
    {
        ElfSection section( argv[1], argv[2]);
        uint64 cur_addr = section.startAddr();
        while( section.isInside( cur_addr, sizeof( uint32)))
        {
            FuncInstr instr( section.read( cur_addr));
            std::cout << instr.Dump( "    ") << std::endl;
            cur_addr += sizeof( uint32);
        }

    } else if( argc == 2 && !strcmp( argv[1], "--help"))
    {
        std::cout << "This program prints disassembler of 32-bit words contained" << std::endl
                  << "in section [section_name] of elf file [elf_filename]." << std::endl
                  << "Usage: disasm [elf_filename] [section_name]" << std::endl;
    } else
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl
                  << "Type \"" << argv[0] << " --help\" for usage." << std::endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}