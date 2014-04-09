/*
 * miss_rate_sim.cpp
 *
 */



#include <iostream>
#include <fstream>
#include <cstring>
#include <types.h>
#include <cache_tag_array.h>
#include <errno.h>
#include <cstdlib>


using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 2){
		cout << "need exactly 1 argument: name of file with addresses" << endl;
		exit(EXIT_FAILURE); 		// is this good style for C++ ?
	}

	ifstream addr_file;
	addr_file.open(argv[1]);
	if (!addr_file.is_open())
	{
		cerr << "Invalid filename: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	CacheTagArray cache(4096, 4, 32, 32);
	uint64 address;
	uint32 i = 0;
	while ( addr_file >> hex >> address)
	{
	   	cout << cache.read(address) << ' ';
		i++;
		if (addr_file.get() != ',')
			break;
	}

	cout << i << endl;
	addr_file.close();

	return 0;
}


