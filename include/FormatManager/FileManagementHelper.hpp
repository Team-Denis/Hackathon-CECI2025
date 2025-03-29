
#ifndef FILE_MANAGEMENT_HELPER_HPP
#define FILE_MANAGEMENT_HELPER_HPP


#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <stdexcept>
#include <filesystem>
#include <fstream>

typedef unsigned char byte;


class FileManagementHelper {

public:

    // File Management
    [[nodiscard]] static int WriteBuffer(std::string &fp, std::vector<byte> &buffer);
    [[nodiscard]] static std::vector<byte> ReadBuffer(std::string &fp, int size);
    
    // Helper with conversiob
    [[nodiscard]] static std::vector<byte> StringToBytes(const std::string &str);
    [[nodiscard]] static std::string BytesToString(const std::vector<byte> &bytes);
    [[nodiscard]] static int BytesToInt(const std::vector<byte> &bytes, int size);
    [[nodiscard]] static std::vector<byte> IntToBytes(int value, int size);

};

#endif