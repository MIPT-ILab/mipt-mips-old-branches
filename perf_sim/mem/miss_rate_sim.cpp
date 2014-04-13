#include "cache_tag_array.h"
#include <iostream>
#include <fstream>
#include <types.h>
#include <vector>

using namespace std;

double CacheTest(  uint64 size_in_bytes,
                   uint64 ways,
                   unsigned short block_size_in_bytes,
                   unsigned short addr_size_in_bits,
                   vector<uint64>& adresses)

{
    vector<uint64>::iterator adress;
    double hit_rate = 0;
    CacheTagArray* cache = new CacheTagArray(size_in_bytes,
                                                 ways ,
                                                 block_size_in_bytes,
                                                 addr_size_in_bits);
    
    cout<<size_in_bytes<<" "<<ways<<" "<<block_size_in_bytes<<" "<<addr_size_in_bits<<"\n";
    for( adress = adresses.begin(); adress < adresses.end(); adress++)
    {
        cache->read(*adress);
    }   
    hit_rate = cache->GetMissRate();
    cache->~CacheTagArray();
    return hit_rate;
}


int main( int argc, char** argv)
{
    if (argc !=3)
        return -1;
    uint64 addr;
    ifstream file;
    file.open(argv[1]);
    ofstream fileOut;
    fileOut.open(argv[2]);
    vector<uint64> adresses;

while ( file >> hex >> addr )
{
    adresses.push_back(addr);
}

uint64 ways[] = {1, 2, 4, 8, 16};
for( int j = 0; j< 5; j++)
{
    for( int i = 1024 ; i <= 1024*1024; i*=2)
        fileOut<<CacheTest(i, ways[j] , 4, 32, adresses )<<", ";
    fileOut<<"\n";
}
for( int i = 1024 ; i <= 1024*1024; i*=2)
    fileOut<<CacheTest(i, i/4 , 4, 32, adresses )<<", ";
fileOut<<"\n";
return 0;
}

