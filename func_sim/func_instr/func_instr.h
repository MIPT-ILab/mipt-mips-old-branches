/**
 * func_instr.h - the module implementing a simple disassembler for MIPS processors.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */


// protection from multi-include
#ifndef FUNC_MEMORY__FUNC_MEMORY_H
#define FUNC_MEMORY__FUNC_MEMORY_H

// Generic C++
#include <vector>
#include <string>
#include <cassert>
#include <sstream>

// uArchSim modules
#include <types.h>
#include <elf_parser.h>

#include "helper.h"


#define MAX_ISA_ENTRIES 15 // Define the number of ISA table entries

using namespace std;
using namespace helper;

class FuncInstr
{
    Format format;
    Parser bytes;
    Type type;
    uint32 isa_id;

    stringstream ss;

    static const ISAEntry isaTable[ ];
    static const string REG_NAME[ 32];

public:
    FuncInstr( uint32 bytes);
    std::string Dump( std::string indent = " ") const;
    friend std::ostream& operator<< ( std::ostream& out, const FuncInstr& instr);

    //std::ostream& operator<<( std::ostream& out, const FuncInstr& instr);
private:
    void initFormat( uint32 bytes);
    void initRType();
    void initIType();
    void initJType();

    void parseR( uint32 bytes);
    void parseI( uint32 bytes);
    void parseJ( uint32 bytes);

};


#endif





