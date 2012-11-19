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
#include <sstream>
#include <iostream>
// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( num_of_elf_sections > 0);
    uint64 i;
    ElfSection* sect;
    for ( i = 0; i < num_of_elf_sections; i++)
    {
        sect = new ElfSection( executable_file_name, elf_sections_names[i]);
        sections[ sect->startAddr()] = sect;
    }
}

FuncMemory::~FuncMemory()
{
    ConstIter itr;
    for ( itr = sections.begin(); itr != sections.end(); itr++)
    {
        delete itr->second;
    }
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    ConstIter itr;
    for ( itr = sections.begin(); itr != sections.end(); itr++)
    {
        if( itr->second->isInside( addr, num_of_bytes))
            return itr->second->read(addr, num_of_bytes);
    }
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    ConstIter itr;
    for ( itr = sections.begin(); itr != sections.end(); itr++)
    {
        oss << itr->second->dump(indent) << endl;
    }
    
    return oss.str();
}

