/** 
 * disasm.cpp - this program reads bytes of the section from the file and gives them for parsing
 * @author Dmitry Ermilov <ermilov@phystech.edu>
 * Copyright 2012 uArchSim iLab project, 2012 uArchSim iLab project
 */
// Genereic C
#include <cassert>
#include <cstdlib>
#include <cstring> 

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_memory.h>
#include <func_instr.h>

using namespace std;

int main( int argc, char* argv[])
{
	const int num_of_args = 2;

	if ( argc - 1 == num_of_args)
    {
        const char* file_name = argv[1];
        const char* section = argv[2];

        FuncMemory func_mem( file_name, 32, 10, 12);       
    
        vector<ElfSection> sections_array;
        ElfSection::getAllElfSections( file_name, sections_array); 

        int i;
        for ( i = 0; strcmp(sections_array[i].name, section); i++, cout << i);   //we find the number of section
//        cout << sections_array[i].start_addr << endl;
//        uint8* ptr = sections_array[i].content;
        uint64 address = sections_array[i].start_addr;                           //we remember the start address
        for ( int j = 0; j < sections_array[i].size; j++)
        {
            FuncInstr instr( func_mem.read( address, 1));                        //we read 1 byte and parse it
            cout << j << address <<  endl;
//            cout << *ptr << endl;
//            FuncInstr instr( *ptr);
//            ptr++;
            cout << instr;                                                       //we putout disasmbled byte
            address++;
        }
    } 
    else if ( argc - 1 > num_of_args)
    {
        cerr << "ERROR: too many arguments!" << endl
             << "Only two argumnt is required, the name of an executable file "     \
                "and name of section." << endl;
        exit( EXIT_FAILURE); 

    } 
    else
    {
        cerr << "ERROR: too few arguments!" << endl
             << "Two argument is required, the name of an executable file "         \
                "and name of section." << endl;
        exit( EXIT_FAILURE);
    }
}
