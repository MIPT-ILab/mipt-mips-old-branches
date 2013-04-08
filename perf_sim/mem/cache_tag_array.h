#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <vector>
#include <map>
//#include <pair>

#include <types.h>


class CacheTagArray
{
    struct UsageTimeEl
    {
        unsigned short tag;
        struct UsageTimeEl* earlier;
        struct UsageTimeEl* later;
    };

    // contains list of all addresses having the same index,
    // the least recently used block and the most for each cache index
    struct IndexEl
    {
        // tag is a key value in map
        // and an element of usage time sequence is a mapped value
        std::map< unsigned short, UsageTimeEl*> tags;
        UsageTimeEl* least_recently;
        UsageTimeEl* most_recently;
    };


    typedef std::map< unsigned short, UsageTimeEl*>::iterator MapIterator;
    typedef std::map< unsigned short, UsageTimeEl*>::const_iterator ConstMapIterator;

    unsigned int cache_size;
    unsigned int number_of_ways;
    unsigned short block_size;
    unsigned short addr_size;

    unsigned short offset_size; // length of offset in block, in bits
    unsigned short index_size; // length of index in address
    unsigned short tag_size; // length of tag in address 
    
    unsigned short curr_offset;
    unsigned short curr_index;
    unsigned short curr_tag;

    unsigned int number_of_indexes;

    std::vector< IndexEl> indexes;

    CacheTagArray() {}

    void parseAddress( uint64 addr);

public:
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);

    bool read( uint64 addr);

    void write( uint64 addr);
};

#endif
