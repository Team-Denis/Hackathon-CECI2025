#include "FormatManager/FileManagementHelper.hpp"

int FileManagementHelper::WriteBuffer(std::string &fp, std::vector<byte> &buffer) {
    // Open file in binary mode for writing (not appending)
    std::ofstream file(fp, std::ios::binary);
    if (!file) {
        throw std::runtime_error("[e] Error opening file for writing: " + fp);
    }

    // Write buffer to file
    file.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());

    // Check if write was successful
    if (!file) {
        file.close();
        throw std::runtime_error("[e] Error writing to file: " + fp);
    }

    file.close();
    return buffer.size();
}

std::vector<byte> FileManagementHelper::ReadBuffer(std::string &fp, int size) {
    // Open file in binary mode for reading
    std::ifstream file(fp, std::ios::binary);
    if (!file) {
        throw std::runtime_error("[e] Error opening file for reading: " + fp);
    }

    // Create buffer of specified size
    std::vector<byte> buffer(size);

    // Read from file into buffer
    file.read(reinterpret_cast<char *>(buffer.data()), size);

    // Check if read was successful and get number of bytes actually read
    int bytesRead = file.gcount();
    file.close();

    // Resize buffer to actual number of bytes read
    buffer.resize(bytesRead);
    return buffer;
}

std::vector<byte> FileManagementHelper::StringToBytes(const std::string &str) {
    return std::vector<byte>(str.begin(), str.end());
}

std::string FileManagementHelper::BytesToString(const std::vector<byte> &bytes) {
    return std::string(reinterpret_cast<const char *>(bytes.data()), bytes.size());
}

int FileManagementHelper::BytesToInt(const std::vector<byte> &bytes, int size) {
    if (size < 1 || size > static_cast<int>(sizeof(int64_t))) {
        throw std::invalid_argument("[e] Size must be between 1 and " + std::to_string(sizeof(int64_t)));
    }

    if (bytes.size() < static_cast<size_t>(size)) {
        throw std::invalid_argument("[e] Bytes vector size is smaller than the requested size");
    }

    int64_t value = 0;
    for (int i = 0; i < size; ++i) {
        value |= (static_cast<int64_t>(bytes[i]) << (i * 8));
    }
    return static_cast<int>(value);
}

std::vector<byte> FileManagementHelper::IntToBytes(int value, int size) {
    if (size < 1 || size > static_cast<int>(sizeof(int64_t))) {
        throw std::invalid_argument("[e] Size must be between 1 and " + std::to_string(sizeof(int64_t)));
    }

    std::vector<byte> bytes(size);
    for (int i = 0; i < size; ++i) {
        bytes[i] = static_cast<byte>((value >> (i * 8)) & 0xFF);
    }
    return bytes;
}
