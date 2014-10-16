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
        string name;
        uint64 size;
        uint64 start_addr;

        SectionInfo( string sec_name, uint64 sec_size, uint64 sec_start_addr)
        : size( sec_size), start_addr( sec_start_addr), name( sec_name) {};

        SectionInfo( const SectionInfo &that): size( that.size), 
        start_addr( that.start_addr), name( that.name){};
        ~SectionInfo(){};
    };
    vector<SectionInfo> sections;
    uint8***  pages_set;
    uint64 addr_size;
    uint64 page_bits;
    uint64 offset_bits;
    int addrCalc( uint64 addr, uint64& set, uint64& page, uint64& offset) const;
    //A function to reverse bits in uint8
    uint8 reverse ( uint8 in ) const { in = (( in & 0x55) << 1) | ( ( in >> 1) & 0x55);
                                in = ( ( in & 0x33) << 2) | ( ( in >> 2) & 0x33);
                     in = (( in & 0x0F) << 4) | (( in >> 4) & 0x0F); return in; };   
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
