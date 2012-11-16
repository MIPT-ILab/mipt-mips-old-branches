/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstring>
#include <cerrno>
#include <unistd.h>
#include <cstdlib>
#include <cstdio>

// Generic C++
#include <string>
#include <sstream>
#include <map>

// uArchSim modules
#include <func_memory.h>

using namespace std;

typedef map<uint64, ElfSection *>::const_iterator my_iter;

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( executable_file_name);
    assert( num_of_elf_sections > 0);
    //remember in FuncMemory class member the name of executable file
    name = executable_file_name; 
    for ( short i = 0; i < num_of_elf_sections; ++i)
    {
        assert( elf_sections_names[i]);
        ElfSection *tmp = new ElfSection( executable_file_name, elf_sections_names[i]);
        sect_stor[tmp->startAddr()] = tmp;
    }
}

FuncMemory::~FuncMemory()
{
    for (my_iter it = sect_stor.begin(); it != sect_stor.end(); ++it)
    {
        delete (*it).second;
    }
}

string FuncMemory::getName() const
{
    return name;
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    short i = 0;
    for( my_iter it = sect_stor.begin(); it != sect_stor.end(); it++)
    {
        if( (*it).second->isInside(addr, num_of_bytes))
        {
            return (*it).second->read(addr, num_of_bytes);
        } else ++i;
    }   
    perror( "right section isn't found!");
    exit(0);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    
    oss << indent << "Dump FuncMemory of file\t\"" << this->getName() << "\"" << endl;
    for( my_iter it = sect_stor.begin(); it != sect_stor.end(); it++)
    {
        oss << indent << (*it).second->dump( indent) << endl;
    }
    return oss.str();
        
}

