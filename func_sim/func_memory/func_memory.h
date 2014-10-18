/**
 * func_memory.h - Header of module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * performed by Semyon Abramov <semyon.abramov.mipt@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>

// uArchSim modules
#include "types.h"
#include "elf_parser.h"

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}
    static string getFormatted( uint64 val, int num = 8, char = '0');
    static vector<int> toBin( uint64 val);
    static vector<int> toBin( uint8 val);
    static uint64 fromBin( vector<int> data);
    static void myWrite( uint8* content,
                         uint64 value,
                         uint64 from,
                         uint64 count);
public:

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    vector<ElfSection> sections_array;
    virtual ~FuncMemory();
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    uint64 startPC() const;
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
