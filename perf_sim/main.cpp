/*
 * main.cpp - mips scalar simulator
 * @author Dmitry Ermilov ermilov@phystech.edu
 * For MIPT-MIPS 2015
 */

#include <iostream>
#include <cstdlib>
#include <cstring>

#include <perf_sim.h>

int main( int argc, char* argv[])
{
    if ( argc < 3 || argc > 4 || ( argc == 4 && strcmp( "-d", argv[3]) != 0))
    {
        std::cout << "2 or 3 arguments required: mips_exe filename and amount" 
                  << " of instrs to run, key \"-d\" optionally" << endl;
        std::exit(EXIT_FAILURE);
    }

    PerfMIPS* mips = new PerfMIPS();
    
    Port<FuncInstr>::portMap->init();
    Port<uint32>::portMap->init();
    Port<bool>::portMap->init();

    if  ( argc == 3)
        mips->run(std::string(argv[1]), atoi(argv[2]));
    else  
    	mips->run(std::string(argv[1]), atoi(argv[2]), false);

    delete mips;

    return 0;
}

