/*
 * miss_rate_sim.cpp - perfomance simulator for 1-level cache
 * @author Aleksandr Kayda aleksandr.kajda@frtk.ru
 * Copyright 2014 uArchSim iLab 
 */

// C++ headers
#include <iostream>
#include <fstream>
#include <cstring>
// pure C headers
#include <errno.h>
#include <cstdlib>
// our headers
#include <types.h>
#include <cache_tag_array.h>


using namespace std;

int main( int argc, char* argv[])
{
    if ( argc != 2){
        cout << "need exactly 1 argument: name of file with addresses" << endl;
        exit( EXIT_FAILURE);         // is this good style for C++ ?
    }

    ifstream addr_file;
    addr_file.open( argv[1]);
    if ( !addr_file.is_open())
    {
        cerr << "Invalid filename: " << strerror( errno) << endl;
        exit( EXIT_FAILURE);
    }
    uint32 trace_size = 0;
    uint32 address;
    while ( addr_file >> hex >> address)
        trace_size++;
    if ( !addr_file.eof())
    {
        cerr << "Error reading file: " << strerror( errno) << endl;
        exit( EXIT_FAILURE);
    }
    addr_file.clear();
    addr_file.seekg( 0);
    uint32 trace_addr[trace_size];
    for ( uint32 i = 0; i < trace_size; i++)
        addr_file >> hex >> trace_addr[i];
    addr_file.close();

    uint32 way_set[] = { 1, 2, 4, 8, 16, 0};
    for ( uint32 i = 0; i < 6; i++)
    {
        for ( uint32 j = 1024; j <= 1024*1024; j <<= 1)
        {
            CacheTagArray cache( j, way_set[i], 4, 32);
            uint32 misses_count = 0;
            uint32 access_count = 0;
            for ( uint32 k = 0; k < trace_size; k++)
                if ( !cache.read( trace_addr[k]))
                    misses_count++;
            float miss_rate = ( float) misses_count / trace_size;
            cout << miss_rate << ", ";
        }
        cout << endl;
    }

    return 0;
}

