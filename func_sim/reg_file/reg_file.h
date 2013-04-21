/**
 * reg_file.h 
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// protection from multi-include
#ifndef REG_FILE_H__REG_FILE_H
#define REG_FILE_H__REG_FILE_H

// Generic C++

// uArchSim modules
#include <types.h>

class RegArray
{
    static const short num_of_reg = 32;
    uint32 reg_arr[ num_of_reg];
public:
    uint32& operator []( unsigned short offset);
};

#endif //REG_FILE_H__REG_FILE_H
