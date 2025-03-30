#include "Encryption/EncryptionHelper.hpp"


void EncryptionHelper::Encrypt(std::vector<byte>& data, const std::string& key) {
    
    if (key.empty()) {
        throw std::invalid_argument("[e] Key cannot be empty.");
    }

    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = data[i] ^ key[i % key.size()];
    }
}

void EncryptionHelper::Decrypt(std::vector<byte>& data, const std::string& key) {
    EncryptionHelper::Encrypt(data, key); // xor encrpytion is reversible lol
}