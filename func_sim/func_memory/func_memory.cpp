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

// Generic C++
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>

// uArchSim modules
#include <func_memory.h>

using namespace std;

int FuncMemory::addrCalc( uint64 addr, uint64& set, uint64& page, uint64& offset) const
{
    set = addr >> ( page_bits + offset_bits);
    page = ( addr << ( sizeof( uint64) * 8 - page_bits - offset_bits)) >> ( sizeof( uint64) * 8 - page_bits);
    offset = ( addr << ( sizeof( uint64) * 8 - offset_bits)) >> ( sizeof( uint64) * 8
                                                                         - offset_bits);
//    cout << hex << addr << "\t" << set << " " << page << " " << offset << "\t";
    if ( ( set > ( 1 << ( addr_size - page_bits - offset_bits))) || ( page >
                     ( 1 << page_bits)) || ( offset > ( 1 << offset_bits)))
        return NO_VAL32;
    return 0;
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_num_size,
                        uint64 offset_size)
{
    if ( !addr_size || !page_num_size || !offset_size )
        exit( NO_VAL32);
    int f_diskr = 0;
    if ( ( f_diskr = open( executable_file_name, O_RDONLY)) < 0 )
        exit( EXIT_FAILURE);
    close (f_diskr);
    cout << executable_file_name << " " << addr_size << " " << page_num_size << " " << offset_size << endl;
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
//   for ( int i = 0; i < sections_array.size(); i++ )
//     {   cout << sections_array[i].dump( "\t") << endl;
    //for ( int i = 0; i < sections_array.size(); i++)
//        for ( int j = 0; j < sections_array[ i].size; j++ )
//            cout << hex << (uint16) *( sections_array[ i].content + j) << endl;}
  //  cout << "1st constr" << endl;
    pages_set = new uint8**[ 1 << ( addr_size - page_num_size - offset_size)];
//    cout << "Created" << endl;
    memset( pages_set, 0, ( 1 << ( addr_size - page_num_size - offset_size)));
//    cout << "Set" << endl;
    assert( pages_set );
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    for ( uint32 i = 0; i < sections_array.size(); i++)
    {
//        cout << "1st cycle, hey" << endl;
        this->addr_size = addr_size;
        this->page_bits = page_num_size;
        this->offset_bits = offset_size;
//        cout << "1st cycle" << endl;
//        cout << sections_array[ i].size << endl;
        for ( int j = 0; j < sections_array[ i].size; j++)
        {
//            cout << "j = " << j << endl;
            if ( addrCalc( sections_array[ i].start_addr + j, set, page, offset))
            {
                cout << "Non-existing address\n";
                exit( EXIT_FAILURE);
            }
           // cout << "2nd cycle" << endl;
            if ( pages_set[ set] == NULL)
            {
                pages_set[ set] = new uint8*[ 1 << page_bits];
//                cout << "Created set " << i << j << endl;
                memset( pages_set[ set], 0, 1 << page_bits);
//                cout << "Set " << i << j<< endl;
            }
           // cout << "2nd cycle 2nd if" << endl;
            assert( pages_set[ set]);
            if ( pages_set[ set][ page] == NULL)
            {
                pages_set[ set][ page] = new uint8[ 1 << offset_bits];
//                cout << "Created page " << i << j << endl;
                memset( pages_set[ set][ page], 0, 1 << offset_bits);
//                cout << "Set " << i << j << endl;
            }
           // cout << "2nd cycle 3rd if" << endl;
            assert( pages_set[ set][ page]);
//            cout << hex << (uint16) *(sections_array[ i].content + j) << "\t" << offset  << "\t" << (uint64)&pages_set[ set][ page][ offset] << "\t";
            pages_set[ set][ page][ offset] = sections_array[ i].content[ j];
//            cout << "Claimed " << i << j << endl;
//            cout << (uint16) pages_set[ set][ page][ offset] << endl;

        }
//        cout << "1st cycle" << endl;
        SectionInfo section( sections_array[ i].name, sections_array[ i].size, sections_array[ i].start_addr);
  //      cout << "I'm here" << endl;
        sections.push_back( section);
//        cout << "And here " << endl;
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
    for ( i = 0; i < sections.size() &&  sections[ i].name.compare( ".text"); i++);
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
    cout << "DUMP" << endl;
    ostringstream oss;
    oss << "Dumping FuncMemory Structure. ";
    oss << "Reason: " << indent << endl;
    for( uint32 i = 0; i < sections.size(); i++)
    {
        oss << "sections[" << i << "].name = " << sections[i].name << ", " 
            << "size = " << sections[i].size << ", " << "start_addr = " 
            << sections[i].start_addr << endl;
    }
    int addr = 0;
   /* for ( uint64 i = 0; i < ( 1 << ( addr_size - page_bits - offset_bits)); i++)
    {
        cout << "For" << endl;
        if ( pages_set[ i] != NULL)
        {
            cout << "For if" << endl;
            oss << "Set number = " << i << endl;
            for ( uint64 j = 0; j < ( 1 << page_bits ); j++)
            {
                cout << "For if for" << endl;
                if ( pages_set[ i][ j] != NULL)
                {
                    cout << "For if for if" << endl;
                    oss << "Page number = " << j << endl;
                    for ( uint64 k = 0; k < ( 1 << offset_bits); k++)
                    {
                        cout << "For if for if for" << endl;
                        addr = i * ( 1 << ( offset_bits + page_bits)) + j * ( 1 << ( offset_bits)) + k;
                        oss << setfill( '0') << showbase << hex << addr << ":\t" << 
                                noshowbase << setw(2) << 
                                ( uint16) reverse( pages_set[ i][ j][ k]) << endl;
                    }
                }
            }
        }
    }*/
    return oss.str();
}


