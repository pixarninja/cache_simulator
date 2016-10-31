/************************************************************************
 *
 *	filename: fun.cpp
 *
 *	description: Holds methods and classes for the cache simulator.
 *
 *	author: Harris, Mark
 *
 *	class:		CSE 331
 *	instructor:	Zheng
 *	assignment:	Lab Project #1
 *
 *
 *	assigned:	Oct 17, 2016
 *
 *	assignmed:	Oct 17, 2016
 *	due:		Oct 31, 2016
 *
 ***********************************************************************/

#include "fun.h"

using namespace std;

void Cache::cache()
{
    this->block_size = 0;
    this->ways = 0;
    this->cache_size = 0;
    this->write_policy = 0;
    this->miss_penalty = 0;
    this->miss_policy = 0;
    this->offset = 0;
    this->index = 0;
    this->tag = 0;
    this->load_hits = 0;
    this->load_misses = 0;
    this->store_hits = 0;
    this->store_misses = 0;
    this->total_hits = 0;
    this->total_misses = 0;
    this->clock_cycles = 0;
}

void Cache::configure(vector<int> config)
{
    this->block_size = config[0];
    this->ways = config[1];
    this->cache_size = config[2] * 1024;
    this->miss_policy = config[3];
    this->miss_penalty = config[4];
    this->write_policy = config[5];
}

void Cache::calculate_bits(int address_size)
{
    this->offset = log2(this->block_size);
    this->index = log2((this->cache_size / this->block_size) / this->ways);
    this->tag = address_size - this->offset - this->index;
}

void Data::data()
{
    this->tag = NULL;
    this->largest = 0;
}

void Data::initialize(int ways, int max)
{
    int i;
    this->tag = new Tag [ways];
    for(i = 0; i < ways; i++) {
        this->tag[i].tag = max;
        this->tag[i].index = -1;
        this->tag[i].dirty = false;
    }
}
