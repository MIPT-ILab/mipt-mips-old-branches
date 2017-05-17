#include <func_instr.h>

using namespace std;

int main( int argc, char* argv[])
{
    std::vector<ElfSection> arr_sect;
    char* sect_name;
    char* exec_name;

    if ( argc ==3)
    {
        sect_name = argv[ 1];
        exec_name = argv[ 2];

        ElfSection::getAllElfSections( exec_name, arr_sect);

        for (int i = 0; i < arr_sect.size(); i++)
        {
            if ( !strcmp( arr_sect[ i].name, sect_name))
            {
                for (int j = 0; j < arr_sect[ i].size; j++)
                {
                    FuncInstr disasm( arr_sect[ i].content[ j]);
                    cout << disasm;
                }
                return 0;
            }
        }
    }
    else
    {
        cerr << "Number of arg must be 2" << endl;
        return EXIT_FAILURE;
    }

}
