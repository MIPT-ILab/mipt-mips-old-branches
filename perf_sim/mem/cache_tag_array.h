#include <vector>
#include "../../common/types.h"
class CacheSet
{
    unsigned int set_size;
    std::vector <unsigned int> tags;
    std::vector <unsigned int> using_time;
    unsigned short tag_offset;

    unsigned int llru;

public:
    CacheSet( unsigned int set_size_in_ways,
              unsigned short tag_offset_in_bytes);
    bool read( uint64 addr, unsigned int time);
    void write( uint64 addr, unsigned int time);
}; 

class CacheTagArray
{
    unsigned int cache_size;
    unsigned short block_size;
    unsigned short addr_size;
    unsigned int ways_count;

    unsigned int set_count;
    unsigned short offset_size;
    unsigned short index_size;

    unsigned int time;

    std::vector <CacheSet *> sets;

public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored 
     *    in the cache, i.e. if the block size is 16 Bytes then the 
     *    number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, i.e. how many 
     *    blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);
    ~CacheTagArray( );
    /**
     * Return true if the byte with the given address is stored in the 
     * cache, otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);
    
    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is 
     * needed to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently 
     * Used) policy.
     */
    void write( uint64 addr);
};
