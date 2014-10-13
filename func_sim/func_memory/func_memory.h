/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

/*******************************************************

    Reducted by:

    @date: October 13, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @version: 1.0 (October 13, 2014) 

*******************************************************/

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
    //DATA **************************************************************
    char *_file_name;

    unit64 _text_start; //< All class data's names start with "_"
    
    uint64 _begin_addr;
    uint64 _end_addr;
    
    uint8 ***_memory;

    uint64 _sets_num; // Maximum number of sets
    uint64 _pages_num; // Number of pages in each set
    uint64 _offsets_num; // Number of offsets in each page
    
    uint64 _addr_bits;
    uint64 _page_bits;
    uint64 _offset_bits;

    uint64 _set_templ;
    uint64 _page_templ;
    uint64 _offset_templ;
    // ******************************************************************

    //INTERFACE *********************************************************
    FuncMemory(){} //< Can't use default constructor
public:
    FuncMemory (const char* executable_file_name,
                uint64 addr_size = 32,
                uint64 page_num_size = 10,
                uint64 offset_size = 12);
    
    virtual ~FuncMemory();
    DestroySet(const uint8 **set_addr); //< Deletes all memory in the current set
    
    //Searchs by "addr" address and returns a value. Modes for reading ("r") and writing ("w").
    //Default: read mode ("r");
    uint64& Search(const uint64 addr, const char mode = 'r');
    uint64 read(uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write(uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    
    inline uint64 startPC() const { return _text_start; }
     
    string dump(string indent = "") const;
    string PrintSet(const uint8 **set_addr, string indent = "") const;
    string PrintPage(const uint8 *page_addr, string indent = "") const;
    //********************************************************************
};

//Sets "num" bytes on the "length" bytes from the first one
uint64 SetBytes(uint64 num, uint length = 0);

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
