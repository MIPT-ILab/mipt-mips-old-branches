/**
 * miss_rate_sim.cpp - module for evaluating miss 
 * rates for various cache types.
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <stdlib.h>
#include <cstring>

// Generic C++
#include <iostream>
#include <iomanip>
#include <fstream>

// uArchSim modules
#include <types.h>
#include <cache_tag_array.h>

int main( int argc, char **argv)
{
    if( argc == 3) // Check of number of arguments
    {
        std::fstream input_fs( argv[1]);
        if( input_fs.fail())
        {
            std::cerr << "ERROR: can not open input file!" << std::endl;
            exit( EXIT_FAILURE);
        }
        input_fs << std::hex << std::showbase;

        std::fstream output_fs( argv[2]);
        if( output_fs.fail())
        {
            std::cerr << "ERROR: can not open output file!" << std::endl;
            exit( EXIT_FAILURE);
        }
        output_fs << std::setiosflags( std::ios::fixed) << std::setprecision( 6); 

        unsigned short block_size = 4;
        unsigned short addr_size = 32; 
        for( unsigned int ways = 1; ways <= 16; ways *= 2)
        {
            for( unsigned int size = 1024; size <= 1024 * 1024; size *= 2)
            {
                CacheTagArray cache( size, ways, block_size, addr_size);
                unsigned int misses = 0;
                unsigned int overall = 0;
                uint32 cur_addr = 0;
                while( input_fs.eof() == false)
                {
                    input_fs >> cur_addr;
                    if( cache.read( cur_addr) == false)
                    {
                        misses++;
                    }
                    overall++;
                }
                output_fs << ( ( double)misses / ( double)overall) << ", ";
                input_fs.seekg( 0, input_fs.beg);
            }
            output_fs << std::endl;
        }

        for( unsigned int size = 1024; size <= 1024 * 1024; size *= 2)
            {
                CacheTagArray cache( size, size / block_size, block_size, addr_size);
                unsigned int misses = 0;
                unsigned int overall = 0;
                uint32 cur_addr = 0;
                while( input_fs.eof() == false)
                {
                    input_fs >> cur_addr;
                    if( cache.read( cur_addr) == false)
                    {
                        misses++;
                    }
                    overall++;
                }
                output_fs << ( ( double)misses / ( double)overall) << ", ";
                input_fs.seekg( 0, input_fs.beg);
            }
            output_fs << std::endl;

        input_fs.close();
        output_fs.close();
    } else if( argc == 2 && !strcmp( argv[1], "--help"))
    {
        std::cout << "This program prints evaluated miss rates based on" << std::endl
                  << "file [input_file] to cvs file [output_file]." << std::endl
                  << "Usage: miss_rate_sim [input_file] [output_file]" << std::endl;
    } else
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl
                  << "Type \"" << argv[0] << " --help\" for usage." << std::endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}