/**
 * disasm.cpp
 * MIPS binary file disassembler.
 * MIPT-MIPS-2014 Assignment 2.
 * Ladin Oleg.
 */

// Generic C
#include <stdlib.h>
#include <string.h>

// uArchSim modules
#include <func_instr.h>
#include <elf_parser.h>
#include <func_memory.h>

using namespace std;

void decode( ElfSection es, char* file); // Reads and decodes instructions

int main( int argc, char* argv[])
{
    /**
     * Get all sections of file 'argv[ 1]' and find the section 'argv[ 2]',
     * then call decoder. Detecting only wrong amount of arguments. Wrong file
     * name detects by getAllElfSections, wrong section - later.
     */
    if ( argc != 3) // Two required arguments: file and section names
    {
        std::cerr << "ERROR: WRONG AMOUNT OF ARGUMENTS: " << argc - 1
                  << " OF 2" << std::endl;
        exit( EXIT_FAILURE);
    }

    vector<ElfSection> sections_array; // Vector of all sections
    ElfSection::getAllElfSections( argv[ 1], sections_array); // Get them
    int i; // Counter
    /* Brute-force searching suitable section name. */
    for ( i = 0; i < sections_array.size(); i++) // Till vector is over
    {
        if ( !strcmp( sections_array[ i].name, argv[ 2]))
        {
            decode( sections_array[ i], argv[ 1]); // Decode this section
            break;
        }
    }
    if ( i == sections_array.size()) // If there is no this section
    {
        std::cerr << "ERROR: CAN'T FIND SECTION: " << argv[ 2] << std::endl;
        exit( EXIT_FAILURE);
    }
    return 0;
}

void decode( ElfSection es, char* file)
{
    /**
     * Function creates functional memory and reads all 4 bytes - instructions
     * from 'start_addr' - begin to 'start_addr' + 'size' - end of the section.
     */
    FuncMemory fm( file, 32, 10, 12); // Creat functional memory
    for ( int i = 0; i < ( es.size / 4); i++) // Till we reach size (by 4-bytes)
    {
        FuncInstr fi( fm.read( es.start_addr + ( i * 4), 4)); // Parse instruction
        std::cout << fi.Dump( "    ") << std::endl; // Print it
    }
    return;
}
