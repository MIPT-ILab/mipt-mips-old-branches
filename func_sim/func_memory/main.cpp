/*******************************************************

    Reducted by:

    @date: October 14, 2014
    @author: Kirill Korolev <kirill.korolef@gmail.com>
    @vertion: 1.0 (October 14, 2014) 

*******************************************************/

// Generci C
#include <stdlib.h>

// Generic C++
#include <iostream>

// uArchSim modules
#include <func_memory.h>

using namespace std;


int main (int argc, char* argv[])
{
    // Only one argumnt is required, the name of an executable file 
    const int num_of_args = 1;

    if ( argc - 1 == num_of_args)
    {
        // set the name of the executable file
        const char * file_name = argv[1];        
        
        cout << "\nCONSTRUCTOR TESTING... \n" << endl;
        // create the functiona memory
        FuncMemory func_mem( file_name);
        
        cout << "\nDUMP TESTING... \n" << endl
        // print content of the memory
             << func_mem.dump() << endl;

        cout << "\nREAD TESTING... \n\n"
             << "First 4 bytes: " << hex << func_mem.read(0x4100c0, 4) << endl;

        cout << "\nSTARTPC TESTING...\n" << endl
             << "address: 0x" << hex << func_mem.startPC() << endl;

        cout << "\nWRITE TESTING...\n" << endl
             << "b1 = 0x" << hex << func_mem.read(0x400095, 1) << endl
             << "b2 = 0xff" << endl
             << "b1 = b2..." << endl;
        func_mem.write(0xff, 0x400095, 1);
        cout << "Now b1 = 0x" << hex << func_mem.read(0x400095, 1) << endl;

        cout << "\nDESTRUCTOR TESTING... " << endl;
 
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
