/**
 * cache_tag_array.h - Header of module implementing the concept of 
 * a real tag array in a CPU cache.
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef CACHE_TAG_ARRAY__CACHE_TAG_ARRAY
#define CACHE_TAG_ARRAY__CACHE_TAG_ARRAY

// Generic C++
#include <string>
#include <vector>
#include <set>

// Generic uarch-sim
#include <types.h>

class CacheTagArray
{
    unsigned int num_of_ways; //a number of associative ways in a set
    unsigned int tag_len; //a number of bits in a tag of block
    unsigned int index_len; //a number of bits in an index of set
    unsigned int offset_len; //a number of bits in a offset of block
    std::vector< std::set< unsigned int> > tag_array; //set of tags inside an array of sets;
    
    //You could not create object using this default constructor
    CacheTagArray();

public:
    /**
     * Constructor create object which contains empty array of tags
     * You can write in or read from this array.
     *
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);
    
    unsigned int indexOf( uint64 addr) const; //return index for this address
    unsigned int tagOf( uint64 addr) const; //return tag for this address
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
};

#endif //CACHE_TAG_ARRAY__CACHE_TAG_ARRAY

