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

int FuncMemory::addrCalc( uint64 addr, uint64& set, uint64& page, uint64& offset) const
{
    //cout << sizeof( uint64) << endl;
    set = addr >> ( page_bits + offset_bits);
    page = ( addr << ( sizeof( uint64) * 8 - page_bits - offset_bits)) >> ( sizeof( uint64) * 8 - page_bits);
    offset = ( addr << ( sizeof( uint64) * 8 - offset_bits)) >> ( sizeof( uint64) * 8
                                                                         - offset_bits);
   // cout << hex << addr << "\t" << set << " " << page << " " << offset << "\t";
    if ( ( set > ( 1L << ( addr_size - page_bits - offset_bits))) || ( page >
                     ( 1L << page_bits)) || ( offset > ( 1L << offset_bits)))
        return NO_VAL32;
    return 0;
}

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_num_size,
                        uint64 offset_size)
{
     cout << executable_file_name << " " << addr_size << " " << page_num_size << " " << offset_size << endl;
    assert( addr_size || page_num_size || offset_size );
   // int f_diskr = open( executable_file_name, O_RDONLY);
   // assert( f_diskr > 0 );
   // close (f_diskr);
   // cout << executable_file_name << " " << addr_size << " " << page_num_size << " " << offset_size << endl;
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
  // for ( int i = 0; i < sections_array.size(); i++ )
   //  {   cout << sections_array[i].dump( "\t") << endl; }
    //for ( int i = 0; i < sections_array.size(); i++)
//        for ( int j = 0; j < sections_array[ i].size; j++ )
//            cout << hex << (uint16) *( sections_array[ i].content + j) << endl;}
    //cout << "1st constr" << endl;
    pages_set = new uint8**[ 1L << ( addr_size - page_num_size - offset_size)];
   // cout << "Created" << endl;
    memset( pages_set, 0, ( 1L << ( addr_size - page_num_size - offset_size)));
    //cout << "Set" << endl;
    assert( pages_set );
    uint64 set = 0L;
    uint64 page = 0L;
    uint64 offset = 0L;
    for ( uint64 i = 0; i < sections_array.size(); i++)
    {
        
       // cout << "1st cycle, i = " << i << endl;
        this->addr_size = addr_size;
        this->page_bits = page_num_size;
        this->offset_bits = offset_size;
       // cout << "1st cycle" << endl;
       // cout << sections_array[ i].size << endl;
        for ( int j = 0; j < sections_array[ i].size; j++)
        {
        //    cout << "j = " << j << endl;
            if ( addrCalc( sections_array[ i].start_addr + j, set, page, offset))
            {
                cout << "Non-existing address\n";
                exit( EXIT_FAILURE);
            }
         //   cout << "2nd cycle" << endl;
            if ( pages_set[ set] == NULL)
            {
                pages_set[ set] = new uint8*[ 1L << page_bits];
               // cout << "Created set " << i << j << endl;
                memset( pages_set[ set], 0, 1L << page_bits);
             //   cout << "Set " << i << j<< endl;
            }
           // cout << "2nd cycle 2nd if" << endl;
            assert( pages_set[ set]);
            if ( pages_set[ set][ page] == NULL)
            {
                pages_set[ set][ page] = new uint8[ 1L << offset_bits];
             //   cout << "Created page " << i << j << endl;
                //memset( pages_set[ set][ page], 0, 1L << offset_bits);
           //     cout << "Set " << i << j << endl;
            }
         //   cout << "2nd cycle 3rd if" << endl;
            assert( pages_set[ set][ page]);
//            cout << hex << (uint16) *(sections_array[ i].content + j) << "\t" << offset  << "\t" << (uint64)&pages_set[ set][ page][ offset] << "\t";
            pages_set[ set][ page][ offset] = sections_array[ i].content[ j];
//            cout << "Claimed " << i << j << endl;
//            cout << (uint16) pages_set[ set][ page][ offset] << endl;

        }
       // cout << "1st cycle" << endl;
        SectionInfo section( sections_array[ i].name, sections_array[ i].size, sections_array[ i].start_addr);
//        cout << hex << sections_array[ i].start_addr << endl;
        if ( !section.name.compare( ".text"))
        {
            text_addr = sections_array[ i].start_addr;
//            cout << text_addr << endl; 
        //cout << "I'm here" << endl;
        sections.push_back( section);
       // cout << "And here " << endl;
        }
    }
}

FuncMemory::~FuncMemory()
{
    for ( uint64 i = 0; i < ( 1L << ( addr_size - page_bits - offset_bits)); i++)
    {
        if ( pages_set[ i] != NULL)
        {
            for ( uint64 j = 0; j < ( 1L << page_bits); j++)
            {
                delete [] pages_set[ i][ j];
            }
        }
    }
}

/*uint64 FuncMemory::startPC() const
{
    uint32 i = 0;
    for ( i = 0; i < sections.size() && sections[ i].name.compare( ".text"); i++);
    if ( i < sections.size())
        return sections[ i].start_addr;
    else 
        return NO_VAL64;
}*/

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert ( num_of_bytes != 0 );
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    uint64 result = 0;
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        cout << "i = " << i << endl;
        addrCalc( addr + i, set, page, offset);
       // assert( !addrCalc( addr + i, set, page, offset));
        assert ( pages_set && pages_set[ set] && pages_set[ set][ page]);
       // {
         //   cerr << "ERROR. uninit address" << SIGABRT <<endl;
           // abort();
      //  }
       // cout << hex << (uint64)(addr + i) << "\t" << ( uint16) pages_set[ set][ page][ offset] << endl;
        result += ( ( uint64) ( pages_set[ set][ page][ offset])) << 8 * i;
        cout << hex << (uint64)(addr + i) << "\t" << ( uint16) pages_set[ set][ page][ offset] << "\t" << result << endl;
    }
    
    return result;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
   // cout << "value = " << hex << value << endl;
    assert( num_of_bytes != 0);
    uint64 set = 0;
    uint64 page = 0;
    uint64 offset = 0;
    for ( unsigned short i = 0; i < num_of_bytes; i++)
    {
        cout << "i = " << i << "\t";
        addrCalc( addr + i, set, page, offset);
        if ( pages_set)
        {
            if ( pages_set[ set] )
            {
                if ( pages_set[ set][ page])
                {
                    cout << "3rd if" << endl;
                    pages_set[ set][ page][ offset] =  ( uint8) ( ( value & ( 0xFFL << ( i * 8)))>> ( i * 8));
                    cout << hex << (uint64)(addr + i) << "\t" << ( uint16) pages_set[ set][ page][ offset] << endl;
                } else {
                    cout << "3rd else" << endl;
                    pages_set[ set][ page] = new uint8[ 1L << ( offset_bits)];
                    pages_set[ set][ page][ offset] = ( ( uint8) ( ( value & ( 0xFFL << ( i * 8))) >> ( i * 8)));
                    cout << hex << (uint64)(addr + i) << "\t" << ( uint16) pages_set[ set][ page][ offset] << "\t" << value << endl;
                }
            } else {
                cout << "2nd else" << endl;
                pages_set[ set] = new uint8*[ 1L << page_bits];
                pages_set[ set][ page] = new uint8[ 1L << ( offset_bits)];
                pages_set[ set][ page][ offset] = ( ( uint8) ( ( value & ( 0xFFL << ( i * 8))) >> ( i * 8)));
                cout << hex << (uint64)(addr + i) << "\t" << ( uint16) pages_set[ set][ page][ offset] << value << endl;
            }
        }
    }
    cout << "RETURN" << endl;

     //   cout << hex << (uint64)(addr + i) << "\t" << ( uint16) pages_set[ set][ page][ offset] << value << endl;
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
            << "size = " << sections[i].size << ", " << hex  << "start_addr = " 
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


