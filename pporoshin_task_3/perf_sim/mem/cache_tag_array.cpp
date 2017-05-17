/**
 * cache_tag_array.cpp - module simulating cache behaviour
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <stdio.h>

// uArchSim modules
#include <types.h>
#include <cache_tag_array.h>

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                              unsigned int ways,
                              unsigned short block_size_in_bytes, 
                              unsigned short addr_size_in_bits)
{
    this->size = size_in_bytes;
    this->lines_in_set = ways;
    this->lines_in_cache = size_in_bytes / block_size_in_bytes;
    this->sets = this->lines_in_cache / ways;
    this->block_size = block_size_in_bytes;
    this->addr_size = addr_size_in_bits;

    this->offset_len = lb( block_size_in_bytes);
    this->index_len = lb( this->sets);
    this->tag_len = addr_size_in_bits - this->offset_len - this->index_len;

    this->lines_array = new LineListEl[this->lines_in_cache];
    this->lines_array[0].valid_bit = false;
    for( int i = 1; i < this->lines_in_cache; i++)
    {
        this->lines_array[i - 1].next_line = this->lines_array + i;
        this->lines_array[i].valid_bit = false;
    }

    this->sets_array = new Set[this->sets];
    for( int i = 0; i < this->sets; i++)
    {
        this->sets_array[i] = this->lines_array + i * this->lines_in_set;
        (this->sets_array[i] + this->lines_in_set - 1)->next_line = NULL;
    }
}

CacheTagArray::~CacheTagArray()
{
    delete [] this->lines_array;
    delete [] this->sets_array;
}

unsigned int CacheTagArray::getTag( uint64 addr)
{
    return addr >> (this->addr_size - this->tag_len);
}

unsigned int CacheTagArray::getIndex( uint64 addr)
{
    addr <<= ( sizeof( addr) * 8 - this->addr_size) + this->tag_len;
    addr >>= ( sizeof( addr) * 8 - this->addr_size) + this->tag_len + this->offset_len;
    return addr;
}

unsigned short CacheTagArray::lb( unsigned int num)
{
    unsigned short result = 0;
    while( num > 1)
    {
        result++;
        num >>= 1;
    }
    return result;
}

void CacheTagArray::moveNextToTop( Set *set, LineListEl *last_line)
{
    if( last_line == NULL)
    {
        return;
    }
    LineListEl *cur_line = last_line->next_line;
    last_line->next_line = cur_line->next_line;
    cur_line->next_line = (*set);
    *set = cur_line;
}

bool CacheTagArray::read( uint64 addr)
{
    unsigned int index = getIndex( addr);
    unsigned int tag   = getTag( addr);
    LineListEl *cur_line = this->sets_array[index];
    LineListEl *last_line = NULL;
    while( true)
    {
        if( cur_line->tag == tag && cur_line->valid_bit)
        {
            moveNextToTop( &(this->sets_array[index]), last_line);
            return true;
        }
        if( cur_line->next_line == NULL)
        {
            break;
        }
        last_line = cur_line;
        cur_line = cur_line->next_line;
    } 
    write( addr);
    return false;
}

void CacheTagArray::write( uint64 addr)
{
    unsigned int index = getIndex( addr);
    unsigned int tag   = getTag( addr);
    LineListEl *cur_line = this->sets_array[index];
    LineListEl *last_line = NULL;
    while( cur_line->valid_bit && cur_line->next_line != NULL)
    {
        last_line = cur_line;
        cur_line = cur_line->next_line;
    }
    cur_line->tag = tag;
    cur_line->valid_bit = true;
    moveNextToTop( &(this->sets_array[index]), last_line);
}