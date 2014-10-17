/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

/***************************************************************

        Reducted by:

        @date: October 13, 2014
        @author: Kirill Korolev <kirill.korolef@gmail.com>
        @version: 1.0 (October 13, 2014) 

***************************************************************/

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
        //DATA ******************************************************************
        char *_file_name; //< All class data's names start with "_".           //
                                                                               //
        uint64 _text_start; //< First address of .text section.                //
                                                                               //
        uint64 _begin_addr; //<= Two boundary addresses of                     //
        uint64 _end_addr;   //<= a whole memory space.                         //
                                                                               //
        //< Hierarchical organised memory:                                     //
        vector< vector< vector<uint8> > > _memory;                             //
                                                                               //
        // Number of bits:                                                     //
        uint64 _page_bits;   // in a "page part;                               //
        uint64 _offset_bits; // in a "offset" part.                            //
                                                                               //
        uint64 _set_mask;    //<= Tree masks for getting set, page and offset  //
        uint64 _page_mask;   //<= addresses from a big one.                    //
        uint64 _offset_mask; //<=                                              //
        // **********************************************************************

        //INTERFACE ************************************************************************************
        FuncMemory(){} //< Can't use default constructor                                              //
public:                                                                                               //
        FuncMemory (const char* executable_file_name, //< Basic class constructor:                    //
                    uint64 addr_size     = 32,                                                        //
                    uint64 page_num_size = 10,                                                        //
                    uint64 offset_size   = 12);                                                       //
                                                                                                      //
        virtual ~FuncMemory(){} //< All allocated memory dectroys in vector class destructor          //
                                                                                                      //
        const uint8* RSearch(const uint64 addr) const; //< Returns pointer by address for reading     //
              uint8* WSearch(const uint64 addr);       //< Returns pointer by address for writing     //
              uint64 read(uint64 addr, unsigned short num_of_bytes = 4) const;                        //
              void   write(uint64 value, uint64 addr, unsigned short num_of_bytes = 4);               // 
                                                                                                      //
        inline uint64 startPC() const { return _text_start; }                                         //
                                                                                                      //
        // Recursive hierarchical memory dump by tree functions:                                      //
        string dump(string indent = "") const;                                                        //
        string SetDump(const vector< vector<uint8> > set_addr, const string indent = "") const;       //
        string PageDump(const vector<uint8> page_addr, const string indent = "") const;               //
        //**********************************************************************************************
};
// 2 BINARY FUNCTIONS:

// Sets "num" bytes on the "length" bytes from the first one
uint64 SetBytes(const uint8 num, const uint8 length = 0);

// Writes first "num" bytes in "val" in different order; 
// used for transferring between Big Endian and Little Endian:
uint64 Reverse(const uint64 val, const uint8 num);

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
