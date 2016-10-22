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

int main(void)
{
    Cache cache;
    cache.cache();
    vector<char> type;
    vector<unsigned int> address;
    vector<int> access;
    vector<int> config;
    string line;
    string sub_address;
    char sub_type;
    int sub_access;
    int i = 0;
    int config_size = 0;
    int trace_size = 0;
    int address_size = 32;
    
    ifstream config_stream;
    config_stream.open("sample_config");

    ifstream trace_stream;
    trace_stream.open("sample_trace");

    while(config_stream >> line) {
        config.push_back(stoi(line));
        config_size++;
    }

    while(trace_stream >> sub_type >> sub_address >> sub_access) {
        type.push_back(sub_type);
        address.push_back(stoul(sub_address, nullptr, 16));
        access.push_back(sub_access);
        trace_size++;
    }
    
    cache.initialize(config);
    cache.calculate_bits(address_size);

    /* printing debug 
    cout << "offset = " << cache.offset << endl;
    cout << "index = " << cache.index << endl;
    cout << "tag = " << cache.tag << endl;
    cout << endl;
    for(i = 0; i < trace_size; i++) {
        cout << type[i] << " " << address[i] << " " << access[i] << endl;
    }*/

    ofstream output;
    output.open("cache.out");
    
    for(i = 0; i < config_size; i++) {
        output << config[i] << endl;
    }

    output.close();
    config_stream.close();
    trace_stream.close();
	return 0;
}
