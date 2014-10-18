/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

#include <libelf.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <gelf.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>
#include <cmath>

// Generic C++
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// uArchSim modules
#include "func_memory.h"

using namespace std;


    vector<int> toBin( uint64 val);
    vector<int> toBin( uint8 val);
    uint64 fromBin( vector<int> data);
    void myWrite( uint8* content,
                   uint64 value,
                   uint64 from,
                   uint64 count);

/* toBin represents number of type uint8 in binary */
vector<int> toBin( uint64 val)
{
    uint64 valCop = val;
    int size = sizeof(val) * 8;
    vector<int> result( size, 0);
    for ( unsigned int i = 0; i < sizeof(val) * 8; ++i)
    {
        int r = valCop % 2;
        result[ size - 1 - i] = r;
        valCop = valCop >> 1;
    }

    return result;
}

/* toBin represents number of type uint8 in binary */
vector<int> toBin( uint8 val)
{
    uint8 valCop = val;
    int size = sizeof( val) * 8;
    vector<int> result( size, 0);

    for ( unsigned int i = 0; i < size; ++i)
    {
        int r = valCop % 2;
        result[ size - 1 - i] = r;
        valCop = valCop >> 1;
    }

    return result;
}

/* fromBin represents number from binary to uint64 */
uint64 fromBin( vector<int> data)
{
    int size = data.size();

    uint64 result = 0;
    uint64 pow = 1;
    for ( int i = size - 1; i >= 0; --i)
    {
        result += data[i] * pow;
        pow = pow << 1;
    }

    return result;
}

/* write input value to content */
void myWrite( uint8* content,
               uint64 value,
               uint64 from,
               uint64 count)
{
    int size8 = sizeof( uint8) * 8;
    int size64 = sizeof( uint64) * 8;

    vector<int> bytes = toBin(value);
    for ( int i = 0; i < count; ++i)
    {
        vector<int> result( size8, 0);

        for ( int j = 0; j < size8; ++j)
        {
            result[j] = bytes[ size64 - ( i + 1) * size8 + j];
        }
        content[ from + i] = fromBin( result);
    }
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    ElfSection::getAllElfSections( executable_file_name, sections_array);
}


FuncMemory::~FuncMemory()
{
    //STL vector has its own destructor
}

/* startPC method returns start address of the ".text" section */
uint64 FuncMemory::startPC() const
{
    uint64 text_addr = 0;
    for ( int i = 0; i < sections_array.size(); i++)
    {
        if ( strcmp(sections_array[ i].name, ".text") ==  0)
           {
                text_addr = sections_array[ i].start_addr;
                return text_addr;
           }
    }
    return 0;
}

/* read required content from memory area which start address equals to required to addr */
uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes != 0);
    int size8 = sizeof( uint8) * 8;
    int size64 = sizeof( uint64) * 8;
    vector<int> result( size64, 0);
    for( int k = 0; k < sections_array.size(); ++k)
    {
        if ( sections_array[ k].start_addr <= addr < sections_array[ k].start_addr + sections_array[ k].size)
        {
            for ( uint64 counter = sections_array[ k].start_addr;
                  counter < sections_array[k].start_addr + sections_array[ k].size;
                  ++counter)
            {
                if ( counter == addr)
                {

                    for ( int i = 0; i < num_of_bytes; ++i)
                    {

                        vector<int> bytes = toBin( sections_array[ k].content[ addr + i - sections_array[ k].start_addr]);
                        for ( int j = 0; j < size8; ++j)
                        {
                            result[ size64 -( i + 1) * 8 + j] = bytes[ j];
                        }
                    }
                    return fromBin( result);
                }
            }
        }

    }
    abort();
}

/* write value to memory's area which start address equals to addr */
void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes != 0);
    int pver_size = sections_array.size();
    for ( int i = 0; i < sections_array.size(); ++i)
    {
        if ( sections_array[ i].start_addr <= addr < sections_array[ i].start_addr + sections_array[ i].size)
        {
            for ( uint64 counter = sections_array[ i].start_addr;
                  counter < sections_array[ i].start_addr + sections_array[ i].size;
                  ++counter)
            {
                if ( counter == addr)
                {
                    myWrite( sections_array[ i].content,
                             value,
                             addr - sections_array[ i].start_addr,
                             num_of_bytes);
                    return;
                }
            }
        }
    }

    sections_array.push_back( sections_array[ 0]);
    sections_array[ pver_size].start_addr = addr;
    delete[] sections_array[ pver_size].content;
    sections_array[ pver_size].content = new uint8( num_of_bytes);
    for( int i = 0; i < sections_array.size(); ++i)
    {
        if ( sections_array[ i].start_addr <= addr < sections_array[ i].start_addr + sections_array[ i].size)
        {
            for ( uint64 counter = sections_array[ i].start_addr;
                  counter < sections_array[ i].start_addr + sections_array[ i].size;
                  ++counter)
            {
                if ( counter == addr)
                {
                    myWrite(sections_array[ i].content,
                            value,
                            addr - sections_array[ i].start_addr,
                            num_of_bytes);
                    return;
                }
            }
        }
    }
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    for( unsigned int i = 0; i < sections_array.size(); ++i)
    {
        oss << sections_array[ i].dump( indent) << endl;
    }

    return oss.str();
}


