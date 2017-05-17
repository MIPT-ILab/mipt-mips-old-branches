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
#include <iostream>
#include <sstream>


// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    // Change it with your implementation.
    int i;
    assert( num_of_elf_sections > 0);
    for( i= 0; i < num_of_elf_sections; i++)
    {
        assert( elf_sections_names[i]);
    }
    sections = new ElfSection* [num_of_elf_sections];
    section_num = num_of_elf_sections;
    filename = executable_file_name;
    for( i = 0; i < num_of_elf_sections; i++ )
    {
        sections[i] = new ElfSection ( executable_file_name, elf_sections_names[i]);
    }
}

FuncMemory::~FuncMemory()
{
    // Change it with your implementation.
    int i;
    for( i = 0; i < section_num; i++) delete sections[i];
    delete [] sections;
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    // Change it with your implementation.
    int i;
    for( i = 0; i < section_num; i++)
    {
        if( sections[i]->isInside( addr, num_of_bytes))
        {
            return sections[i]->read( addr, num_of_bytes);
        }
    }
    cerr<<"mistake in addres:"<<addr<<" and number of bytes"<<num_of_bytes<<endl;
    return NO_VAL64;
}

string FuncMemory::dump( string indent) const
{
    // Change it with your implementation
    ostringstream oss;
    int i;
    oss << indent << "Dump file \"" << filename <<"\""<< endl;
        for( i = 0; i < section_num; i++)
        {
            oss << sections[i]->dump( indent) << endl;
        }

    return oss.str();
}



