/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Lyubogoshchev Mikhail <lyubogoshchev@phystech.edu>
 * Copyright 2014 MIPT-MIPS iLab project
 */

// Generic C
#include <cstdio>
#include <cassert>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>

using namespace std;

int FuncMemory::addrCalc( uint64 addr, uint64& set, uint64& page, uint64& offset) const
{
    set = addr >> ( page_bits + offset_bits);
    page = ( addr << ( addr_size - page_bits - offset_bits))
        >> ( addr_size + page_bits);
    offset = ( addr << ( addr_size - offset)) >> ( addr_size - offset);
    if ( set > ( 1 << ( addr_size - page_bits - offset_bits)) || page > ( 1 << page_bits) ||
                                                               offset > ( 1 << offset_bits))
        return NO_VAL32;
    return 0;
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    pages_set = new uint8**[ 1 << ( addr_size - page_bits - offset_bits)];
    assert( pages_set );
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    for ( uint32 i = 0; i < sections_array.size(); i++)
    {
        this->addr_size = addr_size;
        this->page_bits = page_bits;
        this->offset_bits = offset_bits;
        for ( int j = 0; j < sections_array[ i].size; j++)
        {
            if ( !addrCalc( sections_array[ i].start_addr + 4 * j, set, page, offset))
            {
                cout << "Non-existing address\n";
                exit (NO_VAL32);
            }
            if ( pages_set[ set] == NULL)
                pages_set[ set] = new uint8*[ 1 << page_bits];
            assert( pages_set[ set]);
            if ( pages_set[ set][ page] == NULL)
                pages_set[ set][ page] = new uint8[ 1 << offset_bits];
            assert( pages_set[ set][ page]);
            pages_set[ set][ page][ offset] = sections_array[ i].content[j];
        }
        SectionInfo section( sections_array[ i].name, sections_array[ i].size, sections_array[ i].start_addr);
        sections.push_back( section);
    }
}

FuncMemory::~FuncMemory()
{
    for ( int i = 0; i < ( 1 << ( addr_size - page_bits - offset_bits)); i++)
    {
        if ( pages_set[ i] != NULL)
        {
            for ( int j = 0; j < ( 1 << page_bits); j++)
            {
                delete [] pages_set[ i][ j];
            }
        }
    }
}
uint64 FuncMemory::startPC() const
{
    uint32 i = 0;
    for ( i = 0; i < sections.size() && strcmp( sections[ i].name, ".text"); i++);
    if ( i < sections.size())
        return sections[ i].start_addr;
    else 
        return NO_VAL64;
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    uint64 result = 0;
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        addrCalc( addr + 4 * i, set, page, offset);
        result += reverse( pages_set[ set][ page][ offset]) << ( ( i) * 8);
        }
    return result;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        addrCalc( addr + 4 * i, set, page, offset);
        pages_set[ set][ page][ offset] = reverse ( uint8 (value & ( 0xFF << ( i * 8))));
    }
}

string FuncMemory::dump( string indent) const
{
    cout << "Dumping FuncMemory Structure";
    cout << "Reason: " << indent << endl;
    for( uint32 i = 0; i < sections.size(); i++)
    {
        cout << "sections[i].name = " << sections[i].name << ", " << "size = " << sections[i].size 
        << ", " << "start_addr = " << sections[i].start_addr << endl;
    }
    int addr = 0;
    for ( int i = 0; i < ( 1 << ( addr_size - page_bits - offset_bits)); i++)
    {
        if ( pages_set[ i] != NULL)
        {
            cout << "Set number = " << i << endl;
            for ( int j = 0; j < ( 1 << page_bits ); j++)
            {
                if ( pages_set[ i][ j] != NULL)
                {
                    cout << "Page number = " << j << endl;
                    for ( int k = 0; k < ( 1 << offset_bits); k++)
                    {
                        addr = i * ( 1 << ( offset_bits + page_bits)) + j * ( 1 << ( offset_bits)) + k;
                        cout << "0x" << hex << addr << ":\t" << hex << reverse( pages_set[ i][ j][ k]) << endl;
                    }
                }
            }
        }
    }
    return string("ERROR: You need to implement FuncMemory!");
}


