#include <iostream>
#include <func_instr.h>
#include <string>
#include <cstring>
#include <elf_parser.h>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 3)return 0;
    char* executable_file_name = argv[1];
    char* section_name = argv[2];
    std::vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    vector<ElfSection>::iterator it;
    for ( it = sections_array.begin(); strcmp( section_name, it->name); ++it);

    for( uint64 i = 0; i < it->size/4; ++i)
    {
        uint64 value = *( uint32*)( it->content + i);
        cout << "    " << FuncInstr ( value).Dump() << endl;
    }

    return 0;
}
