#include "cache_tag_array.h"

// ----------------------------------------------------------------------------

// Constructor of CacheTagArray class.

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
{
// Initializate all object's field.
    this -> cache_size = ( uint64)size_in_bytes;

    this -> sets_num = ( uint64)( ( size_in_bytes / block_size_in_bytes) / ways);
    this -> ways_num = ( uint64)ways;
    this -> block_size = ( uint64)block_size_in_bytes;

    this -> addr_size = ( uint8)addr_size_in_bits;
    this -> addr_size_of_offset = ( uint8)log2( block_size_in_bytes);
    this -> addr_size_of_index = ( uint8)log2( sets_num);
    this -> addr_size_of_tag = ( uint8)( addr_size - addr_size_of_offset - addr_size_of_index);

// Allocate memory from cache.
    this -> cache = new block_struct* [sets_num];
    
    for( uint64 i = 0; i < sets_num; i++)
    {
        this -> cache[i] = new block_struct [ways_num];
// Set start value of age for all blocks.        
        for( uint64 j = 0; j < ways_num; j++)
        {
            this -> cache[i][j].age = j;
        } 
    }
}

// ----------------------------------------------------------------------------

// Destructor of CacheTagArray class.

CacheTagArray::~CacheTagArray()
{
    for( uint64 i = 0; i < sets_num; i++)
    {
        delete [] cache[i];
    }
    delete [] cache;
}

// ----------------------------------------------------------------------------

// Function, which tries to find memory addres in cache.

bool CacheTagArray::read( uint64 addr)
{
// Get addres tag and index.
    uint64 read_index = ( ( addr >> addr_size_of_offset) & ( ( 1 << addr_size_of_index) - 1));
    uint64 read_tag = ( addr >> ( addr_size_of_index + addr_size_of_offset));
    
    bool find_flag = false;
    uint64 find_block = ways_num;
// Tries to find block with specified index and tag.    
    for( uint64 i = 0; i < ways_num; i++)
    {
        if( ( cache[read_index][i].tag == read_tag) && ( cache[read_index][i].valid == true)) 
        {
// If block find, mark it.
            find_flag = true;
            find_block = i;
        }
    }
// If block find, update it age and return true.
    if( find_flag == true)
    {
        for( uint64 i = 0; i < ways_num; i++)
        {
            if( cache[read_index][i].age < cache[read_index][find_block].age)
            {
                cache[read_index][i].age++;
            }
        }
        
       cache[read_index][find_block].age = 0;
       return true;
    }
// Else write this block and return false.
    else
    {
        this -> write( addr);
        return false;
    }
}

// ----------------------------------------------------------------------------

// Function which write this addres in cache.

void CacheTagArray::write( uint64 addr)
{
// Get addres tag and index.
    uint64 write_index = ( ( addr >> addr_size_of_offset) & ( ( 1 << addr_size_of_index) - 1));
    uint64 write_tag = ( addr >> ( addr_size_of_index + addr_size_of_offset));
// Write addres in block and update it age.    
    for( int i = 0; i < ways_num; i++)
    {
        cache[write_index][i].age++;
    }
    for( int i = 0; i < ways_num; i++)
    {
        if( cache[write_index][i].age == ways_num)
        {
            cache[write_index][i].valid = true;
            cache[write_index][i].tag = write_tag;
            cache[write_index][i].age = 0;
        }
    }
}

// ----------------------------------------------------------------------------

// This function calculate log2(x), when x - argument.

uint64 log2( uint64 value)
{
    uint64 power = 0;

    while( value > 1)
    {
        value = value / 2;
        power++;
    }

    return power;
}

// ----------------------------------------------------------------------------
