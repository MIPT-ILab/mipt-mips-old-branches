#include <cassert>
#include "types.h"
using namespace std;

class CacheTagArray
{
    uint32 log2( uint32);
    uint32 sets;
    uint32 ways;
    uint32 index_size;
    uint32 offset_size;
    uint32 tag_size;
    struct CacheLine
    {
        bool v;
        uint64 tag;
        uint16 age;
    };
    CacheLine* cache;

public:
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);
    ~CacheTagArray();
    bool read( uint64 addr);
    void write( uint64 addr);
};
