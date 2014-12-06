/**
 * disasm.cpp - The main program, launching an implementation 
 * of disassembler of MIPS ISA.
 * @author Mikhail Lyubogoschev <lyubogoshchev@phystech.edu>
 * Copyright 2014 uArchSim iLab project
 */
#include <iostream>
#include <cassert>
#include <iomanip>
#include <stdlib.h>
#include <func_instr.h>
using namespace::std;


int main( int argc, char** argv)
{
    if ( argc < 3) 
    {
        cout << "You shold pass 2 arguments to the function" << endl;
        exit( 1);
    }
    FuncMemory mem( argv[1]);
    FuncMemory::section workin_sect = mem.FindInVector( argv[2]);
    int cmd1 = 0;
    for ( int i = workin_sect.start_addr; i < workin_sect.start_addr + workin_sect.size; i += 4)
    {
        cmd1 = mem.read( i);
        FuncInstr cmd( cmd1);
        cout << cmd << endl;
    }
    return 0;    
}
