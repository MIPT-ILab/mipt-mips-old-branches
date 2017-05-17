#include <cstring>
#include <stdlib.h>
#include <iostream>
#include <func_instr.h>
#include <types.h>
#include <elf_parser.h>

int main( int argc, char *argv[])
{
    if ( argc == 3)
    {
        ElfSection es (argv[1], argv[2]);
        uint64 addr = es.startAddr();
        while ( es.isInside (addr, sizeof(uint32)))
        {
            FuncInstr  instr( section.read (addr));
            std :: cout << instr.Dump ("    ") << std::endl;
            addr += sizeof (uint32);
        }
    }else {
        std::cerr <<"Error : wrong number of arguments" << std:: end
          }
        return 0;
}
