// Generci C
#include <string.h>
#include <stdlib.h>
#include <assert.h>

// Generic C++
#include <iostream>
#include <iomanip>

// uArchSim modules
#include <elf_parser.h>
#include <func_instr.h>

using namespace std;

int main ( int argc, char* argv[])
{
    // Constants used for pretty-printing
    const string indent_offset_string = "    ";
    const string indent_disasm_string = "        ";
    const int indent_offset = indent_offset_string.length();
    const int table_width = 40;

    const int min_num_of_args = 3;
    
    if ( argc >= min_num_of_args && strcmp( argv[1], "--help"))
    {
        for ( size_t i = 2; i < argc; ++i)
        {
            // TODO: refactor it!
            ElfSection section( argv[ 1], argv[ i]);

            cout << "Disassembly of the section " << argv[ i] << ":" << endl 
                 << endl;

            uint64 instr_addr = section.startAddr();
            for ( ; section.isInside( instr_addr, 4); instr_addr += 4) 
            {
                uint32 instr_bits = section.read( instr_addr);
                FuncInstr instr( instr_bits);

                cout << indent_offset_string << setw( 8);
                cout.fill( '0');
                cout << right << hex << instr_addr 
                     << instr.dump( indent_disasm_string) << endl;
            }

            cout << endl;
        }
    } else if ( argc == 2 && !strcmp( argv[ 1], "--help"))
    {
        cout << "MIPS disassembler." << endl << endl
             << "Usage: \"" << argv[0] 
             << " <ELF binary file> <section name> [<section_name>, ...]\""
             << endl;

    } else
    {
        cerr << "ERROR: wrong number of arguments!" << endl
             << "Type \"" << argv[ 0] << " --help\" for usage." << endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}
