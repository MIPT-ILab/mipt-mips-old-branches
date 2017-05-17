/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <string.h>
#include <malloc.h>

#include <libelf.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <gelf.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>
// Generic C++
#include <iostream>
#include <string>
#include <sstream>
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
    vector<ElfSection> sections_vector ;
    ElfSection::getAllElfSections( executable_file_name, sections_vector);
    this->value = ( char*) malloc( 0x20000);
    this->init = ( bool*) calloc( 0x20000, sizeof( char));

    for ( int i = 0; i < sections_vector.size(); ++i)
    {
        unsigned short num_of_bytes = 1;
        if ( !strcmp( sections_vector[ i].name, ".text"))
        {
            PCstart = sections_vector[ i].start_addr;
        }
        for ( int j = 0; j < sections_vector[ i].size; ++j)
        {
            this->write( sections_vector[ i].content[ j], sections_vector[ i].start_addr + j, num_of_bytes);
        }
    }

}

FuncMemory::~FuncMemory()
{
    free( this->value);
    free( this->init);
}

uint64 FuncMemory::startPC() const
{
    return this->PCstart;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes);
    assert( num_of_bytes <= 8);
    assert( 0x3FFFF0 <= addr && addr <= 0x41FFF0);
    assert( this->init[ addr - 0x3FFFF0]);

    uint64 result = 0;
    uint8 res[ 8];

    for ( int i = 0; i < num_of_bytes; ++i)
    {
        res[ i] = this->value[ addr - 0x3FFFF0 + i];
    }

    for ( int i = num_of_bytes - 1; i >= 0; --i)
    {
        result = result * 256 + res[ i];
    }

    return result;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    uint64 result = value;
    assert( num_of_bytes);
    assert( num_of_bytes <= 8);
    for ( int i = 0; i < num_of_bytes; ++i)
    {
        this->value[ addr - 0x3FFFF0 + i] = result % 256;
        this->init[ addr - 0x3FFFF0 + i] = true;
        result = result / 256;
    }
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    bool skip_was_printed = false;
    for ( int offset = 0; offset < 0x20000; offset += sizeof(uint32))
    {
        if ( this->init[ offset])
        {
            uint64 substr = read ( 0x3FFFF0 + offset, 4);
            if ( substr)
                oss << indent << "0x" << hex << ( 0x3FFFF0 + offset)
                    << ":    " << substr << endl;
                skip_was_printed = false;
        }
        else
        {
            if ( !skip_was_printed)
            {
                oss << indent << "  ....  " << endl;
                skip_was_printed = true;
            }
        }
    }

    return oss.str();
}
