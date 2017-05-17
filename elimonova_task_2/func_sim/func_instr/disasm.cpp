/**
 * disasm.cpp - disassembler for MIPS
 * @author Elena Limonova <elena.e.limonova@gmail.com> 
 * Copyright 2012 uArchSim iLab project
 */

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
    const int num_of_args = 3;
    
    if ( argc == num_of_args)
    {
        ElfSection section( argv[1], argv[2]);
        uint64 start = section.startAddr();
        while( section.isInside( start, sizeof( uint32)))
        {
          FuncInstr curr( section.read( start, sizeof( uint32)));
          std::cout << curr << endl;
          start += sizeof( uint32);
        }
    } else if ( argc != num_of_args)
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl
             << "Type \"" << argv[0] << " --help\" for usage." << std::endl;
        exit( EXIT_FAILURE);

    } else
    {
        std::cout << "This program disassembles content of section" << std::endl
                  << "of ELF binary file. It takes ELF filename and section name as parameters." << std::endl << std::endl << "Usage: \"" << argv[0]
                  << " <ELF binary file>\"" << std::endl; 
    }
    return 0;
}
