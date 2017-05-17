/**
 * main.cpp
 * Start point of the MIPS functional simulator.
 * MIPT-MIPS Assignment 3.
 * Ladin Oleg.
 */

/* Generic C. */
#include <cstdlib>

/* Functional simulator modules. */
#include <func_sim.h>

using namespace std;

int main( int argc, char* argv[])
{
    /* Check arguments. */
    if ( argc != 3)
    {
        cerr << "ERROR: Wrong number of arguments!\n";
        exit( EXIT_FAILURE);
    }
    if ( ( argv[ 1] == NULL) || ( atoi( argv[ 2]) < 0))
    {
        cerr << "ERROR: Wrong arguments!\n";
        exit( EXIT_FAILURE);
    }

    MIPS* mips = new MIPS;
    mips->run( argv[ 1], static_cast< unsigned int>( atoi( argv[ 2])));
    delete mips;
    return 0;
}
