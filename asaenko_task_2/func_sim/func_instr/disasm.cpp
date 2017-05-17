#include <stdlib.h>
#include <iostream>
#include <cstring>
#include <func_instr.h>
#include <elf_parser.h>

using namespace std;

int main(int argc, char *argv[])
{
	if(argc != 3) return 0;

    ElfSection::getAllElfSections(argv[1], sections);
    FuncMemory memory(argv[1]);

    vector<ElfSection>::iterator it = sections.begin();

    if (argc == 3) {
        while (it != sections.end() && strcmp(it->name, argv[2]) != 0)
            { ++it;
             cout << FuncInstr ( value).Dump() << endl;
             }

    } else {
        for (it = sections.begin(); it != sections.end(); ++it)
               }

    return 0;
}
