#ifndef CACHESIM_H
#define CACHESIM_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>

#include <tgmath.h>

class Cache
{
    public:
        void cache();
        void initialize(std::vector<int> config);
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
};

#endif
