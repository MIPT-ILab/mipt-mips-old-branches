// Generic C
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <elf_parser.h>

using namespace std;

/*
//for testing
static const char * valid_elf_file = "./mips_bin_exmpl.out";
static const char * valid_section_name = ".text";
*/

int main (int argc, char* argv[])
{
    const int num_of_args = 2;
    
    if ( argc == num_of_args && strcmp(argv[1],"--help"))
    {
        // parse and dump .reginfo section
        ElfSection section( argv[1], ".reginfo");
        cout << section.dump() << endl;

        /*
        // testing my methods read, isInside and startAddr like as unit_test
        ElfSection sect( valid_elf_file, valid_section_name);
    
        // testing method read 
        // read 4 bytes from the section start addr
        uint64 right_ret = 0x02324020;
        assert( sect.read( sect.startAddr()) == right_ret);
        
        // read 3 bytes from the section start addr + 1
        right_ret = 0x023240;
        assert( sect.read( sect.startAddr() + 1, 3) == right_ret);
        
        // read 2 bytes from the section start addr + 2
        right_ret = 0x0232;
        assert( sect.read( sect.startAddr() + 2, 2) == right_ret);
        
        // read 1 bytes from the section start addr + 3
        right_ret = 0x02;
        assert( sect.read( sect.startAddr() + 3, 1) == right_ret);
    
        // testing method isInside
        for( short i = 1; i < 8; ++i)
            assert( sect.isInside( sect.startAddr() + i, i) == true);
        
        for( short i = 1; i < 8; ++i)
            assert( sect.isInside( sect.startAddr() + 1000 * i) == false);
        
        assert( sect.isInside( sect.startAddr() - 1) == false);
    
        // testing startAddr
        assert( sect.startAddr() == 0x400090);
        // end of testing
        */

    } else if ( argc != num_of_args)
    {
        cerr << "ERROR: wrong number of arguments!" << endl
             << "Type \"" << argv[0] << " --help\" for usage." << endl;
        exit( EXIT_FAILURE);

    } else
    {
        cout << "This program prints content of \".reginfo\" section" << endl
             << "of ELF binary file, which name is given as only parameter." << endl
             << endl
             << "Usage: \"" << argv[0] << " <ELF binary file>\"" << endl; 
    }

    return 0;
}
