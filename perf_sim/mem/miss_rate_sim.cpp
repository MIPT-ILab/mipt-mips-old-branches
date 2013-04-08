#include <iostream>
#include <fstream>

#include <vector>

#include <cassert>

#include <cache_tag_array.h>
#include <types.h>

int main( int argc, char* argv[])
{
    assert( argc == 2);

    std::vector< uint32> addresses;

    std::ifstream fin;
    fin.open( argv[ 1]);
    
    fin >> std::hex;
    for ( int i = 0; !fin.eof(); i++)
    {
        uint32 addr;
        fin >> addr;
        addresses.push_back( addr);
    }

    for ( int i = 1; i <= 1024; i *= 2)
    {
        unsigned int n_misses = 0;
        unsigned int count = 0;
        double miss_rate = 0.;

        CacheTagArray my_cache( 1024*i, 16, 4, 32);
    
        for ( count = 0; count < addresses.size(); count++)
        {
            n_misses += !my_cache.read( addresses[ count]);
            //std::cout << addr << " " << count << std::endl;
        }
    
        miss_rate = ( double)n_misses/ ( double)count;
        std::cout << "size " << i << " miss rate " << miss_rate << std::endl;
    }
    
    /*  
    my_cache.read( 0x2d0);
    my_cache.read( 0x2d0);
    my_cache.read( 0x6d0);
    my_cache.read( 0x6d0);
*/
    fin.close();
    std::cout << "worked successfully" << std:: endl;
    return 0;
}
