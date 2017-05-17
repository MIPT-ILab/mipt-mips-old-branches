#include <func_sim.h>
#include <cstdio>


int main( int arg, char** argv)
{
    MIPS* mips = new MIPS;
    int instr_to_run = atoi(argv[2]);
    cout << "running" << argv[1] << instr_to_run << endl;
    mips->run( argv[1], instr_to_run);
    return 0;
}
