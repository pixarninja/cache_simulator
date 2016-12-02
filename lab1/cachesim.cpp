/************************************************************************
 *
 *	filename: cachesim.cpp
 *
 *	description: Implements the cache simulator.
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

int main(int argc, char *argv[])
{
    if(argc < 3) {
        cout << "Please enter the config file followed by the trace file. " << endl << "Exiting Program. Good day." << endl;
        exit(EXIT_FAILURE);
    }    
    Cache cache;
    cache.cache();
    Data *data;
    vector<char> type;
    vector<int> address;
    vector<int> last_access;
    vector<int> config;
    size_t pos;
    string line;
    string sub_address;
    string binary;
    string delim = "/";
    char sub_type;
    unsigned int total_run_time = 0;
    double total_hit_rate = 0;
    double load_hit_rate = 0;
    double store_hit_rate = 0;
    double latency = 0;
    int sub_last_access = 0;
    int tag = 0;
    int index = 0;
    int index_max = 0;
    int tag_max = 0;
    int i = 0;
    int j = 0;
    int config_size = 0;
    int trace_size = 0;
    int address_size = 32;
    int found = 0;
    /* setup input streams */
    ifstream config_stream;
    config_stream.open(argv[1]);
    ifstream trace_stream;
    trace_stream.open(argv[2]); 
    /* setup output stream */
    ofstream output;
    string output_filename = argv[2];
    string tmp = argv[2];
    /* initialize random seed */
    srand(time(NULL));
    while((pos = tmp.find(delim)) != string::npos) {
        output_filename = tmp.substr(pos + 1, tmp.length());
        tmp.erase(0, pos + delim.length());
    }
    output_filename.append(".out");
    output.open(output_filename);
    /* parse config file */
    while(config_stream >> line) {
        config.push_back(stoi(line));
        config_size++;
    }
    /* parse trace file */
    while(trace_stream >> sub_type >> sub_address >> sub_last_access) {
        type.push_back(sub_type);
        address.push_back(stoul(sub_address, nullptr, 16));
        last_access.push_back(sub_last_access);
        trace_size++;
    }
    /* configure cache */
    cache.configure(config);
    if(cache.ways == 0) { //if direct-mapped
        index_max = 1;
        cache.ways = 1;
    }
    else {
        index_max = (cache.cache_size / (cache.ways * cache.block_size));
    }
    cache.calculate_bits(address_size);
    cout << cache.index << endl;
    index_max = pow(2, cache.index);
    tag_max = pow(2, cache.tag);
    /* initialize data structure */
    data = new Data [index_max];
    for(i = 0; i < index_max; i++) {
        data[i].data();
        data[i].initialize(cache.ways, tag_max);
    }
    /* find the offset, index, and tag for each trace entry */
    for(i = 0; i < trace_size; i++) {
        binary = bitset<32>(address[i]).to_string();
        tag = stoi(binary.substr(0, cache.tag), nullptr, 2);
        binary.erase(0, cache.tag);
        index = stoi(binary.substr(0, cache.index), nullptr, 2);
        binary.erase(0, cache.index);
        /* set clock_cycles and run_time */
        cache.clock_cycles += 1 + last_access[i];
        total_run_time += 1 + last_access[i];
        /* loop through all the tags in the set */
        for(j = 0; j < cache.ways; j++) {
            if(data[index].tag[j].tag == tag) { //tag matches
                found = j;
                j = cache.ways;
            }
            else { //tag doesn't match
                found = tag_max;
            }
        }
        if(found != tag_max) { //tag matched
            total_run_time += 1;
            cache.total_memory_ops++;
            cache.total_hits++;
            switch(type[i]) {
            case 'l': //read hit
                cache.load_hits++;
                cache.load_memory_ops++;
                break;
            case 's': //write hit
                cache.store_hits++;
                cache.store_memory_ops++;
                data[index].tag[j].dirty = true;
                break;
            default:
                exit(EXIT_FAILURE);
            }
        }
        else { //tag didn't match
            //total_run_time += cache.miss_penalty + 1;
            total_run_time += cache.miss_penalty;
            cache.total_memory_ops++;
            cache.total_misses++;
            switch(type[i]) {
            case 'l': //read miss
                cache.load_misses++;
                cache.load_memory_ops++;
                found = 0;
                for(j = 0; j < cache.ways; j++) {
                    if(data[index].tag[j].tag == tag_max) { //empty slot found
                        data[index].tag[j].tag = tag;
                        data[index].tag[j].index = 0;
                        for(j = 0; j < data[index].largest; j++) {
                            data[index].tag[j].index++;
                        }
                        data[index].largest++;
                        if(data[index].largest > cache.ways) { //corrects largest index
                            data[index].largest = cache.ways;
                        }
                        found = 1;
                        j = cache.ways;
                    }
                }
                if(!found) { //check which policy to use if no empty slot was found
                    if(cache.miss_policy) { //FIFO
                        for(j = 0; j < cache.ways; j++) {
                            if(data[index].tag[j].index == data[index].largest - 1) {
                                data[index].tag[j].tag = tag;
                                data[index].tag[j].index = -1;
                                data[index].tag[j].dirty = false;
                                for(j = 0; j < data[index].largest; j++) {
                                    data[index].tag[j].index++;
                                }
                            }
                        }
                    }
                    else { //random
                        j = rand() % cache.ways;
                        data[index].tag[j].tag = tag;
                        data[index].tag[j].dirty = false;
                    }
                }
                break;
            case 's': // write miss
                cache.store_misses++;
                cache.store_memory_ops++;
                if(cache.write_policy == 1) { //write-allocate
                    found = 0;
                    for(j = 0; j < cache.ways; j++) {
                        if(data[index].tag[j].tag == tag_max) { //empty slot found
                            data[index].tag[j].tag = tag;
                            data[index].tag[j].index = 0;
                            for(j = 0; j < data[index].largest; j++) {
                                data[index].tag[j].index++;
                            }
                            data[index].largest++;
                            if(data[index].largest > cache.ways) { //corrects largest index
                                data[index].largest = cache.ways;
                            }
                            found = 1;
                            j = cache.ways;
                        }
                    }
                    if(!found) { //check which policy to use if no empty slot was found
                        if(cache.miss_policy) { //FIFO
                            for(j = 0; j < cache.ways; j++) {
                                if(data[index].tag[j].index == data[index].largest - 1) {
                                    data[index].tag[j].tag = tag;
                                    data[index].tag[j].index = -1;
                                    data[index].tag[j].dirty = true;
                                    for(j = 0; j < data[index].largest; j++) {
                                        data[index].tag[j].index++;
                                    }
                                }
                            }
                        }
                        else { //random
                            j = rand() % cache.ways;
                            data[index].tag[j].tag = tag;
                            data[index].tag[j].dirty = true;
                        }
                    }
                }
                else { //no-write-allocate
                    ;
                }
                break;
            default:
                exit(EXIT_FAILURE);
            }
        }
    }
    /* calculate output */
    //total_run_time = cache.clock_cycles;
    //latency = (double)((cache.miss_penalty + 1) * cache.total_misses + cache.total_hits) / (cache.total_hits + cache.total_misses);
    latency = (double)total_run_time / (double)(cache.clock_cycles + cache.total_memory_ops);
    //total_hit_rate = (double)cache.total_hits / (double)(cache.total_hits + cache.total_misses);
    total_hit_rate = (double)cache.total_hits / (double)(cache.total_memory_ops) * 100;
    //load_hit_rate = (double)cache.load_hits / (double)(cache.load_hits + cache.load_misses);
    load_hit_rate = (double)cache.load_hits / (double)cache.load_memory_ops * 100;
    //store_hit_rate = (double)cache.store_hits / (double)(cache.store_hits + cache.store_misses);
    store_hit_rate = (double)cache.store_hits / (double)cache.store_memory_ops * 100;
    /* write to output file */
    output << "Total Hit Rate: " << total_hit_rate << "%" << endl;
    output << "Load Hit Rate: " << load_hit_rate << "%" << endl;
    output << "Store Hit Rate: " << store_hit_rate << "%" << endl;
    output << "Total Run Time: " << total_run_time << " cycles" << endl;
    output << "Average Memory Access Latency: " << latency << endl;
    /* close streams */
    output.close();
    config_stream.close();
    trace_stream.close();
	exit(EXIT_SUCCESS);
    return 0;
}
