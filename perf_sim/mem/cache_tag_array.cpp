#include <cassert>
#include <cmath>

#include "cache_tag_array.h"
#include <iostream>
#include <bitset>

const unsigned short BYTE_SIZE = 8;
const unsigned short BITSET_SIZE = 32;

CacheTagArray::CacheTagArray(  unsigned int size_in_bytes,
                               unsigned int ways,
                               unsigned short block_size_in_bytes,
                               unsigned short addr_size_in_bits)
{

    assert( block_size_in_bytes > 0);
    assert( size_in_bytes >= block_size_in_bytes);
    assert( ways > 0);
    assert( addr_size_in_bits > 0);
    
    assert( size_in_bytes % block_size_in_bytes == 0);
    assert( ( size_in_bytes / block_size_in_bytes) % ways == 0);

    // power of 2 check
    assert( ( size_in_bytes & ( size_in_bytes - 1)) == 0);
    assert( ( ways & ( ways - 1)) == 0);
    assert( ( block_size_in_bytes & ( block_size_in_bytes - 1)) == 0);
    assert( ( addr_size_in_bits & ( addr_size_in_bits - 1)) == 0);


    this->cache_size = size_in_bytes;
    this->number_of_ways = ways;
    this->block_size = block_size_in_bytes;
    this->addr_size = addr_size_in_bits;

    this->number_of_indexes = size_in_bytes / block_size_in_bytes / ways;
    
    this->index_size = ( unsigned short)logb( ( double)this->number_of_indexes);
    this->offset_size = ( unsigned short)logb( ( double)block_size_in_bytes);
    this->tag_size = addr_size_in_bits - index_size - offset_size;

    this->indexes = std::vector< IndexEl>( this->number_of_indexes);

#if DEBUG
/*
    std::cout << "offset_size " << this->offset_size << std::endl
              << "index_size  " << this->index_size << std::endl
              << "tag_size    " << this->tag_size << std::endl;
    std::cout << "---------------------------- " << std::endl;
    */
#endif
}

void CacheTagArray::parseAddress( uint64 addr)
{
    std::bitset< BITSET_SIZE> addr_bits( ( unsigned long)addr);
    std::bitset< BITSET_SIZE> offset_bits( 0);
    std::bitset< BITSET_SIZE> index_bits( 0);
    std::bitset< BITSET_SIZE> tag_bits( 0);

    // parse address addr to offset, index and tag
    // low bit is placed on the left in bitset
    for ( int i = 0; i < this->offset_size; i++)
    {
        offset_bits[ i] = addr_bits[ i];
    }
    for ( int i = 0; i < this->index_size; i++)
    {
        index_bits[ i] = addr_bits[ i + this->offset_size];
    }
    for ( int i = 0; i < this->tag_size; i++)
    {
        tag_bits[ i] = addr_bits[ i + this->index_size + this->offset_size];
    }

    this->curr_offset = offset_bits.to_ulong();
    this->curr_index = index_bits.to_ulong();
    this->curr_tag = tag_bits.to_ulong();

#if DEBUG
    /*
    std::cout << "address in bits " << addr_bits << std::endl;
    std::cout << "offset  in bits " << offset_bits << std::endl
              << "index   in bits " << index_bits << std::endl
              << "tag     in bits " << tag_bits << std::endl;
   */ 
    std::cout << "address " << addr << " hex " << std::hex << addr 
              << std::dec << std::endl;
    std::cout << "offset  " << this->curr_offset << std::endl
              << "index   " << this->curr_index << std::endl
              << "tag     " << this->curr_tag << std::endl;
#endif

}

bool CacheTagArray::read( uint64 addr)
{
    parseAddress( addr);

    IndexEl* vec_elem = &( this->indexes[ this->curr_index]);

    // find current tag in map
    MapIterator it = vec_elem->tags.find( this->curr_tag);

    // change usage time priority
    if ( it != vec_elem->tags.end()) // current tag is inside of map
    {
        if ( it->second == vec_elem->most_recently)
        {
            // not need to change usage time priority
        
//------------------------------------------
#if DEBUG
        std::cout << "sequence is" << std::endl;
        for ( UsageTimeEl* i = vec_elem->least_recently;
            i != NULL; i = i->earlier)
        {
            std::cout << i->tag << " ";
        }
        std::cout << std::endl;
        std::cout << "---------------------------- " << std::endl;
#endif
//------------------------------------------

            return true;
        } 
        
        if ( it->second != vec_elem->least_recently) 
        {
            // cut element with curr_tag from usage time list
            // via binding earlier and later of current with each other
            it->second->earlier->later = it->second->later;
            it->second->later->earlier = it->second->earlier;
        } else // ( it->second == vec_elem->least_recently)
        {
            vec_elem->least_recently = vec_elem->least_recently->earlier;
            vec_elem->least_recently->later = NULL;
        }

        // curr_tag becomes earlier than most recently used
        vec_elem->most_recently->earlier = it->second;

        // and most recently later than curr_tag
        it->second->later = vec_elem->most_recently;

        // no elements earlier curr_tag now
        it->second->earlier = NULL;

        // curr_tag becomes most recently used now
        vec_elem->most_recently = it->second;

//------------------------------------------
#if DEBUG
        std::cout << "inif" << std::endl;
        std::cout << "map size " << vec_elem->tags.size() << std::endl;
        std::cout << "sequence is" << std::endl;
        for ( UsageTimeEl* i = vec_elem->least_recently;
            i != NULL; i = i->earlier)
        {
            std::cout << i->tag << " ";
        }
        std::cout << std::endl;
        std::cout << "---------------------------- " << std::endl;
#endif
//------------------------------------------

        return true;
    } else // ( it == vec_elem->tags.end()) // i.e. curr_tag is out of map
    {
        UsageTimeEl* new_list_elem = new UsageTimeEl;

        new_list_elem->tag = curr_tag;

        // new_list_elem becomes the most recently used
        new_list_elem->earlier = NULL;
        new_list_elem->later = vec_elem->most_recently;

        if ( vec_elem->most_recently != NULL) 
        {
            vec_elem->most_recently->earlier = new_list_elem;
        } else // no elements having curr_index in cache
        {
            vec_elem->least_recently = new_list_elem;
        }

        vec_elem->most_recently = new_list_elem;

        if ( vec_elem->tags.size() == this->number_of_ways)
        {
            // remove the least recently used
            UsageTimeEl* temp = vec_elem->least_recently->earlier;
            vec_elem->tags.erase( vec_elem->least_recently->tag);
            delete vec_elem->least_recently;
            
            vec_elem->least_recently = temp;
            vec_elem->least_recently->later = NULL;
        } // else not need to remove
        
        // add new element to map
        vec_elem->tags[ this->curr_tag] = new_list_elem;
//------------------------------------------
        #if DEBUG
        std::cout << "inelse" << std::endl;
        std::cout << "sequence is" << std::endl;
        for ( UsageTimeEl* i = vec_elem->least_recently;
            i != NULL; i = i->earlier)
        {
            std::cout << i->tag << " ";
        }
        std::cout << std::endl;
        std::cout << "---------------------------- " << std::endl;
        #endif
//------------------------------------------

        return false;
    }

}

void CacheTagArray::write( uint64 addr)
{
    assert( 0);
}

