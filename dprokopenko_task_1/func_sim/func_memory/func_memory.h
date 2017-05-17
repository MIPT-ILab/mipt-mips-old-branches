/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
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

typedef struct{
    uint8*** addr_mem;
    char** name;
    uint64* start_address;
    uint64* size;
} F_Memory;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}
    vector < ElfSection> sec_array;
    uint64 address_size;
    uint64 addr_set_size;
    uint64 addr_page_size;
    uint64 addr_offset_size;
    uint64 size_of_set;
    uint64 size_of_page;
    uint64 size_of_offset;
    F_Memory F_Mem;

public:
    
    void FuncMemory_find_addr ( uint64*, uint64*, uint64*, const uint64) const;
    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
