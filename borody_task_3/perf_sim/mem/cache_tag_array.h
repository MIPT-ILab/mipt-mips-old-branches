/**
 * cache_tag_array.h - Header of module implementing the model of cache  
 * for performance simulation.
 * @author Dmitry Borody <dmitry.borody@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef MEM__CACHE_TAG_ARRAY_H
#define MEM__CACHE_TAG_ARRAY_H

// Generic C++
#include <vector>

// uArchSim modules
#include <types.h>

class CacheTagArray
{
  struct CacheBlock
  {
    bool valid;
    unsigned tag;
    unsigned last_access_time;
  };

public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in 
     *      the cache is size_in_bytes/16.
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are 
     *      referred by the same index. 
     * 3) block_size_in_bytes is a number of Bytes in a data block
     * 4) addr_size_in_bit is a number of bits in the physical address.
     *
     *   Constructor checks with assert() that addr_size_in_bits is large enough for
     * given size_in_bytes, ways and block_size_in_bytes (for details, see 
     * Wikipedia article): 
     * 
     *   http://en.wikipedia.org/wiki/CPU_cache#Cache_entry_structure
     * 
     */
    CacheTagArray( unsigned size_in_bytes,
                   unsigned ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     */
    bool read( uint64 addr);
    
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     */
    void write( uint64 addr);

private:
    unsigned short bits_for_index;
    unsigned short bits_for_offset;
    unsigned short bits_for_tag;

    unsigned ways;
    unsigned number_of_blocks;
    unsigned number_of_sets;

    unsigned mem_access_counter;

    std::vector< std::vector< CacheBlock>> sets;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H