#ifndef H_TAG_ARR
#define H_TAG_ARR

#define _AFX_SECURE_NO_WARNINGS
#define _ATL_SECURE_NO_WARNINGS

#include "types.h"

//return true if the number is a power of two, otherwise return false
inline bool IsTwoPower(const unsigned int A)
{
    return (A>0) && ((A&(A - 1)) == 0);
}


struct Cache
{
    bool *v;
    uint64 *tag;
    uint8 *data;
    unsigned int *was_used;
};


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
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);
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
    Cache cache_tag_array;        //cache tag array
    unsigned int number_of_ways;

    unsigned int tag_addr_size;
    unsigned int index_addr_size;
    unsigned int offset_addr_size;

    uint64 tag_mask;
    uint64 index_mask;

    uint64 MAX_ADDR;
    uint64 PC;

    //function for creating of cache
    void Create_cach(unsigned int size_in_bytes,
                     unsigned int ways,
                     unsigned short block_size_in_bytes,
                     unsigned short addr_size_in_bits);
};

#endif
