/*
 * disasm.cpp - mips disassembler implemented using elf_parser
 * @author Aleksandr Kayda aleksandr.kajda@frtk.ru
 * Copyright 2014 uArchSim iLab 
 */


#include <func_instr.h>
#include <elf_parser.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

int main(int argc, char* argv[])
{

    if ( argc != 3)
    {
        cout << "Need 2 arguments: mips_exe filename and section name" << endl;
        return 1;
    }

    vector<ElfSection> section;
    ElfSection::getAllElfSections( argv[1], section);
    int i;
    for ( i = 0; i < section.size(); i++)
        if ( !strcmp( section[i].name, argv[2]))
            break;
    if ( i == section.size())
    {
        cout << "Section was not found" << endl;
        return 0;
    }
    int j = 0;
    do
    {
        FuncInstr instr((( uint32*) section[i].content)[j >> 2]);
        cout << hex << setfill('0');
        cout << "0x" << setw(8) << (section[i].start_addr + j);
        cout << '\t' << instr.Dump();
    } while ( (j += 4) < section[i].size);

    return 0;
}
