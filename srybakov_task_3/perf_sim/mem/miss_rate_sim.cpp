#include "cache_tag_array.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>


int main (int argc, char* argv[])
{
    const int num_of_args = 2;
    if ( argc - 1 == num_of_args)
    {
        ifstream input;
        ofstream output;
        input.open(argv[1]);
        output.open(argv[2]);
        if ( input.is_open())
        {
            if ( output.is_open())
            {
                // Main program
                input >> hex;
                uint64 total= 0;
                uint64 hits;
                uint64 misses;
                uint64 addr;
                float64 missrate;
                while ( input >> addr)
                {
                    total++;
                }
                input.clear();
                input.seekg(0, ios::beg);// Carriage return
                uint32 ways = 1;
                for (uint8 i = 0; i < 6; i++)
                {
                    uint32 cachesize = 1024;
                    for ( uint8 j = 0; j < 11; j++)
                    {
                        if ( i == 5)
                        {
                            ways = cachesize / 4;
                        }
                        hits = 0;
                        CacheTagArray cache( cachesize, ways, 4, 32);
                        while ( input >> addr)
                        {
                            hits += cache.read( addr);
                        }
                        misses = total - hits;
                        missrate = (double)misses / total;
                        output << missrate;
                        output << ", ";
                        input.clear();
                        input.seekg(0, ios::beg);// Carriage return
                        cachesize = cachesize << 1;
                    }
                    cout << "Finished " << ways << "-way" << endl;
                    ways = ways << 1;
                    output << endl;
                }
                input.close();
                output.close();
                return 0;
            }
            else
            {
                cerr << "Incorrect output file name" << endl;
                exit( EXIT_FAILURE);
            }

        }
        else
        {
            cerr << "Incorrect input file name" << endl;
            exit( EXIT_FAILURE);
        }
    }
    cerr << "Incorrect number of arguments" << endl
    << "Input file with memory addresses and .csv output file" << endl;
    exit( EXIT_FAILURE);
}
