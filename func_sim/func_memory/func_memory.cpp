/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>

// Generic C++
#include <string> 
#include <map>
#include <sstream>
#include <iostream>

// uArchSim modules
#include <func_memory.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( num_of_elf_sections > 0);
    for( short i = 0; i < num_of_elf_sections; ++i)
    {
        ElfSection *current = new ElfSection( executable_file_name, elf_sections_names[ i]);
        if ( ( sections.insert( make_pair( current->startAddr(), current))).second == false)
        {
           cout << "ERROR: Section " << elf_sections_names[ i] << " is mentioned twice." 
                << endl;
           exit( EXIT_FAILURE);
        }
    }
}

FuncMemory::~FuncMemory()
{
    for ( Iter it = sections.begin(); it != sections.end(); ++it)
    {
        delete ( *it).second;
    } 
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    assert( num_of_bytes > 0);
    ConstIter it = sections.upper_bound( addr);
    it--;
    if ( ( ( *it).second)->isInside( addr, num_of_bytes))
    {
        return ( ( *it).second)->read( addr, num_of_bytes);
    }
    assert( 0);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    oss << indent << "Dump FuncMemory : " << endl;  
    for( ConstIter it = sections.begin(); it != sections.end(); ++it)
    {
        oss << ( ( *it).second)->dump( indent + " ") << endl;
    }
    return oss.str();
}

