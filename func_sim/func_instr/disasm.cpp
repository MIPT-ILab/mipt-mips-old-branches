#include <iostream>
#include "func_instr.h"
#include "stdlib.h"
#include "string.h"
#include <elf_parser.h>

int main( int argc, char** argv)
{
if ( argc != 3 )
    return -1;
// Data initialisation ------------------------------------------
vector<ElfSection> sections;
ElfSection::getAllElfSections( argv[1], sections );
vector<ElfSection>::iterator section;
uint64 i;
while ( strcmp( section->name, ".text")  && (section != sections.end()) )
    section++;
if ( !strcmp( section->name, ".text" ) )
    return -1;
FuncInstr** instructions =(FuncInstr**)malloc( sizeof(void*)* section->size);
for( i =0 ; i< section->size; i++)
{
    instructions[i] = new FuncInstr( section->content[i]);
}

for( i =0 ; i< section->size; i++)
{
    std::cout<<*(instructions[i]);
}
return 0;
}
