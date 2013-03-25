// Generci C
#include <cassert>
#include <cstdlib>

// Generic C++
#include <iostream>
#include <fstream>

// uArchSim modules
#include <cache_tag_array.h>

using namespace std;

const int BLOCK_SIZE = 4;
const int ADDR_SIZE  = 32;

double eval_miss_rate( ifstream& inp, CacheTagArray& cache)
{
    unsigned int hit_num = 0;
    unsigned int read_count = 0;
    size_t tmp_addr;
    char c = 0;
    inp >> hex;
    while ( inp >> c)
    {
        inp >> c >> tmp_addr;
        hit_num += cache.read( tmp_addr);
        ++read_count; 
    }
    return 1 - double(hit_num)/read_count;
}

int main (int argc, char* argv[])
{
    if ( argc != 3)
    {
        cerr << "Wrong number of arguments! You have to enter "
             << "two files: file1.txt file2.csv" << endl;
        exit( EXIT_FAILURE);
    }
    string input(  argv[1]);
    unsigned pos_in = input.find( ".");
    string output( argv[2]);
    unsigned pos_out = output.find( ".");
    if ( input.substr( pos_in + 1) != "txt")
    {
        cerr << "Wrong format of the first file. Needs txt\n";
        exit( EXIT_FAILURE);
    }
    if ( output.substr( pos_out + 1) != "csv")
    {
        cerr << "Wrong format of the second file. Needs csv\n";
        exit( EXIT_FAILURE);
    }
    ofstream out_file( output.c_str());
    unsigned i = 1;
    while ( i <= 16) 
    {
        for ( unsigned int j = 1024; j <= 1048576; j *= 2)
        {
            ifstream inp_file( input.c_str());
            CacheTagArray cache( j, i, BLOCK_SIZE, ADDR_SIZE); 
            out_file << eval_miss_rate( inp_file, cache) << ", ";
            inp_file.close();
        }
        out_file << endl;
        cout << "the loop for " << i << "-way associativity is done\n";
        i *= 2;
    }
    for ( unsigned int j = 1024; j <= 1048576; j *= 2) //for full associativity
    {
        ifstream inp_file( input.c_str());
        CacheTagArray cache( j, j / BLOCK_SIZE, BLOCK_SIZE, ADDR_SIZE);
        out_file << eval_miss_rate( inp_file, cache) << ", ";
        cout << "for " << j << " done\n";
        inp_file.close();
    }
    out_file.close();
    return 0;
}

