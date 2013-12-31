//Generic C++
#include <iostream>

//Generic C
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>

//uArch sim modules
#include <func_instr.h>
#include <elf_parser.h>

void Parse_section(const vector<ElfSection>& sections_array, int sect_number)
{
    std::cout << "Section: " << sections_array[sect_number].name << std::endl;
    std::cout << std::endl;

    uint64 addr = sections_array[sect_number].start_addr;
    uint32 *content = (uint32 *)sections_array[sect_number].content;
    uint64 size = (sections_array[sect_number].size)/(sizeof(uint32));

    for (uint64 cont_pointer=0; cont_pointer < size; cont_pointer++)
    {
        FuncInstr func_instr(content[cont_pointer]);
        std::cout << "0x" << std::hex << addr << "  ";
        std::cout << func_instr.Dump();
        addr = addr + sizeof(uint32);
    }
}

int main(int argc, char** argv)
{
    //check number of arguments

    if (argc > 3)
    {
        std::cerr << "Too many arguments: expected only two arguments\n";
        exit(EXIT_FAILURE);
    }

    if (argc < 3)
    {
        std::cerr << "Too few arguments: expected two arguments\n";
        exit(EXIT_FAILURE);
    }

    char *executable_file_name = argv[1];
    char *sect_name = argv[2];

    // get all elf sections from binary file

    vector<ElfSection> sections_array;
    ElfSection::getAllElfSections(executable_file_name, sections_array);

    //find section with requested name

    int sect_number = 0;
    bool find_flag = false;
    for (sect_number = 0; sect_number < sections_array.size(); sect_number++)
    {
        if (strcmp(sections_array[sect_number].name, sect_name) == 0)
        {
            find_flag = true;
            break;
        }
    }

    if (find_flag == false)
    {
        std::cerr << "ERROR: There is no section with requested name\n";
        exit(EXIT_FAILURE);
    }

    //parse the section with requested name if it is found

    Parse_section(sections_array, sect_number);

    return 0;
}
