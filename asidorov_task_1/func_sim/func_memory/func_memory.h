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
#include <cstring>
#include <string>
#include <cassert>
#include <vector>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory() {}
    
    uint64 takeLeastBits( uint64 address, uint64 bits_number) const;
    void decodeAddress( uint64 address, 
                        uint64* set_index, 
                        uint64* page_index, 
                        uint64* offset) const;
    
    uint64 addr_bits;
    uint64 set_num_bits;
    uint64 page_num_bits;
    uint64 offset_bits;
    
    uint64 addr_size;
    uint64 sets_array_size;
    uint64 set_size;
    uint64 page_size;
    
    uint64 start_pc;
    
    uint8*** sets_array;

public:
    FuncMemory ( const char* executable_file_name,
                 uint64 addr_bits = 32,
                 uint64 page_num_bits = 10,
                 uint64 offset_bits = 12);
    
    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
