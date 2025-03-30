/**
 * @file hamming_code.cpp
 * @brief Implementation of Hamming code functions
 */

#include "PhysicalStorage/HammingCode.h"
#include <cmath>
#include <bitset>

// Private methods
int HammingCode::calculateParityBits(int dataLength) {
    // For Hamming code, we need to satisfy 2^r >= m + r + 1 where r is parity bits and m is data bits
    int r = 0;
    while ((1 << r) < (dataLength + r + 1)) {
        r++;
    }
    return r;
}

bool HammingCode::isParityBit(int position) {
    return (position & (position - 1)) == 0;
}

// Public methods
std::vector<bool> HammingCode::encodeByte(uint8_t byte) {
    // Convert byte to bits
    std::bitset<8> bits(byte);
    std::vector<bool> dataBits;
    for (int i = 0; i < 8; i++) {
        dataBits.push_back(bits[i]);
    }
    
    return encodeData(dataBits);
}

std::vector<bool> HammingCode::encodeData(const std::vector<bool>& dataBits) {
    int dataLength = dataBits.size();
    int parityBits = calculateParityBits(dataLength);
    int totalLength = dataLength + parityBits;
    
    // Create encoded data with space for parity bits
    std::vector<bool> encoded(totalLength, false);
    
    // Copy data bits to their positions
    int dataIndex = 0;
    for (int i = 1; i <= totalLength; i++) {
        if (!isParityBit(i)) {
            encoded[i-1] = dataBits[dataIndex++];
        }
    }
    
    // Calculate parity bits
    for (int i = 0; i < parityBits; i++) {
        int parityPos = (1 << i) - 1; // Position of parity bit (0-indexed)
        bool parity = false;
        
        // Check bits that include this parity bit in their coverage
        for (int j = parityPos; j < totalLength; j++) {
            if ((j + 1) & (1 << i)) { // If position j+1 has the i-th bit set
                parity ^= encoded[j]; // XOR with the bit at position j
            }
        }
        
        encoded[parityPos] = parity;
    }
    
    return encoded;
}

std::vector<bool> HammingCode::decodeData(const std::vector<bool>& encoded) {
    int totalLength = encoded.size();
    int parityBits = log2(totalLength + 1);
    int dataLength = totalLength - parityBits;
    
    // Check if there's an error
    int errorPos = detectError(encoded);
    
    // Create a copy of the encoded data for correction
    std::vector<bool> corrected = encoded;
    
    // If error detected, flip the bit
    if (errorPos > 0 && errorPos <= totalLength) {
        corrected[errorPos - 1] = !corrected[errorPos - 1];
        // Error was detected and corrected
    }
    
    // Extract data bits from the corrected code
    std::vector<bool> decoded;
    for (int i = 1; i <= totalLength; i++) {
        if (!isParityBit(i)) {
            decoded.push_back(corrected[i-1]);
        }
    }
    
    return decoded;
}

uint8_t HammingCode::decodeByte(const std::vector<bool>& encoded) {
    std::vector<bool> decoded = decodeData(encoded);
    
    // Convert back to byte
    uint8_t result = 0;
    for (int i = 0; i < 8 && i < decoded.size(); i++) {
        if (decoded[i]) {
            result |= (1 << i);
        }
    }
    
    return result;
}

int HammingCode::detectError(const std::vector<bool>& encoded) {
    int totalLength = encoded.size();
    int parityBits = log2(totalLength + 1);
    
    // Check if there's an error
    int errorPos = 0;
    for (int i = 0; i < parityBits; i++) {
        int parityPos = (1 << i) - 1;
        bool parity = false;
        
        for (int j = 0; j < totalLength; j++) {
            if ((j + 1) & (1 << i)) {
                parity ^= encoded[j];
            }
        }
        
        // If parity check fails, add to error position
        if (parity) {
            errorPos += (1 << i);
        }
    }
    
    return errorPos;
}

// Utility functions
std::vector<bool> bytesToBits(const std::vector<uint8_t>& bytes) {
    std::vector<bool> bits;
    for (uint8_t byte : bytes) {
        for (int i = 0; i < 8; i++) {
            bits.push_back((byte >> i) & 1);
        }
    }
    return bits;
}

std::vector<uint8_t> bitsToBytes(const std::vector<bool>& bits) {
    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < bits.size(); i += 8) {
        uint8_t byte = 0;
        for (int j = 0; j < 8 && (i + j) < bits.size(); j++) {
            if (bits[i + j]) {
                byte |= (1 << j);
            }
        }
        bytes.push_back(byte);
    }
    return bytes;
}