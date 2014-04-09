/*
 * cache_tag_array.cpp
 *
 */



#include <cache_tag_array.h>
#include <iostream>
#include <assert.h>

#define NOT_LOG2 0xff

// Very bad taking binary logarithm
uint8 log2( uint32 value)
{
	uint8 i = 0;
	while ( value != 1)
	{
		if ( (value & 1) != 0)
			return NOT_LOG2;
		value >>= 1;
		i++;
	}
	return i;
}


CacheTagArray::CacheTagArray( uint32 size_in_bytes,
               				  uint32 ways,
               				  uint16 block_size_in_bytes, 
  				              uint16 addr_size_in_bits)
{
	cache_size = size_in_bytes;
	ways_num = ways;
	block_size = block_size_in_bytes;
	addr_size = addr_size_in_bits;

	assert( cache_size != 0);
	assert( block_size != 0);
	assert( addr_size != 0);
	assert( addr_size <= 64);
	if ( ways_num == 0) 	// it means full associativity
		ways_num = cache_size / block_size;

	offset_bits = log2(block_size);
	assert( offset_bits != NOT_LOG2);
	index_num = cache_size / ways_num / block_size;
	assert( index_num != 0);
	index_bits = log2(index_num); 		// Only power of two is permitted
	assert( index_bits != NOT_LOG2);
	tag_bits = addr_size - offset_bits - index_bits;
	assert( tag_bits >= 0);

	cache_lines = new CacheLine [index_num];
	for ( uint32 i = 0; i < index_num; i++)
	{
		cache_lines[i].tags = new uint32 [ways_num];
		cache_lines[i].stat = new uint32 [ways_num];
		for ( uint32 j = 0; j < ways_num; j++)
			cache_lines[i].stat[j] = ways_num - j - 1;
	}
	//NEED DESTRUCTOR!
}


bool CacheTagArray::read( uint64 addr)
{
	uint32 i;
	for ( i = 0; i < ways_num; i++)
		if (cache_lines[getIndex( addr)].tags[i] == getTag( addr))
			break;
	if ( i != ways_num)
	{
		updateStat( getIndex( addr), i);
		return true;
	}
	else
	{
		write( addr);
		return false;
	}
}


void CacheTagArray::write( uint64 addr)
{
	uint32 i;
	for ( i = 0; i < ways_num; i++)
		if ( cache_lines[getIndex( addr)].stat[i] == ways_num - 1)
			break;
	cache_lines[getIndex( addr)].tags[i] = getTag( addr);
	updateStat( getIndex( addr), i);
}


uint64 CacheTagArray::getTag( uint64 addr)
{
	return addr >> (offset_bits + index_bits);
}


uint64 CacheTagArray::getIndex( uint64 addr)
{
	return (addr >> offset_bits) & ((1 << index_bits) - 1);
}


uint64 CacheTagArray::getOffset( uint64 addr)
{
	return addr & ((1 << offset_bits) - 1);
}


void CacheTagArray::updateStat(uint32 index, uint32 way)
{
	for ( uint32 i = 0; i < ways_num; i++)
		if ( cache_lines[index].stat[i] < cache_lines[index].stat[way])
			cache_lines[index].stat[i]++;
	cache_lines[index].stat[way] = 0;
}


