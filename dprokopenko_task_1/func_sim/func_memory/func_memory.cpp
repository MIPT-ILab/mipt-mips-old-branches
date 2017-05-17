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
    addr_page_size = page_bits;
    addr_offset_size = offset_bits;
    addr_set_size = address_size - ( addr_page_size + addr_offset_size);
    
    size_of_set = 0x1l << addr_set_size;
    size_of_page = 0x1l << addr_page_size;
    size_of_offset = 0x1l << addr_offset_size;
    
    uint64 addr_set, addr_page, addr_offset;
    //cout << "Size_vector and so on: " <<  hex <<sec_array.size() << " " /*<< hex*/ << size_of_set << " " << size_of_page << " " << size_of_offset << dec << "\n" ;

    F_Mem.addr_mem = new uint8 ** [ ( size_of_set) * sizeof ( uint8**)];
    /*for ( int z = 0; z < size_of_set; z++)
        F_Mem.addr_set [ z] = NULL;*/
    F_Mem.name = new char* [ sec_array.size() * sizeof ( char*)];
    F_Mem.start_address = new uint64 [ sec_array.size() * sizeof ( uint64)];
    F_Mem.size = new uint64 [ sec_array.size() * sizeof ( uint64)];
    for ( int i = 0; i < sec_array.size(); i++)
    {
        F_Mem.name [ i] = sec_array [ i].name;
        F_Mem.start_address [ i]= sec_array [ i].start_addr;
        F_Mem.size [ i]= sec_array [ i].size;
        FuncMemory_find_addr ( &addr_set, &addr_page, &addr_offset, sec_array [ i].start_addr); 
        for ( int y = 0; y <= ( ( ( uint64) addr_offset + sec_array [ i].size) / (size_of_page * size_of_offset)); y++)
        {
            //if ( F_Mem.addr_set [ addr_set + y] == NULL)
            {
                F_Mem.addr_mem [ addr_set + y] = new uint8* [ ( size_of_page) * sizeof ( uint8*)];
                
                for ( int z = 0; z < size_of_page; z++)
                    F_Mem.addr_mem [ addr_set + y] [ z] = NULL;
                for ( int j = 0; j <= ( ( ( uint64) addr_offset + sec_array [ i].size) / size_of_offset); j++)
                    //if ( F_Mem.addr_set [ addr_set + y] [ addr_page + j] == NULL)
                    {
                        F_Mem.addr_mem [ addr_set + y] [ addr_page + j] = new uint8 [ ( size_of_offset) * sizeof ( uint8)]; 
                   }
            }
        }
        //write!
        for ( int j = 0; j < sec_array [ i].size; j++)
        {
            uint64 flag_page = ( addr_offset + j) / size_of_offset;
            uint64 flag_set = ( addr_page + flag_page) / size_of_page;
            F_Mem.addr_mem [ addr_set + flag_set] [ addr_page + flag_page - ( flag_set * size_of_page)] [ addr_offset + j - ( flag_page * size_of_offset)] = sec_array [ i].content [ j];
            //cout << F_Mem.addr_mem [ addr_set + flag_set] [ addr_page + flag_page - ( flag_set * size_of_page)] [ addr_offset + j - ( flag_page * size_of_offset)] << " ";
        } 
    }
}

FuncMemory::~FuncMemory()
{
    // put your code here
}

uint64 FuncMemory::startPC() const
{
    for ( int i = 0; i < sec_array.size(); i++)
    {
        if ( ! strcmp ( F_Mem.name [ i], ".text"))
            return F_Mem.start_address [ i];
    }
    // put your code here
}

void FuncMemory::FuncMemory_find_addr ( uint64 *addr_set, uint64 *addr_page, uint64 *addr_offset, const uint64 address) const  
{
    *addr_set = address >> ( addr_offset_size + addr_page_size);    
    *addr_page = ( address >> addr_offset_size) & ( size_of_page - 1);
    *addr_offset = address & (size_of_offset -1);
    
    //cout << "FuncMemory_addr "<< hex << addr << " " << addr_set << " " << addr_page << " " << addr_offset << dec << "\n";
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    uint64 buf, answ = 0;
    uint64 addr_set, addr_page, addr_offset;
    FuncMemory_find_addr ( &addr_set, &addr_page, &addr_offset, addr);
    for ( int i = 0; i < num_of_bytes; i++);
    {
        buf = F_Mem.addr_mem [ addr_set] [ addr_page] [ addr_offset];
        cout << hex << addr_set_size << " " << addr_page_size << " " << addr_offset_size << "\n";
        cout << addr << " " << addr_set << " " << addr_page << " " << addr_offset << " " << buf << "\n" << dec;
        buf = ( ( buf & 0xF0 ) >> 4 ) | ( ( buf & 0x0f ) << 4 );
        buf = ( ( buf & 0xCC ) >> 2 ) | ( ( buf & 0x33 ) << 2 );
        buf = ( ( buf & 0xAA ) >> 1 ) | ( ( buf & 0x55 ) << 1 );
        answ = answ + ( buf << 0x8);
    }
    return answ;
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
