/*
 * cache_tag_array.h - perfomance simulator for 1-level cache
 * @author Aleksandr Kayda aleksandr.kajda@frtk.ru
 * Copyright 2014 uArchSim iLab 
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <types.h>

class CacheTagArray
{
   
public:
    /**
     * Constructor params:
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     * 2) ways is a number of associative ways in a set
     * 3) block_size_in_bytes is a number of Bytes in a data block
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( uint32 size_in_bytes,
                   uint32 ways,
                   uint16 block_size_in_bytes, 
                   uint16 addr_size_in_bits);
    ~CacheTagArray();
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);
    
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache accordinf to the LRU policy.
     */
    void write( uint64 addr);

private:
    uint32 cache_size;
    uint32 ways_num;
    uint16 block_size;
    uint16 addr_size;

    uint8 offset_bits;
    uint8 index_bits;
    uint8 tag_bits;

    uint32 index_num;

    struct CacheSet {
        uint32 *tags;
        uint32 *stat;
        uint8 *valid;
    };
    CacheSet *cache_sets;

    uint64 getTag( uint64 addr);
    uint64 getIndex( uint64 addr);
    uint64 getOffset( uint64 addr);

    uint64 access_count;
    void updateStat( uint32 index, uint32 way);
};




#endif //CACHE_TAG_ARRAY_H

