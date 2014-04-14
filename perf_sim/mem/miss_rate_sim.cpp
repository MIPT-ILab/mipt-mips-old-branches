#include "cache_tag_array.h"

int main(int argc, char* argv[])
{
// Check number of arguments.
    if(argc != 3)
    {
        cerr << "Need two arguments: <input file>, <output file>" << endl;
        exit(1);
    }
// Open output file from write miss rates.    
    ofstream fout(argv[2]);
    if(!fout)
    {
        cerr << "output file cannot open" << endl;
        exit(1);
    }

// For each ways(1,2,4,8,16) and cache size(1,2,...,1024)KB.
    for( uint64 num_ways = 1; num_ways <= 16; num_ways = num_ways * 2)
    {
        for( uint64 cache_size_KB = 1; cache_size_KB <= 1024; cache_size_KB = cache_size_KB * 2)
        {
// Create cache for this parametrs.
            CacheTagArray cache(cache_size_KB * 1024, num_ways);
// Open file for get requests.
            ifstream fin(argv[1]);
            if(!fin)
            {
                cerr << "input file cannot open" << endl;
                exit(1);
            }
// Calculate miss rate. For this we try read all requests addres and write number of succesful tries.    
            uint64 miss_num = 0;
            uint64 hit_num = 0;
   
            uint64 cur_addr;
            while(fin >> hex >> cur_addr)
            {
                bool rezult = cache.read( cur_addr);
                if(rezult == true)
                {
                    hit_num++;
                }
                else
                {
                    miss_num++;
                }
            }
            double miss_rate = (double)miss_num / ((double)miss_num + (double)hit_num);
// Write miss rate in output file.
            fout << setw(3) << miss_rate << " ";
        }
        fout << "\n";
    }


// Repeat all algoritm for full associativity cache. ATTENTION! For this case program can work very long time (some minutes).     
    for(uint64 cache_size_KB = 1; cache_size_KB <= 1024; cache_size_KB = cache_size_KB * 2)
    {
        CacheTagArray cache(cache_size_KB * 1024, cache_size_KB * 1024 / 4);
        ifstream fin(argv[1]);
        if(!fin)
        {
            cerr << "input file cannot open" << endl;
        }

        uint64 miss_num = 0;
        uint64 hit_num = 0;

        uint64 cur_addr;
        while(fin >> hex >> cur_addr)
        {
            bool rezult = cache.read( cur_addr);
            if(rezult)
            {
                hit_num++;
            }
            else
            {
                miss_num++;
            }
        }
        double miss_rate = (double)miss_num / ((double)miss_num + (double)hit_num);
        fout << setw(3) << miss_rate << " ";
    }
    return 0;
}
