#include <func_sim.h>
#include <cstdio>


int main( int arg, char** argv)
{
    MIPS* mips = new MIPS;
    int instr_to_run = atoi(argv[2]);
    mips->run( argv[1], instr_to_run);
}