#include "utils.h"
#include <openssl/md5.h>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace utils {

    // Function to determine if a number is a power of 2
    bool isPowerOfTwo(size_t n) {
        return (n != 0) && ((n & (n - 1)) == 0);
    }

    // Function to get the next power of 2 greater than or equal to n
    size_t nextPowerOfTwo(size_t n) {
        if (n == 0) return 1;
        if (isPowerOfTwo(n)) return n;

        size_t count = 0;
        while (n != 0) {
            n >>= 1;
            count++;
        }

        return 1 << count;
    }

    // Function to calculate MD5 hash of a file
    std::string calculateMD5(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            return "";
        }

        MD5_CTX md5Context;
        MD5_Init(&md5Context);

        char buffer[1024];
        while (file.good()) {
            file.read(buffer, sizeof(buffer));
            MD5_Update(&md5Context, buffer, file.gcount());
        }

        unsigned char result[MD5_DIGEST_LENGTH];
        MD5_Final(result, &md5Context);

        std::stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(result[i]);
        }

        return ss.str();
    }

    // Function to calculate MD5 hash of data in memory
    std::string calculateMD5(const std::vector<unsigned char>& data) {
        if (data.empty()) {
            return "";
        }

        MD5_CTX md5Context;
        MD5_Init(&md5Context);
        MD5_Update(&md5Context, data.data(), data.size());

        unsigned char result[MD5_DIGEST_LENGTH];
        MD5_Final(result, &md5Context);

        std::stringstream ss;
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(result[i]);
        }

        return ss.str();
    }
}