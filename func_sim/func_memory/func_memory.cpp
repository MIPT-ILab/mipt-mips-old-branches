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
#include <iostream>
#include <string> 
#include <sstream>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( num_of_elf_sections > 0);

    for ( short i = 0; i < num_of_elf_sections; ++i) {
        ElfSection* section = new ElfSection(executable_file_name, 
            elf_sections_names[i]);
        sections[section->startAddr()] = section;
    }
}

FuncMemory::~FuncMemory()
{
    for ( ConstIter it = sections.begin(); it != sections.end(); it++) {
        delete it->second;
    }
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    assert( num_of_bytes > 0);

    for ( ConstIter it = sections.begin(); it != sections.end(); it++) {
        ElfSection *section = it->second;
        if ( section->isInside( addr, num_of_bytes)) {
            return section->read( addr, num_of_bytes);
        }
    }

    cerr << "ERROR: address doesn't belong to any section: " 
         << hex << addr << endl;
    exit( EXIT_FAILURE);

    return 0; 
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    for ( ConstIter it = sections.begin(); it != sections.end(); it++) {
        ElfSection *section = it->second;
        oss << section->dump() << endl;
    }

    return oss.str();
}

