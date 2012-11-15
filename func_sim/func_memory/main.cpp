// Generci C
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_memory.h>

using namespace std;

int main (int argc, char* argv[])
{
    // Only one argumnt is required, the name of an executable file 
    const int num_of_args = 1;

    if ( argc - 1 == num_of_args)
    {
        // set the name of the executable file
        const char * file_name = argv[1];        
        
        // define the list of section to extract from the file 
        // into the functional memory
        short num_of_sections = 2;
        const char * const section_names[] = {".text", ".reginfo"};

        // create the functional memory
        FuncMemory func_mem( file_name, section_names, num_of_sections);
        
        // print content of the memory
        cout << func_mem.dump() << endl;


        // check method read
        uint64 valid_addr_of_sect_text = 0x400090;
        uint64 valid_addr_of_sect_reginfo = 0x400088;

        num_of_sections = 1;
        const char* const section_reginfo_name[] = {".reginfo"};

        // create the functional memory
        FuncMemory func_mem_reginfo( file_name,
                                     section_reginfo_name,
                                     num_of_sections);

        // will be printed
        cout << "0x" << hex << valid_addr_of_sect_reginfo << ": "
             << func_mem_reginfo.read( valid_addr_of_sect_reginfo) << dec
             << endl;

        // will be aborted
        cout << "0x" << hex << valid_addr_of_sect_text << ": "
             << func_mem_reginfo.read( valid_addr_of_sect_text) << dec
             << endl;
 
    } else if ( argc - 1 > num_of_args)
    {
        cerr << "ERROR: too many arguments!" << endl
             << "Only one argumnt is required, the name of an executable file." << endl;
        exit( EXIT_FAILURE); 

    } else
    {
        cerr << "ERROR: too few arguments!" << endl
             << "One argument is required, the name of an executable file." << endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}

