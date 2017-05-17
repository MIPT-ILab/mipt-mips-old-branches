// Generci C
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include "func_memory.h"

using namespace std;


int main (int argc, char* argv[])
{
    // Only one argumnt is required, the name of an executable file 
    const int num_of_args = 1;

    if ( argc - 1 == num_of_args)
    {
        // set the name of the executable file
        const char * file_name = argv[1];

        // create the functiona memory
        FuncMemory func_mem( file_name, 32, 10, 12);
        func_mem.write(1512, 0x4100c3, 2);
        cout << "READ:" << func_mem.read(0x4100c3, 2) << endl;
        cout << "DUMP" << endl << func_mem.dump() << "DUMP END" << endl;
        // print content of the memory
        //cout << func_mem.dump() << endl;
 
    } else if ( argc - 1 > num_of_args)
    {
        cerr << "ERROR: too many arguments!" << endl
             << "Only one argumnt is required, the name of an executable file." << endl;
        exit( EXIT_FAILURE); 

    } else
    {
        cerr << "ERROR: too few arguments!" << endl
             << "One argument is required, the name of an executable file." << endl;
        exit( EXIT_FAILURE);
    }

    return 0;
}
