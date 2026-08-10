#include "FileIO.h"
#include "Crypto.h"
#include<stdexcept>
using namespace std;
Filereader::Filereader(const string &path){
    file.open(path,ios::binary| ios::in);
    if(!file.is_open()){
        throw runtime_error("Failed  to oopen input file "+path);
    }
}
pair<vector<uint8_t>,vector<uint8_t>> Filereader::readHeader(){
    vector<uint8_t> salt(Crypto::SALT_SIZ);
    vector<uint8_t> base_iv(Crypto::IV_SIZ);
    file.read(reinterpret_cast<char*>(salt.data()), salt.size());
    file.read(reinterpret_cast<char*>(base_iv.data()), base_iv.size());

    if(file.gcount()!=Crypto::IV_SIZ){
        throw runtime_error("Input file too short for header");
    }
    return {move(salt),move(base_iv)};

}
void Filereader::run(SharedQueue<Chunk>&out_queue,int num_workers){
    uint8_t chunk_id = 0;
    while(file){
        Chunk chunk;
        chunk.chunk_id = chunk_id++;
        chunk.data.resize(CHUNK_SIZ);
        file.read(reinterpret_cast<char*>(chunk.data.data()), CHUNK_SIZ);
        size_t bytes_read = file.gcount();
    
        if (bytes_read == 0) break;  
    
        chunk.data.resize(bytes_read);
        out_queue.push(move(chunk));
    }
    for(int i=0;i<num_workers;i++){
        Chunk p;
        p.is_pp = true;
        out_queue.push(move(p));
    }
}

FileWriter::FileWriter(const string& path) {
    file.open(path, ios::binary | ios::out | ios::trunc);
    if (!file.is_open()) {
        throw runtime_error("Failed to open output file: " + path);
    }
}

void FileWriter::writeHeader(const vector<uint8_t>& salt, const vector<uint8_t>& base_iv){
    file.write(reinterpret_cast<const char*>(salt.data()), salt.size());
    file.write(reinterpret_cast<const char*>(base_iv.data()), base_iv.size());
    if (!file) {
        throw runtime_error("Failed to write file header");
    }
}

void FileWriter::flushReady() { 
    while (!pq.empty() && pq.top().chunk_id == expected_id) {   
        Chunk chunk = move(const_cast<Chunk&>(pq.top()));
        pq.pop();

        file.write(reinterpret_cast<const char*>(chunk.data.data()), chunk.data.size());
        if (!file) {
            throw runtime_error("I/O error while writing chunk " + std::to_string(chunk.chunk_id));
        }

        expected_id++;
    }
}

void FileWriter::run(SharedQueue<Chunk>& in_queue, int num_workers) {
    int pp = 0; 
    while (pp < num_workers) {
        Chunk chunk = in_queue.pop();

        if (chunk.is_pp) {
            pp++;
            continue;
        } 
        pq.push(std::move(chunk)); 
        flushReady();
    }

    flushReady();   
    if (!pq.empty()) {
        throw std::runtime_error("Pipeline ended with missing chunks! Data corruption risk.");
    }
}