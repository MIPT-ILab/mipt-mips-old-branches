/**
 * cache_tag_array.h - header of module simulating cache behaviour
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

#ifndef CACHE_TAG_ARRAY__CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY__CACHE_TAG_ARRAY_H

// uArchSim modules
#include <types.h>

struct LineListEl
{
    unsigned int tag;
    bool valid_bit;
    LineListEl *next_line;
};

typedef LineListEl* Set; // Set == list of lines

class CacheTagArray
{
    unsigned int size;
    unsigned int lines_in_set;
    unsigned int lines_in_cache;
    unsigned int sets;
    unsigned short block_size;
    unsigned short addr_size;
    unsigned short offset_len;
    unsigned short index_len;
    unsigned short tag_len;
    Set        *sets_array;  // Array of pointers at last used lines in set
    LineListEl *lines_array; // Array of lines
   
    unsigned int   getTag( uint64 addr);                            // Parses tag from address
    unsigned int   getIndex( uint64 addr);                          // Parses index from address
    unsigned short lb( unsigned int num);                           // Binary logarithm
    void           moveNextToTop( Set *set, LineListEl *last_line); // Moves line after last_lane to the top of Set
public:
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits);
    ~CacheTagArray();
    bool read( uint64 addr);
    void write( uint64 addr);
};

#endif