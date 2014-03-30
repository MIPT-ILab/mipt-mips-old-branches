/**
 * cache_tag_array.h - the module implementing a generic cache table for MIPS processors.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2014 uArchSim iLab project
 */


#ifndef CACHE_TAG_ARRAY_H
#define CACHE_TAG_ARRAY_H

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <vector>
#include <types.h>
#include <cmath>

class CacheLine
{
private:
    uint64 misusage_count;
    uint64 tag;
    bool valid;

    // These are currently not used but kept for the future dvelopment
    uint64 offset_size;
    uint64 tag_size;

public:
    CacheLine() : misusage_count(0), tag(0), valid(false) {};
    CacheLine( uint64 _tag_size, uint64 _offset_size) : misusage_count(0), tag(0),
                                                        valid(false),
                                                        offset_size(_offset_size),
                                                        tag_size(_tag_size) {};

    inline bool are_tags_equal( uint64 _tag) { return (_tag == this->tag); }
    inline uint64 get_misusage_count() { return this->misusage_count; }
    inline void write(uint64 _tag) { this->tag = _tag; }
    inline void set_valid() { this->valid = true; }
    inline void set_invalid() { this->valid = false; }
    inline bool is_valid()  { return this->valid; }
    inline void touch() { this->misusage_count = 0; }
    inline void spin_once() { this->misusage_count ++; }

    virtual ~CacheLine() {};
};



class Set
{
private:
    std::vector<CacheLine> lines;
    unsigned int ways;
    uint64 offset_size;

public:
    Set( unsigned int _ways, uint64 offset_size);
    void write( uint64 addr);
    bool read( uint64 addr);
    inline void spin_once();

    virtual ~Set() {};

private:
    uint64 cut_offset(uint64 addr);
};




class CacheTagArray
{
private:
    std::vector<Set> sets;
    uint64 offset_size;
    uint64 index_size;
    uint64 tag_size;

public:
    /**
     * Constructor params:
     *
     * 1) size_in_bytes is a number of data bytes that can be stored in the cache,
     *    i.e. if the block size is 16 Bytes then the number of data blocks in the cache is size_in_bytes/16.
     *
     * 2) ways is a number of associative ways in a set, i.e. how many blocks are referred by the same index.
     *
     * 3) block_size_in_bytes is a number of Bytes in a data block
     *
     * 4) addr_size_in_bit is a number of bits in the physical address.
     */
    CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits);
    /**
     * Return true if the byte with the given address is stored in the cache,
     * otherwise, return false.
     *
     * Note that his method updates the LRU information.
     */
    bool read( uint64 addr);

    /**
     * Mark that the block containing the byte with the given address
     * is stored in the cache.
     *
     * Note: in order to put the given address inside the tags it is needed
     * to select a way where it will be written in.
     * This selection is being done according to LRU (Least Recently Used)
     * policy.
     */
    void write( uint64 addr);

private:
    uint64 get_index( uint64 addr);
};


#endif // #ifndef CACHE_TAG_ARRAY_H

