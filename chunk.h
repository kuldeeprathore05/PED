#pragma once
#include<vector>
using namespace std;
inline constexpr size_t CHUNK_SIZ = 4*1024*1024;
inline constexpr size_t QUEUE_CAP = 16;

struct Chunk{
    uint64_t chunk_id = 0;
    vector<uint8_t> data; 
    bool is_pp = false;  
};