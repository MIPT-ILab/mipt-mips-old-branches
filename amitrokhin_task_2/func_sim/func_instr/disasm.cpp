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

using std::cout;
using std::cerr;
using std::endl;



int main (int argc, char* argv[])   
{
    const int num_of_args = 3;

    if ( argc == num_of_args)
    {
        // extract all ELF sections into the section_array variable
        vector<ElfSection> sections_array;
        ElfSection::getAllElfSections( argv[1], sections_array);

        // print the information about each section
        for ( int i = 0; i < sections_array.size(); ++i)
            cout << sections_array[ i].dump() << endl;


        uint32 section_id;
        for( section_id = 0; section_id < sections_array.size(); ++section_id)
            if( strcmp( sections_array[ section_id].name, argv[ 2]) == 0)
                break;


        for ( uint32 j = 0; j < sections_array[ section_id].size/4; j ++)
        {
            uint32 instruction = ((uint32 *)sections_array[ section_id].content)[j];
            FuncInstr func_instr( instruction);
            cout << "0x" << hex << instruction << dec << "\t"
                 << func_instr << endl;
        }
    }
    else if ( argc == 2 && !strcmp( argv[ 1], "--help"))
    {
        cout << "This program prints a disassembly of the ELF binary file section." << endl
             << endl
             << "Usage: \"" << argv[ 0] << " <ELF binary file>\""
             << " <Section name>\"" << endl;
    }
    else
    {
        cerr << "ERROR: wrong number of arguments!" << endl
             << "Type \"" << argv[ 0] << " --help\" for usage." << endl;
        exit( EXIT_FAILURE);
    }


    return 0;
}
