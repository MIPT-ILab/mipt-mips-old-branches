//C++ headers
#include <iostream>
#include <fstream>
#include <cmath>

//uArch headers

//My headers
#include "cache_tag_array.h"

//************************************************************************

int main ( int argc, char *argv[] )
{
	//chech arguments
	if ( argc != 3 ) {
		std::cout << "ERROR: I need only 2 arguments\n";
		return -1;
	}
	//open output file and check
	std::ofstream fout;
	fout.open( argv[2] );

	if ( !fout.is_open() ) {
		std::cout << "ERROR: I can't open " << argv[2] << '\n';
		return -1;
	}
	
	
	//open input file and check
	std::ifstream fin;
	uint32 KB = 1024;
	uint32 addr = 0;
	uint32 missCount = 0;
	uint64 PC = 0;
	uint32 size = 0;
	uint32 ways = 0;
	int resW = 0;

	for ( uint32 iWays = 0; iWays < 6; ++iWays )
	{
		for ( uint32 iKB = 0; iKB < 11; ++iKB ) 
		{
			std::cout << "iWais " << iWays << " iKB  " << iKB << "\n";
			fin.open( argv[1] );
			if ( !fin.is_open() ) {
				std::cout << "ERROR: I can't open " << argv[1] << '\n';
				return -1;
			}
			fin  >> std::hex;
			missCount = 0;
			PC = 0;
			resW = 0;

			size = KB * pow ( 2, iKB );
			if ( iWays != 5 ) {
				ways = pow ( 2, iWays );
			} else {
				ways = FUL_AS;
			}
		
			CacheTagArray Cache( size, ways, 4, 32 );	
		
			fin >> addr;
			while ( !fin.eof() ) {
				if ( !Cache.read( addr, PC ) ) {
					++missCount;
					resW = Cache.write( addr, PC );
					if ( resW ) {
						return -1;				
					}	
				}

				++PC;
				fin >> addr;
			}
            fout << ((double)missCount/PC);

            if ( iKB != 10 ) {
                fout << ", ";
            } else {
                fout << "\n";
            }

			fin.close();
			
        }
	}
	fout.close();

	return 0;
}
