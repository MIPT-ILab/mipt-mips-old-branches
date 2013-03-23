/**
 * cache_tag_array.cpp - the module implementing the concept of
 * a real tag array in a CPU cache.
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cmath> //link with -lm

// Generic C++
#include <string>
#include <vector>
#include <algorithm>

// uArchSim modules
#include <cache_tag_array.h>

using namespace std;

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
               unsigned int ways,
               unsigned short block_size_in_bytes, 
               unsigned short addr_size_in_bits)
{
    assert( log2( size_in_bytes) == int( log2( size_in_bytes)));
    assert( log2( ways) == int( log2( ways)));
    assert( log2( block_size_in_bytes) == int( log2( block_size_in_bytes)));
    assert( log2( addr_size_in_bits) == int( log2( addr_size_in_bits)));
    this->num_of_ways = ways; //we need this number in write func

    unsigned int num_of_set = size_in_bytes / ( ways * block_size_in_bytes);
    assert( num_of_set > 0);
    this->index_len  = log2( num_of_set);
    this->offset_len = log2( block_size_in_bytes);
    this->tag_len    = addr_size_in_bits - index_len - offset_len;
    this->tag_array.resize( num_of_set); //create sets
}

unsigned int CacheTagArray::indexOf( uint64 addr) const
{
    int shift =  sizeof( addr) * 8 - this->index_len - this->offset_len;
    return ( addr << shift >> ( shift + this->offset_len)); 
}

unsigned int CacheTagArray::tagOf( uint64 addr) const
{
    return addr >> ( this->index_len + this->offset_len);   
}

bool CacheTagArray::read( uint64 addr)
{
    bool is_finded = false;
    unsigned int tag   = tagOf( addr);
    unsigned int index = indexOf( addr);
    list< unsigned int> this_set = tag_array[ index]; 
    list< unsigned int>::iterator to_remove = find( this_set.begin(), this_set.end(), tag);
    if ( to_remove != this_set.end())
    {
        remove( to_remove, to_remove, tag);
        this_set.push_front( tag);
        is_finded = true;
    } else
    {
        write( addr);
    } 
    return is_finded;
}

void CacheTagArray::write( uint64 addr)
{
    unsigned int tag   = tagOf( addr);
    unsigned int index = indexOf( addr);

    if ( tag_array[ index].size() == this->num_of_ways)
    {
        tag_array[ index].pop_back();
    } 
    tag_array[ index].push_front( tag);
} 

