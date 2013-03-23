/**
 * miss_rate_sim.cpp - contains a program doing performance measurements
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyrignt 2013 uArchSim iLab project
 */

#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <cache_tag_array.h>

using namespace std;

int main( int argc, char* argv[])
{
    if ( argc != 3)
    {
        cout << "Error: need 2 arguments" << endl;
        exit( EXIT_FAILURE);
    }
    
    ifstream input( argv[ 1]);
    ofstream output( argv[ 2]);
    if ( !input)
    {
        cout << "Error: can't open file " << argv[ 1] << endl;
        exit( EXIT_FAILURE);
    }
    if ( !output)
    {
        cout << "Error: can't open file " << argv[ 2] << endl;
        exit( EXIT_FAILURE);
    }
    input >> hex;
    
    for ( int way = 1; way <= 16; way *= 2)
    {
        for ( int cache_size = 1; cache_size <= 1024; cache_size *= 2) // size of cache from 1kb to 1024 kb
        {
            CacheTagArray myCache( cache_size * 1024, way);
            input.clear();
            input.seekg( 0, input.beg);
            int request_number = 0;
            int miss_number = 0;
            int addr;       
            while ( input >> addr)
            {
                request_number++;
                if ( !myCache.read( addr))
                {
                    miss_number++;
                }
            }
            output << ( double)miss_number / request_number << ", ";
        }
        output << endl;
    }
    /* Full associative cache */
    for ( int cache_size = 1; cache_size <= 1024; cache_size *= 2) // size of cache from 1kb to 1024 kb
    {
        CacheTagArray myCache( cache_size * 1024, cache_size * 256);
        input.clear();
        input.seekg( 0, input.beg);
        int request_number = 0;
        int miss_number = 0;
        int addr;       
        while ( input >> addr)
        {
            request_number++;
            if ( !myCache.read( addr))
            {
                miss_number++;
            }
        }
        output << ( double)miss_number / request_number << ", ";
    }
    return 0;
}
