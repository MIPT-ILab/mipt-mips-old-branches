/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>
#include <cstring>

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
    assert( num_of_elf_sections > 0);

    //Extracting different section names
    short num_of_dif_elf_sections = 0; //Number of different section names
    short *dif_elf_sect_num = new short[ num_of_elf_sections]; //Array of indexes of different section names
                                                               //in input array
    for( int i = 0; i < num_of_elf_sections; i++)
    {
        int dif_flag = 1;
        for( int k = 0; k < i; k++)
        {
            if( !strcmp( elf_sections_names[ i], elf_sections_names[ k]))
            {
                dif_flag = 0;
                break;
            }
        }
        if( dif_flag)
        {
            dif_elf_sect_num[ num_of_dif_elf_sections] = i;
            num_of_dif_elf_sections++;
        }
    }

    this->num_of_sections = num_of_dif_elf_sections;
    this->sections = new ElfSection*[ num_of_dif_elf_sections];
    for( int i = 0; i < num_of_dif_elf_sections; i++)
    {
        this->sections[ i] = new ElfSection( executable_file_name, elf_sections_names[ dif_elf_sect_num[ i]]);
    }
    delete [] dif_elf_sect_num;
}

FuncMemory::~FuncMemory()
{
    for( int i = 0; i < this->num_of_sections; i++)
    {
        delete this->sections[ i];
    }
    delete [] this->sections;
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    assert( num_of_bytes > 0 && num_of_bytes <= sizeof( uint64));

    for( int i = 0; i < this->num_of_sections; i++)
    {
        if( ( this->sections[ i])->isInside( addr, num_of_bytes))
        {
            return ( this->sections[ i])->read( addr, num_of_bytes);
        }
    }

    assert( !"Could not read!");

    return NO_VAL64; 
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    oss << indent << "Dump of FuncMemory:" << endl;
    for( int i = 0; i < this->num_of_sections; i++)
    {
        oss << this->sections[ i]->dump( indent) << endl;
    }
    return oss.str();
}

