/**
 * miss_rate_sim.cpp - Perfomance study cache.
 * Receiving miss rates on different cache configurations 
 * using a real memory request trace as a input workload
 * author Alexander Kravtcov
 */

#include <iostream>
#include <fstream>
#include <vector>

#include <cstdlib>
#include <cassert>
#include <cmath>

#include <cache_tag_array.h>
#include <types.h>

const unsigned short WIDTH_SCREEN = 78;
const unsigned short ONE_KB_IN_BYTES = 1024;
const unsigned short BLOCK_SIZE = 4;
const unsigned short ADDR_SIZE = 32;
const unsigned short FULL_ASSOCIATIVE = 32;

int main( int argc, char* argv[])
{
    const int num_of_args = 3;
   
    if ( argc != num_of_args)
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl;
        std::cout << "Usage: \"" << argv[0]
                  << " <addresses of cache request file>"
                  << " <file for print miss rate>\"" << std::endl; 
        exit( EXIT_FAILURE);

    } 

    std::ifstream fin( argv[ 1]);
    if ( !fin)
    {
        std::cerr << "file " << argv[ 1] << " was not opened" << std::endl;
        exit( EXIT_FAILURE);
    }

    std::ofstream fout( argv[ 2]);
    if ( !fout)
    {
        std::cerr << "file " << argv[ 2] << " was not opened" << std::endl;
        fin.close();
 
        exit( EXIT_FAILURE);
    }

    std::vector< uint32> addresses;

    fin >> std::hex;
    for ( int i = 0; !fin.eof(); i++)
    {
        uint32 addr;
        fin >> addr;
        addresses.push_back( addr);
    }

    if ( addresses.empty())
    {
        std::cerr << " there is no addresses in input file " << argv[ 1]
                  << std::endl;

        fin.close();
        fout.close();
        exit( EXIT_FAILURE);
    }
    
    for ( int n_ways = 1; n_ways <= FULL_ASSOCIATIVE; n_ways *= 2)
    {
        if ( n_ways == FULL_ASSOCIATIVE)
        {
            std::cout << "full associative" << std::endl;
        } else
        {
            std::cout << n_ways << " way" << std::endl;
        }

        for ( int num_of_KB = 1; num_of_KB <= 1024; num_of_KB *= 2)
        {
            unsigned int n_misses = 0;
            unsigned int count = 0;
            double miss_rate = 0.;
    
            std::cout << "  cache size " << num_of_KB << " KB" << std::endl 
                      << "  ";

            if ( n_ways == FULL_ASSOCIATIVE)
            {
                n_ways = ONE_KB_IN_BYTES * num_of_KB / BLOCK_SIZE;
            }

            CacheTagArray my_cache( num_of_KB * ONE_KB_IN_BYTES, n_ways, 
                                    BLOCK_SIZE, ADDR_SIZE);
        
            for ( count = 0; count < addresses.size(); count++)
            {
                n_misses += !my_cache.read( addresses[ count]);
                
                // show execute process
                if ( count % ( addresses.size() / WIDTH_SCREEN)
                     == WIDTH_SCREEN -1)
                {
                    std::cout << "-";
                    std::cout.flush();
                }
#if DEBUG
                uint32 tag = addresses[ count] >> 2;
                std::cout << std::endl 
                          << "vector size " << addresses.size() << std::endl
                          << "addr " << addresses[ count] << std::endl
                          << "tag " << tag << std::endl
                          << "count " << count << std::endl 
                          << "n_misses " << n_misses << std::endl;
                
#endif
            }
            
            miss_rate = ( double)n_misses / ( double)count;

            // format output stream
            fout.width( 8);
            fout.fill( '0');
            fout.precision( 5);
            fout.setf( std::ios::left);

            fout << miss_rate << ", ";
            std::cout << std::endl;
            std::cout << "  miss rate " << miss_rate << std::endl << std::endl;
        }
        fout << std::endl;
    }

    fin.close();
    fout.close();
 
    std::cout << argv[ 0] << " has finished work successfully" << std::endl;
    return 0;
}

