// Generic C
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Generic C++
#include <iostream>
#include <iomanip>

// uArchSim modules
#include <func_sim.h>
#include <elf_parser.h>
#include <func_instr.h>
#include <reg_file.h>

using namespace std;

int main ( int argc, char* argv[])
{
    if ( argc >= 2 && strcmp( argv[1], "--help"))
    {
        FuncSim sim( argv[1]);
        int instr_counter = 0;
        cout << "    Running simulation on " << argv[1] << "..." << endl;
        cout << "----------------------------------------" 
                "----------------------------------------" << endl;
        while ( !sim.isTerminalConditionReached()) {
            sim.doSimulationStep();
            cout << "    " << hex << sim.getPC() << "    " 
                 << sim.dumpCurrentInstruction() << endl;
            instr_counter++;
        }
        cout << "----------------------------------------" 
                "----------------------------------------" << endl;
        cout << "    $v0 = " << dec << sim.getRegisterValue( 2) << endl;
        cout << "    " << instr_counter << " instructions were executed" 
             << endl;
    } else if ( argc == 2 && !strcmp( argv[ 1], "--help"))
    {
        cout << "MIPS simulator." << endl << endl
             << "Usage: \"" << argv[0] << " <ELF binary file>\""
             << endl;
    } else
    {
        cerr << "ERROR: wrong number of arguments!" << endl
             << "Type \"" << argv[ 0] << " --help\" for usage." << endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}
