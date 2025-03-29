#ifndef CRYPTO_UTILS_H
#define CRYPTO_UTILS_H

#include <cstddef>
#include <string>
#include <vector>

namespace PhysicalStorage {
    /**
     * Provides cryptographic and mathematical utility functions
     */
    class CryptoUtils {
    public:
        /**
         * Determines if a number is a power of 2.
         *
         * @param n The number to check
         * @return True if n is a power of 2, false otherwise
         */
        static bool isPowerOfTwo(size_t n);

        /**
         * Gets the next power of 2 greater than or equal to n.
         *
         * @param n The number to find the next power of 2 for
         * @return The next power of 2 >= n
         */
        static size_t nextPowerOfTwo(size_t n);

        /**
         * Calculates the MD5 hash of a file.
         *
         * @param filename Path to the file
         * @return MD5 hash as a hex string, or empty string on error
         */
        static std::string calculateMD5(const std::string &filename);

        /**
         * Calculates the MD5 hash of data in memory.
         *
         * @param data Data to hash
         * @return MD5 hash as a hex string, or empty string on error
         */
        static std::string calculateMD5(const std::vector<unsigned char> &data);

        /**
         * Verifies data integrity using MD5 hash.
         *
         * @param data Data to verify
         * @param expectedHash Expected MD5 hash
         * @return True if hash matches, false otherwise
         */
        static bool verifyIntegrity(const std::vector<unsigned char> &data, const std::string &expectedHash);

        /**
         * Verifies file integrity using MD5 hash.
         *
         * @param filename Path to the file
         * @param expectedHash Expected MD5 hash
         * @return True if hash matches, false otherwise
         */
        static bool verifyIntegrity(const std::string &filename, const std::string &expectedHash);
    };
} // namespace PhysicalStorage

#endif // CRYPTO_UTILS_H
