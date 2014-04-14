#ifndef _CAHCHE_TAG_ARRAY_
#define _CAHCHE_TAG_ARRAY_

using namespace std;
#include <iostream>

#include <types.h>

struct CacheBlock // This structure saves
{                 // cache block meta-data
    uint64 tag;     
    uint8 valid;
    uint64 last_use_time;
};

struct CacheSet // This structure saves
{               // pointers to cache blocs meta-data
    CacheBlock* cache_block;
};

class CacheTagArray
{
private:
    // some useful values
    uint64 hits;    // Used for counting
    uint64 misses;  // of all misses and hits
    uint64 ways;
    uint64 size_in_bytes;
    uint64 block_size_in_bytes;
    uint64 addr_size_in_bits;
    uint64 tag_size; 
    uint64 index_size;
    uint64 offset_size;
    uint64 tag_size_in_bits; 
    uint64 index_size_in_bits;
    uint64 offset_size_in_bits;
    
    CacheSet* cache_sets;
    
    uint64 operation_number;    // used to identify oldest transaction

public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache.
     *
     * 2) ways is a number of associative ways in a set.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( uint64 size_in_bytes,
                   uint64 ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);
    ~CacheTagArray();
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     */
    void write( uint64 addr);
    double GetHitRate();    
    double GetMissRate();    
    
    private:
    //  some useful values and functions
    uint64 offset_mask;
    uint64 tag_mask;
    uint64 index_mask;
    uint64 get_index_from_addr(uint64 address);
    uint64 get_tag_from_addr(uint64 address);
    uint64 get_offset_from_addr(uint64 address);
};

#endif // ifndef _CAHCHE_TAG_ARRAY_

