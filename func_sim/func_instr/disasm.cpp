/**
 * disasm.cpp - disassembler for MIPS
 *@author Andrey Kochetygov <andrey.kochetygov@gmail.com>
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

int main( int argc, char *argv[])
{


    if ( argc == 3)
    {
        ElfSection section( argv[1], argv[2]);
        uint64 startaddr = section.startAddr();
        while( section.isInside( startaddr, sizeof( uint32)))
        {
          FuncInstr instrdisasm( section.read( startaddr, sizeof( uint32)));
          std::cout << instrdisasm << endl;
          startaddr += sizeof( uint32);
        }
    }
    else
    {
       std::cout << "Wrong number of arguments" << std::endl
                  << "This program disassembles content of section" << std::endl
                  << "of ELF binary file.Usage: [elf_filename] [section_name]" << std::endl;
    }
    return 0;
}

