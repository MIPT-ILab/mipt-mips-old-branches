/**
 * I AM EXTREMELY SORRY FOR SUCH MUDDY CODE AND NO COMMENTS. CLEAN VERSION DON'T WORK AT 23.42 OCTOBER 18 
 * func_memory.cpp - the module implementing the concept of
 * programer-visible memory space accesing via memory address.
 * @author Alexander Titov <alexander.igorevich.titov@gmail.com>, Dmitry Ermilov <ermilov@phystech.edu>
 * Copyright 2012 uArchSim iLab project, 2012 uArchSim iLab project
 */

// Genereic C
#include <cstdio>
#include <cstring>
#include <cstdlib>

// Generic C++
#include <iostream>
#include <string>
#include <sstream>

// uArchSim modules
#include <func_memory.h>

FuncMemory::FuncMemory( const char* executable_file_name,
                        uint64 addr_size,
                        uint64 page_bits,
                        uint64 offset_bits)
{
//    cerr << "*^*" << addr_size << "," << page_bits << "," << offset_bits <<endl;
    if ( !addr_size | ! page_bits | ! offset_bits )
    {
        exit( EXIT_FAILURE);
    }

    vector<ElfSection> elfs_array;
    ElfSection::getAllElfSections( executable_file_name, elfs_array);
//    cout << "*" << endl;
    this->addr_size = addr_size;
    this->set_bits = addr_size - page_bits - offset_bits;
    this->page_bits = page_bits;
    this->offset_bits = offset_bits;
    
    int i = 0;
    for ( ; strcmp( elfs_array[ i].name, ".text") != 0; i++);
    this->text_start = elfs_array[ i].start_addr;
//    cout << "**" << endl;
    uint64 array_size = 1ULL << ( this->set_bits);
    this->set_array = new uint8**[ array_size];
    memset( this->set_array, 0, array_size);
    
    uint8* its_content;
    uint64 its_size;
    uint64 set_size = 1ULL << this->page_bits;
    uint64 page_size = 1ULL << offset_bits;
//	cout << array_size << "." << set_size << "," << page_size << "," << this->offset_bits <<endl;
//    cout << "****" << endl;
    for ( int i = 0; i < elfs_array.size(); i++)
    {
//        cout << hex << elfs_array[ i].start_addr << "***" << dec << endl;
	uint64* addr = addr_read( elfs_array[ i].start_addr);
//    cout << i << endl;
        its_content = elfs_array[ i].content;
        its_size = elfs_array[ i].size;

//        assert( addr[ 1] >= set_size);

        do
        {
            addr[ 1] %= set_size;

            if ( !this->set_array[ addr[ 0]])
            {
                this->set_array[ addr[ 0]] = set_add();
            }
            if ( !this->set_array[ addr[ 0]][ addr[ 1]])
            {
                this->set_array[ addr[ 0]][ addr[ 1]] = page_add();
            }

  //          cout << addr[ 0] << "," << addr[ 1] << "," << addr[ 2] << endl;
    //        fprintf( stderr, "%p", its_content );
//	    cout << "," << its_size << "," << page_size << endl;
//	    fprintf( stderr, "%p\n",  this->set_array[ addr[ 0]][ addr[ 1]]);
            while ( ( addr[ 2] + its_size) >  page_size)
            {
                uint64 first = page_size - addr[ 2];
  //              cout << first << ",*" << its_content << "," << its_size << endl; 
		memcpy( this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2],
                        its_content,
                        first);
                addr[ 1]++;
                addr[ 2] = 0;
//	        cout << first << "*," << its_content << "," << its_size << endl;
                its_content += first;
                its_size -= first;

                if ( addr[ 1] >= set_size )
                {
                    addr[ 0]++;
                    break;
                }

                if ( !this->set_array[ addr[ 0]][ addr[ 1]])
                {
                    this->set_array[ addr[ 0]][ addr[ 1]] = page_add();
                }
            }
        } while ( addr[ 1] >= set_size);
  //      cout << "^^^" << endl;
   /*     memcpy( this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2], 
                its_content,
                its_size);*/
	uint8* offs = this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2];
	for( int j = 0; j < its_size; j++)
        {
//	     fprintf( stderr, "%p\t%p\n", offs, its_content);
	     *offs = *its_content;
	     offs++;
	     its_content++;
        }
//	cout << "()" << endl;
        addr_close( addr);
    }
}

FuncMemory::~FuncMemory()
{
    delete [] this->set_array;
}

uint64* FuncMemory::addr_read( uint64 address ) const
{
    assert( address);

    uint64 uint_size = 64;
    address = address << ( uint_size - addr_size);
//    cout << "address =" << hex << address << dec << endl;
    uint64* addr = new uint64[ 3]; 
    addr[ 0] = address >> ( uint_size - this->addr_size 
                          + this->page_bits + this->offset_bits);
    addr[ 1] = address << ( this->set_bits);
    addr[ 1] = addr[ 1] >> ( uint_size - this->addr_size 
                           + this->set_bits + this->offset_bits);
//    cout << "addr2 = " << hex << address << dec << endl;
    addr[ 2] = address << ( this->set_bits + this->page_bits);
//    cout << "addr[ 2] =" << hex << addr[ 2] << dec << endl;
    addr[ 2] = addr[ 2] >> ( uint_size - this->addr_size 
                           + this->set_bits + this->page_bits);
//    cout << "addr[ 2] =" << hex << addr[ 2] << dec << endl;
    assert( addr);
    return addr;
}

void FuncMemory::addr_close( uint64* addr ) const
{
    assert( addr);
//    cerr << "Adress set: " << addr[ 0] << " page: " << addr[ 1] 
//    << " offset: " << addr[ 2] << " was closed." << endl;

    delete [] addr;
}


uint8** FuncMemory::set_add()
{
//	cout << "set_add begins" << endl;
    uint64 set_size = 1ULL << this->page_bits;
    uint8** set_mem = new uint8*[ set_size];
    memset( set_mem, 0, set_size);
//	cout << "set_add ends" << endl;

//    assert( set_mem);
    return set_mem;
}

uint8* FuncMemory::page_add()
{
//	cout << "page_add begins" << endl;
    uint64 page_size = 1ULL << this->offset_bits;
    uint8* page_mem = new uint8[ page_size];
    
//    memset( page_mem, 0, page_size);
//	cout<< "page_add ends" << endl;
    return page_mem;

}

uint64 FuncMemory::startPC() const
{
    return this->text_start;
}

uint64 FuncMemory::read( uint64 address, unsigned short num_of_bytes) const
{
    assert( num_of_bytes);
//	cerr << "Num=" << num_of_bytes << endl;
    uint64* addr = this->addr_read( address);
    if ( !this->set_array[ addr[ 0]])
    {
//        ceer << "ERROR: No information in set. It hasn't written yet." << endl;
        assert( 0);
    }
    if ( !this->set_array[ addr[ 0]][ addr[ 1]])
    {
//        ceer << "ERROR: No information in page. It hasn't written yet " << endl;
        assert( 0);
    }

    uint64 set_size = 1ULL << this->page_bits;
    uint64 page_size = 1ULL << this->offset_bits;
    uint64 value = 0;
    uint64 offs_val = 0;
    uint64 shift = 0;
    uint8* offset_pt = this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2];
    if ( (addr[ 2] + num_of_bytes) > page_size)
    {
//        cerr << "><" << addr[ 2] << "," << page_size << endl;
	for( uint64 i = addr[ 2]; i < page_size; i++, num_of_bytes)
        {
            
	        offs_val = *offset_pt << 8 * shift; 
            value = value | offs_val;
//	    cerr << value << "-value" << endl;
            offset_pt++;
	        shift++;
            
        }

        if ( ( addr[ 1] + 1) >= set_size)
        {
//	cerr << "}{" << endl;
            addr[ 0]++;
            addr[ 1] = 0;
        }
        else
        {
            addr[ 1]++;
        }

        addr[ 2] = 0;
        offset_pt = this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2];
    }

    this->addr_close( addr);

    for( int i = shift; i < num_of_bytes; i++)
    {
        offs_val = *offset_pt << 8 * shift; 
        value = value | offs_val;
        offset_pt++;
        shift++;
    }
    // put your code here

    return value;
}

void FuncMemory::write( uint64 value, uint64 address, unsigned short num_of_bytes)
{
    assert( num_of_bytes);
//	cout << "num_of_bytes=" << num_of_bytes << endl;
    uint64* addr = this->addr_read( address);
//   cout << "value=" << hex << value << dec << endl;  
//    cout << hex << address << dec << "-address" << endl;
//    cout << addr[ 0] << "," << addr[ 1] << "," << addr[ 2] << endl;
    if ( !this->set_array[ addr[ 0]])
    {
        this->set_array[ addr[ 0]] = set_add();
    }
    if ( !this->set_array[ addr[ 0]][ addr[ 1]])
    {
        this->set_array[ addr[ 0]][ addr[ 1]] = page_add();
    }

    uint64 set_size = 1ULL << this->page_bits;
    uint64 page_size = 1ULL << this->offset_bits;
    uint8* offset_pt = this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2];
    uint64 shift = 0x000000ff;
    uint64 shift1 = 0;
    
    if ( (addr[ 2] + num_of_bytes) > page_size)
    {
//	cout << ")(\t" << hex << value << dec << "," << addr[ 2] << "," << page_size << endl;
        for( uint64 i = addr[ 2]; i < page_size; i++)
        {
//		cout << "i=" << i << endl;
//		fprintf( stderr, "*off = %i\t", *offset_pt );
		    uint64 in = value & shift ;
//		cout << "val=" << hex << value << dec << "\t";
//		cout << "shift=" << hex << shift << dec << "\t";
//                fprintf( stderr, "in=%u\t", in );
//		cout << "shift1=" << shift1 << endl;
		    *offset_pt = in >> 8 * shift1;
//		fprintf( stderr, "offset = %p\n", offset_pt );
		    shift1++;
//		fprintf( stderr, "*0ffs = %u\n", *offset_pt );
            offset_pt++; 
            shift = shift << 8;
        }

        if ( ( addr[ 1] + 1) >= set_size)
        {
//            cout << "<>" << endl;
	        addr[ 0]++;
            addr[ 1] = 0;
            if ( !this->set_array[ addr[ 0]])
            {
                this->set_array[ addr[ 0]] = set_add();
            }   
        }
        else
        {
            addr[ 1]++;
        }
        if ( !this->set_array[ addr[ 0]][ addr[ 1]])
            {
                this->set_array[ addr[ 0]][ addr[ 1]] = page_add();
            }

        addr[ 2] = 0;
        offset_pt = this->set_array[ addr[ 0]][ addr[ 1]] + addr[ 2];
    }

    this->addr_close( addr);

    for( uint64 i = shift1; i < num_of_bytes; i++ )
    {
//	cout << "i="  << i << endl;
//	fprintf( stderr, "**off = %i\t", *offset_pt);
	    uint64 in = value & shift ;
//	cout << "val=" << hex << in << dec <<  endl;
        *offset_pt = in >> 8 * shift1;
	    shift1++;
//	fprintf( stderr, "**0ffs = %i\n", *offset_pt);
        offset_pt++; 
        shift = shift << 8;
    }
    // put your code here
}

void FuncMemory::set_dump( int set_num) const
{
    uint64 set_size = 1ULL << this->page_bits;
    for ( int i = 0; i < set_size; i++)
    {
        if ( this->set_array[ set_num][ i])
            cout << "Page " << i << "begins with address " 
                 << this->set_array[ set_num][ i] << endl;
    }
}

string FuncMemory::dump( string indent) const
{
    cout << "Dump FuncMemory:" << endl
         << "address size is " << this->addr_size << endl
         << "amount of set bits is " << this->set_bits << endl
         << "amount of page bits is " << this->page_bits << endl
         << "amount of offset bits is " << this->offset_bits << endl;

    uint64 array_size = 1ULL << this->set_bits;
    for ( int i = 0; i < array_size; i++)
    {
        if ( this->set_array[ i])
        {
            cout << "Set " << i << " consists of :" << endl;
            this->set_dump( i);
            cout << endl;
        }
    }
    // put your code here
    return string("ERROR: You need to implement FuncMemory!");
}
