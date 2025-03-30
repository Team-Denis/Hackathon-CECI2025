#ifndef MATRIX_CHUNKER_H
#define MATRIX_CHUNKER_H

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <array>
#include <iostream>
#include <fstream>
#include <filesystem>

#include "StorageCommon.hpp"

namespace PhysicalStorage {
    /**
     * Structure to hold basic information about the file being chunked.
     */
    struct FileInfo {
        std::string filename; // Original file path
        size_t fileSizeBytes; // File size in bytes
        size_t fileSizeBits; // File size in bits
        size_t totalChunks; // Total number of chunks needed
    };

    /**
     * Simplified MatrixChunker that focuses on reading chunks as arrays for CA processing
     */
    class MatrixChunker {
    public:
        /**
         * Analyzes a file and returns information about how it would be chunked
         *
         * @param filename Path to the file to analyze
         * @param widthBits Width of each matrix chunk in bits (default: 256)
         * @param heightBits Height of each matrix chunk in bits (default: 256)
         * @return Basic file information including number of chunks needed
         */
        static FileInfo analyzeFile(
            const std::string &filename,
            size_t widthBits = Constants::DEFAULT_MATRIX_WIDTH,
            size_t heightBits = Constants::DEFAULT_MATRIX_HEIGHT);

        /**
         * Reads a specific chunk from a file as a bit array
         *
         * @param filename Path to the file
         * @param chunkIndex Index of the chunk to read (0-based)
         * @param widthBits Width of each matrix chunk in bits
         * @param heightBits Height of each matrix chunk in bits
         * @return Array containing the chunk data (1D representation of 2D bit matrix)
         */
        template<size_t WIDTH, size_t HEIGHT>
        static std::array<int, WIDTH * HEIGHT> readChunk(
            const std::string &filename,
            size_t chunkIndex,
            size_t widthBits = WIDTH,
            size_t heightBits = HEIGHT) {
            std::array<int, WIDTH * HEIGHT> chunkArray = {}; // Initialize with zeros

            // Open the input file
            std::ifstream inFile(filename, std::ios::binary);
            if (!inFile) {
                std::cerr << "Cannot open input file: " << filename << std::endl;
                return chunkArray;
            }

            // Calculate chunk dimensions
            const size_t chunkSizeBits = widthBits * heightBits;

            // Calculate bit position in the file
            size_t startBitPos = chunkIndex * chunkSizeBits;

            // Calculate file size in bits
            inFile.seekg(0, std::ios::end);
            size_t fileSizeBytes = inFile.tellg();
            size_t fileSizeBits = fileSizeBytes * 8;
            inFile.seekg(0, std::ios::beg);

            // Check if chunk is beyond file size
            if (startBitPos >= fileSizeBits) {
                std::cerr << "Chunk index is beyond file size" << std::endl;
                return chunkArray;
            }

            // Calculate how many bits to process for this chunk
            size_t bitsToProcess = std::min(chunkSizeBits, fileSizeBits - startBitPos);

            // Read the entire file into memory (or just the part we need)
            size_t bytesToRead = (startBitPos + bitsToProcess + 7) / 8 - startBitPos / 8;
            size_t startBytePos = startBitPos / 8;

            inFile.seekg(startBytePos);
            std::vector<uint8_t> buffer(bytesToRead);
            inFile.read(reinterpret_cast<char *>(buffer.data()), bytesToRead);

            // Process bit by bit
            for (size_t j = 0; j < bitsToProcess; j++) {
                // Calculate file byte and bit position
                size_t fileBytePosSource = (startBitPos + j) / 8 - startBytePos;
                size_t fileBitPosInByte = (startBitPos + j) % 8;

                // Check if we've reached the end of the buffer
                if (fileBytePosSource >= buffer.size()) {
                    break;
                }

                // Get the bit from the file
                bool bitValue = (buffer[fileBytePosSource] & 1 << 7 - fileBitPosInByte) != 0;

                // Set the value in the chunk array
                if (j < chunkArray.size()) {
                    chunkArray[j] = bitValue ? 1 : 0;
                }
            }

            return chunkArray;
        }

        /**
         * Merges processed chunks back into a single data vector
         *
         * @param chunks Vector of processed chunk arrays
         * @param originalSizeBits Original file size in bits (to trim excess padding)
         * @return Vector of bytes representing the merged data
         */
        template<size_t WIDTH, size_t HEIGHT>
        static std::vector<uint8_t> mergeChunks(
            const std::vector<std::array<int, WIDTH * HEIGHT> > &chunks,
            size_t originalSizeBits) {
            // Calculate the size of the output buffer
            const size_t chunkSizeBits = WIDTH * HEIGHT;
            const size_t outputSizeBytes = (originalSizeBits + 7) / 8;

            // Initialize output buffer with zeros
            std::vector<uint8_t> outputBuffer(outputSizeBytes, 0);

            // Process each chunk
            for (size_t i = 0; i < chunks.size(); i++) {
                const auto &chunk = chunks[i];

                // Calculate starting bit position for this chunk
                size_t startBitPos = i * chunkSizeBits;

                // Process each bit in the chunk
                for (size_t j = 0; j < chunkSizeBits; j++) {
                    // Skip if beyond the original file size
                    if (startBitPos + j >= originalSizeBits) {
                        break;
                    }

                    // Calculate output byte and bit position
                    size_t globalBitPos = startBitPos + j;
                    size_t outputBytePos = globalBitPos / 8;
                    size_t outputBitPosInByte = globalBitPos % 8;

                    // Using the same bit ordering as in readChunk
                    if (chunk[j] == 1) {
                        outputBuffer[outputBytePos] |= 1 << 7 - outputBitPosInByte;
                    }
                }
            }

            return outputBuffer;
        }
    };
} // namespace PhysicalStorage

#endif // MATRIX_CHUNKER_H
