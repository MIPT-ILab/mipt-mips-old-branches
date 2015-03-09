#include "func_sim.h"
#include <cstring>
#include <cstdlib>

using namespace std;

int main( int argc, char** argv) 
{
	if ( argc != 3)
    {
        std::cout << "2 arguments required: mips_exe filename and number of"   \
        " instructions to perfom" << endl;
        exit(EXIT_FAILURE);
    }
    
    MIPS* mips = new MIPS;
    mips->run( argv[1],  atoi(argv[2]));
    
    delete mips;
    return 0;
}