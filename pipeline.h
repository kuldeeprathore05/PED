#pragma once
#include "sharedqueue.h"
#include "chunk.h"
#include<string>
#include<vector>
using namespace std;

class Pipeline {
  public:
    Pipeline(bool encrypt, const string& input, const string& output, const string& password);
   
    void run();

  private:
    bool is_encrypt;
    string input_path;
    string output_path;
    string password;
    
    int num_workers;
 
    static constexpr int QUEUE_CAPACITY = 16;
    
    SharedQueue<Chunk> in_queue{QUEUE_CAPACITY};
    SharedQueue<Chunk> out_queue{QUEUE_CAPACITY};
 
    void workerTask(vector<uint8_t> key, vector<uint8_t> base_iv);
};