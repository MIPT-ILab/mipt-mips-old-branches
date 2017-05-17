#include <iostream>
#include <func_sim.h>

using namespace std;

int main(int argc, char** argv)
{
    MIPS* mips = new MIPS;
    mips->run( argv[1] , 3);
    return 0;
}
