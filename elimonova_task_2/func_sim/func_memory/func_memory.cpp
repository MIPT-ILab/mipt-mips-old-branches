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

// uArchSim modules
#include <func_memory.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( num_of_elf_sections > 0);
    ElfSection current;
    for( short i = 0; i < num_of_elf_sections; ++i)
    {
        current = ElfSection( executable_file_name, elf_sections_names[i]);
        sections[ current->startAddr()] = current;
    }
}

FuncMemory::~FuncMemory()
{
    ~sections();
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    assert( num_of_bytes > 0);
    for ( ConstIter it = sections.begin(); it != sections.end(); ++it)
    {
      if ( it->isInside( addr, num_of_bytes))
      {
        return it->read( addr, num_of_bytes);
      }
    }
    cerr << "ERROR: Sequence doen't belong any section. Address : " 
         << addr << " num_of_bytes : " << num_of_bytes << endl;
    exit( EXIT_FAILURE);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    for( ConstIter it = sections.begin(); it != sections.end(); ++it)
    {
      oss << it->dump() << endl;
    }
    return oss.str();
}

