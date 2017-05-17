
/**
 * reg_file.h
 * programer-visible memory space accesing via memory address.
 * @author Dmitry Borody (dmitriy.borodiy@gmail.com)>
 * Copyright 2013 uArchSim iLab project
 */

// protection from multi-include
#ifndef REG_FILE__REG_FILE_H
#define REG_FILE__REG_FILE_H

#include <types.h>

class RegFile
{
public:
    RegFile( size_t size);
    ~RegFile();

    sint64 read( size_t register_id) const;
    void write( size_t register_id, sint64 val);

private:
    sint64 *regfile;
    const size_t size;
};

#endif 