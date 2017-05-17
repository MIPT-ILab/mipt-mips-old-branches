// Generic C/C++
#include <iostream>
#include <math.h>
#include <stdlib.h>

// Generic uArcSim modules
#include <types.h>
#include <cache_tag_array.h>

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
{
    way_num = ways;
    size = size_in_bytes / block_size_in_bytes;

    std::cout << "size = " << size << std::endl;
    std::cout << "blocks = " << blocks << std::endl;
    
    // get index, tag and offset size
    index_size = uint64( log2( ( double)size));
    offset_size = uint64( log2(( double)ways));
    tag_size = addr_size_in_bits - index_size - offset_size;


    std::cout << "index_size = " << index_size << std::endl
        << "offset_size = " << offset_size << std::endl
        << "tag_size = " << tag_size << std::endl;

    // check for wrong args
    if( index_size + offset_size > addr_size_in_bits)
    {
        std::cerr << "ERROR: wrong arguments!" << std::endl;
        exit( EXIT_FAILURE);
    }

    // allocating memory for cache
    cache = new CacheLine [ size];
    for( uint64 i = 0; i < size; i++)
    {
        cache[ i].line = new block [ ways];
        // set all data invalid
        for( uint64 j = 0; j < ways; j++)
        {
            cache[ i].line[ j].valid = 0;
        }
    }

    std::cout << "------------------------------" << std::endl
              << "Memory allocated SUCCESSFULLY!" << std::endl;
}

CacheTagArray::~CacheTagArray()
{
    // deleting memory
    for( uint64 i = 0; i < this->size; i++)
    {
        delete [] cache[ i].line;
    }
    delete [] cache;

    std::cout << "------------------------------" << std::endl
              << "Memory deleted SUCCESSFULLY!" << std::endl;
}

void CacheTagArray::update_LRU_info( uint64 index, uint64 way)
{
    for( uint64 i = 0; i < this->way_num; i++)
    {
        cache[ index].line[ i].lru++;
    }
    cache[ index].line[ way].lru = 0;

    return;
}

uint64 CacheTagArray::get_index( uint64 addr)
{
    uint64 mask = 0;
    uint64 index;

    // creating bit mask
    for( uint64 i = 0; i < this->index_size; i++)
    {
        mask = mask << 1;
        mask++;
    }
    mask = mask << this->offset_size;
    std::cout << "index mask : " << mask << std::endl;

    index = ( addr & mask) >> this->offset_size;
    std::cout << "index in get function = " << index << std::endl;
    return index;
}

uint64 CacheTagArray::get_tag( uint64 addr)
{
    uint64 mask = 0;
    uint64 tag;

    std::cout << this->tag_size << std::endl;
    // creating bit mask
    for( uint64 i = 0; i < this->tag_size; i++)
    {
        mask = mask << 1;
        mask++;
    }
    std::cout << "tag mask : " << mask << std::endl;
    mask = mask << ( this->offset_size + this->index_size);

    std::cout << "tag mask : " << mask << std::endl;
    tag = ( addr & mask) >> ( this->offset_size + this->index_size);
    return tag;
}

bool CacheTagArray::read( uint64 addr)
{
    uint64 reqIndex = get_index( addr);
    uint64 reqTag = get_tag( addr);

    std::cout << "reqIndex = " << reqIndex << std::endl
        << "reqTag = " << reqTag << std::endl;

    for( uint64 i = 0; i < this->way_num; i++)
    {
        std::cout << "read way : " << i << std::endl;
        // cache hit if data is valid and tag is right
        if( cache[ reqIndex].line[ i].valid && ( reqTag == cache[ reqIndex].line[ i].tag))
        {
            std::cout << "Cache hit!" << std::endl;
            update_LRU_info( reqIndex, i);

            return true;
        }
    }

    // cache miss, writing to cache
    std::cout << "Cache miss!" << std::endl;
    write( addr);
    return false;
}

void CacheTagArray::write( uint64 addr)
{
    uint64 writeIndex = get_index( addr);
    uint64 writeTag = get_tag( addr);
    uint64 is_lru = 0;
    uint64 to_replace;
    
    for( uint64 i = 0; i < this->way_num; i++)
    {

        if( !cache[ writeIndex].line[ i].valid)
        {
            cache[ writeIndex].line[ i].valid = true;
            cache[ writeIndex].line[ i].tag = writeTag;
            update_LRU_info( writeIndex, i);
            return;
        }
        if( cache[ writeIndex].line[ i].lru > is_lru)
        {
            is_lru = cache[ writeIndex].line[ i].lru;
            to_replace = i;
        }
    }
    std::cout << "No invalid data" << std::endl;

    cache[ writeIndex].line[ to_replace].tag = writeTag;
    update_LRU_info( writeIndex, to_replace);

    return;
}
