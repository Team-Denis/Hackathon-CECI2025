
#include "FormatManager/FileManagementHelper.hpp"


int FileManagementHelper::WriteBuffer(std::string &fp, std::vector<byte> &buffer) {
    // This function writes a buffer to a file. It opens the file in binary mode and appends the buffer to it.
    // This should return the number of bytes written to the file.
    // If the file cannot be opened, it throws an exception with an error message.

    std::ofstream file(fp, std::ios::binary | std::ios::app);
    if (!file) { throw std::runtime_error("Error opening file for writing: " + fp); }
    file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
    file.close();

    return buffer.size();
}

std::vector<byte> FileManagementHelper::ReadBuffer(std::string &fp, int size) {
    // This function reads a buffer from a file. It opens the file in binary mode and reads the specified number of bytes into a buffer.
    // If the file cannot be opened, it throws an exception with an error message.

    std::ifstream file(fp, std::ios::binary);
    if (!file) { throw std::runtime_error("Error opening file for reading: " + fp); }
    std::vector<byte> buffer(size);
    file.read(reinterpret_cast<char *>(buffer.data()), size);
    file.close();

    return buffer;
}

std::vector<byte> FileManagementHelper::StringToBytes(const std::string &str) {
    // This function converts a string to a vector of bytes. It uses the std::vector constructor that takes two iterators to create the byte vector.
    // The first iterator is the beginning of the string and the second iterator is the end of the string.

    return std::vector<byte>(str.begin(), str.end());
}

std::string FileManagementHelper::BytesToString(const std::vector<byte> &bytes) {
    // This function converts a vector of bytes to a string. It uses the std::string constructor that takes two iterators to create the string.
    // The first iterator is the beginning of the byte vector and the second iterator is the end of the byte vector.

    return std::string(bytes.begin(), bytes.end());
}

int FileManagementHelper::BytesToInt(const std::vector<byte> &bytes, int size) {
    // This function converts a vector of bytes to an integer using little-endian format.
    // The number of bytes this int is on is specified by the size parameter.

    if (size < 1 || size > static_cast<int>(sizeof(int64_t))) {
        throw std::invalid_argument("Size must be between 1 and " + std::to_string(sizeof(int64_t)));
    }

    int value = 0;
    for (int i = 0; i < size; ++i) {
        value |= (static_cast<int>(bytes[i]) << (i * 8));
    }
    return value;
}

std::vector<byte> FileManagementHelper::IntToBytes(int value, int size) {
    // This function converts an integer to a vector of bytes using little-endian format.
    // The number of bytes this int is on is specified by the size parameter.

    if (size < 1 || size > static_cast<int>(sizeof(int64_t))) {
        throw std::invalid_argument("Size must be between 1 and " + std::to_string(sizeof(int64_t)));
    }

    std::vector<byte> bytes(size);
    for (int i = 0; i < size; ++i) {
        bytes[i] = static_cast<byte>((value >> (i * 8)) & 0xFF);
    }
    return bytes;
}

