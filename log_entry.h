// 01BD41C3BF016AD7E8B6F837DF18926EC3E83350
//  log_entry.hpp
//  p3-logman
//
//  Created by Trevor Lee on 10/28/21.
//  Copyright © 2021 Trevor Lee. All rights reserved.
//


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
int64_t time_to_int(string input);

struct log_entry{
     int32_t entryID;
    string timestamp_output;
    long long int timestamp;
    string category;
    string message;
    log_entry(int32_t ide, string time, string cat, string mes):
    entryID(ide),timestamp_output(time),category(cat),message(mes){
        timestamp = (long long int)time_to_int(timestamp_output);
    }
    log_entry():
    entryID(0),timestamp_output(""),  timestamp (0),category(""), message(""){}
    
    
};

int64_t time_to_int(string input){
    int64_t output=0;
    output += static_cast<unsigned long long>(input[0] - '0') * 1000000000ull;
    output += static_cast<unsigned long long>(input[1] - '0') * 100000000ull;
    output += static_cast<unsigned long long>(input[3] - '0') * 10000000ull;
    output += static_cast<unsigned long long>(input[4] - '0') * 1000000ull;
    output += static_cast<unsigned long long>(input[6] - '0') * 100000ull;
    output += static_cast<unsigned long long>(input[7] - '0') * 10000ull;
    output += static_cast<unsigned long long>(input[9] - '0') * 1000ull;
    output += static_cast<unsigned long long>(input[10] - '0') * 100ull;
    output += static_cast<unsigned long long>(input[12] - '0') * 10ull;
    output += static_cast<unsigned long long>(input[13] - '0');
    return output;
}
