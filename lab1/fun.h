#ifndef CACHESIM_H
#define CACHESIM_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#include <tgmath.h>
#include <time.h>

class Cache
{
    public:
        void cache();
        void configure(std::vector<int> config);
        void calculate_bits(int address_size);
        
        int block_size;
        int ways;
        int cache_size;
        int write_policy;
        int miss_penalty;
        int miss_policy;

        /* number of bits out of address */
        unsigned int offset;
        unsigned int index;
        unsigned int tag;

        /* data collection for ouput file */
        unsigned int load_hits;
        unsigned int load_misses;
        unsigned int store_hits;
        unsigned int store_misses;
        unsigned int total_hits;
        unsigned int total_misses;
        unsigned int clock_cycles;
};

class Tag
{
    public:
        int tag;
        int index;
        bool dirty;
};

class Data
{
    public:
        void data();
        void initialize(int ways, int max);
        
        Tag *tag;
        int largest;
};

#endif
