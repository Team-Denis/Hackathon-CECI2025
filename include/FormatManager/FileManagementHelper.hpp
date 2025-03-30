#ifndef FILE_MANAGEMENT_HELPER_HPP
#define FILE_MANAGEMENT_HELPER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <filesystem>
#include <fstream>

// Use standard uint8_t instead of custom byte type
using byte = uint8_t;

class FileManagementHelper {
public:
    /**
     * Writes a buffer to a file
     * 
     * @param fp Path to the output file
     * @param buffer Data buffer to write
     * @return Number of bytes written
     */
    static int WriteBuffer(const std::string &fp, const std::vector<uint8_t> &buffer) {
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

    /**
     * Reads a buffer from a file
     * 
     * @param fp Path to the input file
     * @param size Number of bytes to read (0 for entire file)
     * @return Data buffer read from file
     */
    static std::vector<uint8_t> ReadBuffer(const std::string &fp, size_t size = 0) {
        // Open file in binary mode for reading
        std::ifstream file(fp, std::ios::binary);
        if (!file) {
            throw std::runtime_error("[e] Error opening file for reading: " + fp);
        }

        // If size is 0, read the entire file
        if (size == 0) {
            file.seekg(0, std::ios::end);
            size = file.tellg();
            file.seekg(0, std::ios::beg);
        }

        // Create buffer of specified size
        std::vector<uint8_t> buffer(size);

        // Read from file into buffer
        file.read(reinterpret_cast<char *>(buffer.data()), size);

        // Check how many bytes were actually read
        size_t bytesRead = file.gcount();
        if (bytesRead < size) {
            buffer.resize(bytesRead);
        }

        file.close();
        return buffer;
    }

    /**
     * Converts a string to a byte vector
     * 
     * @param str Input string
     * @return Byte vector
     */
    static std::vector<uint8_t> StringToBytes(const std::string &str) {
        return std::vector<uint8_t>(str.begin(), str.end());
    }

    /**
     * Converts a byte vector to a string
     * 
     * @param bytes Input byte vector
     * @return String
     */
    static std::string BytesToString(const std::vector<uint8_t> &bytes) {
        return std::string(reinterpret_cast<const char *>(bytes.data()), bytes.size());
    }

    /**
     * Converts bytes to an integer
     * 
     * @param bytes Input byte vector
     * @param size Number of bytes to convert
     * @return Integer value
     */
    static int64_t BytesToInt(const std::vector<uint8_t> &bytes, int size) {
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
        return value;
    }

    /**
     * Converts an integer to bytes
     * 
     * @param value Integer value
     * @param size Number of bytes to represent the integer
     * @return Byte vector
     */
    static std::vector<uint8_t> IntToBytes(int64_t value, int size) {
        if (size < 1 || size > static_cast<int>(sizeof(int64_t))) {
            throw std::invalid_argument("[e] Size must be between 1 and " + std::to_string(sizeof(int64_t)));
        }

        std::vector<uint8_t> bytes(size);
        for (int i = 0; i < size; ++i) {
            bytes[i] = static_cast<uint8_t>((value >> (i * 8)) & 0xFF);
        }
        return bytes;
    }
};

#endif // FILE_MANAGEMENT_HELPER_HPP
