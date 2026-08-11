#include "crypto.h"
#include <openssl/rand.h>
#include <stdexcept>
#include <string>
using namespace std;
Crypto::Crypto(vector<uint8_t> key,vector<uint8_t> base_iv)
    :key(move(key)),base_iv(move(base_iv)),ctx(EVP_CIPHER_CTX_new()){
    if(this->key.size()!=KEY_SIZ) throw invalid_argument("Key must be 32 bytes");
    if (this->base_iv.size() != IV_SIZ) throw invalid_argument("Base IV must be 16 bytes");
    if (!ctx) throw runtime_error("Failed to create OpenSSL context");
}
vector<uint8_t> Crypto::genRandB(size_t len){
    vector<uint8_t> buf(len);
    if(RAND_bytes(buf.data(),len)!=1){
        throw runtime_error("FAiled to gen rannd  bytes");
    }
    return buf;
}
vector<uint8_t> Crypto::deriveKey(const string &password,const vector<uint8_t> &salt){
    vector<uint8_t> key(KEY_SIZ);
    if(PKCS5_PBKDF2_HMAC(password.c_str(),password.size(),
                          salt.data(), salt.size(), 600000,
                          EVP_sha256(), KEY_SIZ, key.data()) != 1) {
        throw runtime_error("PBKDF2 key derivation failed");
    }
    return key;
}
vector<uint8_t> Crypto::computeChunkIV(uint64_t chunk_id) const {
    vector<uint8_t> iv = base_iv;  
    uint64_t offset = chunk_id*(CHUNK_SIZ/16); 
    uint64_t carry = offset; 
    for (int i = IV_SIZ - 1; i >= 0 && carry > 0; --i) {
        uint64_t sum = iv[i] + (carry & 0xFF);
        iv[i] = sum & 0xFF;
        carry = (carry >> 8) + (sum >> 8);
    }

    return iv;
}

void Crypto::processChunk(Chunk& chunk) { 
    if (chunk.is_pp || chunk.data.empty()) return;

    vector<uint8_t> iv = computeChunkIV(chunk.chunk_id); 
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), nullptr, key.data(), iv.data()) != 1) {
        throw runtime_error("Cipher init failed");
    }

    int out_len = 0; 
    if (EVP_EncryptUpdate(ctx, chunk.data.data(), &out_len, 
                          chunk.data.data(), chunk.data.size()) != 1) {
        throw runtime_error("Cipher update failed");
    }
}