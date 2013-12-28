/**
 * disasm.cpp - the module implementing a simple disassembler for MIPS processors.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2013 uArchSim iLab project
 */


// Generic C
#include <stdlib.h>

// Generic C++
#include <iostream>
#include <vector>
#include <cstring>

// uArchSim modules
#include <func_instr.h>

using namespace std;


union Word
{
    uint8 byte[4];
    uint32 word;
};


int main (int argc, char* argv[])
{
    // extract all ELF sections into the section_array variable
    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( argv[1], sections_array); //*********

    // print the information about each section
    int i = 0;

    // print the information about each section
    for ( i = 0; i < sections_array.size(); ++i)
        cout << sections_array[ i].dump() << endl;


    for( i = 0; i < sections_array.size(); i++)
        if( strcmp( sections_array[ i].name, ".data") == 0) break;

    Word converter;
    for ( int j = 0; j < sections_array[ i].size; j += 4)
    {
        converter.byte[ 3] = sections_array[ i].content[ j + 0];
        converter.byte[ 2] = sections_array[ i].content[ j + 1];
        converter.byte[ 1] = sections_array[ i].content[ j + 2];
        converter.byte[ 0] = sections_array[ i].content[ j + 3];

        cout << "0x" << hex << converter.word << dec << endl;
        FuncInstr func_instr( converter.word);
        cout << func_instr.Dump("--> ") << endl;
    }







    return 0;
}
