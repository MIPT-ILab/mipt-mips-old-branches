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

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    // validity check of arguments
    assert( num_of_elf_sections);
    assert( elf_sections_names);
    assert( executable_file_name);

    // fill sections map with given sections
    for ( short i = 0; i < num_of_elf_sections; ++i)
    {
        ElfSection* section_pointer = new ElfSection( executable_file_name,
                                                      elf_sections_names[ i]);
        // duplicate section name check 
        assert( this->sections.count( section_pointer->startAddr()) == 0);

        // fill the map
        this->sections[ section_pointer->startAddr()] = section_pointer;
    }
}

FuncMemory::~FuncMemory()
{
    // delete each mapped value, i.e. ElfSection* 
    for ( Iter it = this->sections.begin();
          it != this->sections.end();
          ++it)
    {
        delete it->second;
    }
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    // validity check of arguments
    assert( ( num_of_bytes > 0) && ( num_of_bytes <= sizeof( uint64)));

    /*
     * get iterator which point to a section
     * whose start address is greater than addr
     */
    ConstIter itup = this->sections.upper_bound( addr);

    // abort if addr is less than start address of the first section
    assert( itup != this->sections.begin());

    // decrease to recieve a pointer to a section which can consist of addr
    itup--;

    return itup->second->read( addr, num_of_bytes);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    for ( ConstIter it = this->sections.begin();
          it != this->sections.end();
          ++it)
    {
        oss << it->second->dump( indent) << endl;
    }

    return oss.str();
}

