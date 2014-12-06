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
#include <cstring>
#include <bitset>

// uArchSim modules
#include <func_instr.h>

using namespace std;

int main(int argc, char *argv[])
{
	ASSERT(argc == 3 || argc == 2, "bad input");

	vector<ElfSection> sections;
    ElfSection::getAllElfSections(argv[1], sections);

    vector<ElfSection>::iterator it = sections.begin();

    if (argc == 3) {
        while (it != sections.end() && strcmp(it->name, argv[2]) != 0)
            ++it;

        ASSERT(it != sections.end(), "no needed section");
        
        print_sec(it);
    } else {
        for (it = sections.begin(); it != sections.end(); ++it)
            print_sec(it);
    }

    return 0;
}
