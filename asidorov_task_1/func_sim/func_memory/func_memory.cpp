/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * @author Alexey Sidorov <alexey.s.sidorov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <math.h>

// Generic C++
#include <algorithm>
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>

using namespace std;

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_bits,
                        uint64 page_num_bits,
                        uint64 offset_bits)
{
    assert( executable_file_name != NULL && 
            addr_bits && page_num_bits && offset_bits && 
            ( addr_bits > page_num_bits + offset_bits));
    
    this->addr_bits = addr_bits;
    this->set_num_bits = addr_bits - page_num_bits - offset_bits;
    this->page_num_bits = page_num_bits;
    this->offset_bits = offset_bits;
    
    this->addr_size = ( uint64) 1 << this->addr_bits;
    this->sets_array_size = ( uint64) 1 << this->set_num_bits;
    this->set_size = ( uint64) 1 << this->page_num_bits;
    this->page_size = ( uint64) 1 << this->offset_bits;
    
    this->sets_array = new uint8**[ this->sets_array_size];
    
    for ( uint64 index = 0; index < this->sets_array_size; ++index)
        this->sets_array[ index] = NULL;
    
    vector< ElfSection> sections_array;
    
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    
    for ( uint64 section_index = 0; 
          section_index < sections_array.size(); 
          ++section_index)
    {   
        if ( !strcmp( sections_array[ section_index].name, ".text"))
            this->start_pc = sections_array[ section_index].start_addr;
        
        for ( uint64 content_index = 0, 
                     curr_addr = sections_array[ section_index].start_addr; 
              content_index < sections_array[ section_index].size; 
              ++content_index, ++curr_addr)
        {     
            this->write( ( uint64) sections_array[ section_index].content[ content_index], 
                          curr_addr, 
                          1);
        }
    }
}

FuncMemory::~FuncMemory()
{
    for ( uint64 set_index = 0; set_index < this->sets_array_size; ++set_index)
    {
        if ( this->sets_array[ set_index] == NULL)
            continue;

        for ( uint64 page_index = 0; page_index < this->set_size; ++page_index)
        {
            if ( this->sets_array[ set_index][ page_index] == NULL) 
                continue;
            
            delete [] this->sets_array[ set_index][ page_index];
        }
        
        delete [] this->sets_array[ set_index];
    }
    
    delete [] this->sets_array;
}

uint64 FuncMemory::startPC() const
{
    return this->start_pc;
}

uint64 FuncMemory::takeLeastBits( uint64 number, uint64 bits_number) const
{
    return number & ( ( (uint64) 1 << bits_number) - 1);
}

void FuncMemory::decodeAddress( uint64 address, 
                                uint64* set_index, 
                                uint64* page_index, 
                                uint64* offset) const
{
    *offset = this->takeLeastBits( address, this->offset_bits);
    address >>= this->offset_bits;
    
    *page_index = this->takeLeastBits( address, this->page_num_bits);
    address >>= this->page_num_bits;
    
    *set_index = this->takeLeastBits( address, this->set_num_bits);
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    assert( num_of_bytes);
    
    uint64 set_index;
    uint64 page_index;
    uint64 offset;
    
    stringstream ss;
    
    ss << hex;
    
    for ( uint64 counter = 0, curr_addr = addr + num_of_bytes - 1; 
          counter < num_of_bytes; 
          --curr_addr, ++counter)
    {
        this->decodeAddress( curr_addr, &set_index, &page_index, &offset);
        
        assert( set_index < this->sets_array_size && 
                page_index < this->set_size && 
                offset < this->page_size && 
                this->sets_array[ set_index] != NULL && 
                this->sets_array[ set_index][ page_index] != NULL);
        
        ss.width( 2);
        ss.fill( '0');
        
        ss << (uint16) this->sets_array[ set_index][ page_index][ offset];
    }
    
    uint64 result;
    
    ss >> result;
    return result;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    assert( num_of_bytes);
    
    uint64 set_index;
    uint64 page_index;
    uint64 offset;
    
    uint64 bits_number = 8;
    
    while (value && num_of_bytes)
    {
        this->decodeAddress( addr, &set_index, &page_index, &offset);
        
        assert( set_index < this->sets_array_size && 
                page_index < this->set_size && 
                offset < this->page_size);
        
        if ( this->sets_array[ set_index] == NULL)
        {
            this->sets_array[ set_index] = new uint8*[ this->set_size];
            
            for ( uint64 index = 0; index < this->set_size; ++index)
            {
                if (index == page_index)
                {
                    this->sets_array[ set_index][ index] = new uint8[ this->page_size];
                } else
                {
                    this->sets_array[ set_index][ index] = NULL;
                }
            }
        } else if ( this->sets_array[ set_index][ page_index] == NULL)
        {
            this->sets_array[ set_index][ page_index] = new uint8[ this->page_size];
        }
        
        this->sets_array[ set_index][ page_index][ offset] = (uint8) this->takeLeastBits( value, bits_number);
        
        value >>= bits_number;
        ++addr;
        --num_of_bytes;
    }
    
    while (num_of_bytes)
    {
        this->decodeAddress( addr, &set_index, &page_index, &offset);
        
        assert( set_index < this->sets_array_size && 
                page_index < this->set_size && 
                offset < this->page_size);
                
        if ( this->sets_array[ set_index] == NULL)
        {
            this->sets_array[ set_index] = new uint8*[ this->set_size];
            
            for ( uint64 index = 0; index < this->set_size; ++index)
            {
                if (index == page_index)
                {
                    this->sets_array[ set_index][ index] = new uint8[ this->page_size];
                } else
                {
                    this->sets_array[ set_index][ index] = NULL;
                }
            }
        } else if ( this->sets_array[ set_index][ page_index] == NULL)
        {
            this->sets_array[ set_index][ page_index] = new uint8[ this->page_size];
        }
        
        this->sets_array[ set_index][ page_index][ offset] = (uint8) 0;
        
        ++addr;
        --num_of_bytes;
    }
}

string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    oss << hex;
    
    for ( uint64 set_index = 0; set_index < this->sets_array_size; ++set_index)
    {
        oss << indent << "Dump set #" << set_index << endl;
        
        if ( this->sets_array[ set_index] == NULL)
        {
            oss << indent << "There is no data" << endl;
            continue;
        }
    
        for ( uint64 page_index = 0; page_index < this->set_size; ++page_index)
        {
            oss << indent << "Dump page #" << page_index << endl;
        
            if ( this->sets_array[ set_index][ page_index] == NULL)
            {
                oss << indent << "There is no data" << endl;
                continue;
            }
        
            for ( uint64 offset = 0; offset < this->page_size; ++offset)
            {
                oss << hex;
                oss.width( 2);
                oss.fill( '0');
                
                oss << indent << "offset == " << offset << ", value == " 
                    << (uint16) this->sets_array[ set_index][ page_index][ offset] << endl;
            }           
        }      
    }

    return oss.str();
}
