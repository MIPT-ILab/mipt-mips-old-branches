/**
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

/** Edited by Ladin Oleg. */

// Generic C
#include <cstring>
#include <cstdlib>

// Generic C++
#include <iostream>
#include <sstream>

// uArchSim modules
#include <func_memory.h>

/** Tools for sorting printing memory. */
struct Seq
{
    int addr; // Section start address
    int numb; // Global number of section
};

int qsort_comp( const void* a, const void* b); // Compare function for qsort

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    /** Parsing MIPS executable file, getting amount of sections. */

    /* Checking arguments. */
    assert( executable_file_name != 0);

    ElfSection::getAllElfSections( executable_file_name, sections_array);
    sa_num = sections_array.size();
    sa_all = 0;
}

FuncMemory::~FuncMemory()
{
    /** We don't need to free any resources additionaly. */
}

uint64 FuncMemory::startPC() const
{
    /** Brute-force searching of ".text" - name of start section. */

    for ( int i = 0; i < sa_num; i++)
    {
        if ( strcmp( sections_array[ i].name, ".text") == 0)
        {
            return sections_array[ i].start_addr;
        }
    }
}

uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    /**
     * Reading each byte by its address - section number and offset. Then
     * changing address to another byte. Searching by section, address belongs
     * to. Interval of section is from "start_addr" and "size" lenght.
     */

    /* Checking arguments. */
    assert( num_of_bytes != 0);

    int i; // Global counter - section number
    uint64 adr, ret = 0; // adr - offset var, ret - result var

    for ( int j = 0; j < num_of_bytes; j++)
    {
        adr = addr + j; // Address

        /* If address isn't in standart sections exactly. */
        if ( sections_array[ 0].start_addr > adr)
        {
            for ( i = 0; i < sa_all; i++)
            {
                if ( ( sections_array[ sa_num + i].start_addr <= adr)
                     && ( sections_array[ sa_num + i].start_addr
                          + sections_array[ sa_num + i].size > adr))
                {
                    i = i + sa_num;
                    break;
                }
            }
            if ( i == sa_all) // If suitable interval doesn't exist
            {
                abort();
            }
        } else /* If address is in standart sections possibly. */
        {
            for ( i = 0; i < sa_num; i++)
            {
                if ( ( sections_array[ i].start_addr <= adr)
                     && ( sections_array[ i].start_addr
                          + sections_array[ i].size > adr))
                {
                    break;
                }
            }
            if ( i == sa_num) // If suitable interval doesn't exist
            {
                abort();
            }
        }

        adr = adr - sections_array[ i].start_addr; // Offset
        /* Creating return memory part. */
        ret = ret | ( ( uint64)( sections_array[ i].content[ adr] << ( j * 8)));
    }
    return ret;
}

void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    /**
     * Writing each byte by its address - section number and offset. Then
     * changing address to another byte. Searching by section, address belongs
     * to. Interval of section is from "start_addr" and "size" lenght. If there
     * are no any suitable section, programm creates new section.
     */

    /* Checking arguments. */
    assert( num_of_bytes != 0);

    int i; // Global counter - section number
    uint64 adr; // Offset var

    for ( int j = 0; j < num_of_bytes; j++)
    {
        adr = addr + j; // Address

        /* If address isn't in standart sections exactly. */
        if ( sections_array[ 0].start_addr > adr)
        {
            for ( i = 0; i < sa_all; i++)
            {
                if ( ( sections_array[ sa_num + i].start_addr <= adr)
                     && ( sections_array[ sa_num + i].start_addr
                          + sections_array[ sa_num + i].size > adr))
                {
                    break;
                }
            }
            /* If suitable interval doesn't exist, create new. */
            if ( i == sa_all)
            {
                sections_array.push_back( sections_array[ 0]);
                sections_array[ sa_num + sa_all].size = 1;
                sections_array[ sa_num + sa_all].start_addr = adr;
                sections_array[ sa_num + sa_all].content = ( uint8*)malloc( sizeof( uint8));
                i = sa_num + sa_all;
                sa_all++;
            }
        } else /* If address is in standart sections possibly. */
        {
            for ( i = 0; i < sa_num; i++) //
            {
                if ( ( sections_array[ i].start_addr <= adr)
                     && ( sections_array[ i].start_addr
                          + sections_array[ i].size > adr))
                {
                    break;
                }
            }
            /* If suitable interval doesn't exist, create new. */
            if ( i == sa_num)
            {
                sections_array.push_back( sections_array[ 0]);
                sections_array[ sa_num + sa_all].size = 1;
                sections_array[ sa_num + sa_all].start_addr = adr;
                sections_array[ sa_num + sa_all].content = ( uint8*)malloc( sizeof( uint8));
                i = sa_num + sa_all;
                sa_all++;
            }
        }
        adr = adr - sections_array[ i].start_addr; // Offset
        /* Writing to memory. */
        sections_array[ i].content[ adr] = ( uint8)( ( value & ( 255 << ( j * 8)))
                                                     >> ( j * 8));
    }
}

string FuncMemory::dump( string indent) const
{
    /**
     * Dump method prints all data of the functional memory. Besides, it prints
     * starting address of loaded programm. There are three types of memory
     * content: not allocated ("NOTALL"), contains "0", non-zero contain.
     */

    /* Output. */
    ostringstream oss;

    /* Main info. */
    oss << indent << "Functional Memory Dump" << endl << endl << "  Start address:"
        << endl << "    0x" << hex << FuncMemory::startPC() << dec << endl
        << "  Memory content:" << endl;

    /* Sorting by address sections to successive print. */
    Seq sa[ sa_num + sa_all];
    for ( int i = 0; i < sa_num + sa_all; i++)
    {
        sa[ i].addr = sections_array[ i].start_addr;
        sa[ i].numb = i;
    }
    qsort( sa, sa_num + sa_all, sizeof( Seq), qsort_comp);

    bool is_all = true; // Sign - is memory allocated or not
    int is_not_nul = 0; // Number of "0" bytes wanted to print before
    int i = 0x0, j = 0; // i - start address, j - section counter

    /**
     * Cycle chooses only allocated memory and then prints without multiplicity
     * of "0". All allocated memory are in sections.
     */
    while ( 1)
    {
        /* If we work not after the last section. */
        if ( j < sa_num + sa_all)
        {
            /* If we between sections. No allocated memory. */
            if ( i < sa[ j].addr)
            {
                oss << indent << "    0x" << hex << i << dec << ":    NOTALL"
                    << endl << "      ...." << endl;
                is_all = false;
                i = sa[ j].addr;
                continue;

            /* We reached section. */
            } else
            {
                if ( !is_all) // If before was not allocated memory
                {
                    oss << indent << "    0x" << hex << i - 1 << dec
                        << ":    NOTALL" << endl;
                    is_all = true;
                }

                /* Here we check each byte of section. */
                for ( int k = 0;
                      k < sections_array[ sa[ j].numb].size;
                      k++, i++)
                {
                    /* If it's non-zero value. */
                    if ( ( int)sections_array[ sa[ j].numb].content[ k] != 0)
                    {
                        /* Check did we meet "0". */
                        if ( is_not_nul == 1)
                        {
                            is_not_nul = 0;
                        }
                        if ( is_not_nul == 2)
                        {
                            is_not_nul = 0;
                            oss << indent << "    0x" << hex << i - 1 << dec
                                << ":    0" << endl;
                        }
                        if ( is_not_nul == 3)
                        {
                            is_not_nul = 0;
                            oss << indent << "      ...." << endl << "    0x"
                                << hex << i - 1 << dec << ":    0" << endl;
                        }

                        oss << indent << "    0x" << hex << i << ":    "
                            << ( int)sections_array[ sa[ j].numb].content[ k]
                            << endl;
                        continue;

                    /* If it's a zero. */
                    } else
                    {
                        /* Check how often did we meet "0". */
                        if ( is_not_nul == 0)
                        {
                            is_not_nul = 1;
                            oss << indent << "    0x" << hex << i << ":    0"
                                << endl;
                            continue;
                        }
                        if ( is_not_nul == 1)
                        {
                            is_not_nul = 2;
                            continue;
                        }
                        if ( is_not_nul == 2)
                        {
                            is_not_nul = 3;
                            continue;
                        }
                        if ( is_not_nul == 3)
                        {
                            continue;
                        }
                    }
                }

                /* Check how often did we meet "0", on the section border. */
                if ( is_not_nul == 1)
                {
                    is_not_nul = 0;
                }
                if ( is_not_nul == 2)
                {
                    is_not_nul = 0;
                    oss << indent << "    0x" << hex << i - 1 << dec
                        << ":    0" << endl;
                }
                if ( is_not_nul == 3)
                {
                    is_not_nul = 0;
                    oss << indent << "      ...." << endl << "    0x" << hex
                        << i - 1 << dec << ":    0" << endl;
                }
                j++;
                continue;
            }

        /* If we work after the last section. There are no allocated memory. */
        } else
        {
            oss << indent << "    0x" << hex << i << dec << ":    NOTALL"
                << endl << "      ...." << endl << "    0x100000000:    NOTALL"
                << endl;
            break;
        }
    }
    return oss.str();
}

int qsort_comp( const void* a, const void* b)
{
    return ( ( Seq*)a)->addr - ( ( Seq*)b)->addr;
}
