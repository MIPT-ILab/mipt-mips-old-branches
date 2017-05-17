//Generic C++
#include<iostream>
#include<cstdlib>

//Generic uArchSim modules
#include"func_sim.h"
#include <elf_parser.h>



int main( int argc, char** argv)
{
	if (argc != 3) 
	{
		cerr << "ERROR: wrong number of arguments!" << endl;
		exit( EXIT_FAILURE);
	}	
	else
	{
		MIPS* mips = new MIPS;
		mips->run( argv[ 1], atoi( argv[ 2]));
		delete mips;
	}	
		
}