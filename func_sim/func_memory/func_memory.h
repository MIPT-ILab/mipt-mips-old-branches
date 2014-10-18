/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

 /**
 * updated by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 *
 * v. 1.0: created 16.10.2014 21:05
 *         added implementation of main functions
 *
 * v. 1.1: created 17.10.2014 00:10
 *         added dump function
 *
 * v. 1.2: created 17.10.2014 11:25
 *         fixed bug in destruction
 *         optimized write function
 *
 * v. 1.3: created 17.10.2014 18:35
 *         fixed bug in destruction
 *         some changes in dump
 *
 * v. 1.4: created 17.10.2014 20:25
 *         added start address to dump
 *         added comments
 *         added some checks
 *
 * v. 1.5: created 18.10.2014 18:20
 *         fixed bug in dump function
 *
 * v. 1.6: created 18.10.2014 22:10
 *         some changes in coding style
 *         added new function PageWrite
 *         corrected some grammar mistakes
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

class MemOffset
{
public:
    char value;
};

class MemPage
{
public:
    MemOffset* offset;
};

class MemSet
{
public:
    MemPage** page;
};

string valtohex( uint64 val);

string addrtohex( uint64 addr);

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){}

public:

    MemSet** memory;
    uint64 Set_Size;
    uint64 Page_Size;
    uint64 Offset_Size;
    uint64 PCstart_addr;
    uint64 Set_Bits;
    uint64 Page_Bits;
    uint64 Offset_Bits;

    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);

    virtual ~FuncMemory();
    
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    void   PageWrite( uint8* value, uint64* addr, uint64* size);
    void   MemWrite( uint8* value, uint64 addr, uint64 size);
    uint64 startPC() const;
    
    string dump( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
