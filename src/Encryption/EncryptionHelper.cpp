
#include "EncryptionHelper.hpp"


std::vector<byte> EncryptionHelper::Encrypt(const std::vector<byte>& data, const std::string& key) {
    
    if (key.empty()) {
        throw std::invalid_argument("[e] Key cannot be empty.");
    }

    std::vector<byte> encryptedData(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        encryptedData[i] = data[i] ^ key[i % key.size()];
    }

    return encryptedData;

}

std::vector<byte> EncryptionHelper::Decrypt(const std::vector<byte>& data, const std::string& key) {
    
    return EncryptionHelper::Encrypt(data, key); // xor encrpytion is reversible lol

}