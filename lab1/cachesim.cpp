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
    vector<char> type;
    vector<int> address;
    vector<int> access;
    vector<int> config;
    string line;
    string sub_address;
    string binary;
    char sub_type;
    int **data;
    int offset;
    int index;
    int tag;
    int sub_access;
    int i = 0;
    int j = 0;
    int config_size = 0;
    int trace_size = 0;
    int address_size = 32;
    int found = 0;
 
    ifstream config_stream;
    config_stream.open(argv[1]);

    ifstream trace_stream;
    trace_stream.open(argv[2]);

    /* parse config file */
    while(config_stream >> line) {
        config.push_back(stoi(line));
        config_size++;
    }

    /* parse trace file */
    while(trace_stream >> sub_type >> sub_address >> sub_access) {
        type.push_back(sub_type);
        address.push_back(stoul(sub_address, nullptr, 16));
        access.push_back(sub_access);
        trace_size++;
    }
    
    cache.initialize(config);
    cache.calculate_bits(address_size);
    /* printing debug */
    cout << "offset = " << cache.offset << endl;
    cout << "index = " << cache.index << endl;
    cout << "tag = " << cache.tag << endl;
    cout << endl;

    int index_max = pow(2, cache.index);
    int tag_max = pow(2, cache.tag);
    data = new int * [index_max];
    for(i = 0; i < index_max; i++) {
        if(cache.ways == 0) {
            cache.ways = 1;
        }
        data[i] = new int [cache.ways];
        for(j = 0; j < cache.ways; j++) {
            data[i][j] = tag_max;
        }
    }

    /* find the offset, index, and tag for each trace entry */
    for(i = 0; i < trace_size; i++) {
        binary = bitset<32>(address[i]).to_string();
        tag = stoi(binary.substr(0, cache.tag), nullptr, 2);
        binary.erase(0, cache.tag);
        index = stoi(binary.substr(0, cache.index), nullptr, 2);
        binary.erase(0, cache.index);
        offset = stoi(binary, nullptr, 2);

        cout << "index: " << index << ", ";
        cout << "tag: " << tag << ", ";
        cout << "offset: " << offset << ", ";
        /* simulate cache */
        switch(type[i]) {
        case 'l':
            cout << "WRITE:";
            break;
        case 's':
            cout << "READ:";
            break;
        default:
            cout << "ERROR:";
            exit(EXIT_FAILURE);
        }
        cout << " ";
        /* loop through all the tags in the set */
        for(j = 0; j < cache.ways; j++) {
            if(data[index][j] == tag) { //tag matches
                found = j;
                break;
            }
            else { //tag doesn't match
                found = tag_max;
            }
        }
        if(found != tag_max) { //tag matched
            cout << "HIT" << endl;
        }
        else { //tag didn't match
            cout << "MISS" << endl;
            data[index][found] = tag;
            /* TODO: check other sets and do replacement */
        }
    }

    /* printing debug */
    for(i = 0; i < trace_size; i++) {
        cout << type[i] << " " << address[i] << " " << access[i] << endl;
    }

    /* setup output file */
    ofstream output;
    string output_filename = argv[2];
    string tmp = argv[2];
    string delim = "/";
    size_t pos;
    while((pos = tmp.find(delim)) != string::npos) {
        output_filename = tmp.substr(pos + 1, tmp.length());
        tmp.erase(0, pos + delim.length());
    }
    output_filename.append(".out");
    output.open(output_filename);
    
    /* write to output file */
    for(i = 0; i < config_size; i++) {
        output << config[i] << endl;
    }

    output.close();
    config_stream.close();
    trace_stream.close();
	return 0;
}
