/*
 * disasm.cpp
 *
 *  Created on: Feb 7, 2014
 *      Author: thar0l
 */


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


int main(int argc, char* argv[])
{
    if (argc != 2)
     {
         std::cerr << "Usage error. Use " << argv[0] << " input_file_name" << std::endl;
         exit(EXIT_FAILURE);
     }


     vector<ElfSection> sections;
     ElfSection::getAllElfSections(argv[1], sections);

     for (int i = 0; i < sections.size(); i++)
     {
         if (strcmp(sections[i].name, ".text") == 0)
         {
                 uint32 *content = (uint32 *)sections[i].content;
                 uint64 size = (sections[i].size)/(sizeof(uint32));

                 for (uint64 j=0; j < size; j++)
                 {
                     FuncInstr func_instr(content[j]);
                     std::cout << func_instr.Dump("Dump: ");
                 }
                 exit(EXIT_SUCCESS);
         }
     }

     std::cerr << "No .text section in " << argv[1] << std::endl;;
     exit(EXIT_FAILURE);
}

