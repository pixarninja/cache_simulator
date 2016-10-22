#ifndef CACHESIM_H
#define CACHESIM_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

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
        int replacement;
        int miss_penalty;
        int write_allocate;

        int offset;
        int index;
        int tag;
};

#endif
