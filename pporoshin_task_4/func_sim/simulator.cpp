/**
 * simulator - functional simulator of MIPS 
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <cstring>
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_sim.h>

int main( int argc, char *argv[])
{
    if( argc == 2 && !strcmp( argv[1], "--help")) // Check of number of arguments
    {
        std::cout << "This program is functional simulator of MIPS processor - it runs [elf_filename]" << std::endl
                  << "program until write to $v0 register occurs and prints every processed instruction." << std::endl 
                  << "Usage: simulator [elf_filename]" << std::endl;

    } else if( argc == 2)
    {
        ProgramExecuter executer( argv[1]);
        int instr_counter = 0;
        while( !executer.isTerminated())
        {
            executer.execInstr();
            std::cout << executer.dump( "") << std::endl;
            executer.updatePC();
            instr_counter++;
        }
        std::cout << "Program terminated with status " << executer.getTermStatus() << std::endl
                  << instr_counter << " instructions were executed." << std::endl;
    } else
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl
                  << "Type \"" << argv[0] << " --help\" for usage." << std::endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}