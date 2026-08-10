#pragma once
#include "chunk.h"
#include "sharedqueue.h"
#include <cstdint>
#include <fstream>
#include <queue>
#include <string>
#include <vector>
using namespace std; 
class Filereader{
  public:
    explicit Filereader(const string &path);

    pair<vector<uint8_t>,vector<uint8_t>> readHeader();

    void run(SharedQueue<Chunk>&out_queue,int num_workers);
  private:
    ifstream file;  
};


class FileWriter {
  public:
    explicit FileWriter(const string& path); 
 
    void writeHeader(const std::vector<uint8_t>& salt, const std::vector<uint8_t>& base_iv); 
    void run(SharedQueue<Chunk>& in_queue, int num_workers);

private:
    ofstream file;
    uint64_t expected_id = 0; 

    struct ChunkCompare {
        bool operator()(const Chunk& a, const Chunk& b) const {
            return a.chunk_id > b.chunk_id;
        }
    }; 
    priority_queue<Chunk, vector<Chunk>, ChunkCompare> pq;
 
    void flushReady();
};