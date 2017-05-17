#include <func_sim.h>
#include <iostream>

using namespace std;

int main( int argc, char** argv) {
       MIPS* mips = new MIPS;
       mips->run(argv[1], argv[2]);
       return 0;
   }
