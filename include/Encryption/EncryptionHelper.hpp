#ifndef ENCRYPTION_HELPER_HPP
#define ENCRYPTION_HELPER_HPP


#include <iostream>
#include <string>
#include <vector>


typedef unsigned char byte;


class EncryptionHelper {
public:
    static void Encrypt(std::vector<byte> &data, const std::string &key);

    static void Decrypt(std::vector<byte> &data, const std::string &key);
};

#endif
