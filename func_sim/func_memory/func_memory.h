/**
 * I AM EXTREMELY SORRY FOR SUCH MUDDY CODE AND NO COMMENTS. CLEAN VERSION DON'T WORK AT 23.42 OCTOBER 18 
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>, Dmitry Ermilov <ermilov@phystech.edu>
 * Copyright 2012 uArchSim iLab project, 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}

public:

    uint64 text_start;
    uint64 addr_size;
    uint64 set_bits;
    uint64 page_bits;
    uint64 offset_bits;
    uint8*** set_array;

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;

    uint64* addr_read( uint64 address ) const;
    void addr_close( uint64* addr ) const;

    uint8** set_add();
    uint8* page_add();

    void set_dump( int set_num) const;

};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
