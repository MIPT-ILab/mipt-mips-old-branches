//generic uarchsim
#include "cache_tag_array.h"

//generic c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

//read address from file
unsigned int read_addr(FILE *input)
{
    assert(input != NULL);
    unsigned int addr;
    if (fscanf(input, "%x", &addr) != EOF)
    {
        return addr;
    }
    else
    {
        return EOF;
    }
}


//-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{

    if (argc != 3)
    {
        std::cerr << "Wrong number of arguments: \n"
                  << "Praogram takes two arguments: the first is name of input test file, the second is name of output file\n";
        exit(EXIT_FAILURE);
    }

    FILE *input = fopen(argv[1], "r");
    if (input == NULL)
    {
        std::cerr << "Error: input file was not opened\n";
        exit(EXIT_FAILURE);
    }

    FILE *output = fopen(argv[2], "w");
    if (output == NULL)
    {
        std::cerr << "Error: output file was not opened\n";
        exit(EXIT_FAILURE);
    }

    //2, 4, ... 16-way associative cache of sizes 1kb...1024kb
    std::cout << "Test for:\n";
    for (unsigned int ways = 1; ways <= 16; ways = ways * 2)
    {
        for (unsigned int size_in_bytes = 1024; size_in_bytes <= 1048576; size_in_bytes = size_in_bytes * 2)
        {
            printf("Cache_size: %u KB Number of ways: %u\n", size_in_bytes/1024, ways);

            //initializetion of cache
            CacheTagArray Cache(size_in_bytes, ways, 4, 32);

            unsigned int addr = 0;
            unsigned long long int miss = 0;         //number of miss requests
            unsigned long long int counter = 0;      //number of all requests
            rewind(input);

            while ((addr = read_addr(input)) != EOF)
            {
                if (Cache.read(addr) == false)
                {
                    Cache.write(addr);
                    miss++;
                }
                counter++;
            }
            double miss_rate = (double)((double)miss / (double)counter);
            fprintf(output, "%f, ", miss_rate);

        }
        fprintf(output, "\n");
    }

    //full associative caches of size 1kb...1024kb
    for (unsigned int size_in_bytes = 1024; size_in_bytes <= 1048576; size_in_bytes = size_in_bytes * 2)
    {
        unsigned int ways = size_in_bytes / 4;
        printf("Cache_size: %u KB Full associative\n", size_in_bytes/1024, ways);

        //initialization of cache
        CacheTagArray Cache(size_in_bytes, ways, 4, 32);

        unsigned int addr = 0;
        unsigned long long int miss = 0;         //number of miss requests
        unsigned long long int counter = 0;      //number of all requests
        rewind(input);

        while ((addr = read_addr(input)) != EOF)
        {
            if (Cache.read(addr) == false)
            {
                Cache.write(addr);
                miss++;
            }
            counter++;
        }

        double miss_rate = (double)((double)miss / (double)counter);
        fprintf(output, "%f, ", miss_rate);

    }
    fprintf(output, "\n");

//--------------------------------------------------------------------------------------------------------------------------------------------------------------

    fclose(input);
    fclose(output);

    return 0;
}



