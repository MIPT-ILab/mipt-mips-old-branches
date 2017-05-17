/**
 * reg_file.cpp
 * @author Yury Samarin <yuri.a.samarin@gmail.com>
 * Copyright 2012 uArchSim iLab project
 */

// Generic C
#include <cassert>
#include <cstdlib>
#include <cstdio>

// Generic C++
#include <iostream>
#include <string> 
#include <sstream>

// uArchSim modules
#include <func_sim.h>

using namespace std;

uint32& RegArray::operator []( unsigned short offset)
{
    assert( offset < 32);
    return reg_arr[ offset];
}
