/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <math.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

// Generic C++
#include <iostream>
#include <vector>
#include <sstream>
#include <exception>


// uArchSim modules
#include "func_memory.h"
#include "../elf_parser/elf_parser.h"

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits):
        ArraySet(1 << (addr_size - page_bits - offset_bits)),
        _addr_size(addr_size), _page_bits(page_bits), _offset_bits(offset_bits)
{
    assert(ArraySet.size() > 0);
    assert(executable_file_name != NULL);
    assert(strlen(executable_file_name));
    assert(addr_size > page_bits + offset_bits);
    vector <ElfSection> sections_array;
    ElfSection::getAllElfSections(executable_file_name, sections_array);
    for (int i = 0; i < sections_array.size(); i++)
    {
        if (!strcmp( sections_array.at( i).name, ".text"))
        {
            _segm_text = sections_array.at(i).start_addr;
        }
        for (uint64 j = 0; j < sections_array.at( i).size; j++)
        {
            vector <uint64> numSet = getAddr(sections_array.at( i).start_addr + j, addr_size, page_bits, offset_bits);
            assert( numSet.at( 0) < ArraySet.size());
            assert( numSet.size() == 3);
            checkAddrAndAdd( numSet);
            assert( ArraySet.size() > numSet.at( 0));
            assert( ArraySet.at( numSet.at( 0))->size() > numSet.at( 1));
            if( ArraySet.at( numSet.at( 0))->at( numSet.at( 1))->size() > numSet.at( 2))
            {
                ArraySet.at(numSet.at(0))->
                        at(numSet.at(1))->
                        at(numSet.at(2)) = sections_array.at(i).content[j];
            }
            else
            {
                cerr << "Warling : Not enough memory for address 0x" << hex << sections_array.at( i).start_addr + j << dec << endl;
            }
        }
    }
}

FuncMemory::~FuncMemory()
{
    for( uint64 i = 0; i < ArraySet.size(); i++)
    {
        if( ArraySet.at( i) != NULL) {
            for ( uint64 j = 0; j < ArraySet.at( i)->size(); j++)
            {
                delete ArraySet.at( i)->at( j);
            }
        }
        delete ArraySet.at( i);
    }
}

uint64 FuncMemory::startPC() const
{
    return _segm_text;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( addr > 0);
    assert( num_of_bytes > 0);
    if( ! addr)
    {
        cerr << "read from NULL address" << endl;
        exit( EXIT_FAILURE);
    }
    if( ! num_of_bytes)
    {
        cerr << "Can not to read 0 bytes" << endl;
        exit( EXIT_FAILURE);
    }
    uint64 value = 0;
    for( unsigned short i = 0; i < num_of_bytes; i++) {
        vector<uint64> myAddr = getAddr( addr + i, _addr_size, _page_bits, _offset_bits);
        if( checkAddr( myAddr)) {
            value = value | ( ( ArraySet.at( myAddr[ 0])->at( myAddr[ 1])->at( myAddr[ 2]) ) << (i * BITS_IN_BYTE) );
        }
        else
        {
            cerr << "Not initializate address 0x" << hex << addr + i << dec << endl;
            abort();
            exit( EXIT_FAILURE);
        }
    }
    return value;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( addr > 0);
    assert( num_of_bytes > 0);
    if( addr == 0)
    {
        cerr << "write to NULL address" << endl;
        exit( EXIT_FAILURE);
    }
    if( ! num_of_bytes)
    {
        cerr << "Can not to read 0 bytes" << endl;
        exit( EXIT_FAILURE);
    }
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        uint8 byte = value % ( 1 << ( sizeof(uint8) * BITS_IN_BYTE ) );
        value = value >> ( sizeof(uint8) * BITS_IN_BYTE );
        vector <uint64> myAddr = getAddr( addr + i);
        checkAddrAndAdd( myAddr);
        assert( ArraySet.at( myAddr.at( 0)) != NULL);
        assert( ArraySet.at( myAddr.at( 0))->at(myAddr.at( 1)) != NULL);
        ArraySet.at( myAddr.at( 0))->at( myAddr.at( 1))->at( myAddr.at( 2)) = byte;
    }
}
string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    bool is_skipSet = 0;
    for( uint64 i = 0; i < ArraySet.size(); i++)
    {
        if( ArraySet.at( i) != NULL)
        {
            is_skipSet = 0;
            bool is_skipPage = 0;
            oss << indent << hex << ArraySet.at( i) << dec << endl;
            for( uint64 j = 0; j < ArraySet.at( i)->size(); j++)
            {
                if( ArraySet.at( i)->at( j) != NULL)
                {
                    is_skipPage = 0;
                    bool is_skipVal = 0;
                    oss << indent << "    0x" << hex << ArraySet.at( i)->at( j) << dec << endl;
                    for(uint64 k = 0; k < ArraySet.at( i)->at( j)->size(); k++)
                    {
                        if(ArraySet.at( i)->at( j)->at( k) != 0)
                        {
                            is_skipVal = 0;
                            oss << indent << "    0x" << hex << ( ( i << ( _page_bits + _offset_bits)) + ( j << _offset_bits) + k) << dec << ":"
                                << indent << "    0x" << hex << ArraySet.at( i)->at( j)->at( k) << dec << endl;
                        }
                        else {
                            if( ! is_skipVal)
                            {
                                oss << indent << "    ...." << endl;
                                is_skipVal = 1;
                            }
                        }
                    }
                }
                else
                {
                    if( ! is_skipPage)
                    {
                        is_skipPage = 1;
                    }
                }
            }
        }
        else
        {
            if( ! is_skipSet)
            {
                is_skipSet = 1;
            }
        }
    }
    return oss.str();
}

uint64 FuncMemory::getMask(const uint64 num_digit) const{
    uint64 mask = 0;
    for(uint64 i = 0; i < num_digit && i < _addr_size; i++) {
        mask = (mask << 1) + 1;
    }
    return mask;
}

vector<uint64> FuncMemory::getAddr( const uint64 full_addr,
                                    const uint64 addr_size,
                                    const uint64 page_bits,
                                    const uint64 offset_bits) const
{
    vector<uint64> addr( 3);
    addr.at(0) = (full_addr >> (page_bits + offset_bits)) & getMask(addr_size - page_bits - offset_bits);
    addr.at(1) = (full_addr >> (offset_bits))             & getMask(page_bits);
    addr.at(2) =  full_addr                               & getMask(offset_bits);
    return addr;
}

vector<uint64> FuncMemory::getAddr( const uint64 full_addr) const
{
    vector<uint64> addr( 3);
    addr.at(0) = (full_addr >> ( _page_bits + _offset_bits)) & getMask( _addr_size - _page_bits - _offset_bits);
    addr.at(1) = (full_addr >> ( _offset_bits))              & getMask( _page_bits);
    addr.at(2) =  full_addr                                  & getMask( _offset_bits);
    return addr;
}

bool FuncMemory::checkAddrAndAdd(const vector<uint64> &addr)
{
    assert( addr.size() == 3);
    if ( ArraySet.at( addr.at( 0)) == NULL)
    {
        ArraySet.at(addr.at(0)) = new Page(1 << _page_bits);
        assert(ArraySet.at(addr.at(0)) != NULL);
    }
    assert( ArraySet.at( addr.at( 0))->size() > addr.at( 1));
    if ( ArraySet.at( addr.at( 0))->at( addr.at( 1)) == NULL) {
        uint64 sizeCell = 1 << _offset_bits;
        ArraySet.at( addr.at( 0))->at( addr.at( 1)) = new Cell( sizeCell);
        assert( ArraySet.at( addr.at( 0))->at( addr.at( 1)) != NULL);
        if( ArraySet.at( addr.at( 0))->at( addr.at( 1))->size() < sizeCell )
        {
            cerr << "Warling : Memory limit. Can't allocate enough memory!" << endl;
        }
    }
    return true;
}

bool FuncMemory::checkAddr(const vector<uint64> &addr) const
{
    if( ArraySet.at( addr.at( 0)) == NULL)
    {
        return false;
    }
    if( ArraySet.at( addr.at( 0))->at( addr.at( 1)) == NULL)
    {
        return false;
    }
    return true;
}