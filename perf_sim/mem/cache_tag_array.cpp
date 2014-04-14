#include "cache_tag_array.h"

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
{
    assert( size_in_bytes);
    assert( ways);
    assert( block_size_in_bytes);
    assert( addr_size_in_bits);
    assert( size_in_bytes % block_size_in_bytes == 0);
    assert(( size_in_bytes / block_size_in_bytes) % ways == 0);

    sets = (size_in_bytes / block_size_in_bytes) / ways;
    index_size = log2( sets);
    offset_size = log2( (uint32)block_size_in_bytes);
    assert( addr_size_in_bits > ( index_size + offset_size));
    tag_size = addr_size_in_bits - index_size - offset_size;
    this->ways = ways;

    cache = new CacheLine [ ways * sets];
    for ( uint64 i = 0; i < ways * sets; i++)
    {
        cache[ i].v = 0;
    }
};

CacheTagArray::~CacheTagArray()
{
    delete[] cache;
};

bool CacheTagArray::read( uint64 addr)
{
    // Parsing addr
    uint64 addr0 = addr;
    addr = addr >> offset_size;
    uint64 index = addr & ( sets - 1); // sets - 1 == 2^index_size - 1
    addr = addr >> index_size;
    uint64 tag = addr;

    // Searching for equal tag, while updating LRU information
    bool flag = false;
    for ( uint32 i = 0; i < ways; i++)
    {
        uint64 iway = index + ( i << index_size); // <=> iway = index + i * sets;
        cache[ iway].age++;
        if (( cache[ iway].tag == tag) && cache[ iway].v)
        {
            cache[ iway].age = 0;
            flag = true;
        }
    }
    if ( flag)
    {
        return true;
    }
    else
    {
        // If not found in cache: download from memory (write in cache)
        write( addr0);
        return false;
    }
};

void CacheTagArray::write( uint64 addr)
{
    // Parsing addr
    addr = addr >> offset_size;
    uint32 index = addr & ( sets - 1); // sets - 1 == 2^index_size - 1
    addr = addr >> index_size;
    uint64 tag = addr;

    // Check if there are any free lines
    // Simultaneously runs LRU, which step in, if no free lines are found
    uint16 agemax = 0;
    uint32 iwaymax = 0;
    for ( uint32 i = 0; i < ways; i++)
    {
        uint64 iway = index + ( i << index_size);
        if ( !cache[ iway].v)
        {
            cache[ iway].v = true;
            cache[ iway].tag = tag;
            cache[ iway].age = 0;
            return;
        }
        if ( cache[ iway].age > agemax)
        {
            iwaymax = iway;
            agemax = cache[ iway].age;
        }
    }
    // We updated "age" in read method, and, since we call write from read,
    // we shouldn't update "age" in write method. Therefore, in an exceptional
    // situation, when we call write() ways+1 times, without calling read()
    // all ages will remain 0; LRU cycle will not go into if branch
    // imax will remain 0. In our task, we don't call write() separately from
    // read() and the cycle will always work correctly.
    cache[ iwaymax].tag = tag;
    cache[ iwaymax].age = 0;
    return;
};

uint32 CacheTagArray::log2( uint32 num)
{
    assert( num);
    uint8 flag = 0;
    uint32 result;
    for ( int i = 0; i < 32; i++)
    {
        if ( num & 1)
        {
            if ( !flag)
            {
                flag = 1;
                result = i;
            }
            else
            {
                uint8 log2_not_int = 0;
                assert( log2_not_int);
            }
        }
        num = num >> 1;
    }
    return result;
};
