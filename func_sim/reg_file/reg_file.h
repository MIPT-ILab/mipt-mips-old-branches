/**
 * reg_file.h - header of module implementing register file 
 * @author Pavel Poroshin <pavel.a.poroshin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef REG_FILE__REG_FILE_H
#define REG_FILE__REG_FILE_H

// uArchSim modules
#include <types.h>

class RegFile
{
    uint32 *registers;
    unsigned int size;

public:

    RegFile( unsigned int size);
    ~RegFile();
    uint32 read( unsigned int reg);
    void   write( unsigned int reg, uint32 value);
};

#endif