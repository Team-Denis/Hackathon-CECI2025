#ifndef FILE_CHUNKER_UTILS_H
#define FILE_CHUNKER_UTILS_H

#include <cstddef>
#include <string>
#include <vector>

namespace utils {
    // Function to determine if a number is a power of 2
    bool isPowerOfTwo(size_t n);

    // Function to get the next power of 2 greater than or equal to n
    size_t nextPowerOfTwo(size_t n);

    // Function to calculate MD5 hash of a file
    std::string calculateMD5(const std::string &filename);

    // Function to calculate MD5 hash of data in memory
    std::string calculateMD5(const std::vector<unsigned char> &data);
}

#endif // FILE_CHUNKER_UTILS_H
