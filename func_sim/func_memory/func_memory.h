/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

#define BITS_IN_BYTE 8
#define MAX_ADDRES_LEN 48

// Generic C++
#include <string>
#include <cassert>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

typedef vector<uint64> Cell;
typedef vector<Cell*> Page;
typedef vector<Page*> SetPage;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}
    SetPage ArraySet;
    uint64 _addr_size;
    uint64 _page_bits;
    uint64 _offset_bits;
    uint64 _segm_text;
    vector<uint64> getAddr( uint64 full_addr,
                            uint64 addr_size,
                            uint64 page_bits,
                            uint64 offset_bits) const;
    vector<uint64> getAddr( uint64 full_addr) const;
    uint64         getMask(uint64 num_digit) const;
    bool           checkAddrAndAdd(const vector<uint64> &addr);
    bool           checkAddr(const vector<uint64> &addr) const;
public:

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
