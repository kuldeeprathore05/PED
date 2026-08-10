#include "Pipeline.h"
#include "FileIO.h"
#include "Crypto.h"
#include<iostream>
#include<thread>
#include<vector>
using namespace std;
Pipeline::Pipeline(bool encrypt, const string& input, const string& output, const string& password)
    : is_encrypt(encrypt), input_path(input), output_path(output), password(password) {
     
    num_workers = thread::hardware_concurrency();
    if (num_workers == 0) num_workers = 4; 
}

void Pipeline::workerTask(vector<uint8_t> key, vector<uint8_t> base_iv) { 
    Crypto crypto(move(key), move(base_iv)); 
    while (true) {
        Chunk chunk = in_queue.pop();

        if (chunk.is_pp) { 
            out_queue.push(std::move(chunk));
            break;
        } 
        crypto.processChunk(chunk);

        out_queue.push(move(chunk));
    }
}

void Pipeline::run() {
    cout<<"Pipeline starting "<<(is_encrypt?"Encryption":"Decryption")<<" with "<<num_workers<<" workers"<<endl;;
 
    Filereader reader(input_path);
    FileWriter writer(output_path);

    vector<uint8_t> salt, base_iv, key; 
    if (is_encrypt) {
        salt = Crypto::genRandB(Crypto::SALT_SIZ);
        base_iv = Crypto::genRandB(Crypto::IV_SIZ);
        key = Crypto::deriveKey(password, salt);
        
        writer.writeHeader(salt, base_iv);
    } else {
        auto header = reader.readHeader();
        salt = move(header.first);
        base_iv = move(header.second);
        key = Crypto::deriveKey(password, salt);
    }
 
    thread reader_thread([&]() {
        reader.run(in_queue, num_workers);
    });
    
    vector<thread> workers;
    for (int i = 0; i < num_workers;i++) { 
        workers.emplace_back(&Pipeline::workerTask, this, key, base_iv);
    } 
    std::thread writer_thread([&]() {
        writer.run(out_queue, num_workers);
    }); 
    reader_thread.join();
    for (auto& w : workers) {
        w.join();
    }
    writer_thread.join();

    cout<<"Pipeline sucessfully."<<endl;
}