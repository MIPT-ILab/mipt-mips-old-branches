/**
 * cache_tag_array.h - Header of CacheTagArray class
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyrignt 2013 uArchSim iLab project
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <types.h>

class CacheTagArray
{
    struct CacheLine
    {
         short age_bit;  // zero if line wasn't used recently, one if it was used 
         short used_bit; // zero if line is free, one - contains actual information
         uint64 tag;     // tag of stored data
    };
    unsigned short address_size_in_bits;
    unsigned short tag_size_in_bits;
    unsigned short index_size_in_bits;
    unsigned short offset_size_in_bits;
    unsigned int   number_of_ways;
    unsigned int   number_of_set;
    uint64         mask_for_index;
    uint64         mask_for_tag;
    CacheLine    **cache;
    
    uint64 getTag( uint64 addr); // returns tag of addr
    uint64 getIndex( uint64 addr); //returns index of addr
    unsigned short getLog( unsigned int data); // get binary log of data; data should be power of 2
    void checkInput( unsigned int data); // checks if data is correct input
    void updateLRU( uint64 index, unsigned int i); // update LRU info for set with index, i - used way
public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes = 4, 
                   unsigned short addr_size_in_bits = 32);
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     **/
    bool read( uint64 addr);
    /**
     * Write a byte with address addr into cache
     **/
    void write( uint64 addr);
    void dump();
};

#endif
