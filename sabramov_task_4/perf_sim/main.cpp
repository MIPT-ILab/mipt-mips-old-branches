#include<cstdlib>
#include<iostream>
#include<string.h>
#include<perf_sim.h>

using namespace std;

int main( int argc, char** argv)
{
	bool silence;
	
	if ( argc == 3)
	{
		silence = true;
	}
	else if ( argc == 4)
	{
		if ( strcmp( argv[ 3],"-d") == 0)
		{
			silence = false;
		}
		else
		{
			cout << "INVALID ARGUMENT" << endl;
			return 0;
		}
	}
	else 
	{
		cout << "INVALID NUMBER OF ARGUMENTS" << endl;
		return 0;
	}
	
	PerfMIPS* mips = new PerfMIPS();
	Port< FuncInstr>::init();
	Port< bool>::init();
	Port< uint32>::init(); 
	mips->run( std::string( argv[ 1]), atoi( argv[ 2]), silence);
	delete mips;
	return 0;
}
