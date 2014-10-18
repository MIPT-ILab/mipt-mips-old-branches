/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Implementation of memory methods:
 * @author Ilya Belikov <ilya.i.belikov@gmail.com>
 * Copyright 2012-2014 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    // init address parts sizes
    SetSize = addr_size - page_bits - offset_bits;
    PageSize = page_bits;
    OffsetSize = offset_bits;

    // read data from ELF file
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    text_start_addr = 0;

    for( uint64 section = 0; section < sections_array.size(); ++section)
    {
        if(strcmp(sections_array[section].name, ".text") == 0)
                text_start_addr = sections_array[section].start_addr;

        for( uint64 i = 0; i < sections_array[ section].size/4; ++i)
        {
            uint64 address = sections_array[ section].start_addr + i;
            uint64 value = (*( uint32*)( sections_array[ section].content + i));
            write( value, address, 4);
            if(section == 0)very_first_addr = address;
        }
    }
}

FuncMemory::~FuncMemory()
{
    //delete Memory;
}

uint64 FuncMemory::startPC() const
{
    return text_start_addr;
}

uint64 FuncMemory::returnSet( uint64 addr) const
{
    return (addr & (~( uint32)0 << ( PageSize + OffsetSize)))
                                    >> ( PageSize + OffsetSize);
}

uint64 FuncMemory::returnPage( uint64 addr) const
{
    return ( addr & ( ~( uint32)0 >> SetSize)) >> OffsetSize;
}

uint64 FuncMemory::returnOffset( uint64 addr) const
{
    return ( addr & ( ~( uint32)0 >> ( SetSize + PageSize)));
}

uint8 FuncMemory::read_byte( uint64 Set_addr, uint64 Page_addr, uint64 Offset_addr) const
{
    map< uint64, const void*>::const_iterator it;

    it = Memory.find(Set_addr);

    it = ((map<uint64, const void*>*)(it->second))->
                                        find(Page_addr);

    return ((map< uint64, uint8>*)(it->second))->find(Offset_addr)->second;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert(num_of_bytes != 0);

    //assert(addr >= very_first_addr);
    //if(addr < very_first_addr) abort();

    uint64 ret = 0;

    for(int i = 0; i < num_of_bytes; ++i)
    {
        uint64 Set_addr = returnSet(addr + i);
        uint64 Page_addr = returnPage(addr + i);
        uint64 Offset_addr = returnOffset(addr + i);
        ret |= read_byte(Set_addr, Page_addr, Offset_addr) << (8*i);
    }

    return ret;
}

void FuncMemory::write_byte( uint64 Set_addr, uint64 Page_addr, uint64 Offset_addr, uint8 data)
{
    pair< map< uint64, const void*>::iterator, bool > ret;

    ret = Memory.insert(
                    pair< uint64, const void*>
                    (Set_addr, (const void*)(new map<uint64, const void*>))
                );

    ret = ((map< uint64, const void*>*)(ret.first->second))->
                insert(
                    pair< uint64, const void*>
                    (Page_addr, (const void*)(new map<uint64, uint8>))
                );

    map< uint64, uint8>* dest = ((map< uint64, uint8>*)(ret.first->second));

    dest->erase(Offset_addr);
    dest->insert(pair< uint64, uint8>( Offset_addr, data));
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert(num_of_bytes != 0);
    if(num_of_bytes == 0)return;

    uint8 *data = (uint8*)(&value);

    for(uint64 i = 0; i < num_of_bytes; ++i)
    {
        uint64 Set_addr = returnSet(addr + i);
        uint64 Page_addr = returnPage(addr + i);
        uint64 Offset_addr = returnOffset(addr + i);
        write_byte(Set_addr, Page_addr, Offset_addr, *(data + i));
    }
}

string FuncMemory::dump( string indent) const
{

    return string("ERROR: Dump is not implemented yet!");
}
