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

// uArchSim modules
#include <func_memory.h>
FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert (num_of_elf_sections > 0);
    for (short i = 0; i < num_of_elf_sections; i++)
    {
         ElfSection *es = new ElfSection ( executable_file_name, elf_sections_names[i]);
         this->sections[ es->startAddr()] = es;
    }


}

FuncMemory::~FuncMemory()
{
    for ( ConstIter it = this->sections.begin(); it != this->sections.end(); it++) delete it->second;
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    ConstIter it = this->sections.find( addr);
    
    return it->second->read( addr, num_of_bytes);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    for (  ConstIter it = this->sections.begin(); it != this->sections.end(); it++) oss << it->second->dump( indent) <<endl;
    return oss.str();
}
