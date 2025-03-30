#ifndef ENCRYPTION_HELPER_HPP
#define ENCRYPTION_HELPER_HPP


#include <iostream>
#include <string>
#include <vector>


typedef unsigned char byte;


class EncryptionHelper {

public:

    [[nodiscard]] static std::vector<byte> Encrypt(const std::vector<byte>& data, const std::string& key);
    [[nodiscard]] static std::vector<byte> Decrypt(const std::vector<byte>& data, const std::string& key); 


};

#endif