 /**
 * created by Alexander Pronin <alexander.pronin.mipt@gmail.com>
 *
 * v. 1.0: created 05.12.2014 22:06
 *         first version of MIPS disasm
 *
 */

#include <func_instr.h>

using namespace std;

int main( int argc, char* argv[])
{
    if ( argc != 3)
    {
        cerr << "Wrong number of arguments." << endl;
        return EXIT_FAILURE;
    }
    char* executable_file_name = argv[ 1];
    char* section_name = argv[ 2];

    std::vector<ElfSection> sections_array;
    ElfSection::getAllElfSections( executable_file_name, sections_array);

    for (int i = 0; i < sections_array.size(); i++)
    {
        if ( !strcmp( sections_array[ i].name, section_name))
        {
            for (int j = 0; j < sections_array[ i].size; j++)
            {
                FuncInstr funcinstr( sections_array[ i].content[ j]);
                cout << funcinstr;
            }
            return 0;
        }
    }
    assert( 0);
}
