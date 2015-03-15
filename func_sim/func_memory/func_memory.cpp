/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Lyubogoshchev Mikhail <lyubogoshchev@phystech.edu>
 * Copyright 2014 MIPT-MIPS iLab project
 */

// Generic C
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// uArchSim modules
#include <func_memory.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_num_size,
                        uint64 offset_size)
{
    assert( addr_size || page_num_size || offset_size);
    this->addr_size = addr_size;
    this->page_bits = page_num_size;
    this->offset_bits = offset_size;
    sets_n = 1L << ( addr_size - page_num_size - offset_size);
    pages_n = 1L << page_num_size;
    offset_n = 1L << offset_size;

    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    pages_set = new uint8**[ sets_n];
    memset( pages_set, 0, sets_n);
    assert( pages_set);

    for ( uint64 i = 0; i < sections_array.size(); i++)
    {
        for ( uint64 j = 0; j < sections_array[ i].size; j++)
            write( sections_array[ i].content[ j], sections_array[ i].start_addr + 8 * j, 1);

        SectionInfo section( sections_array[ i].name, sections_array[ i].size,
                                               sections_array[ i].start_addr);
        if ( !section.name.compare( ".text"))
            text_addr = sections_array[ i].start_addr;
        sections.push_back( section);
    }
}

FuncMemory::~FuncMemory()
{/*
    if ( pages_set)
    {
        for ( uint64 i = 0; i < sets_n; i++)
        {
            if ( pages_set[ i])
            {
                for ( uint64 j = 0; j < ( 1L << page_bits); j++)
                {
                    if ( pages_set[ i][ j])
                    {
                        delete[] pages_set[ i][ j];
                    }
                }
            }
            delete[] pages_set[ i];
        }
    }
    delete[] pages_set;*/
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes != 0 );
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    uint64 result = 0;
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        addrCalc( addr + i, set, page, offset);
        assert( pages_set && pages_set[ set] && pages_set[ set][ page]);
        result += ( ( uint64) ( pages_set[ set][ page][ offset])) << 8 * i;
    }
    return result;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{   
    assert( num_of_bytes != 0);
    assert( pages_set);
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        if ( addrCalc( addr + i, set, page, offset))
        {
            cout << "Non-existing address\n";
            return;
        }
        if ( !pages_set[ set])
        {
            pages_set[ set] = new uint8*[ pages_n];
            memset( pages_set[ set], 0, pages_n);
        }
        assert( pages_set[ set]);
        if ( !pages_set[ set][ page])
        {
            pages_set[ set][ page] = new uint8[ offset_n];
           // memset( pages_set[ set][ page], 0, offset_n);
        }
        assert( pages_set[ set][ page]);
        pages_set[ set][ page][ offset] = ( uint8)( ( value & ( 0xFFL << ( i * 8)))>> ( i * 8));
        //cout <<hex<< addr<< "\t"<<(uint16) pages_set[set][page][offset] << "\t" << (uint16)value << endl;
    }
}


string FuncMemory::dump( string indent) const
{
    cout << "DUMP" << endl;
    ostringstream oss;
    oss << "Dumping FuncMemory Structure. ";
    oss << "Reason: " << indent << endl;
    uint64 set, page, offset;
    for( uint32 i = 0; i < sections.size(); i++)
    {
        oss << "sections[" << i << "].name = " << sections[i].name << ", " 
            << "size = " << sections[i].size << ", " << hex  << "start_addr = " 
            << sections[i].start_addr << endl;
        for ( uint64 j = 0; j < sections[ i].size; j++)
        {
            if ( addrCalc( sections[ i].start_addr + 8 * j, set, page, offset))
            {
                oss << "Unknown adress" << endl;
                return oss.str();
            }
            oss << setfill( '0') << showbase << "\t" << hex 
                << sections[ i].start_addr + 8 * j << ":\t"
                << noshowbase << setw(2)
                << ( uint16)(reverse( pages_set[ set][ page][ offset])) 
                << endl; 
        }
    }
    return oss.str();
}


string FuncMemory::dumpFull( string indent) const
{
    cout << "DUMP" << endl;
    ostringstream oss;
    oss << "Dumping FuncMemory Structure. ";
    oss << "Reason: " << indent << endl;
    for( uint32 i = 0; i < sections.size(); i++)
    {
        oss << "sections[" << i << "].name = " << sections[i].name << ", "
            << "size = " << sections[i].size << ", " << hex  << "start_addr = "
            << sections[i].start_addr << endl;
    }
    int addr = 0;
    for ( uint64 i = 0; i < sets_n; i++)
    {
        if ( pages_set[ i])
        {
            oss << "Set number = " << i << endl;
            for ( uint64 j = 0; j < pages_n; j++)
            {
                if ( pages_set[ i][ j])
                {
                    oss << "Page number = " << j << endl;
                    for ( uint64 k = 0; k < offset_n; k++)
                    {
                        addr = i * sets_n + j * offset_n + k;
                        oss << setfill( '0') << showbase << hex << addr << ":\t"
                            << noshowbase << setw(2)
                            << ( uint16) reverse( pages_set[ i][ j][ k]) << endl;
                    }
                }
            }
        }
    }
    return oss.str();
}


int FuncMemory::addrCalc( uint64 addr, uint64& set, uint64& page, uint64& offset) const
{
    set = addr >> ( page_bits + offset_bits);
    page = ( addr << ( sizeof( uint64) * 8 - page_bits - offset_bits)) >>
                                        ( sizeof( uint64) * 8 - page_bits);
    offset = ( addr << ( sizeof( uint64) * 8 - offset_bits)) >>
                        ( sizeof( uint64) * 8 - offset_bits);
    if ( ( set > sets_n) || ( page > pages_n) || ( offset > offset_n))
        return NO_VAL32;
    return 0;
}

inline uint8 FuncMemory::reverse( uint8 in) const
{
    in = ( ( in & 0x55) << 1) | ( ( in >> 1) & 0x55);
    in = ( ( in & 0x33) << 2) | ( ( in >> 2) & 0x33);
    in = ( ( in & 0x0F) << 4) | ( ( in >> 4) & 0x0F);
    return in;
}
