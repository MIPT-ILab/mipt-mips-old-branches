/**
 * func_memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Implementation of memory methods:
 * @author Ilya Belikov <ilya.i.belikov@gmail.com>
 * Copyright 2012-2014 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>
#include <map>

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
    std::map< uint64, const void* > Memory;

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);

    virtual ~FuncMemory();

    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);

    uint64 startPC() const;

    string dump( string indent = "");

private:
    void write_s( uint8* data, uint64 addr, unsigned short num_of_bytes);
    void read_s( uint8* data, uint64 addr, unsigned short num_of_bytes) const;

    string Setdump( string indent, const void* pointer);
    string Pagedump( string indent, const void* pointer);

    uint64 SetSize;
    uint64 PageSize;
    uint64 OffsetSize;

    uint64 returnSet( uint64 addr) const;
    uint64 returnPage( uint64 addr) const;
    uint64 returnOffset( uint64 addr) const;

    uint64 start_addr;
    uint64 text_start_addr;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
