/***************************************************************

	The disasembler implementation using func_instr methods

    @date: December 3, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @version: 1.0 (December 3, 2014) 

***************************************************************/

// Generci C
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_instr.h>

using namespace std;

int main(int argc, char *argv[])
{
	ASSERT(argc == 3, "bad input");

	std::vector<ElfSection> sections;
    ElfSection::getAllElfSections(argv[1], sections);

    bool got_it = 0;
    vector<ElfSection>::iterator curr_sec;

    for (curr_sec = sections.begin(); curr_sec != sections.end(); ++it) {
    	if (curr_sec->name == argv[2]) {
    		got_it = 1;
    		break;
    	}
    }

    ASSERT(got_it, "no needed section");
    ASSERT(!(curr_sec->size % 4), "not a full instruction");

    FuncInstr instr;

    for (uint64 i = 0; i < curr_sec->size / 4; i++)
    	instr.add(get_instr(curr_sec->content + i * 4));

    cout << instr;

    return 0;
}