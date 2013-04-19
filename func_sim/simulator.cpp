/*
 * simulator.cpp - The programm that does functional simulation
 * author Alexander Kravtcov
 */

// Generic C++
#include <iostream>

// Generic C
#include <cassert>
#include <cstdlib>

// uArchSim module
#include <func_sim.h>

int main( int argc, char* argv[])
{
    const int NUM_OF_ARGS = 1;

    if ( argc - 1 != NUM_OF_ARGS)
    {
        std::cerr << "ERROR. Wrong data format!" << std::endl;
        std::cerr << " Usage: " << argv[ 0] << " <elf filename>"
                  << std::endl;

        exit( EXIT_FAILURE);
    }
    

    FuncSim sim( argv[ 1]);

    while ( !sim.getExitFlag())
    {
        sim.decode();
        sim.readFromReg();
        sim.alu();
        sim.memReadWrite();
        sim.writeBack();
        sim.updatePC();

        std::cout << sim.dumpExecutedInstr();
    }
    sim.stopExecution();

    std::cout << std::endl << "Simulator has finished successully" << std::endl;
    return 0;
}
