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
#include <set>
#include <iostream>
#include <string> 
#include <sstream>
#include <algorithm>

// uArchSim modules
#include <func_memory.h>
#include <elf_parser.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( num_of_elf_sections > 0);

    set<string> section_names;
    for ( short i = 0; i < num_of_elf_sections; ++i) 
    {
	pair<set<string>::iterator, bool> res = section_names.insert( elf_sections_names[i]);
	if ( !res.second)
        {
            cerr << "ERROR: section '" << elf_sections_names[i] 
                 << "' requested several times." << endl;
            exit( EXIT_FAILURE);
        }

        ElfSection* section = new ElfSection( executable_file_name, 
            elf_sections_names[i]);

        sections.push_back( section);
    }
}

FuncMemory::~FuncMemory()
{
    for ( ConstIter it = sections.begin(); it != sections.end(); it++) 
    {
        delete *it;
    }   
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    assert( num_of_bytes > 0);

    bool address_in_some_section = false;
    for ( ConstIter it = sections.begin(); it != sections.end(); it++) 
    {
        ElfSection *section = *it;
        if ( section->isInside( addr, num_of_bytes)) 
        {
            return section->read( addr, num_of_bytes);
        }
    }

    assert( address_in_some_section);
    return 0; 
}

string FuncMemory::dump( string indent) const
{
    cerr << indent << "Memory dump:" << endl;

    ostringstream oss;

    for ( ConstIter it = sections.begin(); it != sections.end(); it++) 
    {
        ElfSection *section = *it;
        oss << section->dump( indent + "    ") << endl;
    }

    return oss.str();
}

