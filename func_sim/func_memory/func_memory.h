/**
 * func_memory.h - Header of module implementing the concept of 
 * programer-visible memory space accesing via memory address.
 * @author Lyubogoshchev Mikhail <lyubogoshchev@phystech.edu>
 * Copyright 2014 MIPT-MIPS iLab project
 */

// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <string>
#include <cassert>
#include <errno.h>
// uArchSim modules
#include <types.h>
#include <elf_parser.h>

using namespace std;

class FuncMemory
{
    // You could not create the object
    // using this default constructor
    FuncMemory(){};
    class SectionInfo
    {
        SectionInfo(){};
    public:
/*************************************************************************************
 * The information about sections, which are loadede to the memory the executable file
*************************************************************************************/
        string name;
        uint64 size;
        uint64 start_addr;
/*****************************
 * And methods to work with it
******************************/
        SectionInfo( string sec_name, uint64 sec_size, uint64 sec_start_addr)
        : size( sec_size), start_addr( sec_start_addr), name( sec_name) {};

        SectionInfo( const SectionInfo &that): size( that.size), 
               start_addr( that.start_addr), name( that.name){};

        ~SectionInfo(){};
    };
/**************************************************
 * Main part of the data, kept in Functional Memory
**************************************************/
    vector<SectionInfo> sections;
    uint8***  pages_set;
    uint64 text_addr;
    uint8 addr_size;
    uint8 page_bits;
    uint8 offset_bits;
    uint64 sets_n; //calculated parameters of the Func Memory
    uint64 pages_n;
    uint64 offset_n;
/**********************************
 * An interface for working with it
**********************************/
    int addrCalc( uint64 addr, uint64& set, uint64& page, uint64& offset) const;
    //A function to reverse bits in uint8
    inline uint8 reverse ( uint8 in) const;
public:
    FuncMemory ( const char* executable_file_name,
                 uint64 addr_size = 32,
                 uint64 page_num_size = 10,
                 uint64 offset_size = 12);
    virtual ~FuncMemory();
    uint64 read( uint64 addr, unsigned short num_of_bytes = 4) const;
    void   write( uint64 value, uint64 addr, unsigned short num_of_bytes = 4);
    uint64 startPC() const { return text_addr;};    
/*Is used to dump only data stored in sections
which are created while initialising the FuncMemory*/
    string dump( string indent = "") const;
//Is used to dump all the inintialised data
    string dumpFull( string indent = "") const;
};

#endif // #ifndef FUNC_MEMORY__FUNC_MEMORY_H
