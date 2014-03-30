/**
 * cache_tag_array.cpp - the module implementing a generic cache table for MIPS processors.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2014 uArchSim iLab project
 */

#include "cache_tag_array.h"




CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes,
                              unsigned short addr_size_in_bits)
{
    unsigned int data_blocks = size_in_bytes/block_size_in_bytes;
    unsigned int set_num     = data_blocks/ways;

    this->offset_size = ceil(log(double(block_size_in_bytes))/log(2.0)); // base 2 logarithm
    this->index_size  = ceil(log(double(set_num))/log(2.0));
    if(addr_size_in_bits < index_size + offset_size)
    {
        std::cerr << "Invalid address size (too short). terminating...";
        exit(-1);
    }
    this->tag_size = addr_size_in_bits - index_size - offset_size;

    for(unsigned int i = 0; i < set_num; i++)
    {
        Set set(ways, this->offset_size);
        this->sets.push_back(set);
    }
};


bool CacheTagArray::read( uint64 addr)
{
    return this->sets[this->get_index(addr)].read(addr);
};


void CacheTagArray::write( uint64 addr)
{
    this->sets[this->get_index(addr)].write(addr);
};


uint64 CacheTagArray::get_index( uint64 addr)
{
    uint64 index = addr;
    uint64 mask = 0;
    mask = ~mask;
    index = (index >> this->offset_size);
    mask = ~(mask << this->index_size);

    index = (index & mask);
    return index;
};



Set::Set( unsigned int _ways, uint64 _offset_size)
{
    this->ways = _ways;
    this->offset_size = _offset_size;
    for( unsigned int i = 0; i < this->ways; i++)
    {
        CacheLine cl;
        this->lines.push_back(cl);
    }
};


void Set::spin_once()
{
    for( unsigned int i = 0; i < this->ways; i++)
    this->lines[i].spin_once();
};


void Set::write( uint64 addr)
{
    uint64 max_misusage_count = 0; // Standard search algoritm
    uint64 id = 0;
    for( unsigned int i = 0; i < this->ways; i++)
    {
        if( this->lines[i].get_misusage_count() > max_misusage_count)
        {
            id = i;
            max_misusage_count = this->lines[i].get_misusage_count();
        }
    }
    // Actually, we'll get tag+index here, but since all indexes are
    // Equal in the set it does not matter
    this->lines[id].set_valid(); // Set the valid bit
    this->lines[id].write(this->cut_offset(addr)); // Actually remember that we remember the data :)
    this->lines[id].touch(); // Set misusage counter to zero
};

bool Set::read( uint64 addr)
{
    this->spin_once(); // Increment all misusage counters
    uint64 tag = this->cut_offset(addr);
    for( unsigned int i = 0; i < this->ways; i++)
        if(this->lines[i].are_tags_equal(tag)) return true;
    return false;
};

uint64 Set::cut_offset(uint64 addr)
{
    uint64 mask = 0;
    mask = ~mask; // mask = 1111...11
    mask = (mask << this->offset_size); // mask = 11...11110...00
    return (addr & mask); // addr = xxxxxxxx0...00
};


