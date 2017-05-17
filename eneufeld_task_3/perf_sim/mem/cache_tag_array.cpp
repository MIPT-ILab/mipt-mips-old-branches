#include <vector>
#include <iostream>
#include "cache_tag_array.h"

int lb( int n)
{
    int result = 0;
    while (n != 0)
    {
        n /= 2;
        result++;
    }
    return --result;
}
    
CacheSet::CacheSet( unsigned int set_size_in_ways,
                    unsigned short tag_offset_in_bytes)
{
    set_size = set_size_in_ways;
    tag_offset = tag_offset_in_bytes;
    
    tags.reserve (set_size);
    
    using_time.reserve (set_size);
    for( int i = 0; i < set_size; i++) 
    {
        tags[ i] = 0;
        using_time[ i] = 0;
    }
}

bool CacheSet::read( uint64 addr, unsigned int time)
{
    unsigned int tag = addr >> tag_offset;
    for( int i = 0; i < set_size; i++) 
    {
        if( tags[ i] == tag && using_time[ i] != 0) 
        {
            using_time[ i] = time;
            return true;
        }
    }
    write( addr, time);
    return false;
}

void CacheSet::write( uint64 addr, unsigned int time)
{
    unsigned int tag = addr >> tag_offset;
    unsigned int lru = using_time[ 0];
    unsigned int lru_num = 0;
    for( int i = 0; i < set_size; i++)
    {
        if( using_time[ i] < lru)
        {
            lru_num = i;
            lru = using_time[ i];
        }
    }
    tags[ lru_num] = tag;
    using_time[ lru_num] = time;
}

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes, 
                              unsigned short addr_size_in_bits)
{
    cache_size = size_in_bytes;
    block_size = block_size_in_bytes;
    addr_size = addr_size_in_bits;
    ways_count = ways;

    set_count = cache_size / ( ways_count * block_size);
    offset_size = lb( block_size);
    index_size = lb( set_count);

    time = 1;

    sets.reserve( set_count);
    for( int i = 0; i < set_count; i++)
    {
        sets[ i] = new CacheSet( ways, offset_size+index_size);
    }
}

CacheTagArray::~CacheTagArray( )
{
    for( int i = 0; i < set_count; i++)
    {
        delete sets[ i];
    }
}
          
bool CacheTagArray::read( uint64 addr)
{
    unsigned int index = ( addr >> offset_size) % ( 1 << index_size);
    time++;
    return sets[ index]->read( addr, time);
}

void CacheTagArray::write( uint64 addr)
{
    unsigned int index = ( addr >> offset_size) % ( 1 << index_size);
    time++;
    sets[ index]->write( addr, time);
}
    
