/**
 * @file hamming_code.h
 * @brief Hamming code implementation for error detection and correction
 * 
 * This header provides a C++ implementation of Hamming code for encoding and
 * decoding data with single-bit error correction capabilities.
 */

#ifndef HAMMING_CODE_H
#define HAMMING_CODE_H

#include "StorageCommon.hpp"
#include <bitset>

/**
 * @class HammingCode
 * @brief Class providing static methods for Hamming code encoding and decoding
 */
class HammingCode {
private:
    /**
     * @brief Calculate the number of parity bits needed for a given data length
     * @param dataLength The number of data bits to encode
     * @return The number of parity bits required
     */
    static int calculateParityBits(int dataLength);

    /**
     * @brief Check if the given position is a power of 2 (parity bit position)
     * @param position The position to check
     * @return True if position is a power of 2, false otherwise
     */
    static bool isParityBit(int position);

public:
    /**
     * @brief Encode a byte using Hamming code
     * @param byte The byte to encode
     * @return Vector of booleans representing the encoded data with parity bits
     */
    static std::vector<bool> encodeByte(uint8_t byte);
    
    /**
     * @brief Encode a vector of bits using Hamming code
     * @param dataBits Vector of booleans representing the data bits to encode
     * @return Vector of booleans representing the encoded data with parity bits
     */
    static std::vector<bool> encodeData(const std::vector<bool>& dataBits);
    
    /**
     * @brief Decode Hamming encoded data and correct single-bit errors
     * @param encoded Vector of booleans representing the encoded data
     * @return Vector of booleans representing the decoded (and corrected if needed) data
     */
    static std::vector<bool> decodeData(const std::vector<bool>& encoded);
    
    /**
     * @brief Decode a Hamming encoded byte
     * @param encoded Vector of booleans representing the encoded byte
     * @return The decoded byte
     */
    static uint8_t decodeByte(const std::vector<bool>& encoded);
    
    /**
     * @brief Check if the encoded data contains an error
     * @param encoded Vector of booleans representing the encoded data
     * @return The error position (0 if no error detected)
     */
    static int detectError(const std::vector<bool>& encoded);
};

/**
 * @brief Convert a byte vector to a bit vector
 * @param bytes Vector of bytes to convert
 * @return Vector of booleans representing the bits
 */
std::vector<bool> bytesToBits(const std::vector<uint8_t>& bytes);

/**
 * @brief Convert a bit vector back to bytes
 * @param bits Vector of booleans representing the bits
 * @return Vector of bytes
 */
std::vector<uint8_t> bitsToBytes(const std::vector<bool>& bits);

#endif // HAMMING_CODE_H