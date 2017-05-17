#include <iostream>
#include <fstream>

#include "cache_tag_array.h"

int main( int argc, char *argv[])
{
    std::ofstream out( "file.out");
    for( int j = 1; j <= 32; j <<= 1)
    {
        if ( j == 32)
        {
            j = 256;
        }
        std::cout << "associativity = " << j << std::endl;
        for( int i = 1; i <= 1024; i <<= 1)
        {
            std::ifstream in( argv[1]);
            std::cout << "size = " << i << " ";
            CacheTagArray cache = CacheTagArray( 1024*i, 
                                                 j*i, 4, 32);
            uint64 addr;
            int count = 0;
            int miss_count = 0;
            while( !in.eof()) 
            {
                in >> std::hex >> addr;
                count += 1;
                if ( !cache.read( addr))
                {
                    miss_count += 1.0;
                }
                if (count % 70000 == 0)
                {
                    std::cout << "-" << std::flush;
                }
            }
            std::cout << std::endl << "miss = " << miss_count 
                      << " count =  " << count << std::endl;
            std::cout << (1.0*miss_count)/(1.0*count) << std::endl;
            out << (1.0*miss_count)/(1.0*count) << ", ";
            in.seekg(in.beg);
            in.close();
        }
        out << std::endl;
    }
    return 0;
}
