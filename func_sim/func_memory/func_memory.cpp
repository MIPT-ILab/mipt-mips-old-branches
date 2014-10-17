/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cstring>
// Generic C++
#include <iostream>
#include <string>

using namespace std;

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    ElfSection::getAllElfSections ( executable_file_name, sec_array);
    address_size = addr_size;
    cout << addr_size << " " << page_bits << " " << offset_bits << "\n";
    addr_page_size = page_bits;
    addr_offset_size = offset_bits;
    addr_set_size = address_size - ( addr_page_size + addr_offset_size);
    cout << addr_set_size << " " << addr_page_size << " " << addr_offset_size << "\n";
    // put your code here
    F_Mem.addr_set = new uint8 ** [ 1024 * sizeof ( uint8**)];
    //memset ( F_Mem.sddr_set, NULL);
    for ( int i = 0; i < sec_array.size(); i++)
    {
        FuncMemory_addr ( sec_array [ i]. start_addr); 
        F_Mem.addr_set [ addr_set] = new uint8* [1024 * sizeof ( uint8*)];
        //for ( int j = 0; j < 
    }
        
}

FuncMemory::~FuncMemory()
{
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    for ( int i = 0; i < sec_array.size(); i++)
        if ( ! ( strcmp ( sec_array [ i].name, ".text"))) 
            return sec_array [ i].start_addr;
    // put your code here
}

void FuncMemory::FuncMemory_addr ( const uint64 addr)
{
    addr_set = ( addr >> addr_page_size + addr_offset_size);
    addr_page = ( addr - ( addr_set << ( addr_page_size + addr_offset_size))) >> addr_offset_size; 
    addr_offset = ( ( addr >> addr_offset_size) & 0x0) << addr_offset_size;
    cout << hex << addr << " " << addr_set << " " << addr_page << " " << addr_offset << "\n\n";
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    /*uint64  answ = 0;
    uint8 buf = 0;
    uint64 end_addr = addr + num_of_bytes;
    uint64 end_sec_addr, diff_addr = 0;
    cout << ( int) sec_array [ 0]. content [ 0]<< " I'm here\n";
    for ( int i = 0; i < sec_array.size(); i++)
    {
        end_sec_addr = sec_array [ i].start_addr + sec_array [ i].size;
        if (( sec_array [ i].start_addr <= addr) && ( addr <= end_sec_addr) && ( end_addr  < end_sec_addr))
        {
            diff_addr = addr - sec_array [ i].start_addr;
            for ( int j = 0; j < num_of_bytes; j++)
            {
                buf = sec_array [ i].content [ addr - sec_array [ i].start_addr];
                cout << ( int) buf << "\n";
                buf = ( ( buf & 0xF0 ) >> 4 ) | ( ( buf & 0x0f ) << 4 );
                buf = ( ( buf & 0xCC ) >> 2 ) | ( ( buf & 0x33 ) << 2 );
                buf = ( ( buf & 0xAA ) >> 1 ) | ( ( buf & 0x55 ) << 1 );
                cout << ( int) buf << "\n\n";
                answ = answ * 256 + buf; 
            }
        }
    }*/
    //cout << "Before return answ\n" << answ << "\n";
    return 0;
    // put your code here
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // put your code here
}

string FuncMemory::dump( string indent) const
{
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
