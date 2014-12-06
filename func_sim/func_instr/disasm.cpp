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

    vector<ElfSection>::iterator curr_sec = sections.begin();

    if (argc == 3) {
        while (curr_sec != sections.end() && strcmp(curr_sec->name, argv[2]) != 0)
            ++curr_sec;

        cout << curr_sec->name << endl;

        ASSERT(curr_sec != sections.end(), "no needed section");
        ASSERT(!(curr_sec->size % 4), "not a full instruction");

        InstrList instr(argv[2]);

        for (uint64 i = 0; i < curr_sec->size / 4; i++)
    	    instr.add(get_instr(curr_sec->content + i * 4));

        cout << instr;
    } else {
        for (vector<ElfSection>::iterator it = sections.begin();
             it != sections.end(); ++it) {
            ASSERT(!(it->size % 4), "not a full instruction");

            InstrList instr(it->name);

            for (uint64 i = 0; i < it->size / 4; i++) {
                instr.add(get_instr(it->content + i * 4));
                cout << instr.isaTable.back() << endl;
            }
            
            cout << instr;
        } 
    }

    return 0;
}
