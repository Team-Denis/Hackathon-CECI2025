#include "../include/chunker.h"
#include "../include/utils.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <regex>
#include <algorithm>

namespace fs = std::filesystem;

namespace chunker {
    // Function to split file into matrix chunks of 256x256 bits
    ChunkingInfo splitFileIntoBitMatrixChunks(const std::string &filename) {
        // Fixed dimensions 256x256 bits
        size_t widthBits = 256;
        size_t heightBits = 256;

        ChunkingInfo info;
        info.originFilename = filename;
        info.widthBits = widthBits;
        info.heightBits = heightBits;

        // Open the input file
        std::ifstream inFile(filename, std::ios::binary);
        if (!inFile) {
            std::cerr << "Cannot open input file: " << filename << std::endl;
            return info;
        }

        // Get file size in bytes
        info.fileSizeBytes = fs::file_size(filename);
        info.fileSizeBits = info.fileSizeBytes * 8;

        // Calculate chunk size in bits and bytes
        info.chunkSizeBits = widthBits * heightBits;
        info.chunkSizeBytes = (info.chunkSizeBits + 7) / 8; // Ceiling division to get bytes

        // If file is smaller than chunkSize, no need to split
        if (info.fileSizeBits <= info.chunkSizeBits) {
            std::cout << "File is smaller than chunk size. No splitting required." << std::endl;
            return info;
        }

        // Calculate MD5 hash of original file
        info.md5Original = utils::calculateMD5(filename);

        std::cout << "Using matrix chunks with dimensions: " << widthBits << "x" << heightBits << " bits" << std::endl;
        std::cout << "Chunk size: " << info.chunkSizeBits << " bits (" << info.chunkSizeBytes << " bytes)" << std::endl;
        std::cout << "Original file MD5: " << info.md5Original << std::endl;

        // Create output directory based on input filename (without extension)
        std::string baseName = fs::path(filename).stem().string();
        std::string outDirName = baseName + "_chunks";
        info.outputDir = outDirName;

        if (!fs::exists(outDirName)) {
            fs::create_directory(outDirName);
        }

        // Calculate total chunks needed
        info.totalChunks = (info.fileSizeBits + info.chunkSizeBits - 1) / info.chunkSizeBits; // Ceiling division
        info.totalRows = (info.totalChunks + 999) / 1000; // Limit to 1000 chunks per row (arbitrary)
        info.chunksPerRow = std::min(info.totalChunks, static_cast<size_t>(1000));

        // Read the entire file into memory
        std::vector<unsigned char> fileBuffer(info.fileSizeBytes);
        inFile.read(reinterpret_cast<char *>(fileBuffer.data()), info.fileSizeBytes);
        inFile.close();

        // Process chunks
        std::vector<unsigned char> chunkBuffer(info.chunkSizeBytes);

        for (size_t i = 0; i < info.totalChunks; i++) {
            // Calculate row and column indices for the chunk
            size_t row = i / info.chunksPerRow;
            size_t col = i % info.chunksPerRow;

            // Calculate bit position in the file
            size_t startBitPos = i * info.chunkSizeBits;

            // Clear the chunk buffer
            std::fill(chunkBuffer.begin(), chunkBuffer.end(), 0);

            // Calculate how many bits to process for this chunk
            size_t bitsToProcess = std::min(info.chunkSizeBits, info.fileSizeBits - startBitPos);

            // Process bit by bit
            for (size_t j = 0; j < bitsToProcess; j++) {
                // Calculate file byte and bit position
                size_t fileBytePosSource = (startBitPos + j) / 8;
                size_t fileBitPosInByte = (startBitPos + j) % 8;

                // Calculate chunk byte and bit position
                size_t chunkBytePos = j / 8;
                size_t chunkBitPosInByte = j % 8;

                // Check if we've reached the end of the file
                if (fileBytePosSource >= info.fileSizeBytes) {
                    break;
                }

                // Get the bit from the file
                bool bitValue = (fileBuffer[fileBytePosSource] & 1 << 7 - fileBitPosInByte) != 0;

                // Set the bit in the chunk buffer
                if (bitValue) {
                    chunkBuffer[chunkBytePos] |= 1 << 7 - chunkBitPosInByte;
                }
            }

            // Create output filename with matrix coordinates
            std::string outFileName = outDirName + "/" + baseName + "_chunk_r" + std::to_string(row) +
                                      "_c" + std::to_string(col) + ".bin";

            // Write chunk to output file
            std::ofstream outFile(outFileName, std::ios::binary);
            if (!outFile) {
                std::cerr << "Cannot create output file: " << outFileName << std::endl;
                return info;
            }

            // Write the full chunk size, even if we didn't fill it completely
            outFile.write(reinterpret_cast<char *>(chunkBuffer.data()), info.chunkSizeBytes);
            outFile.close();

            // Print progress every 100 chunks or for the last chunk
            if (i % 100 == 0 || i == info.totalChunks - 1) {
                std::cout << "Created chunk " << i + 1 << " of " << info.totalChunks
                        << " (r" << row << ", c" << col << "): " << outFileName << std::endl;
            }
        }

        // Create a metadata file with information about the chunking
        std::string metadataFileName = outDirName + "/" + baseName + "_metadata.txt";
        std::ofstream metadataFile(metadataFileName);
        if (metadataFile) {
            metadataFile << "Original file: " << filename << std::endl;
            metadataFile << "Original size: " << info.fileSizeBytes << " bytes (" << info.fileSizeBits << " bits)" <<
                    std::endl;
            metadataFile << "Original MD5: " << info.md5Original << std::endl;
            metadataFile << "Chunk dimensions: " << widthBits << "x" << heightBits << " bits" << std::endl;
            metadataFile << "Chunk size: " << info.chunkSizeBits << " bits (" << info.chunkSizeBytes << " bytes)" <<
                    std::endl;
            metadataFile << "Total chunks: " << info.totalChunks << std::endl;
            metadataFile << "Matrix layout: " << info.chunksPerRow << " chunks per row, " << info.totalRows << " rows"
                    << std::endl;
            metadataFile.close();
            std::cout << "Created metadata file: " << metadataFileName << std::endl;
        }

        std::cout << "File successfully split into " << info.totalChunks << " chunks of " << widthBits << "x" <<
                heightBits
                << " bits each, organized in a " << info.chunksPerRow << "x" << info.totalRows << " grid." << std::endl;

        // Immediately verify by merging and comparing hash
        std::string tempMergedFile = outDirName + "/" + baseName + "_merged_verification.bin";
        mergeAndVerifyChunks(outDirName, tempMergedFile);

        // Remove the temporary file after verification
        if (fs::exists(tempMergedFile)) {
            fs::remove(tempMergedFile);
        }

        return info;
    }

    // Helper function to parse a metadata file
    ChunkingInfo parseMetadataFile(const std::string &metadataFilePath) {
        ChunkingInfo info;
        std::ifstream metadataFile(metadataFilePath);
        if (!metadataFile) {
            std::cerr << "Cannot open metadata file: " << metadataFilePath << std::endl;
            return info;
        }

        std::string line;
        std::regex originalFileRegex("Original file: (.+)");
        std::regex originalSizeRegex("Original size: (\\d+) bytes \\((\\d+) bits\\)");
        std::regex originalMd5Regex("Original MD5: ([a-f0-9]+)");
        std::regex chunkDimensionsRegex("Chunk dimensions: (\\d+)x(\\d+) bits");
        std::regex chunkSizeRegex("Chunk size: (\\d+) bits \\((\\d+) bytes\\)");
        std::regex totalChunksRegex("Total chunks: (\\d+)");
        std::regex matrixLayoutRegex("Matrix layout: (\\d+) chunks per row, (\\d+) rows");

        std::smatch matches;
        while (std::getline(metadataFile, line)) {
            if (std::regex_search(line, matches, originalFileRegex)) {
                info.originFilename = matches[1];
            } else if (std::regex_search(line, matches, originalSizeRegex)) {
                info.fileSizeBytes = std::stoull(matches[1]);
                info.fileSizeBits = std::stoull(matches[2]);
            } else if (std::regex_search(line, matches, originalMd5Regex)) {
                info.md5Original = matches[1];
            } else if (std::regex_search(line, matches, chunkDimensionsRegex)) {
                info.widthBits = std::stoull(matches[1]);
                info.heightBits = std::stoull(matches[2]);
            } else if (std::regex_search(line, matches, chunkSizeRegex)) {
                info.chunkSizeBits = std::stoull(matches[1]);
                info.chunkSizeBytes = std::stoull(matches[2]);
            } else if (std::regex_search(line, matches, totalChunksRegex)) {
                info.totalChunks = std::stoull(matches[1]);
            } else if (std::regex_search(line, matches, matrixLayoutRegex)) {
                info.chunksPerRow = std::stoull(matches[1]);
                info.totalRows = std::stoull(matches[2]);
            }
        }

        // Set output directory as the directory containing the metadata file
        info.outputDir = fs::path(metadataFilePath).parent_path();

        return info;
    }

    // Function to merge chunks back into the original file and verify MD5
    bool mergeAndVerifyChunks(const std::string &outputDir, const std::string &outputFilename) {
        // Find metadata file in the output directory
        std::string metadataFilePath;
        for (const auto &entry: fs::directory_iterator(outputDir)) {
            if (entry.path().extension() == ".txt" && entry.path().filename().string().find("_metadata") !=
                std::string::npos) {
                metadataFilePath = entry.path().string();
                break;
            }
        }

        if (metadataFilePath.empty()) {
            std::cerr << "Cannot find metadata file in directory: " << outputDir << std::endl;
            return false;
        }

        ChunkingInfo info = parseMetadataFile(metadataFilePath);

        if (info.originFilename.empty() || info.totalChunks == 0) {
            std::cerr << "Invalid metadata file: " << metadataFilePath << std::endl;
            return false;
        }

        std::cout << "Merging " << info.totalChunks << " chunks into: " << outputFilename << std::endl;

        // Create output file
        std::ofstream outFile(outputFilename, std::ios::binary);
        if (!outFile) {
            std::cerr << "Cannot create output file: " << outputFilename << std::endl;
            return false;
        }

        // Get the base name from the original filename to construct chunk filenames
        std::string baseName = fs::path(info.originFilename).stem().string();

        // Allocate buffer for the entire file
        std::vector<unsigned char> fileBuffer(info.fileSizeBytes, 0);

        // Process each chunk
        for (size_t i = 0; i < info.totalChunks; i++) {
            // Calculate row and column indices for the chunk
            size_t row = i / info.chunksPerRow;
            size_t col = i % info.chunksPerRow;

            // Construct chunk filename
            std::string chunkFileName = (info.outputDir / (baseName + "_chunk_r" + std::to_string(row) +
                                                           "_c" + std::to_string(col) + ".bin")).string();

            // Open the chunk file
            std::ifstream chunkFile(chunkFileName, std::ios::binary);
            if (!chunkFile) {
                std::cerr << "Cannot open chunk file: " << chunkFileName << std::endl;
                return false;
            }

            // Read the chunk into a buffer
            std::vector<unsigned char> chunkBuffer(info.chunkSizeBytes);
            chunkFile.read(reinterpret_cast<char *>(chunkBuffer.data()), info.chunkSizeBytes);
            chunkFile.close();

            // Calculate starting bit position in the output file
            size_t startBitPos = i * info.chunkSizeBits;

            // Calculate how many bits to process for this chunk
            size_t bitsToProcess = std::min(info.chunkSizeBits, info.fileSizeBits - startBitPos);

            // Process bit by bit
            for (size_t j = 0; j < bitsToProcess; j++) {
                // Calculate chunk byte and bit position
                size_t chunkBytePos = j / 8;
                size_t chunkBitPosInByte = j % 8;

                // Calculate file byte and bit position
                size_t fileBytePosTarget = (startBitPos + j) / 8;
                size_t fileBitPosInByte = (startBitPos + j) % 8;

                // Check if we've reached the end of the file
                if (fileBytePosTarget >= info.fileSizeBytes) {
                    break;
                }

                // Get the bit from the chunk
                bool bitValue = (chunkBuffer[chunkBytePos] & 1 << 7 - chunkBitPosInByte) != 0;

                // Set the bit in the file buffer
                if (bitValue) {
                    fileBuffer[fileBytePosTarget] |= 1 << 7 - fileBitPosInByte;
                }
            }

            // Print progress every 100 chunks or for the last chunk
            if (i % 100 == 0 || i == info.totalChunks - 1) {
                std::cout << "Processed chunk " << i + 1 << " of " << info.totalChunks
                        << " (r" << row << ", c" << col << ")" << std::endl;
            }
        }

        // Write the entire file
        outFile.write(reinterpret_cast<char *>(fileBuffer.data()), info.fileSizeBytes);
        outFile.close();

        // Calculate MD5 hash of the merged file
        std::string md5Merged = utils::calculateMD5(outputFilename);
        std::cout << "Merged file MD5: " << md5Merged << std::endl;
        std::cout << "Original file MD5: " << info.md5Original << std::endl;

        // Verify the hashes
        bool success = md5Merged == info.md5Original;
        if (success) {
            std::cout << "MD5 checksums match! Chunks successfully verified." << std::endl;
        } else {
            std::cerr << "ERROR: MD5 checksums do not match! Chunking process was not successful." << std::endl;
        }

        return success;
    }
}
