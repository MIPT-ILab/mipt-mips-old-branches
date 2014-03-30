/**
 * miss_rate_sim.cpp - the test comparing different cache types.
 * @author Anton Mitrokhin <anton.v.mitrokhin@gmail.com>
 * Copyright 2014 uArchSim iLab project
 */

#include <iostream>
#include <fstream>

#include "cache_tag_array.h"

//#define NOMSGS

double get_miss_rate(CacheTagArray cta, char path[])
{
    std::ifstream cache_req__file;
    cache_req__file.open (path, std::ios::in);

    if (!cache_req__file.is_open())
    {
        std::cerr << "Error opening a file " << path << std::endl;
        exit(-1);
    }


    uint64 hit_cnt = 0, miss_cnt = 0;

    uint64 addr = 0;
    while(cache_req__file >> std::hex >> addr)
    {
        if(cta.read(addr))
            hit_cnt++;
        else
        {
            cta.write(addr);
            miss_cnt++;
        }
    }

    cache_req__file.close();
    return double(miss_cnt)/double(hit_cnt+miss_cnt);
};


int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input_file> <output_file>" << std::endl;
        return -1;
    }

    std::ofstream hitrate_file;
    hitrate_file.open (argv[2], std::ios::out | std::ios::trunc);

    if (!hitrate_file.is_open())
    {
        std::cerr << "Error opening a file " << argv[2] << std::endl;
        exit(-1);
    }



    for(int ways = 1; ways <= 16; ways *= 2)
    {
        for(int cache_size = 1; cache_size <= 1024; cache_size *= 2)
        {
            CacheTagArray cta(cache_size*1024, ways, 4, 32);
            double miss_rate = get_miss_rate(cta, argv[1]);
    #ifndef NOMSGS
            std::cout << miss_rate << ", ";
    #endif // ifndef NOMSGS
            hitrate_file << miss_rate << ", ";
        }
    #ifndef NOMSGS
        std::cout << std::endl;
    #endif // ifndef NOMSGS
        hitrate_file << std::endl;
    }

    // Full associative cache
    #ifndef NOMSGS
    std::cout << std::endl << "Full associative cache test. May take a long time" << std::endl;
    #endif // ifndef NOMSGS
    for(int cache_size = 1; cache_size <= 1024; cache_size *= 2)
    {
        CacheTagArray cta(cache_size*1024, cache_size*1024/4, 4, 32);
        double miss_rate = get_miss_rate(cta, argv[1]);
    #ifndef NOMSGS
        std::cout << miss_rate << ", \n";
    #endif // ifndef NOMSGS
        hitrate_file << miss_rate << ", ";
    }




    hitrate_file.close();
    return 0;
};
