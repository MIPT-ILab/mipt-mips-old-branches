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
#include <unistd.h>


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

	CacheTagArray cache(4096, 4, 4, 32);
	uint64 address;
	uint32 access_count = 0;
	uint32 misses_count = 0;
	while ( addr_file >> hex >> address)
	{
		if ( !cache.read( address))
			misses_count++;
		access_count++;
	}
	
	float miss_rate = (float) misses_count / access_count;
	cout << miss_rate << endl;
	cout << access_count << endl;
	addr_file.close();

	return 0;
}


