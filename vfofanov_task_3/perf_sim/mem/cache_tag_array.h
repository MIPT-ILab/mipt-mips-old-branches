#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include "../../common/types.h"

using namespace std;

// Class for block.
class block_struct
{
    public:
// Tag, which written in block.
        uint64 tag;
// Value, which show, does block be valid or not valid.
        bool valid;
// Value, which show, as long ago was last handling to this block.       
        uint64 age;

        block_struct( )
        {
            valid = false;
            tag = 0;
            age = 0;
        } 
};

// Class for cache.
class CacheTagArray
{
    private:
// Pointer on array of sets.
        block_struct** cache;
// Size of all cache in bytes.
        uint64 cache_size;
// Number of sets in cache.
        uint64 sets_num;
// Number of ways in sets.
        uint64 ways_num;
// Size of block in bytes.
        uint64 block_size;
// Size of addres in bits.
        uint8 addr_size;
// Size of offset addres in bits.
        uint8 addr_size_of_offset;
// Size of index addres in bits.
        uint8 addr_size_of_index;
// Size of tag addres in bits.
        uint8 addr_size_of_tag;

    public:
// Constructor.
        CacheTagArray( unsigned int size_in_bytes,
                       unsigned int ways,
                       unsigned short block_size_in_bytes = 4,
                       unsigned short addr_size_in_bits = 32);
        ~CacheTagArray();
// Function for find addres in cache.
        bool read( uint64 addr);
// Function for write addres in cache.
        void write( uint64 addr);
};

// Function for calculate log2(x).
uint64 log2( uint64 value);
