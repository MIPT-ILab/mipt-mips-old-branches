/**
 * cache_tag_array.cpp - Implementation of CacheTagArray class
 * @author Elena Limonova <elena.e.limonova@gmail.com>
 * Copyrignt 2013 uArchSim iLab project
 */

#include <assert.h>

#include <iostream>

#include <cache_tag_array.h>

using namespace std;

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes, 
                              unsigned short addr_size_in_bits)
{
    checkInput( size_in_bytes);
    checkInput( ways);
    checkInput( block_size_in_bytes);
    checkInput( addr_size_in_bits);

    number_of_ways       = ways;
    number_of_set        = size_in_bytes / ( ways * block_size_in_bytes);
    address_size_in_bits = addr_size_in_bits;
    index_size_in_bits   = getLog( number_of_set);
    offset_size_in_bits  = getLog( block_size_in_bytes);
    tag_size_in_bits     = addr_size_in_bits - index_size_in_bits - offset_size_in_bits;
    mask_for_index       = ( ( 1 << index_size_in_bits) - 1) << offset_size_in_bits;
    mask_for_tag         = ( ( 1 << tag_size_in_bits) - 1) << ( offset_size_in_bits + index_size_in_bits);
    assert( tag_size_in_bits > 0);
    
    cache = new CacheLine* [ number_of_set];
    for ( unsigned int i = 0; i < number_of_set; i++)
    {
        cache[ i] = new CacheLine [ number_of_ways];
        for ( unsigned int j = 0; j < number_of_ways; j++)
        {
            cache[ i][ j].age_bit  = 0;
            cache[ i][ j].used_bit = 0;
            cache[ i][ j].tag      = 0;
        }
    }   
}

bool CacheTagArray::read( uint64 addr)
{
    uint64 index       = getIndex( addr);
    uint64 current_tag = getTag( addr);
    for ( unsigned int i = 0; i < number_of_ways; i++)
    {
        if ( cache[ index][ i].used_bit && cache[ index][ i].tag == current_tag)
        {
            updateLRU( index, i);
            return true;
        }
    }
    write( addr); // byte is not in the cache, load it and write
    return false;
}

void CacheTagArray::write( uint64 addr)
{
    uint64 index       = getIndex( addr);
    uint64 current_tag = getTag( addr);
    /* Check for free places*/
    for( unsigned int i = 0; i < number_of_ways; i++)
    {
        if ( cache[ index][ i].used_bit == 0)
        {
            cache[ index][ i].used_bit = 1;
            cache[ index][ i].tag      = current_tag;
            updateLRU( index, i);
            return;
        }
    }
    /* No free places, find LRU line and replace it */
    for( unsigned int i = 0; i < number_of_ways; i++)
    {
        if ( cache[ index][ i].age_bit == 0)
        {
            cache[ index][ i].used_bit = 1;
            cache[ index][ i].tag      = current_tag;
            updateLRU( index, i);
            return;
        }
    }
    cache[ index][ 0].used_bit = 1;
    cache[ index][ 0].tag      = current_tag;
    updateLRU( index, 0);
    return;
}

uint64 CacheTagArray::getIndex( uint64 addr)
{
    return ( addr & mask_for_index) >> offset_size_in_bits;
}

uint64 CacheTagArray::getTag( uint64 addr)
{
    return ( addr & mask_for_tag) >> ( offset_size_in_bits + index_size_in_bits);
}

void CacheTagArray::checkInput( unsigned int data)
{
    assert( data > 0); 
    getLog( data);
}

unsigned short CacheTagArray::getLog( unsigned int data)
{
    for ( unsigned short i = 0; i < 8 * sizeof( unsigned int); i++)
    {
        if ( ( ( 1 << i) ^ data) == 0)
        {
            return i;
        }
    }
    assert( 0); // data needs to be power of 2
}

void CacheTagArray::updateLRU( uint64 index, unsigned int i)
{
    for( unsigned int j = 0; j < number_of_ways; j++)
    {
        cache[ index][ j].age_bit = 0;
    }
    cache[ index][ i].age_bit = 1;
    return;
}

void CacheTagArray::dump()
{
    cout << "Dump info: " << endl;
    cout << "offset_size = " << offset_size_in_bits << endl;
    cout << "index_size = " << index_size_in_bits << endl;
    cout << "tag_size = " << tag_size_in_bits << endl;
    for( unsigned int i = 0; i < number_of_set; i++)
    {
        cout << " index = " << i << endl;
        for ( unsigned int j = 0; j < number_of_ways; j++)
        {
            cout << "    way[" << j << "] = " << cache[ i][ j].age_bit << " " << cache[ i][ j].age_bit 
                 << " " << cache[ i][ j].tag << endl;
        }
    }
    return;
}
