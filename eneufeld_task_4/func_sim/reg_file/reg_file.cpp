#include <string>
#include <sstream>

#include <types.h>
#include "reg_file.h"

RegFile::RegFile()
{
    for( int i = 0; i < RegCount; i++) 
    {
        reg_array[ i] = 0;
    }
    return;
}
const char *RegFile::reg_name[] = { "$zero", "$at", "$v0", "$v1",
            "$a0", "$a1", "$a2", "$a3", "$t0", "$t1", "$t2", "$t3",
            "$t4", "$t5", "$t6", "$t7", "$s0", "$s1", "$s2", "$s3",
            "$s4", "$s5", "$s6", "$s7", "$t8", "$t9", "$k0", "$k1",
            "$gp", "$sp", "$fp", "$ra"}; 

sint64 RegFile::read( int number)
{
    return reg_array[ number];
}

void RegFile::write( sint64 reg_data, int number)
{
    if( number != 0)
    {
        reg_array[ number] = reg_data;
    }
}

std::string RegFile::dump( std::string indent) const
{
    std::ostringstream oss;
    oss << indent << "Dump Register File" << std::endl
        << indent << "  size = " << RegCount << std::endl
        << indent << "  Content:" << std::endl;
    for( int i = 0; i < RegCount; i++)
    {
        oss << indent << dump( i) << " ";
        if( i % 4 == 3)
        {
            oss << std::endl << "  "; 
        }
    }
    oss << std::endl;
    return oss.str();
}

std::string RegFile::dump( int number) const
{
    std::ostringstream oss;
    if( 0 <= number && number < RegCount)
    {
        oss << reg_name[ number] << "( " << reg_array[ number] << ")";
    }
    return oss.str();
}


