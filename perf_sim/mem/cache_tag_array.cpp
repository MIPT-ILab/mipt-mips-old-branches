/*
 * cache_tag_array.cpp - perfomance simulator for 1-level cache
 * @author Aleksandr Kayda aleksandr.kajda@frtk.ru
 * Copyright 2014 uArchSim iLab 
 */

#include <cache_tag_array.h>
#include <assert.h>

#define NOT_LOG2 0xff

/* Compute binary log, 
 * returns NOT_LOG2 if is not integer power.
 * Static non-member function. Hope this is not bad style.
 */
static uint8 log2( uint32 value)
{
    uint8 i = 0;
    assert( value != 0);
    while ( value != 1)
    {
        if ( ( value & 1) != 0)
            return NOT_LOG2;
        value >>= 1;
        i++;
    }
    return i;
}


CacheTagArray::CacheTagArray( uint32 size_in_bytes,
                                 uint32 ways,
                                 uint16 block_size_in_bytes, 
                                uint16 addr_size_in_bits)
{
    cache_size = size_in_bytes;
    ways_num = ways;
    block_size = block_size_in_bytes;
    addr_size = addr_size_in_bits;

    assert( cache_size != 0);
    assert( block_size != 0);
    assert( addr_size != 0);
    assert( addr_size <= 64);
    if ( ways_num == 0)                 // it means full associativity
        ways_num = cache_size / block_size;

    offset_bits = log2( block_size);
    assert( offset_bits != NOT_LOG2);
    index_num = cache_size / ways_num / block_size;
    assert( index_num != 0);
    index_bits = log2( index_num);         // Only power of two is permitted
    assert( index_bits != NOT_LOG2);
    tag_bits = addr_size - offset_bits - index_bits;
    assert( tag_bits >= 0);

    cache_lines = new CacheLine [index_num];
    for ( uint32 i = 0; i < index_num; i++)
    {
        cache_lines[i].tags = new uint32 [ways_num]();
        cache_lines[i].stat = new uint32 [ways_num]();
    }
    access_count = 0;
}


CacheTagArray::~CacheTagArray()
{
    for ( uint32 i = 0; i < index_num; i++)
    {
        delete cache_lines[i].tags;
        delete cache_lines[i].stat;
    }
    delete cache_lines;
}


/* Access into cache by addr.
 * Return true if hit, else - miss.
 */
bool CacheTagArray::read( uint64 addr)
{
    uint64 index = getIndex( addr);
    uint64 tag = getTag( addr);
    // search tag hit
    for ( uint32 i = 0; i < ways_num; i++)
        if ( cache_lines[index].tags[i] == tag)
        {
            updateStat( index, i);
            return true;
        }
    write( addr);
    return false;
}

/* Load address into cache in oldest way ( LRU)
 * update LRU stat
 */
void CacheTagArray::write( uint64 addr)
{
    uint64 index = getIndex( addr);
    uint64 tag = getTag( addr);
    uint64 oldest_access_time = -1;         // uint64_max
    uint32 oldest_way_num = 0;
    for ( uint32 i = 0; i < ways_num; i++)
        if ( cache_lines[index].stat[i] < oldest_access_time)
        {
            oldest_access_time = cache_lines[index].stat[i];
            oldest_way_num = i;
        }
    cache_lines[index].tags[oldest_way_num] = tag;
    updateStat( index, oldest_way_num);
}


uint64 CacheTagArray::getTag( uint64 addr)
{
    return addr >> ( offset_bits + index_bits);
}


uint64 CacheTagArray::getIndex( uint64 addr)
{
    return ( addr >> offset_bits) & (( 1 << index_bits) - 1);
}


uint64 CacheTagArray::getOffset( uint64 addr)
{
    return addr & (( 1 << offset_bits) - 1);
}


/* Update stat for given index, set given way access time
 */
void CacheTagArray::updateStat( uint32 index, uint32 way)
{
    cache_lines[index].stat[way] = ++access_count;
}


