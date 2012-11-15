/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>

// Generic C++
#include <sstream>
#include <string>
#include <map>

// uArchSim modules
#include <func_memory.h>
#include <types.h>
#include <elf_parser.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    //validity check of arguments
    assert( num_of_elf_sections);
    assert( elf_sections_names);
    assert( executable_file_name);

    // fill sections map with given sections
    for ( short i = 0; i < num_of_elf_sections; ++i)
    {
        ElfSection* section_pointer = new ElfSection( executable_file_name,
                                                      elf_sections_names[ i]);
       
        // fill the map
        this->sections[ section_pointer->startAddr()] = section_pointer;
    }
}

FuncMemory::~FuncMemory()
{
    // delete each mapped value, i.e. ElfSection* 
    for ( Iter it = ( this->sections).begin();
          it != ( this->sections).end();
          ++it)
    {
        delete it->second;
    }
    
    //free map
    ( this->sections).clear();
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    //validity check of arguments
    assert( ( num_of_bytes > 0) && ( num_of_bytes <= sizeof( uint64)));
    assert( addr);

    // look through all given sections and try to find section which contains
    // the address addr and read from the one
    for ( ConstIter it = ( this->sections).begin();
          it != ( this->sections).end();
          ++it)
    {
        // belonging check of only address addr
        if ( ( it->second)->isInside( addr, 1))
        {
            return ( it->second)->read( addr, num_of_bytes);
        }
    }

    // it will be executed
    // if address addr does not belong to any section from given
    assert( 0);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    for ( ConstIter it = ( this->sections).begin();
          it != ( this->sections).end();
          ++it)
    {
        oss << ( it->second)->dump( indent) << endl;
    }

    return oss.str();
}

