#include <func_memory.h>
#include <func_instr.h>

using namespace std;

int main( int argc, char* argv[])
{
	const int num_of_args = 2;

	if ( argc - 1 == num_of_args)
    {
        const char* file_name = argv[1];
        const char* section = argv[2];        
        
        // create the functiona memory
        FuncMemory func_mem( file_name, 32, 10, 12);

        vector<ElfSection> sections_array;
        ElfSection::getAllElfSections( argv[1], sections_array); 

        int i;
        for ( i = 0; strcmp(sections_array[i].name, section); i++);

        uint8* ptr = sections_array[i].content;
        uint64 address = sections_array[i].start_addr;
        for ( int j = 0; j < sections_array[i].size; j++, ptr++)
        {
            FuncInstr instr( read( ptr, 1));
            cout << "    " << instr << endl;
        }
    } 
    else if ( argc - 1 > num_of_args)
    {
        cerr << "ERROR: too many arguments!" << endl
             << "Only two argumnt is required, the name of an executable file "     \
                "and name of section." << endl;
        exit( EXIT_FAILURE); 

    } 
    else
    {
        cerr << "ERROR: too few arguments!" << endl
             << "Two argument is required, the name of an executable file "         \
                "and name of section." << endl;
        exit( EXIT_FAILURE);
    }
}
