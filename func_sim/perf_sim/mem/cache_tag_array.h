// Generic C/C++
#include <iostream>

// uArchSim modules
#include <types.h>

class CacheTagArray
{
    uint64 size;
    uint64 blocks;
    uint64 way_num;
    uint64 index_size;
    uint64 tag_size;
    uint64 offset_size;
    public:
        
        CacheTagArray( unsigned int size_in_bytes,
                       unsigned int ways,
                       unsigned short block_size_in_bytes,
                       unsigned short addr_size_in_bits);

        virtual ~CacheTagArray();

        uint64 get_tag( uint64 addr);
        uint64 get_index( uint64 addr);

        bool read( uint64 addr);
        void write( uint64 addr);
        void update_LRU_info( uint64 index, uint64 way);

        // Actual data field
        struct block
        {
            bool valid;
            uint64 tag;
            uint64 lru;
        };

        // Cache line
        struct CacheLine
        {
            block *line;
        };

        CacheLine *cache;
};
