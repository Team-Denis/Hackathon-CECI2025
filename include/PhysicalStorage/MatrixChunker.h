#ifndef MATRIX_CHUNKER_H
#define MATRIX_CHUNKER_H

#include <string>
#include <filesystem>
#include <functional>
#include <vector>

#include "CryptoUtils.h"
#include "StorageCommon.hpp"

namespace PhysicalStorage {

/**
 * Structure to hold information about the matrix chunking process.
 */
struct MatrixChunkingInfo {
    // Original file information
    std::string originFilename;
    size_t fileSizeBytes;
    size_t fileSizeBits;
    std::string md5Original;

    // Chunk parameters
    size_t widthBits;      // Width of each matrix chunk in bits
    size_t heightBits;     // Height of each matrix chunk in bits
    size_t chunkSizeBits;  // Total bits per chunk (widthBits * heightBits)
    size_t chunkSizeBytes; // Bytes per chunk (ceil(chunkSizeBits / 8))

    // Chunking results
    size_t totalChunks;    // Total number of chunks created
    size_t chunksPerRow;   // Chunks per row in the grid layout
    size_t totalRows;      // Number of rows in the grid layout
    std::filesystem::path outputDir; // Directory where chunks are stored
};

/**
 * MatrixChunker handles splitting files into 2D bit matrices for processing
 * and reassembling them afterwards.
 */
class MatrixChunker {
public:
    /**
     * Splits a file into matrix chunks of specified dimensions.
     *
     * @param filename Path to the file to split
     * @param widthBits Width of each matrix chunk in bits (default: 256)
     * @param heightBits Height of each matrix chunk in bits (default: 256)
     * @param outputDir Custom output directory (optional)
     * @return Information about the chunking process
     */
    static MatrixChunkingInfo splitFileIntoMatrices(
        const std::string& filename,
        size_t widthBits = Constants::DEFAULT_MATRIX_WIDTH,
        size_t heightBits = Constants::DEFAULT_MATRIX_HEIGHT,
        const std::string& outputDir = "");

    /**
     * Merges previously split chunks back into the original file and verifies integrity.
     *
     * @param chunksDir Directory containing the chunks
     * @param outputFilename Path where the merged file will be written
     * @return True if merge was successful and verified, false otherwise
     */
    static bool mergeAndVerify(
        const std::string& chunksDir,
        const std::string& outputFilename);

    /**
     * Performs batch operations on individual chunks.
     *
     * @param chunksDir Directory containing the chunks
     * @param processFunction Function to apply to each chunk
     * @param outputDir Directory to write processed chunks (defaults to chunksDir)
     * @return Number of successfully processed chunks
     */
    static int batchProcessChunks(
        const std::string& chunksDir,
        std::function<std::vector<unsigned char>(const std::vector<unsigned char>&)> processFunction,
        const std::string& outputDir = "");

private:
    /**
     * Parses a metadata file to retrieve chunking information.
     *
     * @param metadataFilePath Path to the metadata file
     * @return Information parsed from the metadata file
     */
    static MatrixChunkingInfo parseMetadataFile(const std::string& metadataFilePath);

    /**
     * Creates a metadata file with chunking information.
     *
     * @param info Chunking information to write
     * @param metadataFilePath Path where the metadata file will be written
     * @return True if metadata was written successfully, false otherwise
     */
    static bool writeMetadataFile(const MatrixChunkingInfo& info, const std::string& metadataFilePath);
};

} // namespace PhysicalStorage

#endif // MATRIX_CHUNKER_H