#include "cache_tag_array.h"

using namespace std;

// Some functions, which used only in this scope
namespace CacheHelper
{
    // give unt64 with 1 from first to last positions
    uint64 get_mask(int first, int last)
    {
        uint64 mask = MAX_VAL64;
        mask = mask >> (first );
        mask = mask << (first  + 63 - last);
        mask = mask >> (63 - last);
        return mask;
    }
    // rounded up log2 from natural uint64 value
    int log2(uint64 value)
    {
        uint64 temp = 1;
        for (int i = 0; i < 64; i++)
        {
            if (temp >= value) return i;
            else
                 temp = temp << 1;
        }
        return -1;
    }
}

// Constructor of main class 
// ise uint64 (replace uint ) as parameter
CacheTagArray::CacheTagArray( uint64 size_in_bytes,
                   uint64 ways,
                   unsigned short block_size_in_bytes, 
                   unsigned short addr_size_in_bits)
{
    hits = 0;
    misses = 0;
    operation_number = 1;
    
    this->ways = ways;
    
    offset_size = block_size_in_bytes;
    offset_size_in_bits = CacheHelper::log2(offset_size);

    index_size = size_in_bytes / block_size_in_bytes / ways;
    if (!index_size) index_size = 1;
    index_size_in_bits = CacheHelper::log2(index_size);
    
    tag_size = ((uint64)1 << (addr_size_in_bits -1 )) / index_size/offset_size ;
    tag_size *= 2;
    tag_size_in_bits = CacheHelper::log2(tag_size);

    offset_mask = CacheHelper::get_mask( 0, offset_size_in_bits-1 );
    
    index_mask = CacheHelper::get_mask( offset_size_in_bits ,
                              offset_size_in_bits + index_size_in_bits -1 );
    
    tag_mask = CacheHelper::get_mask ( offset_size_in_bits+index_size_in_bits,
                                        addr_size_in_bits -1 );
    
    /* Some debugging output
    cout<<"offset_size = "<<offset_size<<"\n";
    cout<<"offset_size_in_bits = "<<offset_size_in_bits<<"\n";
    cout<<"index_size = "<<index_size<<"\n";
    cout<<"index_size_in_bits = "<<index_size_in_bits<<"\n";
    cout<<"tag_size = "<<tag_size<<"\n";
    cout<<"tag_size_in_bits = "<<tag_size_in_bits<<"\n";
    */
    
    cache_sets = new CacheSet[index_size];// Allocating for block pointers
    for( uint64 i = 0; i < index_size;i++)
     {
         cache_sets[i].cache_block = new CacheBlock[ways];
         for( uint64 j = 0; j < ways; j++)
         {
             cache_sets[i].cache_block[j].valid = 0;
             cache_sets[i].cache_block[j].tag = 0; // for valgrind
             cache_sets[i].cache_block[j].last_use_time = 0;
         }
     }
}

bool CacheTagArray:: read( uint64 addr)
{
    // this way to identify tranzaction number will be broken by
    // overfloating after 60 years after laynching this aplication
    // without reboots
    operation_number++;
    uint64 index = get_index_from_addr( addr);
    uint64 tag = get_tag_from_addr( addr );
    for(uint64 i = 0; i < ways ; i++)
    {
        if ( (cache_sets[index].cache_block[i].tag == tag) && (cache_sets[index].cache_block[i].valid))
         {
            /* !!IMPORTANT
             * uncomment this line to do program better
             * means, that it will delete from cache
             * last used data ( not last loaded)
            **/
            //cache_sets[index].cache_block[i].last_use_time = operation_number;
            hits++;
            return true;
         }
    }
     misses++;
     write(addr);
     return false;
}

void CacheTagArray:: write(uint64 addr)
{
    uint64 index = get_index_from_addr( addr);
    uint64 tag = get_tag_from_addr( addr );
    uint64 min_time = MAX_VAL64;
    uint64 min_time_number = ways -1;
    for(uint64 i = 0; i < ways ; i++)
    {
        if ( cache_sets[index].cache_block[i].last_use_time < min_time )
        {
            min_time = cache_sets[index].cache_block[i].last_use_time;
            min_time_number = i;
        }
    }
    cache_sets[index].cache_block[min_time_number].tag = tag;
    cache_sets[index].cache_block[min_time_number].last_use_time = operation_number;
    cache_sets[index].cache_block[min_time_number].valid = 1; // it is faster
                                                   // without if( valid == 0)
}

uint64 CacheTagArray:: get_index_from_addr( uint64 address)
{
    address = address & index_mask;
    return (address >> offset_size_in_bits );
}


uint64 CacheTagArray:: get_offset_from_addr( uint64 address)
{
    return address & offset_mask;
}

uint64 CacheTagArray:: get_tag_from_addr( uint64 address)
{
    address = address & tag_mask;
    return (address >> (offset_size_in_bits + index_size_in_bits) );
}

CacheTagArray::~CacheTagArray()
{

    cout<<"hits "<<hits<<" misses "<<misses<<"\n";    
    for( uint64 i = 0; i < index_size;i++)
     {
        delete[] cache_sets[i].cache_block;
     }
    delete[] cache_sets;
}

double CacheTagArray::GetHitRate()
{
    return (double) hits/( hits + misses );
}

double CacheTagArray::GetMissRate()
{
    return (double) misses/( hits+ misses );
}



