// Generic C/C++
#include <iostream>
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>

// uArchSim modules
#include <types.h>
#include <cache_tag_array.h>

using namespace std;

uint64 read_mem( std::string s)
{
    uint64 addr;
    stringstream str;
    s = s.substr( 2);
    std::cout << s << std::endl;
    str << s;
    str >> std::hex >> addr; 

    std::cout << addr << std::endl;
    return addr;
}

int main( int argc, char* argv[])
{
    const int num_of_args = 2;

    if( argc - 1 != num_of_args)
    {
        std::cerr << "ERROR: wrong number of arguments!" << std::endl
                  << "miss_rate_sim takes two args:" << std::endl
                  << "names of input and output files" << std::endl;
        exit( EXIT_FAILURE);
    }

    CacheTagArray cache_array( 1024, 2, 4, 32);

    // open files
    ifstream in( argv[ 1]);
    ofstream out( argv[ 2]);

    // check for correctness
    if( !in.is_open())
    {
        std::cerr << "ERROR: unable to open " << argv[ 1] << " file" << std::endl;
        exit( EXIT_FAILURE); 
    }

    if( !out.is_open())
    {
        std::cerr << "ERROR: unable to open " << argv[ 2] << " file" << std::endl;
        exit( EXIT_FAILURE); 
    }

    // set counters
    uint64 total_requests = 0;
    uint64 misses = 0;

    string temp;
    in >> temp;
    while( !in.eof())
    {
        total_requests++;
        
        std::cout << temp << std::endl;
        if( !cache_array.read( read_mem( temp)))
            misses++;

        in >> temp;
    }

    std::cout << "total requests : " << total_requests << std::endl
              << "misses : " << misses << std::endl;

    out << total_requests << " " << misses;

    in.close();
    out.close();

    return 0;
}
