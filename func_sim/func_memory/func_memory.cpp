/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C

// Generic C++

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    ElfSection::getAllElfSections ( executable_file_name, sec_array);
    address_size = addr_size;
    addr_page_size = page_bits;
    addr_offset_size = offset_bits;
    addr_set_size = address_size - ( addr_page_size + addr_offset_size);
    // put your code here
}

FuncMemory::~FuncMemory()
{
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    return sec_array [ 0].start_addr;
    // put your code here
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // put your code here

    return 0;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
