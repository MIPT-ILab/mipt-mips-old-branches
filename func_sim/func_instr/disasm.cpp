#include <func_instr.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

int main(int argc, char *argv[])
{
	
	
	vector<ElfSection> STL_vector;
	ElfSection::getAllElfSections(argv[1], STL_vector);
	
	bool find = 0;
	
	vector<ElfSection>::iterator itr;
	
	for (itr = STL_vector.begin(); itr != STL_vector.end(); ++itr)
	{
		if (itr->name == argv[2])
		{
			find = 1;
			break;
		}
	}
	
	ASSERT(find, "no needed setion");
	ASSERT(!(itr->size % 4), "not a full instr");
	
	
	for ( int i = 0; i < (itr->size / 4); i++)
		{
		FuncInstr finstr(get_instr(itr->content + i*4));
		finstr.dump("");
		}		

}

