/**
 * cache_tag_array.h - Header of a configurable model of a cache
 * with initial interfaces 
 * author Alexander Kravtcov
 */

// protection from multi-include
#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <vector>
#include <map>

// uArchSim modules
#include <types.h>

class CacheTagArray
{
    // an element of usage time list for using in Least Recently Used policy
    struct UsageTimeEl
    {
        unsigned int tag;
        struct UsageTimeEl* earlier;
        struct UsageTimeEl* later;
    };

    // contains list of all addresses having the same index,
    // the least recently used block and the most for each cache index
    struct IndexEl
    {
        // tag is a key value in map
        // and an element of usage time sequence is a mapped value
        std::map< unsigned int, UsageTimeEl*> tags;
        UsageTimeEl* least_recently;
        UsageTimeEl* most_recently;
    };

    unsigned int cache_size;
    unsigned int number_of_ways;
    unsigned short block_size;
    unsigned short addr_size;

    // lengths of parsed field at address
    unsigned short offset_size;
    unsigned short index_size;
    unsigned short tag_size;
    
    // fields for current used address for read and write methods
    unsigned int curr_offset;
    unsigned int curr_index;
    unsigned int curr_tag;

    unsigned int number_of_indexes;

    std::vector< IndexEl> indexes;

    // avoid call constructor without parameters
    CacheTagArray() {}
    
    // parse to offset, index, tag
    void parseAddress( uint64 addr);

    typedef std::map< unsigned int, UsageTimeEl*>::iterator MapIterator;

    void loadAddrIntoCache( MapIterator it, IndexEl* vec_elem);
    void makeMostRecently( MapIterator it, IndexEl* vec_elem);

public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored 
     *    in the cache, i.e. if the block size is 16 Bytes then 
     *    the number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, 
     *    i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);

    ~CacheTagArray();

    /*
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     */
    bool read( uint64 addr);

    void write( uint64 addr);
};

#endif

