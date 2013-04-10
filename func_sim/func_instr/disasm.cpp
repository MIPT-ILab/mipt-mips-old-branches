/**
 * disasm.cpp - MIPS instruction disassembler 
 * @author Alexander Kravtcov
 */

// Generic C
#include <cerrno>
#include <cstdlib>

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_instr.h>
#include <elf_parser.h>

int main( int argc, char* argv[])
{
    const int NUM_OF_ARGS = 2;
    std::string indent = "    ";

    if ( argc - 1 != NUM_OF_ARGS)
    {
        std::cerr << "ERROR: Wrong data format!" << std::endl;
        std::cerr << " Usage: " << argv[ 0] << " <elf filename> <section name>"
                  << std::endl;

        exit( EXIT_FAILURE);
    }
    
    std::cout << "Disassembly of section \"" << argv[ 2] << "\":"
              << std::endl;

    ElfSection section( argv[ 1],  // file name
                        argv[ 2]); // section name

    uint64 start_addr_sect = section.startAddr();

    int num_of_bytes = 4; // wil be read every num_of_bytes bytes

    for ( uint64 offset = 0; 
          section.isInside( start_addr_sect + offset, num_of_bytes);
          offset += num_of_bytes)
    {
        uint32 bytes = ( uint32)section.read( start_addr_sect + offset, num_of_bytes);
        
        FuncInstr instr( bytes);

        std::cout << indent << "0x" << std::hex << start_addr_sect + offset 
                  << std::dec << ":" << indent << instr;
    }

    return 0;
}
