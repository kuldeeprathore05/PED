#pragma once
#include "chunk.h"
#include<cstdint>
#include<string>
#include<vector>
#include<memory>
#include<openssl/evp.h>
using namespace std; 
class Crypto{
  public:
    static constexpr size_t KEY_SIZ = 32;
    static constexpr size_t IV_SIZ = 16;
    static constexpr size_t SALT_SIZ = 16;
    static constexpr size_t PBKDF2_ITER = 7;
    static constexpr size_t BPC = CHUNK_SIZ/IV_SIZ; // blocks per chunks   

    Crypto(vector<uint8_t> key, vector<uint8_t> base_iv);
    static vector<uint8_t> deriveKey(const string& password, const vector<uint8_t>&salt); 
    static vector<uint8_t> genRandB(size_t len);

    void processChunk(Chunk& chunk);
    vector<uint8_t> computeChunkIV(uint64_t chunk_id) const;

  private:
    vector<uint8_t> key;
    vector<uint8_t> base_iv;
    EVP_CIPHER_CTX * ctx;

};