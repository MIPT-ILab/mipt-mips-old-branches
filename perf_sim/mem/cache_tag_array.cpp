//generic uarchsim
#include "cache_tag_array.h"

//generic c
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>

//generic c++
#include <cmath>
#include <iostream>

//Constructor--------------------------------------------------------------------------------------------------------------------------------------------------

CacheTagArray::CacheTagArray( unsigned int size_in_bytes,
                   unsigned int ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits)
    {
        //check paprametres for correct!!!

    if ((size_in_bytes % (ways*block_size_in_bytes)) != 0)
    {
        std::cerr << "Wrong parametres of cache: number of segments must be integer\n";
        exit(EXIT_FAILURE);
    }

    if ((IsTwoPower(size_in_bytes) == false) || (IsTwoPower(ways) == false) || (IsTwoPower(addr_size_in_bits) == false))
    {
        std::cerr << "Wrong parametres of cache: size_in_bytes, ways, addr_size_in_bits must be power of 2\n";
        exit(EXIT_FAILURE);
    }

        this->number_of_ways = ways;
        this->offset_addr_size = (unsigned int)ceil(log2(block_size_in_bytes));
        this->index_addr_size = (unsigned int)ceil(log2(size_in_bytes/(block_size_in_bytes*ways)));
        this->tag_addr_size = addr_size_in_bits - this->offset_addr_size - this->index_addr_size;

        //allocate memory for cache
        this->Create_cach(size_in_bytes, ways, block_size_in_bytes, addr_size_in_bits);

        //masks for counting tag and index
        unsigned int tag_shift = pow(2, this->tag_addr_size) - 1;
        this->tag_mask = tag_shift  << (this->index_addr_size+this->offset_addr_size);
        unsigned int index_shift = pow(2, this->index_addr_size) - 1;
        this->index_mask = index_shift << (this->offset_addr_size);

        this->PC = 0;
        this->MAX_ADDR = pow(2, addr_size_in_bits)-1;

    }

//Allocate memory for cache_tag_array-------------------------------------------------------------------------------------------------------------------------

void CacheTagArray::Create_cach(unsigned int size_in_bytes,
                                unsigned int ways,
                                unsigned short block_size_in_bytes,
                                unsigned short addr_size_in_bits)
                                {
                                    unsigned int number_of_blocks = size_in_bytes/block_size_in_bytes;
                                    this->cache_tag_array.v = (bool *)calloc(sizeof(bool), number_of_blocks);
                                    assert(this->cache_tag_array.v != NULL);
                                    this->cache_tag_array.tag = (uint64 *)calloc(sizeof(uint64), number_of_blocks);
                                    assert(this->cache_tag_array.tag != NULL);
                                    this->cache_tag_array.data = NULL; //not used
                                    this->cache_tag_array.was_used = (unsigned int *)calloc(sizeof(int), number_of_blocks);
                                    assert(this->cache_tag_array.was_used != NULL);

                                }

//Read data from cache: if data was found return true else return false------------------------------------------------------------------------------------------

    bool CacheTagArray::read( uint64 addr)
    {
        //chaeck adddress for correct
        if (addr > this->MAX_ADDR)
        {
            std::cerr << "Wrong address in memory\n";
            exit(EXIT_FAILURE);
        }

        //calculate cache index and tag
        uint64 tag = ((addr & this->tag_mask) >> (this->offset_addr_size + this->index_addr_size));
        uint64 index = ((addr & this->index_mask) >> (this->offset_addr_size));

        //search requaired address in cache
        for ( int i = 0; i < this->number_of_ways; i++)
        {
            if ((cache_tag_array.v[index+i] == true) && (cache_tag_array.tag[index+i] == tag))
            {
                cache_tag_array.was_used[index + i] = PC;
                return true;
            }
        }

        //increase PC
        this->PC = this->PC + 1;
        return false;
    }

//Write data in cache: policy of chnging lru----------------------------------------------------------------------------------------------------------------

    void CacheTagArray::write( uint64 addr)
    {
        //chaeck address for correct
        if (addr > this->MAX_ADDR)
        {
            std::cerr << "Wrong address in memory\n";
            exit(EXIT_FAILURE);
        }

        //calculate cache index and tag
        uint64 tag = (addr & this->tag_mask) >> this->offset_addr_size + this->index_addr_size;
        uint64 index = (addr & this->index_mask) >> this->offset_addr_size;

        //search data which is not valid
        for ( int i = 0; i < this->number_of_ways; i++)
        {
            if (cache_tag_array.v[index+i] == false)
            {
                cache_tag_array.tag[index+i] = tag;
                cache_tag_array.v[index+i] = true;
                cache_tag_array.was_used[index+i] = PC;
                return;
            }
        }

        //if there is no data which is not valid then find "LRU" data
        int lru_index = index;
        int least_used = this->PC;
        int i = 0;
        for ( i = 0; i < this->number_of_ways; i++)
        {
            if (cache_tag_array.was_used[index+i] <= least_used)
            {
                least_used = cache_tag_array.was_used[index+i];
                lru_index = index+i;
            }
        }

        cache_tag_array.tag[lru_index] = tag;
        cache_tag_array.v[lru_index] = true;
        cache_tag_array.was_used[lru_index] = PC;

        //increase PC
        this->PC = this->PC + 1;
    }


