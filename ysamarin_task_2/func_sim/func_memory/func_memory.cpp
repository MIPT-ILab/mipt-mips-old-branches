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

typedef map<uint64, ElfSection& >::const_iterator my_iter_const;
typedef map<uint64, ElfSection& >::iterator my_iter;

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( executable_file_name);
    
    func_memory_name = executable_file_name; //remember the name of executable file
    for ( short i = 0; i < num_of_elf_sections; ++i)
    {
        assert( elf_sections_names[i]);
        ElfSection *tmp = new ElfSection( executable_file_name, elf_sections_names[i]);
        
        pair<my_iter, bool> insert_ret = sect_stor.insert
        ( pair<uint64, ElfSection& >( tmp->startAddr(), *tmp));
        if ( insert_ret.second == false)
        {
            perror( "twice written sections in the list of names!");
            exit( EXIT_FAILURE);
        }       
    }
}

FuncMemory::~FuncMemory()
{
    for ( my_iter it = sect_stor.begin(); it != sect_stor.end(); ++it)
    {
        delete &( it->second);
    }
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    my_iter_const it = sect_stor.upper_bound( addr);
    if ( it != sect_stor.begin())
    {
        return (--it)->second.read( addr, num_of_bytes); //read from elf_parser check addr itself
    }
    perror( "right section isn't found!");
    exit( EXIT_FAILURE);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    
    oss << indent << endl << "Dump FuncMemory of file\t\"" << this->func_memory_name << "\"" << endl;
    for( my_iter_const it = sect_stor.begin(); it != sect_stor.end(); it++)
    {
        oss << indent << it->second.dump( indent) << endl;
    }
    return oss.str();
        
}

