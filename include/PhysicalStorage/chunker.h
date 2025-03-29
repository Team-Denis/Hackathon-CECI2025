#ifndef FILE_CHUNKER_H
#define FILE_CHUNKER_H

#include <string>
#include <filesystem>

namespace chunker {
    // Structure to hold chunking parameters and results
    struct ChunkingInfo {
        std::string originFilename;
        size_t fileSizeBytes;
        size_t fileSizeBits;
        size_t widthBits;
        size_t heightBits;
        size_t chunkSizeBits;
        size_t chunkSizeBytes;
        size_t totalChunks;
        size_t chunksPerRow;
        size_t totalRows;
        std::string md5Original;
        std::filesystem::path outputDir;
    };

    // Function to split file into matrix chunks of width x height bits
    ChunkingInfo splitFileIntoBitMatrixChunks(const std::string &filename);

    // Function to merge chunks back into the original file and verify MD5
    bool mergeAndVerifyChunks(const std::string &outputDir, const std::string &outputFilename);
}

#endif // FILE_CHUNKER_H
