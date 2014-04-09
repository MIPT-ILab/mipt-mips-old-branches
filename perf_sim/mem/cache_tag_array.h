/*
 * cache_tag_array.h
 *
 */

#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <types.h>

class CacheTagArray
{
   
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
    CacheTagArray( uint32 size_in_bytes,
                   uint32 ways,
                   uint16 block_size_in_bytes, 
                   uint16 addr_size_in_bits);
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
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently Used)
     * policy.
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

	struct CacheLine {
		uint32 *tags;
		uint32 *stat;
	};
	CacheLine *cache_lines;

	uint64 getTag( uint64 addr);
	uint64 getIndex( uint64 addr);
	uint64 getOffset( uint64 addr);

	void updateStat(uint32 index, uint32 way);
};




#endif //CACHE_TAG_ARRAY_H

