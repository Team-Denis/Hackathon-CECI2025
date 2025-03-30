#include "PhysicalStorage/MatrixChunker.h"

namespace fs = std::filesystem;

namespace PhysicalStorage {
    FileInfo MatrixChunker::analyzeFile(const std::string &filename, size_t widthBits, size_t heightBits) {
        FileInfo info;
        info.filename = filename;

        // Check if file exists
        if (!fs::exists(filename)) {
            std::cerr << "File does not exist: " << filename << std::endl;
            return info;
        }

        // Get file size
        info.fileSizeBytes = fs::file_size(filename);
        info.fileSizeBits = info.fileSizeBytes * 8;

        // Calculate chunk size and total chunks needed
        size_t chunkSizeBits = widthBits * heightBits;
        info.totalChunks = (info.fileSizeBits + chunkSizeBits - 1) / chunkSizeBits; // Ceiling division

        std::cout << "File: " << filename << std::endl;
        std::cout << "Size: " << info.fileSizeBytes << " bytes (" << info.fileSizeBits << " bits)" << std::endl;
        std::cout << "Chunk dimensions: " << widthBits << "x" << heightBits << " bits" << std::endl;
        std::cout << "Total chunks needed: " << info.totalChunks << std::endl;

        return info;
    }
} // namespace PhysicalStorage
