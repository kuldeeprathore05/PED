#include "crypto.h"
// #include <openssl/rand.h>
#include <stdexcept>
#include <string>
using namespace std;
Crypto::Crypto(vector<uint8_t> key,vector<uint8_t> base_iv){

}
vector<uint8_t> Crypto::genRandB(size_t len){
    vector<uint8_t> buf(len);
    
    return buf;
}
vector<uint8_t> Crypto::deriveKey(const string &password,const vector<uint8_t> &salt){
    vector<uint8_t> key(KEY_SIZ);
    
    return key;
}