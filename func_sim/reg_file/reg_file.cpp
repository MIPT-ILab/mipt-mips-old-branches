/*
 * Implementation of register file
 * author Alexander Kravtcov
 */

// Generic C++
#include <iostream>

// Generic C
#include <cstdlib>
#include <cassert>

// uArchSim module
#include <reg_file.h>

RegFile::RegFile()
{
    for ( int i = 0; i <= SIZE_OF_REGISTERS; ++i)
    {
        this->reg_file[ i] = NO_VAL64;
    }
}

uint64 RegFile::read( RegFile::Registers reg)
{
    assert( reg != SIZE_OF_REGISTERS);
    return this->reg_file[ reg];
}

void RegFile::write( uint64 value, RegFile::Registers reg)
{
    assert( reg != SIZE_OF_REGISTERS);

    if ( reg == ZERO)
    {
        std::cerr << "ERROR. \"$zero\" is read-only register" << std::endl;
        exit( EXIT_FAILURE);
    } else
    {
        this->reg_file[ reg] = value;
    }
}

