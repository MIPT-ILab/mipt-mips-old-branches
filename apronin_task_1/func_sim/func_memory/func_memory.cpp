/**
 * func_memory.cpp - the module implementing the concept of
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

// Generic C
#include <string.h>

// Generic C++
#include <sstream>

// uArchSim modules
#include <func_memory.h>

/**
 * FuncMemory::FuncMemory - construct FuncMemory class using binary ELF file
 * 
 * param    executable_file_name        name of binary ELF file
 * param    addr_size                   size of address in bits
 * param    page_bits                   size of page part of 
 *                                      address in bits
 * param    offset_bits                 size of offset part of
 *                                      address in bits
 *
 */
FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
    // check input data
    assert( addr_size);
    assert( page_bits);
    assert( offset_bits);
    assert( executable_file_name);

    // give start value to number of address elements
    this->Offset_Size = 1;
    this->Page_Size = 1;
    this->Set_Size = 1;

    // save sizes of address parts
    this->Offset_Bits = offset_bits;
    this->Page_Bits = page_bits;
    this->Set_Bits = addr_size - page_bits - offset_bits;

    // initializate correct start address
    this->PCstart_addr = 0;

    // count number of address elements
    for ( int i = 0; i < offset_bits; i++)
        this->Offset_Size *= 2;
    for ( int i = 0; i < page_bits; i++)
        this->Page_Size *= 2;
    for ( int i = 0; i < ( addr_size - page_bits - offset_bits); i++)
        this->Set_Size *= 2;

    // create memory
    this->memory = new MemSet* [ Set_Size];

    // save addresses of sets as NULL
    for ( int i = 0; i < Set_Size; i++)
        this->memory[ i] = NULL;

    // get information from input file
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);
    
    // save first address and write data from the file to functional memory model
    for ( int i = 0; i < sections_array.size(); i++)
    {
        if ( !strcmp( ".text", sections_array[ i].name))
        {
            PCstart_addr = sections_array[ i].start_addr;
        }
        this->MemWrite( sections_array[ i].content, sections_array[ i].start_addr, sections_array[ i].size);
    }
}
/**
 * FuncMemory::PageWrite - write information on one page
 * 
 * param    value                       array of input data
 * param    addr                        address to write to
 * param    size                        number of bytes to write
 *
 */
void FuncMemory::PageWrite( uint8* value, uint64* addr, uint64* size)
{
    // find current numbers of address parts
    uint64 cur_set_addr = ( ( this->Set_Size - 1) & ( ( *addr) >> ( this->Offset_Bits + this->Page_Bits)));
    uint64 cur_page_addr = ( ( this->Page_Size - 1) & ( ( *addr) >> this->Offset_Bits));
    uint64 cur_offset_addr = ( ( this->Offset_Size - 1) & ( *addr));
    
    int pos = 0;
    bool written = false;

    // write data to current page until its end or until end of data
    while ( ( cur_set_addr || !written) && ( *size))
    {
        this->memory[ cur_set_addr]->page[ cur_page_addr]->offset[ cur_offset_addr].value = value[ pos];
        ( *addr)++;
        pos++;
        ( *size)--;
        written = true;
        cur_offset_addr = ( ( this->Offset_Size - 1) & ( *addr));
    }
}

/**
 * FuncMemory::MemWrite - write information to the functional memory model
 * 
 * param    value                       array of input data
 * param    addr                        address to write to
 * param    size                        number of bytes to write
 *
 */
void FuncMemory::MemWrite( uint8* value, uint64 addr, uint64 size)
{
    // check input data
    assert( value);
    assert( size);

    // find current numbers of address parts
    uint64 cur_set_addr = ( ( this->Set_Size - 1) & ( addr >> ( this->Offset_Bits + this->Page_Bits)));
    uint64 cur_page_addr = ( ( this->Page_Size - 1) & ( addr >> this->Offset_Bits));
    uint64 cur_offset_addr = ( ( this->Offset_Size - 1) & addr);
    
    // initializate counter of wroten data
    uint64 start_size = size;

    // write data
    while ( size > 0)
    {
        if ( this->memory[ cur_set_addr])
        {
            if ( this->memory[ cur_set_addr]->page[ cur_page_addr])
            {
                this->PageWrite( value + start_size - size, &addr, &size);

                cur_set_addr = ( ( this->Set_Size - 1) & ( addr >> ( this->Offset_Bits + this->Page_Bits)));
                cur_page_addr = ( ( this->Page_Size - 1) & ( addr >> this->Offset_Bits));
            } else
            {
                // creating new page
                this->memory[ cur_set_addr]->page[ cur_page_addr] = new MemPage;
                
                // creating array of values
                this->memory[ cur_set_addr]->page[ cur_page_addr]->offset = new MemOffset[ this->Offset_Size];
                this->PageWrite( value + start_size - size, &addr, &size);

                cur_set_addr = ( ( this->Set_Size - 1) & ( addr >> ( this->Offset_Bits + this->Page_Bits)));
                cur_page_addr = ( ( this->Page_Size - 1) & ( addr >> this->Offset_Bits));
            }
        } else
        {
            // create new set
            this->memory[ cur_set_addr] = new MemSet;
            this->memory[ cur_set_addr]->page = new MemPage*[ this->Page_Size];
            
            // save addresses of pages as NULL
            for ( int i = 0; i < Page_Size; i++)
            {
                this->memory[ cur_set_addr]->page[ i] = NULL;
            }

            // create new page
            this->memory[ cur_set_addr]->page[ cur_page_addr] = new MemPage;
            
            // creating array of values
            this->memory[ cur_set_addr]->page[ cur_page_addr]->offset = new MemOffset[ this->Offset_Size];
            this->PageWrite( value + start_size - size, &addr, &size);

            cur_set_addr = ( ( this->Set_Size - 1) & ( addr >> ( this->Offset_Bits + this->Page_Bits)));
            cur_page_addr = ( ( this->Page_Size - 1) & ( addr >> this->Offset_Bits));
        }
    }
}

/**
 * FuncMemory::~FuncMemory - destruct memory model
 *
 */
FuncMemory::~FuncMemory()
{
    for ( int i = 0; i < this->Set_Size; i++)
    {
        if ( this->memory[ i])
        {
            for ( int j = 0; j < this->Page_Size; j++)
            {
                if ( this->memory[ i]->page[ j])
                {
                    delete [] this->memory[ i]->page[ j]->offset;
                    delete this->memory[ i]->page[ j];
                }
            }
            delete [] this->memory[ i]->page;
            delete  this->memory[ i];
        }
    }
    delete [] this->memory;
}

/**
 * FuncMemory::startPC - give address of the beginning".text" section
 *
 */
uint64 FuncMemory::startPC() const
{
    return this->PCstart_addr;
}

/**
 * FuncMemory::read - read data from memory model
 * 
 * param    addr                        address to read from
 * param    num_of_bytes                number of bytes to read
 *
 */
uint64 FuncMemory::read( uint64 addr, unsigned short num_of_bytes) const
{
    // check input data
    assert( num_of_bytes);
    assert( num_of_bytes <= 8);
    
    // find current numbers of address parts
    uint64 cur_set_addr = ( ( this->Set_Size - 1) & ( addr >> ( this->Offset_Bits + this->Page_Bits)));
    uint64 cur_page_addr = ( ( this->Page_Size - 1) & ( addr >> this->Offset_Bits));
    uint64 cur_offset_addr = ( ( this->Offset_Size - 1) & addr);
    
    // initializate items for output
    uint8 res[ 8];
    uint64 result = 0;

    bool done = false;
    for ( int i = 0; i < num_of_bytes; i++)
    {
        // check existance
        assert( this->memory[ cur_set_addr]);
        assert( this->memory[ cur_set_addr]->page[ cur_page_addr]);
        
        // read one byte from memory
        res[ i] = this->memory[ cur_set_addr]->page[ cur_page_addr]->offset[ cur_offset_addr].value;
        addr++;
        cur_set_addr = ( ( this->Set_Size - 1) & ( addr >> ( this->Offset_Bits + this->Page_Bits)));
        cur_page_addr = ( ( this->Page_Size - 1) & ( addr >> this->Offset_Bits));
        cur_offset_addr = ( ( this->Offset_Size - 1) & addr);
    }

    // link found bytes together
    for ( int i = num_of_bytes - 1; i >= 0; i--)
    {
        result = result * 256 + res[ i];
    }
    return result;
}

/**
 * FuncMemory::write - write information to the functional memory model
 * 
 * param    value                       input data
 * param    addr                        address to write to
 * param    num_of_bytes                number of bytes to write
 *
 */
void FuncMemory::write( uint64 value, uint64 addr, unsigned short num_of_bytes)
{
    // check input data
    assert( num_of_bytes);

    // reinitialization of input data
    uint8 res[ 8];
    for ( int i = 0; i < num_of_bytes; i++)
    {
        res[ i] = uint8 ( ( value >> ( 8 * i)) & 255);
    }

    // now we can write information using MemWrite
    this->MemWrite( res, addr, num_of_bytes);
}

/**
 * FuncMemory::dump - dump of memory model data
 * 
 * param    indent                      some string
 *
 */
string FuncMemory::dump( string indent) const
{
    ostringstream oss;
    
    oss << indent << " Dump of FuncMemory:" << endl;
    bool skipping = false;

    oss << indent << endl << " Address start from:    " << addrtohex( this->startPC()) << endl << indent << endl;
    oss << indent << endl << " Memory:" << endl << indent << endl;

    for ( int i = 0; i < this->Set_Size; i++)
    {
        if ( this->memory[ i])
        {
            for ( int j = 0; j < this->Page_Size; j++)
            {
                if ( this->memory[ i]->page[ j])
                {
                    int offset_last = 0;
                    for ( int k = 0; k < ( this->Offset_Size / 4); k++)
                    {
                        uint64 addr = this->Offset_Size * this->Page_Size * i + this->Offset_Size * j + 4 * k;
                        uint64 val = this->read( addr, 4);
                        if ( val == 0)
                        {
                            if ( !skipping)
                                oss << indent << endl << indent << "          ....  " << endl << indent << endl;
                            skipping = true;
                        } else
                        {
                            skipping = false;
                            oss << indent << "  " << addrtohex( addr) << ":    " << valtohex( val) << endl;
                        }
                        offset_last = k;
                    }
                    offset_last++;
                    if ( this->Offset_Size % 4)
                    {
                        uint64 addr = this->Offset_Size * this->Page_Size * i + this->Offset_Size * j + offset_last * 4;
                        uint64 val = this->read( addr, this->Offset_Size % 4);
                        if ( val == 0)
                        {
                            if ( !skipping)
                                oss << indent << endl << indent << "          ....  " << endl << indent << endl;
                            skipping = true;
                        } else
                        {
                            skipping = false;
                            oss << indent << "  " << addrtohex( addr) << ":    " << valtohex( val) << endl;
                        }
                    }
                }
            }
        }
    }
    oss << indent << " End of FuncMemory" << endl;
    return oss.str();
}

/**
 * valtohex - give stream with hex number
 * 
 * param    val                        number to transform
 *
 */
string valtohex( uint64 val)
{
    ostringstream oss;
    oss << hex;

    oss.width( 8);
    oss.fill( '0');

    oss << val;

    return oss.str();
}

/**
 * addrtohex - give stream with hex address
 * 
 * param    addr                       address to transform
 *
 */
string addrtohex( uint64 addr)
{
    ostringstream oss;
    oss << "0x";
    oss << valtohex( addr);

    return oss.str();
}
