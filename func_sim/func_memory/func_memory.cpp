/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

  /**
 * updated by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 * v. 1.0: created 16.10.2014 21:05
 *         added implementation of main functions
 */

// Generic C
#include <string.h>

// Generic C++

// uArchSim modules
#include <func_memory.h>


FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    assert( addr_size);
    assert( page_bits);
    assert( offset_bits);
    assert( executable_file_name);
    this->OffsetSize = 1;
    this->PageSize = 1;
    this->SetSize = 1;
    this->OffsetBits = offset_bits;
    this->PageBits = page_bits;
    this->SetBits = addr_size - page_bits - offset_bits;
    this->PCstart_addr = 0;
    for ( int i = 0; i < offset_bits; i++)
        this->OffsetSize *= 2;
    for ( int i = 0; i < page_bits; i++)
        this->PageSize *= 2;
    for ( int i = 0; i < ( addr_size - page_bits - offset_bits); i++)
        this->SetSize *= 2;
    this->memory = new MemSet* [ SetSize];
    for ( int i = 0; i < SetSize; i++)
        this->memory[ i] = NULL;
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    for ( int i = 0; i < sections_array.size(); i++)
    {
        if ( !strcmp( ".text", sections_array[ i].name))
        {
            PCstart_addr = sections_array[ i].start_addr;
        }
        this->MemWrite( sections_array[ i].content, sections_array[ i].start_addr, sections_array[ i].size);
    }
}


void FuncMemory::MemWrite( uint8* value, uint64 addr, uint64 size)
{
    assert( size);
    uint64 cur_set_addr = ( ( this->SetSize - 1) & ( addr >> ( this->OffsetBits + this->PageBits)));
    uint64 cur_page_addr = ( ( this->PageSize - 1) & ( addr >> this->OffsetBits));
    uint64 cur_offset_addr = ( ( this->OffsetSize - 1) & addr);
    uint64 pos = 0;
    while ( size > 0)
    {
        if ( this->memory[ cur_set_addr])
        {
            if ( this->memory[ cur_set_addr]->page[ cur_page_addr])
            {
                this->memory[ cur_set_addr]->page[ cur_page_addr]->offset[ cur_offset_addr].value = value[ pos];
            }
            else
            {
                this->memory[ cur_set_addr]->page[ cur_page_addr] = new MemPage;
                this->memory[ cur_set_addr]->page[ cur_page_addr]->offset = new MemOffset[ this->OffsetSize];
                this->memory[ cur_set_addr]->page[ cur_page_addr]->offset[ cur_offset_addr].value = value[ pos];
            }
        }
        else
        {
            this->memory[ cur_set_addr] = new MemSet;
            this->memory[ cur_set_addr]->page = new MemPage*[ this->PageSize];
            for ( int i = 0; i < PageSize; i++)
            {
                this->memory[ cur_set_addr]->page[ i] = NULL;
            }
            this->memory[ cur_set_addr]->page[ cur_page_addr] = new MemPage;
            this->memory[ cur_set_addr]->page[ cur_page_addr]->offset = new MemOffset[ this->OffsetSize];
            this->memory[ cur_set_addr]->page[ cur_page_addr]->offset[ cur_offset_addr].value = value[ pos];
        }
        addr++;
        pos++;
        size--;
        cur_set_addr = ( ( this->SetSize - 1) & ( addr >> ( this->OffsetBits + this->PageBits)));
        cur_page_addr = ( ( this->PageSize - 1) & ( addr >> this->OffsetBits));
        cur_offset_addr = ( ( this->OffsetSize - 1) & addr);
    }
}


FuncMemory::~FuncMemory()
{
    for ( int i = 0; i < this->SetSize; i++)
    {
        if ( this->memory[ i])
        {
            for ( int j = 0; j < this->PageSize; j++)
            {
                if ( this->memory[ i]->page[ j])
                {
                    delete [] this->memory[ i]->page[ j];
                }
            }
            delete [] this->memory[ i];
        }
    }
    delete [] this->memory;
}


uint64 FuncMemory::startPC() const
{
    return this->PCstart_addr;
}


uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes);
    assert( num_of_bytes <= 8);
    uint64 cur_set_addr = ( ( this->SetSize - 1) & ( addr >> ( this->OffsetBits + this->PageBits)));
    uint64 cur_page_addr = ( ( this->PageSize - 1) & ( addr >> this->OffsetBits));
    uint64 cur_offset_addr = ( ( this->OffsetSize - 1) & addr);
    uint8 res[ 8];
    uint64 result = 0;
    bool done = false;
    for ( int i = 0; i < num_of_bytes; i++)
    {
        assert( this->memory[ cur_set_addr]);
        assert( this->memory[ cur_set_addr]->page[ cur_page_addr]);
        res[ i] = this->memory[ cur_set_addr]->page[ cur_page_addr]->offset[ cur_offset_addr].value;
        addr++;
        cur_set_addr = ( ( this->SetSize - 1) & ( addr >> ( this->OffsetBits + this->PageBits)));
        cur_page_addr = ( ( this->PageSize - 1) & ( addr >> this->OffsetBits));
        cur_offset_addr = ( ( this->OffsetSize - 1) & addr);
    }
    for ( int i = num_of_bytes - 1; i >= 0; i--)
    {
        result = result * 256 + res[ i];
    }
    return result;
}


void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    uint8 res[ 8];
    for ( int i = 0; i < num_of_bytes; i++)
    {
        res[ i] = uint8 ( ( value >> ( 8 * i)) & 255);
    }
    this->MemWrite( res, addr, num_of_bytes);
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
