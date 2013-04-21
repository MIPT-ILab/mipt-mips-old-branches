// Generic C
#include <cstdlib>

// Generic C++
#include <iostream>

//uArchSim modules
#include <func_sim.h>
#include <types.h>

using namespace std;

int main( int argc, char *argv[])
{
    if ( argc != 2)
    {
        cerr << "Error: wrong number of arguments!\n"
             << "Just enter elf-file\n";
        exit( EXIT_FAILURE);
    }
    FuncSim sim( argv[1]); 
    sim.simulRun();
    return 0;
}
