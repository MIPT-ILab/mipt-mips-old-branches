/**
 * miss_rate_sim.cpp - The program runs the cache model
 * performance simulation on a given memory trace and stores
 * the resulting miss rates for different cache parameters.
 *
 * Usage: miss_rate_sim <memory-trace-file> <output-csv-file>
 *
 * Input and output file formats are described here:
 *
 * http://code.google.com/p/uarch-sim/wiki/Assignment_3
 *
 * @author Dmitry Borody <dmitry.borody@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Generic C
#include <cstdlib>

// Generic C++
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// uArchSim modules
#include <cache_tag_array.h>

// Parameters for the simulation
const int associativity_table[] = { 1, 2, 4, 8, 16, };
const int cache_size_in_kb_table[] = { 1, 2, 4, 8, 16, 32, 64, 
								 128, 256, 512, 1024, }; 

int main( int argc, char **argv)
{
	if ( argc != 3) 
	{
		std::cerr << "Usage: " << argv[ 0] << " <memory-trace-file> "
			 	  << "<output-csv-file>" << std::endl;
		exit( EXIT_FAILURE);
	}

	// Open memory trace file
	std::ifstream fin( argv[1]);
	if (fin.fail()) 
	{
		std::cerr << "Can't open file " << argv[1] << std::endl;
		exit( EXIT_FAILURE);
	}
	std::ofstream fout( argv[2]);

	// Read in the memory trace
	std::vector <uint64> mem_trace;
	uint64 cur_addr;
	char skip;
	while ( fin >> skip >> skip >> std::hex >> cur_addr) 
		mem_trace.push_back( cur_addr);
	fin.close();

	std::cerr << "Memory trace has " << mem_trace.size() << " accesses" << std::endl;

	for ( auto associativity : associativity_table) 
	{
		for ( auto cache_size_in_kb : cache_size_in_kb_table)
		{
			std::cerr << "Performing simulation for cache size " << cache_size_in_kb << "kb"
				<< " and " << associativity << "-way associativity..." << std::endl;

			CacheTagArray cache( cache_size_in_kb * 1024, associativity, 4, 32);

			unsigned miss_counter = 0;
			for ( uint64 addr 	: mem_trace) 
			{
				if (!cache.read( addr)) 
				{
					cache.write( addr);
					miss_counter++;
				}
			}

			fout << std::fixed << double(miss_counter) / mem_trace.size() << ", ";
			std::cerr << "Done!" << std::endl;
		}

		fout << std::endl;
	}

	// Simulate fully associative cache
	for ( auto cache_size_in_kb : cache_size_in_kb_table)
	{
		std::cerr << "Performing simulation for cache size " << cache_size_in_kb << "kb"
			<< " and full associativity" << std::endl;

		CacheTagArray cache( cache_size_in_kb * 1024, cache_size_in_kb * 1024 / 4, 4, 32);

		unsigned miss_counter = 0;
		for ( uint64 addr : mem_trace) 
		{
			if (!cache.read( addr)) 
			{
				cache.write( addr);
				miss_counter++;
			}
		}

		fout << std::fixed << double(miss_counter) / mem_trace.size() << ", ";
		std::cerr << "Done!" << std::endl;
	}

	fout << std::endl;
	fout.close();
}
