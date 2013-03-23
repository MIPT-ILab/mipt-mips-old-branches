/**
 * cache_tag_array.cpp - The module implementing 
 * the model of cache for performance simulation.
 * @author Dmitry Borody <dmitry.borody@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// uArchSim modules
#include <types.h>
#include <cache_tag_array.h>

// General C
#include <cassert>

// General C++
#include <vector>
#include <iostream>

CacheTagArray::CacheTagArray( unsigned size_in_bytes,
                              unsigned ways,
                              unsigned short block_size_in_bytes, 
                              unsigned short addr_size_in_bits)
{
    this->ways = ways;
    number_of_blocks = size_in_bytes / block_size_in_bytes;
    number_of_sets = number_of_blocks / ways;

    assert( size_in_bytes % block_size_in_bytes == 0);
    assert( number_of_blocks % ways == 0);
    assert( number_of_sets > 0);
    assert( number_of_blocks > 0);

    bits_for_index = 0;
    for ( unsigned power_of_two = 1; power_of_two < number_of_sets; power_of_two <<= 1)
      ++bits_for_index;
    bits_for_offset = 0;
    for ( unsigned power_of_two = 1; power_of_two < block_size_in_bytes; power_of_two <<= 1)
      ++bits_for_offset;
    bits_for_tag = addr_size_in_bits - bits_for_index - bits_for_tag;

    assert( ( 1 << bits_for_index) == number_of_sets);
    assert( ( 1 << bits_for_offset) == block_size_in_bytes);

    std::vector< CacheBlock> set( ways, CacheBlock { false, 0, 0});
    sets.assign( number_of_sets, set);
}

bool CacheTagArray::read( uint64 addr)
{
  unsigned index = (addr >> bits_for_offset) & ((1 << bits_for_index) - 1);
  unsigned tag = (addr >> bits_for_offset) >> bits_for_index;
  std::vector< CacheBlock> &ways = sets[index];

  for (auto &way : ways) 
  {
    if (way.valid && way.tag == tag) 
    {
      way.last_access_time = mem_access_counter;
      mem_access_counter++;
      return true;
    }
  }
  return false;
}

void CacheTagArray::write( uint64 addr)
{
  unsigned index = (addr >> bits_for_offset) & ((1 << bits_for_index) - 1);
  unsigned tag = (addr >> bits_for_offset) >> bits_for_index;
  std::vector< CacheBlock> &ways = sets[index];

  CacheBlock *lru_block = &ways[0];
  for (auto &way : ways)
  {
    if (way.last_access_time < lru_block->last_access_time)
      lru_block = &way;
  }

  // Now lru_block points to the victim block
  lru_block->valid = true;
  lru_block->tag = tag;
  lru_block->last_access_time = mem_access_counter;
  mem_access_counter++;
}

