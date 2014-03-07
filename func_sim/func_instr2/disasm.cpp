#include <iostream>
#include "func_instr.h"
#include "stdlib.h"
#include "string.h"
#include <elf_parser.h>
#include "stdio.h"

int main( int argc, char** argv)
{
    if ( argc != 3 )
        return -1;
    // Data initialisation ------------------------------------------
    vector<ElfSection> sections;
    ElfSection::getAllElfSections( argv[1], sections );
    vector<ElfSection>::iterator section;
    uint64 i;
    section = sections.begin();
    while ( strcmp( section->name, argv[2])  && (section != sections.end()) )
        section++;
    if ( strcmp( section->name, argv[2] ) )
        return -1;
    
    
    // disasembling
    FuncInstr** instructions =(FuncInstr**)malloc( sizeof(void*)* section->size/4);
    uint32* content = (uint32*)section->content;
    for( i =0 ; i< section->size/4; i++)// char = 8 bit !!!
    {
        instructions[i] = new FuncInstr( content[i] );
    }
    std::cout<<"section: "<<argv[2]<<"\n";
    for( i =0 ; i< section->size/4; i++)
    {
        std::cout<<"\t"<<*(instructions[i]);
    }
    return 0;
}

