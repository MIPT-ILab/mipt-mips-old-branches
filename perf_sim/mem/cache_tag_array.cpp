//C++ Headers
#include <iostream>
#include <cerrno>
#include <cmath>
#include <cstdlib>

//uArch Headers
#include "types.h"

//My Headers
#include "cache_tag_array.h"

//************************************************************************

void CacheTagArray::dumpInfo ()
{
	std::cout << "sizeCache = "    << size_in_bytes       << "bytes\n"
		  << "ways = " 	       << ways		      << "\n"
		  << "blockSize = "    << block_size_in_bytes << "bytes\n"
		  << "addrSize = "     << addr_size_in_bits   << "bits\n"
		  << "offsetInBlock = "<< offsetInBlock       << "bits\n"
		  << "bitForIndex = "  << bitForIndex	      << "bits\n"
		  << "maxIndex = "     << maxIndex	      << "\n"
		  << "maskIndex = "    << std::hex << "0x" << maskIndex << "\n"
		  << "maskTag = "      << "0x" << maskTag << "\n" << std::dec;
}

//************************************************************************

CacheTagArray::CacheTagArray ( 	uint32 size,
			     	uint32 ways,
				uint16 blockSize,
				uint16 addrSize )
{
	//Check input data
	if ( (blockSize > size) ||
	     ((ways != FUL_AS) && (blockSize*ways > size))) {
		std::cout << "ERROR: Size of cache is must more than"
			  << " blockSize*ways\n";
		exit (EXIT_FAILURE); 
	}
	if ( addrSize > blockSize * BIT_IN_BYTE ) {
		std::cout << "ERROR: addrSize > blockSize\n";
		exit (EXIT_FAILURE);
	}
	
	//set things that independ of associative
	size_in_bytes = size;
	addr_size_in_bits = addrSize;

	if ( (size % blockSize) != 0 ) {
		std::cout << "ERROR: sizeCache \% blockSize != 0\n";
		exit (EXIT_FAILURE);
	} else 
		block_size_in_bytes = blockSize;
	
	offsetInBlock = (uint32) ceil (log2 ( blockSize ));
	bitForIndex =  (uint32) ceil (log2(size/(ways*blockSize)));

	
	/*set things that depend of associative
	* maskTag
	* maskIndex
	* ways
	*/
	if ( ways == MAX_VAL32 ) {
		maskTag = (MAX_VAL32 >> offsetInBlock) << offsetInBlock; 
		maskIndex = 0;
		this->ways = size / block_size_in_bytes;
		maxIndex = 1;
	}
	else {
		maskTag = (MAX_VAL32 >> (bitForIndex + offsetInBlock))\
				<< (bitForIndex + offsetInBlock);
		maskIndex = ((MAX_VAL32 ^ maskTag) >> offsetInBlock )\
				 << offsetInBlock;
		this->ways = ways;
		maxIndex = size/(blockSize*ways);
	} 

	//Allocate memory for cahce array
	// length = sezi/(blockSize * ways)
	cache = new blockCache*[maxIndex];
	if ( !cache ) {
		std::cerr << "ERROR: Memory for cache isn't allocate\n";
		exit (EXIT_FAILURE);
	}
	for ( uint32 i = 0; i < maxIndex; ++i )
		cache[i] = NULL;
}

//************************************************************************

CacheTagArray::~CacheTagArray ()
{
	for ( uint32 i = 0; i < maxIndex; ++i )
	{
		if ( cache[i] != NULL ) {
			delete [] cache[i];
		}
	}
	
	delete [] cache; 
}

//************************************************************************

bool CacheTagArray::read ( uint32 addr, uint64 PC ) 
{
	uint32 Index = (addr & maskIndex) >> offsetInBlock;	
	uint32 Tag = (addr & maskTag) >> (offsetInBlock + bitForIndex);	

//	std::cout << "addrRead " << std::hex << addr << " Index " << Index << " Tag " << Tag << "\n" << std::dec;	
	for ( uint32 i = 0; i < ways; ++i ) {
		if ( cache[Index] != NULL ) {
			if ( cache[Index][i].tag  == Tag ) {
				cache[Index][i].ltu = PC;
				return true;
			}
		}
		else
			return false;
	}

	return false;
}

//************************************************************************

int CacheTagArray::write ( uint32 addr, uint64 PC )
{
	uint32 Index = (addr & maskIndex) >> offsetInBlock;
	uint32 Tag = (addr & maskTag) >> (offsetInBlock + bitForIndex);

//	std::cout << "addrWrite " << std::hex << addr << " Index " << Index << " Tag " << Tag << "\n" << std::dec;	
	
	if ( cache[Index] == NULL )
	{
		cache[Index] = new blockCache[ways];
		if ( cache[Index] == NULL ) {
			errno = ENOMEM;	
			return -1;
		}
		for ( uint32 i = 0; i < ways; ++i )
		{
			cache[Index][i].valid = false;
			cache[Index][i].ltu   = 0;
			cache[Index][i].tag   = 0;
			errno = 0;
		}
	}

	uint64 minLtu = MAX_VAL64;
	uint32 numMinLtu = 0;
	for ( uint32 i = 0; i < ways; ++i )
	{
		if ( cache[Index][i].valid == false ) {
			cache[Index][i].valid = true;
			cache[Index][i].ltu   = PC;
			cache[Index][i].tag   = Tag;
			errno = 0;
			return 0;	
		} else {
			if ( cache[Index][i].ltu < minLtu ) {
				minLtu = cache[Index][i].ltu;
				numMinLtu = i;
			}
			if ( (i + 1) == ways ) {
				cache[Index][numMinLtu].valid = true;
				cache[Index][numMinLtu].ltu   = PC;
				cache[Index][numMinLtu].tag   = Tag;

			//	std::cout << "ltu " << minLtu+1 << "\n";
			}
		} 
	}

	errno = 0;
	return 0;	
}










