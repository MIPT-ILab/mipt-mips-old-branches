#include <func_sim.h>
#include <func_instr.h>
#include <elf_parser.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main( int argc, char** argv) {
    if ( argc != 3)
    {
        cout << "WRONG NUMBER OF ARGUMENT" << endl;
        return -1;
    }
    else
    {
        MIPS* mips = new MIPS;
        mips->run( argv[ 1], atoi( argv[ 2]));
    }
    return 0;
}
