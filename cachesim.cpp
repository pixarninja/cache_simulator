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
