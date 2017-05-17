/**
 * reg_file.cpp - module implementing register file 
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// Genereic C
#include <cassert>

// uArchSim modules
#include <types.h>
#include <reg_file.h>

RegFile::RegFile( unsigned int size)
{
    this->size = size; 
    registers = new uint32[size];
    for( unsigned int i = 0; i < size; i++)
    {
        registers[i] = 0;
    }
}

RegFile::~RegFile()
{
    delete [] registers;
}

uint32 RegFile::read( unsigned int reg)
{
    assert( reg < size);
    return registers[reg];
}

void RegFile::write( unsigned int reg, uint32 value)
{
    assert( reg < size);
    registers[reg] = value;
}