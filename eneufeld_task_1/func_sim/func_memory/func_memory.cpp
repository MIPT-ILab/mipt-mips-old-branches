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
#include <vector> 
#include <sstream>
#include <iostream>


// uArchSim modules
#include <func_memory.h>


FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    this->sections.reserve( num_of_elf_sections);
            //reserve memory for points at ElfSection's

    this->cout_sections = num_of_elf_sections;
    this->file_name = executable_file_name;

    for( size_t i = 0; i < num_of_elf_sections; i++)
    {
        this->sections[ i] = new ElfSection(executable_file_name, 
                                            elf_sections_names[i]);
                //load all sections
    }
}

FuncMemory::~FuncMemory()
{
    for( size_t i = 0; i < this->cout_sections; i++)
    {
        delete sections[i];
            //delete all sections
    }
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    for( int i = 0; i < cout_sections; i++)
    {
        if( sections[i]->isInside( addr, num_of_bytes))
        {
            return sections[i]->read( addr, num_of_bytes);
        }
    }
        //exiting if can't find section with data 
    cerr << "ERROR: Could not read data from adress 0x" << hex 
         << addr << dec << endl; 
    exit ( EXIT_FAILURE); 
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    oss << indent << "Dump ELF file \"" << this->file_name << "\"" << endl
        << indent << "sections count = " << this->cout_sections << endl
        << indent << "Sections:" << endl;
    for( size_t i = 0; i < cout_sections; i++)
    {
        oss << this->sections[i]->dump( indent);
    }
    oss << "end dump \"" << this->file_name << "\"" << endl;
    
    return oss.str();
}

