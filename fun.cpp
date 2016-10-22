#include "fun.h"

using namespace std;

void Cache::cache()
{
    this->block_size = 0;
    this->offset = 0;
    this->index = 0;
    this->tag = 0;
}

void Cache::initialize(vector<int> config)
{
    this->block_size = config[0];
    this->ways = config[1];
    this->cache_size = config[2];
    this->replacement = config[3];
    this->miss_penalty = config[4];
    this->write_allocate = config[5];
}

void Cache::calculate_bits(int address_size)
{
    this->offset = log2(this->block_size);
    this->index = log2((this->cache_size / this->block_size) / this->ways);
    this->tag = address_size - this->offset - this->index;
}
