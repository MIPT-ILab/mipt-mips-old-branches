#ifndef CACHE_TAG_ARRAY
#define CACHE_TAG_ARRAY

//uArch Headrs
#include "types.h"

//Constants
const uint32 FUL_AS = MAX_VAL32;
const uint8 BIT_IN_BYTE = 8;

struct blockCache {
	bool valid;
	uint64 ltu;//last time used
	uint32 tag;
};

//************************************************************************

class CacheTagArray
{
private:  
	uint32 size_in_bytes;
	uint32 ways;
	uint16 block_size_in_bytes;
	uint16 addr_size_in_bits;

	uint32 offsetInBlock;//size in bit offset in block 
	uint32 bitForIndex;
	uint32 maxIndex;

	uint32 maskIndex;
	uint32 maskTag;
	
	blockCache **cache;		
public:
	void dumpInfo ();
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
    	CacheTagArray ( uint32 size_in_bytes,
                   uint32 ways,
                   uint16 block_size_in_bytes, 
                   uint16 addr_size_in_bits );

       ~CacheTagArray();
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
	bool read ( uint32 addr, uint64 PC );
    
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently Used)
     * policy.
     */
    int write ( uint32 addr, uint64 PC );
	//PC - program counter for LRU
	//PC will contain in elemCache.ltu
};

#endif
