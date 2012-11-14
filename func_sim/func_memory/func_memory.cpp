/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <libelf.h>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <gelf.h>
#include <cstdlib>
#include <cerrno>
#include <cassert>

// Generic C++
#include <iostream>
#include <string> 
#include <sstream>

// uArchSim modules
#include <func_memory.h>
#include <types.h>
#include <elf_parser.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert( num_of_elf_sections > 0);
    assert( elf_sections_names);
    assert( executable_file_name);

    // fill sections map with given sections
    for ( short i = 0; i < num_of_elf_sections; ++i)
    {
        ElfSection* section_pointer = new ElfSection( executable_file_name,
                                                      elf_sections_names[ i]);
       
        // fill the map
        this->sections[ section_pointer->startAddr()] = section_pointer;
    }

    // fill all_sections map
    
    // copy found sections
    this->all_sections = this->sections;

    // open the binary file, we have to use C-style open,
    // because it is required by elf_begin function
    int file_descr = open( executable_file_name, O_RDONLY); 
    if ( file_descr < 0)
    {
        cerr << "ERROR: Could not open file " << executable_file_name << ": "
             << strerror( errno) << endl;
        exit( EXIT_FAILURE);
    }

    // set ELF library operating version
    if ( elf_version( EV_CURRENT) == EV_NONE)
    {
        cerr << "ERROR: Could not set ELF library operating version:"
             <<  elf_errmsg( elf_errno()) << endl;
        exit( EXIT_FAILURE);
    }
   
    // open the file in ELF format 
    Elf* elf = elf_begin( file_descr, ELF_C_READ, NULL);
    if ( !elf)
    {
        cerr << "ERROR: Could not open file " << executable_file_name
             << " as ELF file: "
             <<  elf_errmsg( elf_errno()) << endl;
        exit( EXIT_FAILURE);
    }

    size_t shstrndx;
    elf_getshdrstrndx( elf, &shstrndx);

    // look through all sections and 
    // fill all_sections map with the ones which are not in sections map
    Elf_Scn *section = NULL;
    while ( ( section = elf_nextscn( elf, section)) != NULL)
    {
        GElf_Shdr shdr;
        gelf_getshdr( section, &shdr);
        char* found_section_name = elf_strptr( elf, shstrndx, shdr.sh_name);
        
        ElfSection* section_pointer = new ElfSection( executable_file_name,
                                                      found_section_name);

        // if amount of mapped value with the key in the map is 0
        if ( !( this->all_sections).count( section_pointer->startAddr()))
        {
            // fill all_sections map
            this->all_sections[ section_pointer->startAddr()] = section_pointer;
        }
    }
}

FuncMemory::~FuncMemory()
{
    // delete each mapped value, i.e. ElfSection* 
    // only in all_sections map, because it contain sections map
    for ( Iter it = ( this->all_sections).begin();
          it != ( this->all_sections).end();
          ++it)
    {
        delete it->second;
    }
    
    ( this->sections).clear();
    ( this->all_sections).clear();
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    //typedef map<uint64, ElfSection *>::iterator Iter;
    
    // look through all sections and try to find section which contains 
    // the address addr and read from the one
    for ( ConstIter it = ( this->all_sections).begin();
          it != ( this->all_sections).end();
          ++it)
    {
        if ( ( it->second)->isInside( addr, 1))
        {
            return ( it->second)->read( addr, num_of_bytes);
        }
    }

    cerr << "ERROR: Could not find a section which contains address "
         << hex << "0x" << addr << dec << endl;
    exit( EXIT_FAILURE);
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;

    for ( ConstIter it = ( this->sections).begin();
          it != ( this->sections).end();
          ++it)
    {
        oss << ( it->second)->dump( indent) << endl;
    }

    return oss.str();
}

