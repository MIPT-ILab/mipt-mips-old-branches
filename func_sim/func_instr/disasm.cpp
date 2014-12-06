#include "../elf_parser/elf_parser.h"
//#include "../func_memory/func_memory.h"
#include "func_instr.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <stdlib.h>


int main(int argc, char **argv) {
  assert(argc == 3);
  
  const char *elf_filename = argv[1];
  const char *section_name = argv[2];
  
  vector<ElfSection> elf_sections;
  size_t vector_size;
  
  
  ElfSection::getAllElfSections(elf_filename, elf_sections);
  vector_size = elf_sections.size();
  
  for (size_t section_index = 0; section_index < vector_size; ++section_index) {
    if (elf_sections[section_index].name != section_name) {
      continue;
    }
    
    for (size_t memory_index = 0; 
         (memory_index + 3) < elf_sections[section_index].size; 
         memory_index += 4) {
      FuncInstr funcInstr( *( ( uint32 *) elf_sections[section_index].content));
      
      
      funcInstr.Dump("    ");
    }
  }
  
  return EXIT_SUCCESS;
} 
