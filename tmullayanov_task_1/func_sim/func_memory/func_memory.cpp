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
#include <sstream>

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        const char* const elf_sections_names[],
                        short num_of_elf_sections)
{
    assert ( num_of_elf_sections > 0);
    assert ( executable_file_name);
    num_of_sect = num_of_elf_sections;
    name = executable_file_name;
    short cnt;
    for ( cnt=0; cnt < num_of_sect; cnt++)
    {
        assert( elf_sections_names[ cnt]);
        this -> curr_sect[ cnt] = new ElfSection ( executable_file_name, elf_sections_names[ cnt]);
    }
   
}

FuncMemory::~FuncMemory()
{
    for (short cnt = 0; cnt < num_of_sect; cnt++)
        delete this -> curr_sect[ cnt]; 
}

uint64 FuncMemory::read( uint64 addr, short num_of_bytes) const
{
    //checking if the desired are can be read
    assert( num_of_bytes > 0);
    assert ( num_of_bytes < sizeof (ElfSection));
    short cnt;
    for ( cnt = 0; cnt < num_of_sect; cnt++)
    {
	if ( curr_sect[ cnt] -> isInside ( addr, num_of_bytes))
	return curr_sect[ cnt] -> read ( addr, num_of_bytes);
    } 
    
    return NO_VAL64;
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    oss << indent << "Dump of FUNC memory " << name << ": " << endl;
    for ( short cnt = 0; cnt < num_of_sect; cnt++)
    {
        this -> curr_sect[ cnt] -> dump( indent);
    }    
    return oss.str();
}

